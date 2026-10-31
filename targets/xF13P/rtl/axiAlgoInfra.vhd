library IEEE;
use IEEE.STD_LOGIC_1164.all;
use ieee.numeric_std.all;


library surf;
use surf.StdRtlPkg.all;
use surf.AxiLitePkg.all;

library apx_fs;

entity axiAlgoInfra is
  generic (
    TPD_G        : time    := 1 ns;
    SLR_CNT_G : natural := 4
    );
  port (

    -- AXI-Lite Bus
    axiClk   : in sl;
    axiRstIn : in sl;

    sAxiReadMaster  : in  AxiLiteReadMasterType;
    sAxiReadSlave   : out AxiLiteReadSlaveType;
    sAxiWriteMaster : in  AxiLiteWriteMasterType;
    sAxiWriteSlave  : out AxiLiteWriteSlaveType;

    axiRstOut : out slv(SLR_CNT_G-1 downto 0);

    mAxiWriteMasters : out AxiLiteWriteMasterArray(SLR_CNT_G-1 downto 0) := (others => AXI_LITE_WRITE_MASTER_INIT_C);
    mAxiWriteSlaves  : in  AxiLiteWriteSlaveArray(SLR_CNT_G-1 downto 0)  := (others => AXI_LITE_WRITE_SLAVE_EMPTY_DECERR_C);
    mAxiReadMasters  : out AxiLiteReadMasterArray(SLR_CNT_G-1 downto 0)  := (others => AXI_LITE_READ_MASTER_INIT_C);
    mAxiReadSlaves   : in  AxiLiteReadSlaveArray(SLR_CNT_G-1 downto 0)   := (others => AXI_LITE_READ_SLAVE_EMPTY_DECERR_C)
    );
end axiAlgoInfra;

architecture rtl of axiAlgoInfra is

  component axilRegSliceWrapper
    generic (
      REG_SLICE_TYPE_G : string := "LIGHT"
      ); port (
        axiClk : in sl;
        axiRst : in sl;

        sAxiReadMaster  : in  AxiLiteReadMasterType;
        sAxiReadSlave   : out AxiLiteReadSlaveType;
        sAxiWriteMaster : in  AxiLiteWriteMasterType;
        sAxiWriteSlave  : out AxiLiteWriteSlaveType;

        mAxiReadMaster  : out AxiLiteReadMasterType;
        mAxiReadSlave   : in  AxiLiteReadSlaveType;
        mAxiWriteMaster : out AxiLiteWriteMasterType;
        mAxiWriteSlave  : in  AxiLiteWriteSlaveType
        );
  end component;

  constant SLR_ADDR_BIT_CNT_C : integer := 16;

  constant SLR0_XBAR_MASTERS : integer := 2;
  constant SLR1_XBAR_MASTERS : integer := 2;
  constant SLR2_XBAR_MASTERS : integer := 2;
  
  
  constant SLR_0_BASE_ADDR_C  : slv(31 downto 0) := x"6C00_0000";
  constant SLR_1_BASE_ADDR_C  : slv(31 downto 0) := x"6C01_0000";
  constant SLR_2_BASE_ADDR_C  : slv(31 downto 0) := x"6C02_0000";
  constant SLR_3_BASE_ADDR_C  : slv(31 downto 0) := x"6C03_0000";

  constant XBAR_CONFIG_SLR0_C : AxiLiteCrossbarMasterConfigArray(SLR0_XBAR_MASTERS-1 downto 0) := (
    0              => (
      baseAddr     => SLR_0_BASE_ADDR_C,
      addrBits     => SLR_ADDR_BIT_CNT_C,
      connectivity => x"FFFF"),
    1              => (
      baseAddr     => SLR_1_BASE_ADDR_C,
      addrBits     => SLR_ADDR_BIT_CNT_C + 2,
      connectivity => x"FFFF")
    );

  constant XBAR_CONFIG_SLR1_C : AxiLiteCrossbarMasterConfigArray(SLR1_XBAR_MASTERS-1 downto 0) := (
    0              => (
      baseAddr     => SLR_1_BASE_ADDR_C,
      addrBits     => SLR_ADDR_BIT_CNT_C,
      connectivity => x"FFFF"),
    1              => (
      baseAddr     => SLR_2_BASE_ADDR_C,
      addrBits     => SLR_ADDR_BIT_CNT_C + 2,
      connectivity => x"FFFF")
    );

  constant XBAR_CONFIG_SLR2_C : AxiLiteCrossbarMasterConfigArray(SLR2_XBAR_MASTERS-1 downto 0) := (
    0              => (
      baseAddr     => SLR_2_BASE_ADDR_C,
      addrBits     => SLR_ADDR_BIT_CNT_C,
      connectivity => x"FFFF"),
    1              => (
      baseAddr     => SLR_3_BASE_ADDR_C,
      addrBits     => SLR_ADDR_BIT_CNT_C,
      connectivity => x"FFFF")
    );

  signal axiRstSec : slv(SLR_CNT_G-1 downto 0);

