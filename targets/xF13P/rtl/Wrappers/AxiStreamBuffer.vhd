library IEEE;
use IEEE.STD_LOGIC_1164.all;
use ieee.numeric_std.all;

library surf;
use surf.StdRtlPkg.all;
use surf.AxiLitePkg.all;
use surf.AxiStreamPkg.all;

entity AxiStreamBuffer is
GENERIC(
    BUFFER_DEPTH : INTEGER := 24
);
Port ( 
algoClk : IN std_logic;

axiStreamBuffIn  : IN AxiStreamMasterType;
axiStreamBuffOut : OUT AxiStreamMasterType
);
end AxiStreamBuffer;

architecture Behavioral of AxiStreamBuffer is

SIGNAL PipelineBuffer : AxiStreamMasterArray(BUFFER_DEPTH-2 downto 0);

begin

Process(algoClk)
begin
    if(rising_edge(algoClk)) then
        PipelineBuffer   <= PipelineBuffer(PipelineBuffer'HIGH-1 downto PipelineBuffer'LOW) & axiStreamBuffIn;
        axiStreamBuffOut <= PipelineBuffer(PipelineBuffer'HIGH);
    end if;
end process;

end Behavioral;

