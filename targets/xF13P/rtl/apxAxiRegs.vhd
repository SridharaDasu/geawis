----------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
--use ieee.std_logic_arith.all;
--use ieee.std_logic_unsigned.all;


library surf;
use surf.StdRtlPkg.all;
use surf.AxiLitePkg.all;

entity apxAxiRegs is
   generic (
      TPD_G            : time                  := 1 ns;
      RST_ASYNC_G      : boolean               := false;
      NUM_REG_G  : integer range 1 to 32 := 1
      );
   port (
      -- AXI-Lite Bus
      axiClk         : in  sl;
      axiClkRst      : in  sl;
      axiReadMaster  : in  AxiLiteReadMasterType;
      axiReadSlave   : out AxiLiteReadSlaveType;
      axiWriteMaster : in  AxiLiteWriteMasterType;
      axiWriteSlave  : out AxiLiteWriteSlaveType;
      -- User Read/Write registers
      regs  : out Slv32Array(NUM_REG_G-1 downto 0));
end apxAxiRegs;

architecture rtl of apxAxiRegs is

   subtype regArray is Slv32Array( regs'range );

   type RegType is record
      regs : regArray;
      axiReadSlave  : AxiLiteReadSlaveType;
      axiWriteSlave : AxiLiteWriteSlaveType;
   end record RegType;

   constant REG_INIT_C : RegType := (
      regs => (others => (others => '0')),
      axiReadSlave  => AXI_LITE_READ_SLAVE_INIT_C,
      axiWriteSlave => AXI_LITE_WRITE_SLAVE_INIT_C);

   signal r   : RegType := REG_INIT_C;
   signal rin : RegType;

begin

   comb : process (axiClkRst, axiReadMaster, axiWriteMaster, r) is
      variable v      : RegType;
      variable regCon : AxiLiteEndPointType;
   begin
      -- Latch the current value
      v := r;

      -- Determine the transaction type
      axiSlaveWaitTxn(regCon, axiWriteMaster, axiReadMaster, v.axiWriteSlave, v.axiReadSlave);
      
      -- Map the write registers = [0x100:0x1FF]
      for i in NUM_REG_G-1 downto 0 loop
         axiSlaveRegister(regCon, toSlv((i*4), 9), 0, v.regs(i));
      end loop;

      -- Closeout the transaction
      axiSlaveDefault(regCon, v.axiWriteSlave, v.axiReadSlave, AXI_RESP_DECERR_C);

      -- Synchronous Reset
      if (RST_ASYNC_G = false and axiClkRst = '1') then
         v := REG_INIT_C;
      end if;

      -- Register the variable for next clock cycle
      rin <= v;

      -- Outputs
      axiReadSlave  <= r.axiReadSlave;
      axiWriteSlave <= r.axiWriteSlave;
      regs <= r.regs;

   end process comb;

   seq : process (axiClk, axiClkRst) is
   begin
      if (RST_ASYNC_G and axiClkRst = '1') then
         r <= REG_INIT_C after TPD_G;
      elsif rising_edge(axiClk) then
         r <= rin after TPD_G;
      end if;
   end process seq;

end architecture rtl;