---------------------------------
-- SLR 0 signals
-- Crossbar signals
  signal AWM_XB_In_SLR0 : AxiLiteWriteMasterArray(0 downto 0) := (others => AXI_LITE_WRITE_MASTER_INIT_C);
  signal AWS_XB_In_SLR0 : AxiLiteWriteSlaveArray(0 downto 0)  := (others => AXI_LITE_WRITE_SLAVE_EMPTY_DECERR_C);
  signal ARM_XB_In_SLR0 : AxiLiteReadMasterArray(0 downto 0)  := (others => AXI_LITE_READ_MASTER_INIT_C);
  signal ARS_XB_In_SLR0 : AxiLiteReadSlaveArray(0 downto 0)   := (others => AXI_LITE_READ_SLAVE_EMPTY_DECERR_C);

  signal AWM_XB_Out_SLR0 : AxiLiteWriteMasterArray(SLR0_XBAR_MASTERS-1 downto 0) := (others => AXI_LITE_WRITE_MASTER_INIT_C);
  signal AWS_XB_Out_SLR0 : AxiLiteWriteSlaveArray(SLR0_XBAR_MASTERS-1 downto 0)  := (others => AXI_LITE_WRITE_SLAVE_EMPTY_DECERR_C);
  signal ARM_XB_Out_SLR0 : AxiLiteReadMasterArray(SLR0_XBAR_MASTERS-1 downto 0)  := (others => AXI_LITE_READ_MASTER_INIT_C);
  signal ARS_XB_Out_SLR0 : AxiLiteReadSlaveArray(SLR0_XBAR_MASTERS-1 downto 0)   := (others => AXI_LITE_READ_SLAVE_EMPTY_DECERR_C);

---------------------------------
-- SLR 1 signals
-- Crossbar signals
  signal AWM_XB_In_SLR1 : AxiLiteWriteMasterArray(0 downto 0) := (others => AXI_LITE_WRITE_MASTER_INIT_C);
  signal AWS_XB_In_SLR1 : AxiLiteWriteSlaveArray(0 downto 0)  := (others => AXI_LITE_WRITE_SLAVE_EMPTY_DECERR_C);
  signal ARM_XB_In_SLR1 : AxiLiteReadMasterArray(0 downto 0)  := (others => AXI_LITE_READ_MASTER_INIT_C);
  signal ARS_XB_In_SLR1 : AxiLiteReadSlaveArray(0 downto 0)   := (others => AXI_LITE_READ_SLAVE_EMPTY_DECERR_C);

  signal AWM_XB_Out_SLR1 : AxiLiteWriteMasterArray(SLR1_XBAR_MASTERS-1 downto 0) := (others => AXI_LITE_WRITE_MASTER_INIT_C);
  signal AWS_XB_Out_SLR1 : AxiLiteWriteSlaveArray(SLR1_XBAR_MASTERS-1 downto 0)  := (others => AXI_LITE_WRITE_SLAVE_EMPTY_DECERR_C);
  signal ARM_XB_Out_SLR1 : AxiLiteReadMasterArray(SLR1_XBAR_MASTERS-1 downto 0)  := (others => AXI_LITE_READ_MASTER_INIT_C);
  signal ARS_XB_Out_SLR1 : AxiLiteReadSlaveArray(SLR1_XBAR_MASTERS-1 downto 0)   := (others => AXI_LITE_READ_SLAVE_EMPTY_DECERR_C);

