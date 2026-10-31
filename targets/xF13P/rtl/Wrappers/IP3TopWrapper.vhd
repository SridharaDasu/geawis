library IEEE;
use IEEE.STD_LOGIC_1164.all;
use ieee.numeric_std.all;

library surf;
use surf.StdRtlPkg.all;
use surf.AxiLitePkg.all;
use surf.AxiStreamPkg.all;

entity IP3TopWrapper is
 generic (
    N_INPUT_STREAMS  : integer := 32;
    N_OUTPUT_STREAMS : integer := 1
    );
  port (
    -- Algo Control/Status Signals
    algoClk   : in  std_logic;
    algoRst   : in  std_logic;
    algoStart : in  std_logic;
    algoDone  : out std_logic := '0';
    algoIdle  : out std_logic := '0';
    algoReady : out std_logic := '0';

    -- AXI-Stream In/Out Ports
    axiStreamIn  : in  AxiStreamMasterArray(0 TO N_INPUT_STREAMS-1);
    axiStreamOut : out AxiStreamMasterArray(0 TO N_OUTPUT_STREAMS-1)
    );
end IP3TopWrapper;

architecture Behavioral of IP3TopWrapper is

begin

   i_IP21_algo_top : entity work.algo_top
    port map (
        ap_clk      => algoClk,
        ap_rst      => algoRst,
        ap_start    => algoStart,
        ap_done     => algoDone,
        ap_idle     => algoIdle,
        ap_ready    => algoReady,

        link_in_0  => axiStreamIn(0).tdata(63 downto 0),
        link_in_1  => axiStreamIn(1).tdata(63 downto 0),
        link_in_2  => axiStreamIn(2).tdata(63 downto 0),
        link_in_3  => axiStreamIn(3).tdata(63 downto 0),
        link_in_4  => axiStreamIn(4).tdata(63 downto 0),
        link_in_5  => axiStreamIn(5).tdata(63 downto 0),
        link_in_6  => axiStreamIn(6).tdata(63 downto 0),
        link_in_7  => axiStreamIn(7).tdata(63 downto 0),
        link_in_8  => axiStreamIn(8).tdata(63 downto 0),
        link_in_9  => axiStreamIn(9).tdata(63 downto 0),
        link_in_10 => axiStreamIn(10).tdata(63 downto 0),
        link_in_11 => axiStreamIn(11).tdata(63 downto 0),
        link_in_12 => axiStreamIn(12).tdata(63 downto 0),
        link_in_13 => axiStreamIn(13).tdata(63 downto 0),
        link_in_14 => axiStreamIn(14).tdata(63 downto 0),
        link_in_15 => axiStreamIn(15).tdata(63 downto 0),
        link_in_16 => axiStreamIn(16).tdata(63 downto 0),
        link_in_17 => axiStreamIn(17).tdata(63 downto 0),
        link_in_18 => axiStreamIn(18).tdata(63 downto 0),
        link_in_19 => axiStreamIn(19).tdata(63 downto 0),
        link_in_20 => axiStreamIn(20).tdata(63 downto 0),
        link_in_21 => axiStreamIn(21).tdata(63 downto 0),
        link_in_22 => axiStreamIn(22).tdata(63 downto 0),
        link_in_23 => axiStreamIn(23).tdata(63 downto 0),
        link_in_24 => axiStreamIn(24).tdata(63 downto 0),
        link_in_25 => axiStreamIn(25).tdata(63 downto 0),
        link_in_26 => axiStreamIn(26).tdata(63 downto 0),
        link_in_27 => axiStreamIn(27).tdata(63 downto 0),
        link_in_28 => axiStreamIn(28).tdata(63 downto 0),
        link_in_29 => axiStreamIn(29).tdata(63 downto 0),
        link_in_30 => axiStreamIn(30).tdata(63 downto 0),
        link_in_31 => axiStreamIn(31).tdata(63 downto 0),
        link_out => axiStreamOut(0).tdata(63 downto 0),   
        link_out_ap_vld => axiStreamOut(0).tValid
);


end Behavioral;

