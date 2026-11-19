#!/usr/bin/env python3
"""Create tau histograms from tb_geawis_taus_external captured output."""

import argparse
import math
import re
import shutil
import subprocess
import sys
from collections import Counter
from pathlib import Path


TAU_LINE = re.compile(
    r"^(?P<file>\S+\.csv) event=(?P<event>\d+) "
    r"pt=(?P<pt>[-+0-9.eE]+) eta=(?P<eta>-?\d+) phi=(?P<phi>-?\d+) "
    r"motherid=(?P<motherid>-?\d+) "
    r"charge=(?P<charge>-?\d+) prongs=(?P<prongs>\d+) "
    r"photons=(?P<photons>\d+) (?P<isolation_label>reliso|isolation)="
    r"(?P<isolation>[-+0-9.eE]+)$"
)
SUMMARY_LINE = re.compile(
    r"^(?P<file>\S+\.csv): processed (?P<events>\d+) events, "
    r"found (?P<taus>\d+) taus$"
)

HISTOGRAMS = (
    ("pt", "Tau pT (GeV)", 0, 5.0),
    ("eta", "Tau eta (hardware units)", 1, 100.0),
    ("phi", "Tau phi (hardware units)", 2, 100.0),
    ("charge", "Tau charge", 3, None),
    ("prongs", "Charged prongs", 4, None),
    ("photons", "Signal photons", 5, 1.0),
    ("isolation", "Relative tau isolation", 6, 0.05),
    ("motherid", "Mother particle ID", 7, None),
)


def parse_output(path):
    datasets = {}
    summaries = {}
    source_names = {}

    with path.open(encoding="utf-8") as output:
        for line_number, line in enumerate(output, 1):
            line = line.rstrip("\n")
            match = TAU_LINE.match(line)
            if match:
                filename = match.group("file")
                tag = Path(filename).stem
                previous = source_names.setdefault(tag, filename)
                if previous != filename:
                    raise ValueError(
                        f"line {line_number}: distinct input paths share "
                        f"the dataset name {tag!r}"
                    )
                tau_pt = float(match.group("pt"))
                isolation = float(match.group("isolation"))
                if match.group("isolation_label") == "isolation":
                    if tau_pt <= 0:
                        raise ValueError(f"line {line_number}: non-positive tau pT")
                    isolation /= tau_pt
                values = (
                    tau_pt,
                    int(match.group("eta")),
                    int(match.group("phi")),
                    int(match.group("charge")),
                    int(match.group("prongs")),
                    int(match.group("photons")),
                    float(match.group("isolation")),
                    int(match.group("motherid")),
                )
                if not all(math.isfinite(value) for value in (values[0], values[6])):
                    raise ValueError(f"line {line_number}: non-finite tau value")
                datasets.setdefault(tag, []).append(values)
                continue

            match = SUMMARY_LINE.match(line)
            if match:
                filename = match.group("file")
                tag = Path(filename).stem
                previous = source_names.setdefault(tag, filename)
                if previous != filename:
                    raise ValueError(
                        f"line {line_number}: distinct input paths share "
                        f"the dataset name {tag!r}"
                    )
                if tag in summaries:
                    raise ValueError(f"line {line_number}: duplicate summary for {filename}")
                summaries[tag] = (
                    filename,
                    int(match.group("events")),
                    int(match.group("taus")),
                )

    if not summaries:
        raise ValueError(f"{path}: no testbench summary lines found")

    for tag, (filename, _, expected_count) in summaries.items():
        actual_count = len(datasets.get(tag, ()))
        if actual_count != expected_count:
            raise ValueError(
                f"{filename}: summary reports {expected_count} taus but "
                f"{actual_count} tau rows were found"
            )
    unexpected = datasets.keys() - summaries.keys()
    if unexpected:
        raise ValueError(f"tau rows have no matching file summary: {', '.join(sorted(unexpected))}")
    return datasets, summaries


def histogram(values, width):
    counts = Counter(math.floor(value / width) for value in values)
    if counts:
        first, last = min(counts), max(counts)
    else:
        first = last = 0
    return [(index * width + width / 2.0, counts[index]) for index in range(first, last + 1)]


def discrete_histogram(values, category_values):
    counts = Counter(values)
    return [(value, counts[value]) for value in category_values]


def gnuplot_quote(value):
    return "'" + str(value).replace("\\", "\\\\").replace("'", "\\'") + "'"


def write_histogram(path, rows):
    with path.open("w", encoding="utf-8") as output:
        for x, count in rows:
            output.write(f"{x:g} {count}\n")