-- Register slice signals
  signal AWM_RS_In_SLR1 : AxiLiteWriteMasterType := AXI_LITE_WRITE_MASTER_INIT_C;
  signal AWS_RS_In_SLR1 : AxiLiteWriteSlaveType  := AXI_LITE_WRITE_SLAVE_EMPTY_DECERR_C;
  signal ARM_RS_In_SLR1 : AxiLiteReadMasterType  := AXI_LITE_READ_MASTER_INIT_C;
  signal ARS_RS_In_SLR1 : AxiLiteReadSlaveType   := AXI_LITE_READ_SLAVE_EMPTY_DECERR_C;

  signal AWM_RS_Out_SLR1 : AxiLiteWriteMasterType              := AXI_LITE_WRITE_MASTER_INIT_C;
  signal AWS_RS_Out_SLR1 : AxiLiteWriteSlaveType               := AXI_LITE_WRITE_SLAVE_EMPTY_DECERR_C;
  signal ARM_RS_Out_SLR1 : AxiLiteReadMasterType               := AXI_LITE_READ_MASTER_INIT_C;
  signal ARS_RS_Out_SLR1 : AxiLiteReadSlaveType                := AXI_LITE_READ_SLAVE_EMPTY_DECERR_C;
---------------------------------
-- SLR 2 signals
-- Crossbar signals
  signal AWM_XB_In_SLR2  : AxiLiteWriteMasterArray(0 downto 0) := (others => AXI_LITE_WRITE_MASTER_INIT_C);
  signal AWS_XB_In_SLR2  : AxiLiteWriteSlaveArray(0 downto 0)  := (others => AXI_LITE_WRITE_SLAVE_EMPTY_DECERR_C);
  signal ARM_XB_In_SLR2  : AxiLiteReadMasterArray(0 downto 0)  := (others => AXI_LITE_READ_MASTER_INIT_C);
  signal ARS_XB_In_SLR2  : AxiLiteReadSlaveArray(0 downto 0)   := (others => AXI_LITE_READ_SLAVE_EMPTY_DECERR_C);

  signal AWM_XB_Out_SLR2 : AxiLiteWriteMasterArray(SLR2_XBAR_MASTERS-1 downto 0) := (others => AXI_LITE_WRITE_MASTER_INIT_C);
  signal AWS_XB_Out_SLR2 : AxiLiteWriteSlaveArray(SLR2_XBAR_MASTERS-1 downto 0)  := (others => AXI_LITE_WRITE_SLAVE_EMPTY_DECERR_C);
  signal ARM_XB_Out_SLR2 : AxiLiteReadMasterArray(SLR2_XBAR_MASTERS-1 downto 0)  := (others => AXI_LITE_READ_MASTER_INIT_C);
  signal ARS_XB_Out_SLR2 : AxiLiteReadSlaveArray(SLR2_XBAR_MASTERS-1 downto 0)   := (others => AXI_LITE_READ_SLAVE_EMPTY_DECERR_C);

