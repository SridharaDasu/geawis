open_project -reset algo_top_cosim

set_top algo_top

add_files -tb tb_algo_top.cpp -cflags "${cflags}"
add_files firmware/algo_top.cpp -cflags "${cflags}"
add_files firmware/topx.cpp -cflags "${cflags}"
add_files firmware/geawis.cpp -cflags "${cflags}"

# Solution
open_solution -reset "solution"
set_part {xcvu13p-flga2577-2-e}
create_clock -period 2.778 -name default

launch_simulation
run all

exit