def create_plots(datasets, summaries, output_dir, gnuplot):
    output_dir.mkdir(parents=True, exist_ok=True)
    script_lines = [
        'set terminal svg size 1600,1800 dynamic enhanced font "Arial,11"',
        "set datafile separator whitespace",
        'set style fill solid 0.72 border lc rgb "#333333"',
        'set style line 1 lc rgb "#32688e"',
        'set style line 2 lc rgb "#d1495b"',
        "set border linewidth 1",
        "set tics out",
        'set grid ytics lc rgb "#dddddd"',
        "set key off",
    ]

    for tag, (filename, events, expected_taus) in summaries.items():
        values = datasets.get(tag, [])
        isolated = [row for row in values if row[6] < 0.1]
        for suffix, selected in (("all", values), ("iso_lt_10pct", isolated)):
            output_svg = output_dir / f"{tag}_{suffix}.svg"
            title = f"{tag}: taus with pT > 10 GeV"
            if suffix == "iso_lt_10pct":
                title += " and relative isolation < 10%"
            script_lines.extend(
                [
                    f"set output {gnuplot_quote(output_svg.resolve())}",
                    f"set multiplot layout 5,2 title {gnuplot_quote(title)} font ',16'",
                ]
            )

            for name, xlabel, column, width in HISTOGRAMS:
                histogram_path = output_dir / f"{tag}_{suffix}_{name}.dat"
                if name == "charge":
                    histogram_rows = discrete_histogram(
                        (row[column] for row in selected), (-1, 1)
                    )
                    plot_width = 1.0
                    xtics = 'set xtics ("-1" -1, "1" 1)'
                elif name == "prongs":
                    histogram_rows = discrete_histogram(
                        (row[column] for row in selected), (1, 3)
                    )
                    plot_width = 1.0
                    xtics = 'set xtics ("1" 1, "3" 3)'
                elif name == "motherid":
                    mother_ids = sorted({row[column] for row in selected}) or (0,)
                    histogram_rows = discrete_histogram(
                        (row[column] for row in selected), mother_ids
                    )
                    plot_width = 1.0
                    xtics = "set xtics autofreq"
                else:
                    selected_width = 0.025 if suffix == "iso_lt_10pct" and name == "isolation" else width
                    histogram_rows = histogram(
                        (row[column] for row in selected), selected_width
                    )
                    plot_width = selected_width
                    xtics = "set xtics autofreq"
                write_histogram(histogram_path, histogram_rows)
                script_lines.extend(
                    [
                        f"set title {gnuplot_quote(name)}",
                        f"set xlabel {gnuplot_quote(xlabel)}",
                        'set ylabel "Tau count"',
                        "set autoscale x",
                        "set yrange [0:*]",
                        xtics,
                    ]
                )
                if suffix == "iso_lt_10pct" and name == "isolation":
                    script_lines.append("set xrange [0:0.1]")
                script_lines.extend(
                    [
                        f"set boxwidth {plot_width:g} absolute",
                        f"plot {gnuplot_quote(histogram_path.resolve())} "
                        "using 1:2 with boxes ls 1",
                    ]
                )

            scatter_path = output_dir / f"{tag}_{suffix}_eta_phi.dat"
            if suffix == "all" or isolated:
                with scatter_path.open("w", encoding="utf-8") as output:
                    for row in selected:
                        output.write(f"{row[1]} {row[2]}\n")
            if selected:
                script_lines.extend(
                    [
                        'set title "eta-phi"',
                        'set xlabel "Tau eta (hardware units)"',
                        'set ylabel "Tau phi (hardware units)"',
                        "set autoscale x",
                        "set autoscale y",
                        f"plot {gnuplot_quote(scatter_path.resolve())} "
                        'using 1:2 with points pt 7 ps 0.45 lc rgb "#d1495b"',
                    ]
                )
            else:
                script_lines.extend(
                    [
                        'set title "eta-phi (no taus in selection)"',
                        'set xlabel "Tau eta (hardware units)"',
                        'set ylabel "Tau phi (hardware units)"',
                        "set xrange [-1:1]",
                        "set yrange [-1:1]",
                        "plot 0 with lines lc rgb 'white' notitle",
                    ]
                )
            script_lines.extend(["unset multiplot", "unset output"])

    script_path = output_dir / "plot_tau_histograms.gp"
    script_path.write_text("\n".join(script_lines) + "\n", encoding="utf-8")
    command = shutil.which(gnuplot)
    if command is None:
        raise RuntimeError(f"gnuplot executable not found: {gnuplot}")
    subprocess.run([command, str(script_path.resolve())], check=True)

    print(f"Plots and gnuplot inputs saved in {output_dir}")
    for tag, (filename, events, expected_taus) in summaries.items():
        rows = datasets.get(tag, [])
        print(
            f"{filename}: {events} events, {expected_taus} taus with pT > 10, "
            f"{sum(row[6] < 0.1 for row in rows)} with relative isolation < 10%"
        )


def main():
    parser = argparse.ArgumentParser(
        description="Plot tau testbench output using gnuplot."
    )
    parser.add_argument("testbench_output", type=Path, help="captured stdout from the tau testbench")
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=Path("tau_plots"),
        help="directory for SVG plots, gnuplot script, and histogram data (default: tau_plots)",
    )
    parser.add_argument(
        "--gnuplot",
        default="gnuplot",
        help="gnuplot executable name or path (default: gnuplot)",
    )
    args = parser.parse_args()

    try:
        datasets, summaries = parse_output(args.testbench_output)
        create_plots(datasets, summaries, args.output_dir, args.gnuplot)
    except (OSError, ValueError, RuntimeError, subprocess.CalledProcessError) as error:
        print(f"Error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