-- Register slice signals
  signal AWM_RS_In_SLR2 : AxiLiteWriteMasterType := AXI_LITE_WRITE_MASTER_INIT_C;
  signal AWS_RS_In_SLR2 : AxiLiteWriteSlaveType  := AXI_LITE_WRITE_SLAVE_EMPTY_DECERR_C;
  signal ARM_RS_In_SLR2 : AxiLiteReadMasterType  := AXI_LITE_READ_MASTER_INIT_C;
  signal ARS_RS_In_SLR2 : AxiLiteReadSlaveType   := AXI_LITE_READ_SLAVE_EMPTY_DECERR_C;

  signal AWM_RS_Out_SLR2 : AxiLiteWriteMasterType := AXI_LITE_WRITE_MASTER_INIT_C;
  signal AWS_RS_Out_SLR2 : AxiLiteWriteSlaveType  := AXI_LITE_WRITE_SLAVE_EMPTY_DECERR_C;
  signal ARM_RS_Out_SLR2 : AxiLiteReadMasterType  := AXI_LITE_READ_MASTER_INIT_C;
  signal ARS_RS_Out_SLR2 : AxiLiteReadSlaveType   := AXI_LITE_READ_SLAVE_EMPTY_DECERR_C;
---------------------------------
-- SLR 3 signals
-- Register slice signals
  signal AWM_RS_In_SLR3  : AxiLiteWriteMasterType := AXI_LITE_WRITE_MASTER_INIT_C;
  signal AWS_RS_In_SLR3  : AxiLiteWriteSlaveType  := AXI_LITE_WRITE_SLAVE_EMPTY_DECERR_C;
  signal ARM_RS_In_SLR3  : AxiLiteReadMasterType  := AXI_LITE_READ_MASTER_INIT_C;
  signal ARS_RS_In_SLR3  : AxiLiteReadSlaveType   := AXI_LITE_READ_SLAVE_EMPTY_DECERR_C;

  signal AWM_RS_Out_SLR3 : AxiLiteWriteMasterType              := AXI_LITE_WRITE_MASTER_INIT_C;
  signal AWS_RS_Out_SLR3 : AxiLiteWriteSlaveType               := AXI_LITE_WRITE_SLAVE_EMPTY_DECERR_C;
  signal ARM_RS_Out_SLR3 : AxiLiteReadMasterType               := AXI_LITE_READ_MASTER_INIT_C;
  signal ARS_RS_Out_SLR3 : AxiLiteReadSlaveType                := AXI_LITE_READ_SLAVE_EMPTY_DECERR_C;
---------------------------------

  attribute KEEP              : string;
  attribute KEEP of axiRstSec : signal is "TRUE";

begin

