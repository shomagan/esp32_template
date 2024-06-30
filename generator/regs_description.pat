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
{ &def_num_of_vars, NULL, NULL, (u8*)&regs_global.vars.num_of_vars, 0,"num_of_vars","num_of_vars", U16_REGS_FLAG, 24, 160, 0x30050, 1, 3, 0 },//!<"number of vars self + config(user) &ro &def
{ &def_client_num_of_vars, NULL, NULL, (u8*)&regs_global.vars.client_num_of_vars, 0,"number of client vars self","client_num_of_vars", U16_REGS_FLAG, 25, 162, 0x30051, 1, 3, 0 },//!<"number of client vars self" &ro &def        
{ NULL, NULL, NULL, (u8*)&regs_global.vars.temperature_mcu, 0,"temperature mcu Celsius","temperature_mcu", FLOAT_REGS_FLAG, 26, 164, 0x30052, 1, 3, 0 },//!<"temperature mcu Celsius" &ro
{ &def_fw_version, NULL, NULL, (u8*)&regs_global.vars.fw_version[0], 0,"version like 0.1.1.0","fw_version", U8_REGS_FLAG, 27, 168, 0x30054, 4, 3, 0 },//!<"version like 0.1.1.0",&ro,&def
{ &def_board_ver, NULL, NULL, (u8*)&regs_global.vars.board_ver, 0,"board version","board_ver", U16_REGS_FLAG, 28, 172, 0x30056, 1, 3, 0 },//!< "board version", &ro, &def
{ NULL, NULL, NULL, (u8*)&regs_global.vars.firmware_hash, 0,"hash os","firmware_hash", U32_REGS_FLAG, 29, 174, 0x30057, 1, 3, 0 },//!< "hash os" &ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.system_error, 0,"system global error","system_error", U32_REGS_FLAG, 30, 178, 0x30059, 1, 3, 0 },//!< "system global error" &ro
{ &def_permission, NULL, NULL, (u8*)&regs_global.vars.permission, 0,"flags with permissions","permission", U16_REGS_FLAG, 31, 182, 0x3005b, 1, 3, 0 },//!<"flags with permissions" &ro &def
{ NULL, NULL, NULL, (u8*)&regs_global.vars.async_flags, 0,"async flags","async_flags", U32_REGS_FLAG, 32, 184, 0x3005c, 1, 3, 0 },//!< "async flags" &ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.slip_packet_counter, 0,"count all slip packet","slip_packet_counter", U64_REGS_FLAG, 33, 188, 0x3005e, 1, 1, 0 },//!< "count all slip packet"
{ NULL, NULL, NULL, (u8*)&regs_global.vars.ap_connections_number, 0,"number of connections","ap_connections_number", U16_REGS_FLAG, 34, 196, 0x30062, 1, 3, 0 },//!< "number of connections" &ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.sta_connect, 0,"sta connect state","sta_connect", U16_REGS_FLAG, 35, 198, 0x30063, 1, 3, 0 },//!< "sta connect state" &ro
{ &def_table_version, NULL, NULL, (u8*)&regs_global.vars.table_version, 134,"change value in def_table_version for drop all regs to default value","table_version", U32_REGS_FLAG, 36, 200, 0x30064, 1, 7, 0 },//!< "change value in def_table_version for drop all regs to default value" &ro &def &save
{ NULL, NULL, NULL, (u8*)&regs_global.vars.iirls_pairing_status, 0,"status","iirls_pairing_status", U16_REGS_FLAG, 37, 204, 0x30066, 1, 3, 0 },//!< "status" &ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.iirls_paired_ip[0], 0,"ip address of paired device","iirls_paired_ip", U8_REGS_FLAG, 38, 206, 0x30067, 4, 3, 0 },//!< "ip address of paired device" &ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.modbus_master_test_value, 0,"status","modbus_master_test_value", U16_REGS_FLAG, 39, 210, 0x30069, 1, 3, 0 },//!< "status" &ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.iirls_state, 138,"state","iirls_state", U16_REGS_FLAG, 40, 212, 0x3006a, 1, 5, 0 },//!< "state" &save
{ NULL, NULL, NULL, (u8*)&regs_global.vars.paired_device_model_number[0], 0,"state","paired_device_model_number", U16_REGS_FLAG, 41, 214, 0x3006b, 40, 3, 0 },//!< "state" &ro
{ &def_slave_modbus_address, &def_min_slave_modbus_address, &def_max_slave_modbus_address, (u8*)&regs_global.vars.slave_modbus_address, 140,"slave modbus address","slave_modbus_address", U16_REGS_FLAG, 42, 294, 0x30093, 1, 197, 0 },//!< "slave modbus address" &def &save &max &min
{ NULL, NULL, NULL, (u8*)&regs_global.vars.modbus_master_comm_period_current, 0,"modbus_master_comm_period_current","modbus_master_comm_period_current", U32_REGS_FLAG, 43, 296, 0x30094, 1, 1, 0 },//!< ""
{ NULL, NULL, NULL, (u8*)&regs_global.vars.modbus_master_comm_period_max, 0,"modbus_master_comm_period_max","modbus_master_comm_period_max", U32_REGS_FLAG, 44, 300, 0x30096, 1, 1, 0 },//!< ""
{ NULL, NULL, NULL, (u8*)&regs_global.vars.modbus_master_comm_period_min, 0,"modbus_master_comm_period_min","modbus_master_comm_period_min", U32_REGS_FLAG, 45, 304, 0x30098, 1, 1, 0 },//!< ""
{ NULL, NULL, NULL, (u8*)&regs_global.vars.modbus_master_succ_transactions_number, 0,"modbus_master_succ_transactions_number","modbus_master_succ_transactions_number", U32_REGS_FLAG, 46, 308, 0x3009a, 1, 1, 0 },//!< ""
{ NULL, NULL, NULL, (u8*)&regs_global.vars.modbus_master_error_transactions_number, 0,"modbus_master_error_transactions_number","modbus_master_error_transactions_number", U32_REGS_FLAG, 47, 312, 0x3009c, 1, 1, 0 },//!< ""
{ NULL, NULL, NULL, (u8*)&regs_global.vars.i2c_display_address, 0,"address of display","i2c_display_address", U16_REGS_FLAG, 48, 316, 0x3009e, 1, 3, 0 },//!< "address of display" &ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.sta_ip[0], 142,"ip address of sta","sta_ip", U8_REGS_FLAG, 49, 318, 0x3009f, 4, 7, 0 },//!< "ip address of sta" &ro &save
{ NULL, NULL, NULL, (u8*)&regs_global.vars.live_time, 0,"live time in seconds","live_time", U32_REGS_FLAG, 50, 322, 0x300a1, 1, 3, 0 },//!< "live time in seconds" &ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.unix_time, 0,"unix time , not implemented","unix_time", S64_REGS_FLAG, 51, 326, 0x300a3, 1, 1, 0 },//!< "unix time , not implemented" 
{ NULL, NULL, NULL, (u8*)&regs_global.vars.seconds_of_the_day, 0,"seconds of the day","seconds_of_the_day", U32_REGS_FLAG, 52, 334, 0x300a7, 1, 1, 0 },//!< "seconds of the day" 
{ NULL, NULL, NULL, (u8*)&regs_global.vars.flash_write_number, 146,"increments every flash write by an app","flash_write_number", U32_REGS_FLAG, 53, 338, 0x300a9, 1, 7, 0 },//!< "increments every flash write by an app" &ro &save
{ NULL, NULL, NULL, (u8*)&regs_global.vars.current_state[0], 0,"current state of proccess","current_state", U32_REGS_FLAG, 54, 342, 0x300ab, 4, 3, 0 },//!< "current state of proccess" &ro
{ &def_sleep_time, &def_min_sleep_time, NULL, (u8*)&regs_global.vars.sleep_time, 0,"seconds,couple with SLEEP_TASK_DEEP_SLEEP_FOR_N_SEC","sleep_time", U16_REGS_FLAG, 55, 358, 0x300b3, 1, 65, 0 },//!< "seconds,couple with SLEEP_TASK_DEEP_SLEEP_FOR_N_SEC"  &def &min
{ NULL, NULL, NULL, (u8*)&regs_global.vars.rssi_ap[0], 0,"wifi rssi of Access point","rssi_ap", I8_REGS_FLAG, 56, 360, 0x300b4, 2, 3, 0 },//!< "wifi rssi of Access point" &ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.primary_channel_ap[0], 0,"wifi channel used","primary_channel_ap", I8_REGS_FLAG, 57, 362, 0x300b5, 2, 3, 0 },//!< "wifi channel used" &ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.wake_up_cause, 0,"wake up reason","wake_up_cause", S32_REGS_FLAG, 58, 364, 0x300b6, 1, 3, 0 },//!< "wake up reason" &ro
{ &def_test_pwm_value, &def_min_test_pwm_value, &def_max_test_pwm_value, (u8*)&regs_global_part1.vars.test_pwm_value, 150,"test pwm value [0;100]","test_pwm_value", FLOAT_REGS_FLAG, 59, 368, 0x303e8, 1, 197, 1 },//!<"test pwm value [0;100]" &def &save &min &max
{ &def_test_pwm_value, &def_min_test_pwm_value, &def_max_test_pwm_value, (u8*)&regs_global_part1.vars.touch_0, 0,"touch_0","touch_0", U16_REGS_FLAG, 60, 372, 0x303ea, 1, 1, 1 },
{ &def_test_pwm_value, &def_min_test_pwm_value, &def_max_test_pwm_value, (u8*)&regs_global_part1.vars.touch_1, 0,"touch_1","touch_1", U16_REGS_FLAG, 61, 374, 0x303eb, 1, 1, 1 },
{ &def_test_pwm_value, &def_min_test_pwm_value, &def_max_test_pwm_value, (u8*)&regs_global_part1.vars.touch_2, 0,"touch_2","touch_2", U16_REGS_FLAG, 62, 376, 0x303ec, 1, 1, 1 },
{ &def_test_pwm_value, &def_min_test_pwm_value, &def_max_test_pwm_value, (u8*)&regs_global_part1.vars.touch_3, 0,"touch_3","touch_3", U16_REGS_FLAG, 63, 378, 0x303ed, 1, 1, 1 },
{ NULL, NULL, NULL, (u8*)&regs_global_part1.vars.water_counter, 154,"di counter","water_counter", U64_REGS_FLAG, 64, 380, 0x303ee, 1, 7, 1 },//!< "di counter" &save &ro
{ &def_impulse_cost, NULL, NULL, (u8*)&regs_global_part1.vars.impulse_cost, 162,"cost of di impulse in liters","impulse_cost", FLOAT_REGS_FLAG, 65, 388, 0x303f2, 1, 5, 1 },//!< "cost of di impulse in liters" &save &def
{ NULL, NULL, NULL, (u8*)&regs_global_part1.vars.liters, 166,"liters calculated","liters", FLOAT_REGS_FLAG, 66, 392, 0x303f4, 1, 5, 1 },//!< "liters calculated" &save
{ &def_touch_1_count, NULL, NULL, (u8*)&regs_global_part1.vars.touch_1_count, 170,"ms ","touch_1_count", U32_REGS_FLAG, 67, 396, 0x303f6, 1, 5, 1 },//!< "ms " &save &def
{ &def_touch_2_count, NULL, NULL, (u8*)&regs_global_part1.vars.touch_2_count, 174,"ms ","touch_2_count", U32_REGS_FLAG, 68, 400, 0x303f8, 1, 5, 1 },//!< "ms " &save &def
{ &def_touch_3_count, NULL, NULL, (u8*)&regs_global_part1.vars.touch_3_count, 178,"ms ","touch_3_count", U32_REGS_FLAG, 69, 404, 0x303fa, 1, 5, 1 },//!< "ms " &save &def
{ &def_touch_1_liters, NULL, NULL, (u8*)&regs_global_part1.vars.touch_1_liters, 182,"ms ","touch_1_liters", FLOAT_REGS_FLAG, 70, 408, 0x303fc, 1, 5, 1 },//!< "ms " &save &def
{ &def_touch_2_liters, NULL, NULL, (u8*)&regs_global_part1.vars.touch_2_liters, 186,"ms ","touch_2_liters", FLOAT_REGS_FLAG, 71, 412, 0x303fe, 1, 5, 1 },//!< "ms " &save &def
{ &def_touch_3_liters, NULL, NULL, (u8*)&regs_global_part1.vars.touch_3_liters, 190,"ms ","touch_3_liters", FLOAT_REGS_FLAG, 72, 416, 0x30400, 1, 5, 1 },//!< "ms " &save &def
{ &def_touch_0_trshld, NULL, NULL, (u8*)&regs_global_part1.vars.touch_0_trshld, 194,"in percents","touch_0_trshld", FLOAT_REGS_FLAG, 73, 420, 0x30402, 1, 5, 1 },//!< "in percents" &save &def
{ &def_touch_1_trshld, NULL, NULL, (u8*)&regs_global_part1.vars.touch_1_trshld, 198,"in percents","touch_1_trshld", FLOAT_REGS_FLAG, 74, 424, 0x30404, 1, 5, 1 },//!< "in percents" &save &def
{ &def_touch_2_trshld, NULL, NULL, (u8*)&regs_global_part1.vars.touch_2_trshld, 202,"in percents","touch_2_trshld", FLOAT_REGS_FLAG, 75, 428, 0x30406, 1, 5, 1 },//!< "in percents" &save &def
{ &def_touch_3_trshld, NULL, NULL, (u8*)&regs_global_part1.vars.touch_3_trshld, 206,"in percents","touch_3_trshld", FLOAT_REGS_FLAG, 76, 432, 0x30408, 1, 5, 1 },//!< "in percents" &save &def
{ NULL, NULL, NULL, (u8*)&regs_global_part1.vars.filter_use, 210,"use inside filter fot touch sensors or not ","filter_use", U32_REGS_FLAG, 77, 436, 0x3040a, 1, 5, 1 },//!< "use inside filter fot touch sensors or not " &save
{ &def_touch_handle_period, NULL, NULL, (u8*)&regs_global_part1.vars.touch_handle_period, 214,"in ms period of handle touchs","touch_handle_period", U32_REGS_FLAG, 78, 440, 0x3040c, 1, 5, 1 },//!< "in ms period of handle touchs" &save &def
{ &def_by_time, NULL, NULL, (u8*)&regs_global_part1.vars.by_time, 218,"by time or counter","by_time", U32_REGS_FLAG, 79, 444, 0x3040e, 1, 5, 1 },//!< "by time or counter" &save &def
{ &def_servo_0, &def_min_servo_0, &def_max_servo_0, (u8*)&servo_control_part.vars.servo_0, 0,"servo pwm value [0;100]","servo_0", FLOAT_REGS_FLAG, 80, 448, 0x307d0, 1, 193, 2 },//!<"servo pwm value [0;100]" &def &min &max
{ &def_servo_1, &def_min_servo_1, &def_max_servo_1, (u8*)&servo_control_part.vars.servo_1, 0,"servo pwm value [0;100]","servo_1", FLOAT_REGS_FLAG, 81, 452, 0x307d2, 1, 193, 2 },//!<"servo pwm value [0;100]" &def &min &max
{ &def_servo_2, &def_min_servo_2, &def_max_servo_2, (u8*)&servo_control_part.vars.servo_2, 0,"servo pwm value [0;100]","servo_2", FLOAT_REGS_FLAG, 82, 456, 0x307d4, 1, 193, 2 },//!<"servo pwm value [0;100]" &def &min &max
{ &def_servo_3, &def_min_servo_3, &def_max_servo_3, (u8*)&servo_control_part.vars.servo_3, 0,"servo pwm value [0;100]","servo_3", FLOAT_REGS_FLAG, 83, 460, 0x307d6, 1, 193, 2 },//!<"servo pwm value [0;100]" &def &min &max
{ NULL, NULL, NULL, (u8*)&di_control.vars.pin_state, 0,"current states of digital inputs","pin_state", U32_REGS_FLAG, 84, 464, 0x30bb8, 1, 1, 3 },//!<"current states of digital inputs"
{ NULL, NULL, NULL, (u8*)&sync_time_regs.vars.sync_sys_tick_dev, 0,"deviation between master and slave","sync_sys_tick_dev", S32_REGS_FLAG, 85, 468, 0x30fa0, 1, 3, 4 },//!< "deviation between master and slave" &ro
{ NULL, NULL, NULL, (u8*)&sync_time_regs.vars.sync_sys_tick_slave, 0,"time read from slave","sync_sys_tick_slave", U64_REGS_FLAG, 86, 472, 0x30fa2, 1, 3, 4 },//!< "time read from slave" &ro
{ NULL, NULL, NULL, (u8*)&sync_time_regs.vars.sync_sys_tick_master, 0,"time read from master","sync_sys_tick_master", U64_REGS_FLAG, 87, 480, 0x30fa6, 1, 3, 4 },//!< "time read from master" &ro 
{ NULL, NULL, NULL, (u8*)&sync_time_regs.vars.sync_average_time_ms, 0,"average send receive time ","sync_average_time_ms", U16_REGS_FLAG, 88, 488, 0x30faa, 1, 3, 4 },//!< "average send receive time " &ro
{ NULL, NULL, NULL, (u8*)&sync_time_regs.vars.sync_last_req_time_ms, 0,"last send receive time ","sync_last_req_time_ms", U16_REGS_FLAG, 89, 490, 0x30fab, 1, 3, 4 },//!< "last send receive time " &ro
{ NULL, NULL, NULL, (u8*)&sync_time_regs.vars.sync_active, 0,"activated measurement","sync_active", U16_REGS_FLAG, 90, 492, 0x30fac, 1, 3, 4 },//!< "activated measurement" &ro
{ NULL, NULL, NULL, (u8*)&sr04_reg.vars.lap_state, 0,"state sr04, bit0 - activated, bit1 - echo signal received","lap_state", U16_REGS_FLAG, 91, 494, 0x31004, 1, 3, 5 },//!< "state sr04, bit0 - activated, bit1 - echo signal received" &ro
{ NULL, NULL, NULL, (u8*)&sr04_reg.vars.lap_distance, 0,"current distance","lap_distance", FLOAT_REGS_FLAG, 92, 496, 0x31005, 1, 3, 5 },//!< "current distance" &ro
{ NULL, NULL, NULL, (u8*)&sr04_reg.vars.lap, 0,"when we have sharp change of a distance, save it ","lap", U64_REGS_FLAG, 93, 500, 0x31007, 1, 3, 5 },//!< "when we have sharp change of a distance, save it " &ro 
{ NULL, NULL, NULL, (u8*)&sr04_reg.vars.lap_paired_dev, 0,"lap from paired device","lap_paired_dev", U64_REGS_FLAG, 94, 508, 0x3100b, 1, 3, 5 },//!< "lap from paired device" &ro
{ NULL, NULL, NULL, (u8*)&sr04_reg.vars.distance_filtered, 0,"current distance filterd","distance_filtered", FLOAT_REGS_FLAG, 95, 516, 0x3100f, 1, 3, 5 },//!< "current distance filterd" &ro
{ NULL, NULL, NULL, (u8*)&feeder_reg.vars.feeder_counter, 222,"how many time step motor started","feeder_counter", U32_REGS_FLAG, 96, 520, 0x31068, 1, 7, 6 },//!<"how many time step motor started" &save &ro
{ &def_feeder_interval, &def_min_feeder_interval, NULL, (u8*)&feeder_reg.vars.feeder_interval, 226,"minutes between a feeds","feeder_interval", U16_REGS_FLAG, 97, 524, 0x3106a, 1, 69, 6 },//!< "minutes between a feeds" &save &def &min
{ NULL, NULL, NULL, (u8*)&feeder_reg.vars.feeder_reserv0, 0,"reserved","feeder_reserv0", U16_REGS_FLAG, 98, 526, 0x3106b, 1, 1, 6 },//!< "reserved"
{ &def_feeder_time_sec, &def_min_feeder_time_sec, &def_max_feeder_time_sec, (u8*)&feeder_reg.vars.feeder_time_sec, 228,"turn time is seconds","feeder_time_sec", FLOAT_REGS_FLAG, 99, 528, 0x3106c, 1, 197, 6 },//!< "turn time is seconds" &save &def &min &max
{ NULL, NULL, NULL, (u8*)&polisher_reg.vars.polisher_sec, 232,"how many time in seconds it was active","polisher_sec", U32_REGS_FLAG, 100, 532, 0x310cc, 1, 7, 7 },//!<"how many time in seconds it was active" &save &ro
{ NULL, NULL, NULL, (u8*)&polisher_reg.vars.polisher_last_sec, 0,"how many time in seconds it was active last session","polisher_last_sec", U32_REGS_FLAG, 101, 536, 0x310ce, 1, 3, 7 },//!<"how many time in seconds it was active last session" &ro
{ &def_polisher_speed, NULL, &def_max_polisher_speed, (u8*)&polisher_reg.vars.polisher_speed, 236,"polisher_speed","polisher_speed", U16_REGS_FLAG, 102, 540, 0x310d0, 1, 133, 7 },//!< "polisher_speed" &save &def &max
{ &def_polisher_direction, NULL, &def_max_polisher_direction, (u8*)&polisher_reg.vars.polisher_direction, 238,"polisher_direction","polisher_direction", U16_REGS_FLAG, 103, 542, 0x310d1, 1, 133, 7 },//!< "polisher_direction" &save &def &max 
