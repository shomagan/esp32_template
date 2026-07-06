regs_description_t const regs_description_user[NUM_OF_CLIENT_VARS]={
{ NULL, NULL, NULL, (u8*)&regs_main.client_part_0.vars.cli_mdb_addr, 0,"modbus address","cli_mdb_addr", U16_REGS_FLAG, 132, 922, 0x30000, 1, 1, 0xc03 },//!<"modbus address"
{ NULL, NULL, NULL, (u8*)&regs_main.client_part_0.vars.cli_ip[0], 0,"device ip address, warning!!! ","cli_ip", U8_REGS_FLAG, 133, 924, 0x30001, 4, 1, 0xc03 },//!<"device ip address, warning!!! "
{ NULL, NULL, NULL, (u8*)&regs_main.client_part_0.vars.cli_netmask[0], 0,"netmask address for main wifi net","cli_netmask", U8_REGS_FLAG, 134, 928, 0x30003, 4, 1, 0xc03 },//!<"netmask address for main wifi net",
{ NULL, NULL, NULL, (u8*)&regs_main.client_part_0.vars.cli_gate[0], 0,"gateaway address, warning!!! ","cli_gate", U8_REGS_FLAG, 135, 932, 0x30005, 4, 1, 0xc03 },//!<"gateaway address, warning!!! "
{ NULL, NULL, NULL, (u8*)&regs_main.client_part_0.vars.cli_slip_ip[0], 0,"ip address for local net","cli_slip_ip", U8_REGS_FLAG, 136, 936, 0x30007, 4, 1, 0xc03 },//!<"ip address for local net",
{ NULL, NULL, NULL, (u8*)&regs_main.client_part_0.vars.cli_slip_netmask[0], 0,"netmask address for local net","cli_slip_netmask", U8_REGS_FLAG, 137, 940, 0x30009, 4, 1, 0xc03 },//!<"netmask address for local net",
{ NULL, NULL, NULL, (u8*)&regs_main.client_part_0.vars.cli_slip_gate[0], 0,"gateaway address for local net","cli_slip_gate", U8_REGS_FLAG, 138, 944, 0x3000b, 4, 1, 0xc03 },//!<"gateaway address for local net",
{ NULL, NULL, NULL, (u8*)&regs_main.sync_time_client.vars.sys_tick_slave, 0,"time read from slave","sys_tick_slave", U64_REGS_FLAG, 139, 948, 0x3003f, 1, 3, 0xd03 },//!< "time read from slave" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.client_part_1.vars.cli_num_of_vars, 0,"cli_num_of_vars","cli_num_of_vars", U16_REGS_FLAG, 140, 956, 0x30050, 1, 3, 0xe03 },//!<"number of vars self + config(user) &ro
{ NULL, NULL, NULL, (u8*)&regs_main.client_part_1.vars.cli_client_num_of_vars, 0,"number of client vars self","cli_client_num_of_vars", U16_REGS_FLAG, 141, 958, 0x30051, 1, 3, 0xe03 },//!<"number of client vars self" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sync_time_regs_from_client.vars.cli_sys_tick_dev, 0,"deviation between master and slave","cli_sys_tick_dev", S32_REGS_FLAG, 142, 960, 0x30fa0, 1, 3, 0xf03 },//!< "deviation between master and slave" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sync_time_regs_from_client.vars.cli_sys_tick_slave, 0,"time read from slave","cli_sys_tick_slave", U64_REGS_FLAG, 143, 964, 0x30fa2, 1, 3, 0xf03 },//!< "time read from slave" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sync_time_regs_from_client.vars.cli_sys_tick_master, 0,"time read from master","cli_sys_tick_master", U64_REGS_FLAG, 144, 972, 0x30fa6, 1, 3, 0xf03 },//!< "time read from master" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sync_time_regs_from_client.vars.cli_average_time_ms, 0,"average send receive time ","cli_average_time_ms", U16_REGS_FLAG, 145, 980, 0x30faa, 1, 3, 0xf03 },//!< "average send receive time " &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sync_time_regs_from_client.vars.cli_last_req_time_ms, 0,"last send receive time ","cli_last_req_time_ms", U16_REGS_FLAG, 146, 982, 0x30fab, 1, 3, 0xf03 },//!< "last send receive time " &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sync_time_regs_from_client.vars.cli_sync_state, 0,"activated measurement","cli_sync_state", U16_REGS_FLAG, 147, 984, 0x30fac, 1, 3, 0xf03 },//!< "activated measurement" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sr04_reg_client.vars.cli_state, 0,"state sr04, bit0 - activated, bit1 - echo signal received","cli_state", U16_REGS_FLAG, 148, 986, 0x31004, 1, 3, 0x1003 },//!< "state sr04, bit0 - activated, bit1 - echo signal received" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sr04_reg_client.vars.cli_distance, 0,"current distance","cli_distance", FLOAT_REGS_FLAG, 149, 988, 0x31005, 1, 3, 0x1003 },//!< "current distance" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sr04_reg_client.vars.cli_lap, 0,"when we have sharp change of a distance, save it ","cli_lap", U64_REGS_FLAG, 150, 992, 0x31007, 1, 3, 0x1003 },//!< "when we have sharp change of a distance, save it " &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sr04_reg_client.vars.cli_lap_paired_dev, 0,"lap from paired device","cli_lap_paired_dev", U64_REGS_FLAG, 151, 1000, 0x3100b, 1, 3, 0x1003 },//!< "lap from paired device" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sr04_reg_client.vars.cli_distance_filtered, 0,"current distance filtered","cli_distance_filtered", FLOAT_REGS_FLAG, 152, 1008, 0x3100f, 1, 3, 0x1003 },//!< "current distance filtered" &ro
};