-- Axi Reset signal fanout
  process(axiClk) is
  begin
    if rising_edge(axiClk) then
      for i in 0 to SLR_CNT_G-1 loop
        axiRstSec(i) <= axiRstIn;
      end loop;
    end if;
  end process;

  axiRstOut <= axiRstSec;

  --------------------------
  -- SLR 0
  --------------------------

  -- SLR 1 connects to top level AXI-lite slave interface
  AWM_XB_In_SLR0(0) <= saxiWriteMaster;
  ARM_XB_In_SLR0(0) <= saxiReadMaster;
  saxiWriteSlave    <= AWS_XB_In_SLR0(0);
  saxiReadSlave     <= ARS_XB_In_SLR0(0);

  U_XBAR_SLR0 : entity surf.AxiLiteCrossbar
    generic map (
      TPD_G              => TPD_G,
      NUM_SLAVE_SLOTS_G  => 1,
      NUM_MASTER_SLOTS_G => SLR0_XBAR_MASTERS,
      MASTERS_CONFIG_G   => XBAR_CONFIG_SLR0_C
      )
    port map (
      axiClk           => axiClk,
      axiClkRst        => axiRstSec(0),
      sAxiWriteMasters => AWM_XB_In_SLR0,
      sAxiWriteSlaves  => AWS_XB_In_SLR0,
      sAxiReadMasters  => ARM_XB_In_SLR0,
      sAxiReadSlaves   => ARS_XB_In_SLR0,
      mAxiWriteMasters => AWM_XB_Out_SLR0,
      mAxiWriteSlaves  => AWS_XB_Out_SLR0,
      mAxiReadMasters  => ARM_XB_Out_SLR0,
      mAxiReadSlaves   => ARS_XB_Out_SLR0
      );

  --------------------------
  -- SLR 1
  --------------------------

  AWM_RS_In_SLR1     <= AWM_XB_Out_SLR0(1);
  ARM_RS_In_SLR1     <= ARM_XB_Out_SLR0(1);
  AWS_XB_Out_SLR0(1) <= AWS_RS_In_SLR1;
  ARS_XB_Out_SLR0(1) <= ARS_RS_In_SLR1;

  U_Axi_RegSlice_SLR1 : axilRegSliceWrapper
    generic map (
      REG_SLICE_TYPE_G => "LIGHT"
      )
    port map(
      axiClk          => axiClk,
      axiRst          => axiRstSec(1),
      sAxiWriteMaster => AWM_RS_In_SLR1,
      sAxiWriteSlave  => AWS_RS_In_SLR1,
      sAxiReadMaster  => ARM_RS_In_SLR1,
      sAxiReadSlave   => ARS_RS_In_SLR1,
      mAxiWriteMaster => AWM_RS_Out_SLR1,
      mAxiWriteSlave  => AWS_RS_Out_SLR1,
      mAxiReadMaster  => ARM_RS_Out_SLR1,
      mAxiReadSlave   => ARS_RS_Out_SLR1);

  AWM_XB_In_SLR1(0) <= AWM_RS_Out_SLR1;
  ARM_XB_In_SLR1(0) <= ARM_RS_Out_SLR1;
  AWS_RS_Out_SLR1   <= AWS_XB_In_SLR1(0);
  ARS_RS_Out_SLR1   <= ARS_XB_In_SLR1(0);

  U_XBAR_SLR1 : entity surf.AxiLiteCrossbar
    generic map (
      TPD_G              => TPD_G,
      NUM_SLAVE_SLOTS_G  => 1,
      NUM_MASTER_SLOTS_G => SLR1_XBAR_MASTERS,
      MASTERS_CONFIG_G   => XBAR_CONFIG_SLR1_C)
    port map (
      axiClk           => axiClk,
      axiClkRst        => axiRstSec(1),
      sAxiWriteMasters => AWM_XB_In_SLR1,
      sAxiWriteSlaves  => AWS_XB_In_SLR1,
      sAxiReadMasters  => ARM_XB_In_SLR1,
      sAxiReadSlaves   => ARS_XB_In_SLR1,
      mAxiWriteMasters => AWM_XB_Out_SLR1,
      mAxiWriteSlaves  => AWS_XB_Out_SLR1,
      mAxiReadMasters  => ARM_XB_Out_SLR1,
      mAxiReadSlaves   => ARS_XB_Out_SLR1);

  --------------------------
  -- SLR 2
  --------------------------

  AWM_RS_In_SLR2     <= AWM_XB_Out_SLR1(1);
  ARM_RS_In_SLR2     <= ARM_XB_Out_SLR1(1);
  AWS_XB_Out_SLR1(1) <= AWS_RS_In_SLR2;
  ARS_XB_Out_SLR1(1) <= ARS_RS_In_SLR2;

  U_Axi_RegSlice_SLR2 : axilRegSliceWrapper
    generic map (
      REG_SLICE_TYPE_G => "LIGHT"
      )
    port map(
      axiClk          => axiClk,
      axiRst          => axiRstSec(2),
      sAxiWriteMaster => AWM_RS_In_SLR2,
      sAxiWriteSlave  => AWS_RS_In_SLR2,
      sAxiReadMaster  => ARM_RS_In_SLR2,
      sAxiReadSlave   => ARS_RS_In_SLR2,
      mAxiWriteMaster => AWM_RS_Out_SLR2,
      mAxiWriteSlave  => AWS_RS_Out_SLR2,
      mAxiReadMaster  => ARM_RS_Out_SLR2,
      mAxiReadSlave   => ARS_RS_Out_SLR2);

  AWM_XB_In_SLR2(0) <= AWM_RS_Out_SLR2;
  ARM_XB_In_SLR2(0) <= ARM_RS_Out_SLR2;
  AWS_RS_Out_SLR2   <= AWS_XB_In_SLR2(0);
  ARS_RS_Out_SLR2   <= ARS_XB_In_SLR2(0);

  U_XBAR_SLR2 : entity surf.AxiLiteCrossbar
    generic map (
      TPD_G              => TPD_G,
      NUM_SLAVE_SLOTS_G  => 1,
      NUM_MASTER_SLOTS_G => SLR2_XBAR_MASTERS,
      MASTERS_CONFIG_G   => XBAR_CONFIG_SLR2_C)
    port map (
      axiClk           => axiClk,
      axiClkRst        => axiRstSec(2),
      sAxiWriteMasters => AWM_XB_In_SLR2,
      sAxiWriteSlaves  => AWS_XB_In_SLR2,
      sAxiReadMasters  => ARM_XB_In_SLR2,
      sAxiReadSlaves   => ARS_XB_In_SLR2,
      mAxiWriteMasters => AWM_XB_Out_SLR2,
      mAxiWriteSlaves  => AWS_XB_Out_SLR2,
      mAxiReadMasters  => ARM_XB_Out_SLR2,
      mAxiReadSlaves   => ARS_XB_Out_SLR2);

  --------------------------
  -- SLR 3
  --------------------------

  AWM_RS_In_SLR3     <= AWM_XB_Out_SLR2(1);
  ARM_RS_In_SLR3     <= ARM_XB_Out_SLR2(1);
  AWS_XB_Out_SLR2(1) <= AWS_RS_In_SLR3;
  ARS_XB_Out_SLR2(1) <= ARS_RS_In_SLR3;

  U_Axi_RegSlice_SLR3 : axilRegSliceWrapper
    generic map (
      REG_SLICE_TYPE_G => "LIGHT"
      )
    port map(
      axiClk          => axiClk,
      axiRst          => axiRstSec(3),
      sAxiWriteMaster => AWM_RS_In_SLR3,
      sAxiWriteSlave  => AWS_RS_In_SLR3,
      sAxiReadMaster  => ARM_RS_In_SLR3,
      sAxiReadSlave   => ARS_RS_In_SLR3,
      mAxiWriteMaster => AWM_RS_Out_SLR3,
      mAxiWriteSlave  => AWS_RS_Out_SLR3,
      mAxiReadMaster  => ARM_RS_Out_SLR3,
      mAxiReadSlave   => ARS_RS_Out_SLR3);

