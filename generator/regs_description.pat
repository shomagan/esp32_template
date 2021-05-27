{ &def_mdb_addr, NULL, NULL, (u8*)&regs_global.vars.mdb_addr, 16,"modbus address","mdb_addr", U16_REGS_FLAG, 0, 0, 0x30000, 1, 5, 0 },//!<"modbus address" &save &def
{ &def_ip, NULL, NULL, (u8*)&regs_global.vars.ip[0], 18,"device ip address, warning!!! changes can lead to lost connection","ip", U8_REGS_FLAG, 1, 2, 0x30001, 4, 5, 0 },//!<"device ip address, warning!!! changes can lead to lost connection" &save &def &crtcl
{ &def_netmask, NULL, NULL, (u8*)&regs_global.vars.netmask[0], 22,"netmask address for main wifi net","netmask", U8_REGS_FLAG, 2, 6, 0x30003, 4, 5, 0 },//!<"netmask address for main wifi net", &save , &def , &crtcl
{ &def_gate, NULL, NULL, (u8*)&regs_global.vars.gate[0], 26,"gateaway address, warning!!! changes can lead to lost connection ","gate", U8_REGS_FLAG, 3, 10, 0x30005, 4, 5, 0 },//!<"gateaway address, warning!!! changes can lead to lost connection " &save &def &crtcl
{ &def_slip_ip, NULL, NULL, (u8*)&regs_global.vars.slip_ip[0], 30,"ip address for local net","slip_ip", U8_REGS_FLAG, 4, 14, 0x30007, 4, 5, 0 },//!<"ip address for local net",&save , &def &crtcl
{ &def_slip_netmask, NULL, NULL, (u8*)&regs_global.vars.slip_netmask[0], 34,"netmask address for local net","slip_netmask", U8_REGS_FLAG, 5, 18, 0x30009, 4, 5, 0 },//!<"netmask address for local net", &save , &def , &crtcl
{ &def_slip_gate, NULL, NULL, (u8*)&regs_global.vars.slip_gate[0], 38,"gateaway address for local net","slip_gate", U8_REGS_FLAG, 6, 22, 0x3000b, 4, 5, 0 },//!<"gateaway address for local net", &save, &def &crtcl
{ &def_wifi_name, NULL, NULL, (u8*)&regs_global.vars.wifi_name[0], 42,"must be strong full filled","wifi_name", U8_REGS_FLAG, 7, 26, 0x3000d, 12, 5, 0 },//!<"must be strong full filled", &save &def &crtcl
{ &def_wifi_password, NULL, NULL, (u8*)&regs_global.vars.wifi_password[0], 54,"must be strong 8 byte","wifi_password", U8_REGS_FLAG, 8, 38, 0x30013, 8, 5, 0 },//!<"must be strong 8 byte", &save &def &crtcl
{ &def_wifi_router_name, NULL, NULL, (u8*)&regs_global.vars.wifi_router_name[0], 62,"must be ended by zero","wifi_router_name", U8_REGS_FLAG, 9, 46, 0x30017, 32, 5, 0 },//!<"must be ended by zero", &save &def &crtcl
{ &def_wifi_router_password, NULL, NULL, (u8*)&regs_global.vars.wifi_router_password[0], 94,"must be more or equal 8 byte","wifi_router_password", U8_REGS_FLAG, 10, 78, 0x30027, 32, 5, 0 },//!<"must be more or equal 8 byte", &save &def &crtcl
{ &def_wifi_setting, NULL, NULL, (u8*)&regs_global.vars.wifi_setting, 126,"type of wifi and settings","wifi_setting", U16_REGS_FLAG, 11, 110, 0x30037, 1, 5, 0 },//!<"type of wifi and settings" &save &def &crtcl
{ NULL, NULL, NULL, (u8*)&regs_global.vars.reset_num, 128,"number of reset","reset_num", U32_REGS_FLAG, 12, 112, 0x30038, 1, 5, 0 },//!<"number of reset" &save
{ NULL, NULL, NULL, (u8*)&regs_global.vars.wifi_state, 0,"wifi state","wifi_state", U16_REGS_FLAG, 13, 116, 0x3003a, 1, 3, 0 },//!<"wifi state" &ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.command, 0,"command register","command", U16_REGS_FLAG, 14, 118, 0x3003b, 1, 1, 0 },//!<"command register"
{ &def_uart_debug_sets, NULL, NULL, (u8*)&regs_global.vars.uart_debug_sets, 132,"settings debug uart speed,parity and stop bits, default 115200 ,parity - none, 1 stop bit","uart_debug_sets", U16_REGS_FLAG, 15, 120, 0x3003c, 1, 5, 0 },//!<"settings debug uart speed,parity and stop bits, default 115200 ,parity - none, 1 stop bit" &save &def
{ NULL, NULL, NULL, (u8*)&regs_global.vars.device_state, 0,"BIT(0) - interlock","device_state", U16_REGS_FLAG, 16, 122, 0x3003d, 1, 3, 0 },//!< "BIT(0) - interlock" &ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.unused_3, 0,"unused_3","unused_3", U16_REGS_FLAG, 17, 124, 0x3003e, 1, 3, 0 },//!<"" &ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.sys_tick_counter, 0,"tick in ms","sys_tick_counter", U64_REGS_FLAG, 18, 126, 0x3003f, 1, 3, 0 },//!<"tick in ms",&ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.mac_addr[0], 0,"ethernet mac address","mac_addr", U8_REGS_FLAG, 19, 134, 0x30043, 6, 3, 0 },//!<"ethernet mac address",&ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.flags_succ_init, 0,"success inited modules","flags_succ_init", U32_REGS_FLAG, 20, 140, 0x30046, 1, 3, 0 },//!<"success inited modules" &ro for init in task
{ NULL, NULL, NULL, (u8*)&regs_global.vars.cur_free_heap, 0,"in bytes","cur_free_heap", U32_REGS_FLAG, 21, 144, 0x30048, 1, 3, 0 },//!<"in bytes",&ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.min_free_heap, 0,"in bytes","min_free_heap", U32_REGS_FLAG, 22, 148, 0x3004a, 1, 3, 0 },//!<"in bytes",&ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.debug_info[0], 0,"reserved use for debug","debug_info", U8_REGS_FLAG, 23, 152, 0x3004c, 8, 3, 0 },//!<"reserved use for debug"&ro
{ &def_num_of_vars, NULL, NULL, (u8*)&regs_global.vars.num_of_vars, 0,"num_of_vars","num_of_vars", U16_REGS_FLAG, 24, 160, 0x30050, 1, 3, 0 },//!<"number of vars self + config(user)&ro &def
{ NULL, NULL, NULL, (u8*)&regs_global.vars.temperature_mcu, 0,"temperature mcu Celsius","temperature_mcu", FLOAT_REGS_FLAG, 25, 162, 0x30051, 1, 3, 0 },//!<"temperature mcu Celsius" &ro
{ &def_fw_version, NULL, NULL, (u8*)&regs_global.vars.fw_version[0], 0,"version like 0.1.1.0","fw_version", U8_REGS_FLAG, 26, 166, 0x30053, 4, 3, 0 },//!<"version like 0.1.1.0",&ro,&def
{ &def_board_ver, NULL, NULL, (u8*)&regs_global.vars.board_ver, 0,"board version","board_ver", U16_REGS_FLAG, 27, 170, 0x30055, 1, 3, 0 },//!< "board version", &ro, &def
{ NULL, NULL, NULL, (u8*)&regs_global.vars.firmware_hash, 0,"hash os","firmware_hash", U32_REGS_FLAG, 28, 172, 0x30056, 1, 3, 0 },//!< "hash os" &ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.system_error, 0,"system global error","system_error", U32_REGS_FLAG, 29, 176, 0x30058, 1, 3, 0 },//!< "system global error" &ro
{ &def_permission, NULL, NULL, (u8*)&regs_global.vars.permission, 0,"flags with permissions","permission", U16_REGS_FLAG, 30, 180, 0x3005a, 1, 3, 0 },//!<"flags with permissions" &ro &def
{ NULL, NULL, NULL, (u8*)&regs_global.vars.async_flags, 0,"async flags","async_flags", U32_REGS_FLAG, 31, 182, 0x3005b, 1, 3, 0 },//!< "async flags" &ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.slip_packet_counter, 0,"count all slip packet","slip_packet_counter", U64_REGS_FLAG, 32, 186, 0x3005d, 1, 1, 0 },//!< "count all slip packet"
{ NULL, NULL, NULL, (u8*)&regs_global.vars.ap_connections_number, 0,"number of connections","ap_connections_number", U16_REGS_FLAG, 33, 194, 0x30061, 1, 3, 0 },//!< "number of connections" &ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.sta_connect, 0,"sta connect state","sta_connect", U16_REGS_FLAG, 34, 196, 0x30062, 1, 3, 0 },//!< "sta connect state" &ro
{ &def_table_version, NULL, NULL, (u8*)&regs_global.vars.table_version, 134,"change value in def_table_version for drop all regs to default value","table_version", U32_REGS_FLAG, 35, 198, 0x30063, 1, 7, 0 },//!< "change value in def_table_version for drop all regs to default value" &ro &def &save
{ &def_test_pwm_value, NULL, NULL, (u8*)&regs_global_part1.vars.test_pwm_value, 138,"test pwm value [0;100]","test_pwm_value", FLOAT_REGS_FLAG, 36, 202, 0x303e8, 1, 5, 1 },//!<"test pwm value [0;100]" &def &save
{ &def_servo_0, &def_min_servo_0, &def_max_servo_0, (u8*)&servo_control_part.vars.servo_0, 142,"servo pwm value [0;100]","servo_0", FLOAT_REGS_FLAG, 37, 206, 0x307d0, 1, 197, 2 },//!<"servo pwm value [0;100]" &def &save &min &max
{ &def_servo_1, &def_min_servo_1, &def_max_servo_1, (u8*)&servo_control_part.vars.servo_1, 146,"servo pwm value [0;100]","servo_1", FLOAT_REGS_FLAG, 38, 210, 0x307d2, 1, 197, 2 },//!<"servo pwm value [0;100]" &def &save &min &max
{ &def_servo_2, &def_min_servo_2, &def_max_servo_2, (u8*)&servo_control_part.vars.servo_2, 150,"servo pwm value [0;100]","servo_2", FLOAT_REGS_FLAG, 39, 214, 0x307d4, 1, 197, 2 },//!<"servo pwm value [0;100]" &def &save &min &max
{ &def_servo_3, &def_min_servo_3, &def_max_servo_3, (u8*)&servo_control_part.vars.servo_3, 154,"servo pwm value [0;100]","servo_3", FLOAT_REGS_FLAG, 40, 218, 0x307d6, 1, 197, 2 },//!<"servo pwm value [0;100]" &def &save &min &max
