create_pblock root_infra
add_cells_to_pblock [get_pblocks root_infra] [get_cells -quiet [list U_XBAR U_axiInfra U_ApxTcds2 U_APxInfo U_algoCtrl]]
remove_cells_from_pblock root_infra [get_cells [list U_axiInfra/gen_osc_300.U_clk_gen_300/inst/*]]
resize_pblock root_infra -add {CLOCKREGION_X0Y0:CLOCKREGION_X1Y0}

# APx Sectors 0
   create_pblock S_0
   add_cells_to_pblock S_0 [get_cells gen_sector[0].*/*Ctrl*]
   add_cells_to_pblock S_0 [get_cells gen_sector[0].*/U_XBAR]
   add_cells_to_pblock S_0 [get_cells U_ApxSectorChassisWrapper/*/U_XBAR_Sec0]
   resize_pblock [get_pblocks S_0] -add {CLOCKREGION_X0Y0:CLOCKREGION_X0Y3}


# APx Sectors 1
   create_pblock S_1
   add_cells_to_pblock S_1 [get_cells gen_sector[1].*/*Ctrl*]
   add_cells_to_pblock S_1 [get_cells gen_sector[1].*/U_XBAR]
   add_cells_to_pblock S_1 [get_cells U_ApxSectorChassisWrapper/*/U_XBAR_Sec1]

   add_cells_to_pblock S_1 [get_cells [list {U_apxFsTcdsCmdChassisWrapper/genTcdsDistApd1.U_ChassisApd1/tcdsCmdsSec1*}]]
   resize_pblock [get_pblocks S_1] -add {CLOCKREGION_X0Y4:CLOCKREGION_X0Y7}


# APx Sectors 2
   create_pblock S_2
   add_cells_to_pblock S_2 [get_cells gen_sector[2].*/U_XBAR]
   add_cells_to_pblock S_2 [get_cells gen_sector[2].*/*Ctrl*]
   add_cells_to_pblock S_2 [get_cells U_ApxSectorChassisWrapper/*/U_XBAR_Sec2]
   resize_pblock [get_pblocks S_2] -add {CLOCKREGION_X0Y8:CLOCKREGION_X0Y11}

# APx Sectors 3
   create_pblock S_3
   add_cells_to_pblock S_3 [get_cells gen_sector[3].*/*Ctrl*]
   add_cells_to_pblock S_3 [get_cells gen_sector[3].*/U_XBAR]
   resize_pblock [get_pblocks S_3] -add {CLOCKREGION_X0Y12:CLOCKREGION_X0Y15}

# APx Sectors 4
   create_pblock S_4
   add_cells_to_pblock S_4 [get_cells gen_sector[4].*/*Ctrl*]
   add_cells_to_pblock S_4 [get_cells gen_sector[4].*/U_XBAR]
   add_cells_to_pblock S_4 [get_cells U_ApxSectorChassisWrapper/*/U_XBAR_Sec4]
  resize_pblock [get_pblocks S_4] -add {CLOCKREGION_X7Y0:CLOCKREGION_X7Y3}

# APx Sectors 5
  create_pblock S_5
   add_cells_to_pblock S_5 [get_cells gen_sector[5].*/*Ctrl*]
add_cells_to_pblock S_5 [get_cells gen_sector[5].*/U_XBAR]
   add_cells_to_pblock S_5 [get_cells U_ApxSectorChassisWrapper/*/U_XBAR_Sec5]
  resize_pblock [get_pblocks S_5] -add {CLOCKREGION_X7Y4:CLOCKREGION_X7Y7}

# APx Sectors 6
  create_pblock S_6
   add_cells_to_pblock S_6 [get_cells gen_sector[6].*/*Ctrl*]
add_cells_to_pblock S_6 [get_cells gen_sector[6].*/U_XBAR]
   add_cells_to_pblock S_6 [get_cells U_ApxSectorChassisWrapper/*/U_XBAR_Sec6]
  resize_pblock [get_pblocks S_6] -add {CLOCKREGION_X7Y8:CLOCKREGION_X7Y11}
  
  # APx Sectors 7
  create_pblock S_7
   add_cells_to_pblock S_7 [get_cells gen_sector[7].*/*Ctrl*]
add_cells_to_pblock S_7 [get_cells gen_sector[7].*/U_XBAR]
  resize_pblock [get_pblocks S_7] -add {CLOCKREGION_X7Y12:CLOCKREGION_X7Y15}




   create_pblock AxiRs_Sec1
   add_cells_to_pblock -quiet AxiRs_Sec1 [get_cells [list U_ApxSectorChassisWrapper/genxF13PFabric0.U_ChassisxF13PFabric0/U_Axi_RegSlice_Sec1]]
   add_cells_to_pblock -quiet AxiRs_Sec1 [get_cells [list {U_apxFsTcdsCmdChassisWrapper/genTcdsDistApd1.U_ChassisApd1/tcdsCmdsSec1*}]]
   resize_pblock AxiRs_Sec1 -add {SLICE_X0Y240:SLICE_X30Y249 LAGUNA_X0Y240:LAGUNA_X3Y259 RAMB36_X0Y48:RAMB36_X1Y49}
   set_property IS_SOFT 0 [get_pblocks AxiRs_Sec1]

   create_pblock AxiRs_Sec2
   add_cells_to_pblock -quiet AxiRs_Sec2 [get_cells [list U_ApxSectorChassisWrapper/genxF13PFabric0.U_ChassisxF13PFabric0/U_Axi_RegSlice_Sec2]]
   add_cells_to_pblock -quiet AxiRs_Sec2 [get_cells [list {U_apxFsTcdsCmdChassisWrapper/genTcdsDistApd1.U_ChassisApd1/tcdsCmdsSec2*}]]   
resize_pblock AxiRs_Sec2 -add {SLICE_X0Y480:SLICE_X30Y489 LAGUNA_X0Y480:LAGUNA_X3Y499}
   set_property IS_SOFT 0 [get_pblocks AxiRs_Sec2]

   create_pblock AxiRs_Sec3
   add_cells_to_pblock -quiet AxiRs_Sec3 [get_cells [list U_ApxSectorChassisWrapper/genxF13PFabric0.U_ChassisxF13PFabric0/U_Axi_RegSlice_Sec3]]
   add_cells_to_pblock -quiet AxiRs_Sec3 [get_cells [list {U_apxFsTcdsCmdChassisWrapper/genTcdsDistApd1.U_ChassisApd1/tcdsCmdsSec3*}]]   
resize_pblock AxiRs_Sec3 -add {SLICE_X0Y720:SLICE_X30Y729 LAGUNA_X0Y720:LAGUNA_X3Y739}
   set_property IS_SOFT 0 [get_pblocks AxiRs_Sec3]
   
   create_pblock AxiRs_Sec4
   add_cells_to_pblock -quiet AxiRs_Sec4 [get_cells [list U_ApxSectorChassisWrapper/genxF13PFabric0.U_ChassisxF13PFabric0/U_Axi_RegSlice_Sec4]]
   add_cells_to_pblock -quiet AxiRs_Sec4 [get_cells [list {U_apxFsTcdsCmdChassisWrapper/genTcdsDistApd1.U_ChassisApd1/tcdsCmdsSec4*}]]   
resize_pblock  AxiRs_Sec4 -add {SLICE_X206Y0:SLICE_X232Y9 LAGUNA_X28Y0:LAGUNA_X31Y19}
   set_property IS_SOFT 0 [get_pblocks AxiRs_Sec4]
   
   create_pblock AxiRs_Sec5
   add_cells_to_pblock -quiet  AxiRs_Sec5 [get_cells [list U_ApxSectorChassisWrapper/genxF13PFabric0.U_ChassisxF13PFabric0/U_Axi_RegSlice_Sec5]]
   add_cells_to_pblock -quiet  AxiRs_Sec5 [get_cells [list {U_apxFsTcdsCmdChassisWrapper/genTcdsDistApd1.U_ChassisApd1/tcdsCmdsSec5*}]]   
resize_pblock  AxiRs_Sec5 -add {SLICE_X206Y240:SLICE_X232Y249 LAGUNA_X28Y240:LAGUNA_X31Y259}
   set_property IS_SOFT 0 [get_pblocks AxiRs_Sec5]
   
   create_pblock AxiRs_Sec6
   add_cells_to_pblock -quiet  AxiRs_Sec6 [get_cells [list U_ApxSectorChassisWrapper/genxF13PFabric0.U_ChassisxF13PFabric0/U_Axi_RegSlice_Sec6]]
   add_cells_to_pblock -quiet  AxiRs_Sec6 [get_cells [list {U_apxFsTcdsCmdChassisWrapper/genTcdsDistApd1.U_ChassisApd1/tcdsCmdsSec6*}]]   
resize_pblock AxiRs_Sec6 -add {SLICE_X206Y480:SLICE_X232Y489 LAGUNA_X28Y480:LAGUNA_X31Y499}
   set_property IS_SOFT 0 [get_pblocks AxiRs_Sec6]
   
   create_pblock AxiRs_Sec7
   add_cells_to_pblock -quiet  AxiRs_Sec7 [get_cells [list U_ApxSectorChassisWrapper/genxF13PFabric0.U_ChassisxF13PFabric0/U_Axi_RegSlice_Sec7]]
   add_cells_to_pblock -quiet  AxiRs_Sec7 [get_cells [list {U_apxFsTcdsCmdChassisWrapper/genTcdsDistApd1.U_ChassisApd1/tcdsCmdsSec7*}]]   
resize_pblock AxiRs_Sec7 -add {SLICE_X206Y720:SLICE_X232Y729 LAGUNA_X28Y720:LAGUNA_X31Y739}
   set_property IS_SOFT 0 [get_pblocks AxiRs_Sec7]   
   
# -- Sector 0, Subsector 0
create_pblock SS_0_0
add_cells_to_pblock [get_pblocks SS_0_0] [get_cells -quiet {get_cells */*/gen*[0].*}]
add_cells_to_pblock [get_pblocks SS_0_0] [get_cells -quiet {get_cells */*/gen*[1].*}]
add_cells_to_pblock [get_pblocks SS_0_0] [get_cells -quiet {get_cells */*/gen*[2].*}]
add_cells_to_pblock [get_pblocks SS_0_0] [get_cells -quiet {get_cells */*/gen*[3].*}]
resize_pblock SS_0_0 -add {CLOCKREGION_X0Y1:CLOCKREGION_X0Y1}

# -- Sector 0, Subsector 1
create_pblock SS_0_1
add_cells_to_pblock [get_pblocks SS_0_1] [get_cells -quiet {get_cells */*/gen*[4].*}]
add_cells_to_pblock [get_pblocks SS_0_1] [get_cells -quiet {get_cells */*/gen*[5].*}]
add_cells_to_pblock [get_pblocks SS_0_1] [get_cells -quiet {get_cells */*/gen*[6].*}]
add_cells_to_pblock [get_pblocks SS_0_1] [get_cells -quiet {get_cells */*/gen*[7].*}]
resize_pblock SS_0_1 -add {CLOCKREGION_X0Y2:CLOCKREGION_X0Y2}

# -- Sector 0, Subsector 2
create_pblock SS_0_2
add_cells_to_pblock [get_pblocks SS_0_2] [get_cells -quiet {get_cells */*/gen*[8].*}]
add_cells_to_pblock [get_pblocks SS_0_2] [get_cells -quiet {get_cells */*/gen*[9].*}]
add_cells_to_pblock [get_pblocks SS_0_2] [get_cells -quiet {get_cells */*/gen*[10].*}]
add_cells_to_pblock [get_pblocks SS_0_2] [get_cells -quiet {get_cells */*/gen*[11].*}]
resize_pblock SS_0_2 -add {CLOCKREGION_X0Y3:CLOCKREGION_X0Y3}

# -- Sector 1, Subsector 0
create_pblock SS_1_0
add_cells_to_pblock [get_pblocks SS_1_0] [get_cells -quiet {get_cells */*/gen*[12].*}]
add_cells_to_pblock [get_pblocks SS_1_0] [get_cells -quiet {get_cells */*/gen*[13].*}]
add_cells_to_pblock [get_pblocks SS_1_0] [get_cells -quiet {get_cells */*/gen*[14].*}]
add_cells_to_pblock [get_pblocks SS_1_0] [get_cells -quiet {get_cells */*/gen*[15].*}]
resize_pblock SS_1_0 -add {CLOCKREGION_X0Y4:CLOCKREGION_X0Y4}

# -- Sector 1, Subsector 1
create_pblock SS_1_1
add_cells_to_pblock [get_pblocks SS_1_1] [get_cells -quiet {get_cells */*/gen*[16].*}]
add_cells_to_pblock [get_pblocks SS_1_1] [get_cells -quiet {get_cells */*/gen*[17].*}]
add_cells_to_pblock [get_pblocks SS_1_1] [get_cells -quiet {get_cells */*/gen*[18].*}]
add_cells_to_pblock [get_pblocks SS_1_1] [get_cells -quiet {get_cells */*/gen*[19].*}]
resize_pblock SS_1_1 -add {CLOCKREGION_X0Y5:CLOCKREGION_X0Y5}

# -- Sector 1, Subsector 2
create_pblock SS_1_2
add_cells_to_pblock [get_pblocks SS_1_2] [get_cells -quiet {get_cells */*/gen*[20].*}]
add_cells_to_pblock [get_pblocks SS_1_2] [get_cells -quiet {get_cells */*/gen*[21].*}]
add_cells_to_pblock [get_pblocks SS_1_2] [get_cells -quiet {get_cells */*/gen*[22].*}]
add_cells_to_pblock [get_pblocks SS_1_2] [get_cells -quiet {get_cells */*/gen*[23].*}]
resize_pblock SS_1_2 -add {CLOCKREGION_X0Y6:CLOCKREGION_X0Y6}

# -- Sector 1, Subsector 3
create_pblock SS_1_3
add_cells_to_pblock [get_pblocks SS_1_3] [get_cells -quiet {get_cells */*/gen*[24].*}]
add_cells_to_pblock [get_pblocks SS_1_3] [get_cells -quiet {get_cells */*/gen*[25].*}]
add_cells_to_pblock [get_pblocks SS_1_3] [get_cells -quiet {get_cells */*/gen*[26].*}]
add_cells_to_pblock [get_pblocks SS_1_3] [get_cells -quiet {get_cells */*/gen*[27].*}]
resize_pblock SS_1_3 -add {CLOCKREGION_X0Y7:CLOCKREGION_X0Y7}

# -- Sector 2, Subsector 0
create_pblock SS_2_0
add_cells_to_pblock [get_pblocks SS_2_0] [get_cells -quiet {get_cells */*/gen*[28].*}]
add_cells_to_pblock [get_pblocks SS_2_0] [get_cells -quiet {get_cells */*/gen*[29].*}]
add_cells_to_pblock [get_pblocks SS_2_0] [get_cells -quiet {get_cells */*/gen*[30].*}]
add_cells_to_pblock [get_pblocks SS_2_0] [get_cells -quiet {get_cells */*/gen*[31].*}]
resize_pblock SS_2_0 -add {CLOCKREGION_X0Y8:CLOCKREGION_X0Y8}



# -- Sector 2, Subsector 1
create_pblock SS_2_1
add_cells_to_pblock [get_pblocks SS_2_1] [get_cells -quiet {get_cells */*/gen*[32].*}]
add_cells_to_pblock [get_pblocks SS_2_1] [get_cells -quiet {get_cells */*/gen*[33].*}]
add_cells_to_pblock [get_pblocks SS_2_1] [get_cells -quiet {get_cells */*/gen*[34].*}]
add_cells_to_pblock [get_pblocks SS_2_1] [get_cells -quiet {get_cells */*/gen*[35].*}]
resize_pblock SS_2_1 -add {CLOCKREGION_X0Y9:CLOCKREGION_X0Y9}

# -- Sector 2, Subsector 2
create_pblock SS_2_2
add_cells_to_pblock [get_pblocks SS_2_2] [get_cells -quiet {get_cells */*/gen*[36].*}]
add_cells_to_pblock [get_pblocks SS_2_2] [get_cells -quiet {get_cells */*/gen*[37].*}]
add_cells_to_pblock [get_pblocks SS_2_2] [get_cells -quiet {get_cells */*/gen*[38].*}]
add_cells_to_pblock [get_pblocks SS_2_2] [get_cells -quiet {get_cells */*/gen*[39].*}]
resize_pblock SS_2_2 -add {CLOCKREGION_X0Y10:CLOCKREGION_X0Y10}

# -- Sector 2, Subsector 3
create_pblock SS_2_3
add_cells_to_pblock [get_pblocks SS_2_3] [get_cells -quiet {get_cells */*/gen*[40].*}]
add_cells_to_pblock [get_pblocks SS_2_3] [get_cells -quiet {get_cells */*/gen*[41].*}]
add_cells_to_pblock [get_pblocks SS_2_3] [get_cells -quiet {get_cells */*/gen*[42].*}]
add_cells_to_pblock [get_pblocks SS_2_3] [get_cells -quiet {get_cells */*/gen*[43].*}]
resize_pblock SS_2_3 -add {CLOCKREGION_X0Y11:CLOCKREGION_X0Y11}

# -- Sector 3, Subsector 0
create_pblock SS_3_0
add_cells_to_pblock [get_pblocks SS_3_0] [get_cells -quiet {get_cells */*/gen*[44].*}]
add_cells_to_pblock [get_pblocks SS_3_0] [get_cells -quiet {get_cells */*/gen*[45].*}]
add_cells_to_pblock [get_pblocks SS_3_0] [get_cells -quiet {get_cells */*/gen*[46].*}]
add_cells_to_pblock [get_pblocks SS_3_0] [get_cells -quiet {get_cells */*/gen*[47].*}]
resize_pblock [get_pblocks SS_3_0] -add {CLOCKREGION_X0Y12:CLOCKREGION_X0Y12}


# -- Sector 3, Subsector 1
create_pblock SS_3_1
add_cells_to_pblock [get_pblocks SS_3_1] [get_cells -quiet {get_cells */*/gen*[48].*}]
add_cells_to_pblock [get_pblocks SS_3_1] [get_cells -quiet {get_cells */*/gen*[49].*}]
add_cells_to_pblock [get_pblocks SS_3_1] [get_cells -quiet {get_cells */*/gen*[50].*}]
add_cells_to_pblock [get_pblocks SS_3_1] [get_cells -quiet {get_cells */*/gen*[51].*}]
resize_pblock [get_pblocks SS_3_1] -add {CLOCKREGION_X0Y13:CLOCKREGION_X0Y13}



# -- Sector 3, Subsector 2
create_pblock SS_3_2
add_cells_to_pblock [get_pblocks SS_3_2] [get_cells -quiet {get_cells */*/gen*[52].*}]
add_cells_to_pblock [get_pblocks SS_3_2] [get_cells -quiet {get_cells */*/gen*[53].*}]
add_cells_to_pblock [get_pblocks SS_3_2] [get_cells -quiet {get_cells */*/gen*[54].*}]
add_cells_to_pblock [get_pblocks SS_3_2] [get_cells -quiet {get_cells */*/gen*[55].*}]
resize_pblock [get_pblocks SS_3_2] -add {CLOCKREGION_X0Y14:CLOCKREGION_X0Y14}

# -- Sector 3, Subsector 3
create_pblock SS_3_3
add_cells_to_pblock [get_pblocks SS_3_3] [get_cells -quiet {get_cells */*/gen*[56].*}]
add_cells_to_pblock [get_pblocks SS_3_3] [get_cells -quiet {get_cells */*/gen*[57].*}]
add_cells_to_pblock [get_pblocks SS_3_3] [get_cells -quiet {get_cells */*/gen*[58].*}]
add_cells_to_pblock [get_pblocks SS_3_3] [get_cells -quiet {get_cells */*/gen*[59].*}]
resize_pblock [get_pblocks SS_3_3] -add {CLOCKREGION_X0Y15:CLOCKREGION_X0Y15}



# -- Sector 4, Subsector 0
create_pblock SS_4_0
add_cells_to_pblock [get_pblocks SS_4_0] [get_cells -quiet {get_cells */*/gen*[60].*}]
add_cells_to_pblock [get_pblocks SS_4_0] [get_cells -quiet {get_cells */*/gen*[61].*}]
add_cells_to_pblock [get_pblocks SS_4_0] [get_cells -quiet {get_cells */*/gen*[62].*}]
add_cells_to_pblock [get_pblocks SS_4_0] [get_cells -quiet {get_cells */*/gen*[63].*}]
resize_pblock [get_pblocks SS_4_0] -add {CLOCKREGION_X7Y0:CLOCKREGION_X7Y0}


# -- Sector 4, Subsector 1
create_pblock SS_4_1
add_cells_to_pblock [get_pblocks SS_4_1] [get_cells -quiet {get_cells */*/gen*[64].*}]
add_cells_to_pblock [get_pblocks SS_4_1] [get_cells -quiet {get_cells */*/gen*[65].*}]
add_cells_to_pblock [get_pblocks SS_4_1] [get_cells -quiet {get_cells */*/gen*[66].*}]
add_cells_to_pblock [get_pblocks SS_4_1] [get_cells -quiet {get_cells */*/gen*[67].*}]
resize_pblock [get_pblocks SS_4_1] -add {CLOCKREGION_X7Y1:CLOCKREGION_X7Y1}


# -- Sector 4, Subsector 2
create_pblock SS_4_2
add_cells_to_pblock [get_pblocks SS_4_2] [get_cells -quiet {get_cells */*/gen*[68].*}]
add_cells_to_pblock [get_pblocks SS_4_2] [get_cells -quiet {get_cells */*/gen*[69].*}]
add_cells_to_pblock [get_pblocks SS_4_2] [get_cells -quiet {get_cells */*/gen*[70].*}]
add_cells_to_pblock [get_pblocks SS_4_2] [get_cells -quiet {get_cells */*/gen*[71].*}]
resize_pblock [get_pblocks SS_4_2] -add {CLOCKREGION_X7Y2:CLOCKREGION_X7Y2}


# -- Sector 4, Subsector 3
create_pblock SS_4_3
add_cells_to_pblock [get_pblocks SS_4_3] [get_cells -quiet {get_cells */*/gen*[72].*}]
add_cells_to_pblock [get_pblocks SS_4_3] [get_cells -quiet {get_cells */*/gen*[73].*}]
add_cells_to_pblock [get_pblocks SS_4_3] [get_cells -quiet {get_cells */*/gen*[74].*}]
add_cells_to_pblock [get_pblocks SS_4_3] [get_cells -quiet {get_cells */*/gen*[75].*}]
resize_pblock [get_pblocks SS_4_3] -add {CLOCKREGION_X7Y3:CLOCKREGION_X7Y3}



# -- Sector 5, Subsector 0
create_pblock SS_5_0
add_cells_to_pblock [get_pblocks SS_5_0] [get_cells -quiet {get_cells */*/gen*[76].*}]
add_cells_to_pblock [get_pblocks SS_5_0] [get_cells -quiet {get_cells */*/gen*[77].*}]
add_cells_to_pblock [get_pblocks SS_5_0] [get_cells -quiet {get_cells */*/gen*[78].*}]
add_cells_to_pblock [get_pblocks SS_5_0] [get_cells -quiet {get_cells */*/gen*[79].*}]
resize_pblock [get_pblocks SS_5_0] -add {CLOCKREGION_X7Y4:CLOCKREGION_X7Y4}


# -- Sector 5, Subsector 1
create_pblock SS_5_1
add_cells_to_pblock [get_pblocks SS_5_1] [get_cells -quiet {get_cells */*/gen*[80].*}]
add_cells_to_pblock [get_pblocks SS_5_1] [get_cells -quiet {get_cells */*/gen*[81].*}]
add_cells_to_pblock [get_pblocks SS_5_1] [get_cells -quiet {get_cells */*/gen*[82].*}]
add_cells_to_pblock [get_pblocks SS_5_1] [get_cells -quiet {get_cells */*/gen*[83].*}]
resize_pblock [get_pblocks SS_5_1] -add {CLOCKREGION_X7Y5:CLOCKREGION_X7Y5}



# -- Sector 5, Subsector 2
create_pblock SS_5_2
add_cells_to_pblock [get_pblocks SS_5_2] [get_cells -quiet {get_cells */*/gen*[84].*}]
add_cells_to_pblock [get_pblocks SS_5_2] [get_cells -quiet {get_cells */*/gen*[85].*}]
add_cells_to_pblock [get_pblocks SS_5_2] [get_cells -quiet {get_cells */*/gen*[86].*}]
add_cells_to_pblock [get_pblocks SS_5_2] [get_cells -quiet {get_cells */*/gen*[87].*}]
resize_pblock [get_pblocks SS_5_2] -add {CLOCKREGION_X7Y6:CLOCKREGION_X7Y6}



# -- Sector 5, Subsector 3
create_pblock SS_5_3
add_cells_to_pblock [get_pblocks SS_5_3] [get_cells -quiet {get_cells */*/gen*[88].*}]
add_cells_to_pblock [get_pblocks SS_5_3] [get_cells -quiet {get_cells */*/gen*[89].*}]
add_cells_to_pblock [get_pblocks SS_5_3] [get_cells -quiet {get_cells */*/gen*[90].*}]
add_cells_to_pblock [get_pblocks SS_5_3] [get_cells -quiet {get_cells */*/gen*[91].*}]
resize_pblock [get_pblocks SS_5_3] -add {CLOCKREGION_X7Y7:CLOCKREGION_X7Y7}



# -- Sector 6, Subsector 0
create_pblock SS_6_0
add_cells_to_pblock [get_pblocks SS_6_0] [get_cells -quiet {get_cells */*/gen*[92].*}]
add_cells_to_pblock [get_pblocks SS_6_0] [get_cells -quiet {get_cells */*/gen*[93].*}]
add_cells_to_pblock [get_pblocks SS_6_0] [get_cells -quiet {get_cells */*/gen*[94].*}]
add_cells_to_pblock [get_pblocks SS_6_0] [get_cells -quiet {get_cells */*/gen*[95].*}]
resize_pblock [get_pblocks SS_6_0] -add {CLOCKREGION_X7Y8:CLOCKREGION_X7Y8}


# -- Sector 6, Subsector 1
create_pblock SS_6_1
add_cells_to_pblock [get_pblocks SS_6_1] [get_cells -quiet {get_cells */*/gen*[96].*}]
add_cells_to_pblock [get_pblocks SS_6_1] [get_cells -quiet {get_cells */*/gen*[97].*}]
add_cells_to_pblock [get_pblocks SS_6_1] [get_cells -quiet {get_cells */*/gen*[98].*}]
add_cells_to_pblock [get_pblocks SS_6_1] [get_cells -quiet {get_cells */*/gen*[99].*}]
resize_pblock [get_pblocks SS_6_1] -add {CLOCKREGION_X7Y9:CLOCKREGION_X7Y9}



# -- Sector 6, Subsector 2
create_pblock SS_6_2
add_cells_to_pblock [get_pblocks SS_6_2] [get_cells -quiet {get_cells */*/gen*[100].*}]
add_cells_to_pblock [get_pblocks SS_6_2] [get_cells -quiet {get_cells */*/gen*[101].*}]
add_cells_to_pblock [get_pblocks SS_6_2] [get_cells -quiet {get_cells */*/gen*[102].*}]
add_cells_to_pblock [get_pblocks SS_6_2] [get_cells -quiet {get_cells */*/gen*[103].*}]
resize_pblock [get_pblocks SS_6_2] -add {CLOCKREGION_X7Y10:CLOCKREGION_X7Y10}



# -- Sector 6, Subsector 3
create_pblock SS_6_3
add_cells_to_pblock [get_pblocks SS_6_3] [get_cells -quiet {get_cells */*/gen*[104].*}]
add_cells_to_pblock [get_pblocks SS_6_3] [get_cells -quiet {get_cells */*/gen*[105].*}]
add_cells_to_pblock [get_pblocks SS_6_3] [get_cells -quiet {get_cells */*/gen*[106].*}]
add_cells_to_pblock [get_pblocks SS_6_3] [get_cells -quiet {get_cells */*/gen*[107].*}]
resize_pblock [get_pblocks SS_6_3] -add {CLOCKREGION_X7Y11:CLOCKREGION_X7Y11}


# -- Sector 7, Subsector 0
create_pblock SS_7_0
add_cells_to_pblock [get_pblocks SS_7_0] [get_cells -quiet {get_cells */*/gen*[108].*}]
add_cells_to_pblock [get_pblocks SS_7_0] [get_cells -quiet {get_cells */*/gen*[109].*}]
add_cells_to_pblock [get_pblocks SS_7_0] [get_cells -quiet {get_cells */*/gen*[110].*}]
add_cells_to_pblock [get_pblocks SS_7_0] [get_cells -quiet {get_cells */*/gen*[111].*}]
resize_pblock [get_pblocks SS_7_0] -add {CLOCKREGION_X7Y12:CLOCKREGION_X7Y12}


# -- Sector 7, Subsector 1
create_pblock SS_7_1
add_cells_to_pblock [get_pblocks SS_7_1] [get_cells -quiet {get_cells */*/gen*[112].*}]
add_cells_to_pblock [get_pblocks SS_7_1] [get_cells -quiet {get_cells */*/gen*[113].*}]
add_cells_to_pblock [get_pblocks SS_7_1] [get_cells -quiet {get_cells */*/gen*[114].*}]
add_cells_to_pblock [get_pblocks SS_7_1] [get_cells -quiet {get_cells */*/gen*[115].*}]
resize_pblock [get_pblocks SS_7_1] -add {CLOCKREGION_X7Y13:CLOCKREGION_X7Y13}



# -- Sector 7, Subsector 2
create_pblock SS_7_2
add_cells_to_pblock [get_pblocks SS_7_2] [get_cells -quiet {get_cells */*/gen*[116].*}]
add_cells_to_pblock [get_pblocks SS_7_2] [get_cells -quiet {get_cells */*/gen*[117].*}]
add_cells_to_pblock [get_pblocks SS_7_2] [get_cells -quiet {get_cells */*/gen*[118].*}]
add_cells_to_pblock [get_pblocks SS_7_2] [get_cells -quiet {get_cells */*/gen*[119].*}]
resize_pblock [get_pblocks SS_7_2] -add {CLOCKREGION_X7Y14:CLOCKREGION_X7Y14}



# -- Sector 7, Subsector 3
create_pblock SS_7_3
add_cells_to_pblock [get_pblocks SS_7_3] [get_cells -quiet {get_cells */*/gen*[120].*}]
add_cells_to_pblock [get_pblocks SS_7_3] [get_cells -quiet {get_cells */*/gen*[121].*}]
add_cells_to_pblock [get_pblocks SS_7_3] [get_cells -quiet {get_cells */*/gen*[122].*}]
add_cells_to_pblock [get_pblocks SS_7_3] [get_cells -quiet {get_cells */*/gen*[123].*}]
resize_pblock [get_pblocks SS_7_3] -add {CLOCKREGION_X7Y15:CLOCKREGION_X7Y15}


#create_pblock pblock_IP3
#add_cells_to_pblock [get_pblocks pblock_IP3] [get_cells -quiet [list U_algoTopWrapper/ALGO_IP3_SLR0]]
#resize_pblock [get_pblocks pblock_IP3] -add {CLOCKREGION_X4Y0:CLOCKREGION_X5Y3}
#set_property IS_SOFT TRUE [get_pblocks pblock_IP3]



create_pblock pblock_DAQ_left
resize_pblock pblock_DAQ_left -add {SLICE_X31Y60:SLICE_X41Y959 RAMB18_X2Y24:RAMB18_X2Y383 RAMB36_X2Y12:RAMB36_X2Y191 URAM288_X0Y16:URAM288_X0Y255}
set_property IS_SOFT TRUE [get_pblocks pblock_DAQ_left]


create_pblock pblock_DAQ_right
resize_pblock pblock_DAQ_right -add {SLICE_X195Y0:SLICE_X205Y959 URAM288_X4Y0:URAM288_X4Y255}
set_property IS_SOFT TRUE [get_pblocks pblock_DAQ_right]

set_property PROHIBIT TRUE [get_sites {RAMB18_X11Y* RAMB36_X11Y*}]