------------------------
-- Outputs assignments
------------------------

-- SLR0 output assignments
  mAxiWriteMasters(0) <= AWM_XB_Out_SLR0(0);
  mAxiReadMasters(0)  <= ARM_XB_Out_SLR0(0);
  AWS_XB_Out_SLR0(0)  <= mAxiWriteSlaves(0);
  ARS_XB_Out_SLR0(0)  <= mAxiReadSlaves(0);

-- SLR1 output assignments
  mAxiWriteMasters(1) <= AWM_XB_Out_SLR1(0);
  mAxiReadMasters(1)  <= ARM_XB_Out_SLR1(0);
  AWS_XB_Out_SLR1(0)  <= mAxiWriteSlaves(1);
  ARS_XB_Out_SLR1(0)  <= mAxiReadSlaves(1);

-- SLR2 output assignments
  mAxiWriteMasters(2) <= AWM_XB_Out_SLR2(0);
  mAxiReadMasters(2)  <= ARM_XB_Out_SLR2(0);
  AWS_XB_Out_SLR2(0)  <= mAxiWriteSlaves(2);
  ARS_XB_Out_SLR2(0)  <= mAxiReadSlaves(2);

  -- SLR3 output assignments
  mAxiWriteMasters(3) <= AWM_RS_Out_SLR3;
  mAxiReadMasters(3)  <= ARM_RS_Out_SLR3;
  AWS_RS_Out_SLR3     <= mAxiWriteSlaves(3);
  ARS_RS_Out_SLR3     <= mAxiReadSlaves(3);

end rtl;
