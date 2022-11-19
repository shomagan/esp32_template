{ &def_mdb_addr, NULL, NULL, (u8*)&client_part_0.vars.mdb_addr, 218,"modbus address","mdb_addr", U16_REGS_FLAG, 75, 418, 0x30000, 1, 5, 0x303 },//!<"modbus address" &save &def
{ NULL, NULL, NULL, (u8*)&client_part_0.vars.ip[0], 220,"device ip address, warning!!! ","ip", U8_REGS_FLAG, 76, 420, 0x30001, 4, 5, 0x303 },//!<"device ip address, warning!!! " &save
{ NULL, NULL, NULL, (u8*)&client_part_0.vars.netmask[0], 224,"netmask address for main wifi net","netmask", U8_REGS_FLAG, 77, 424, 0x30003, 4, 5, 0x303 },//!<"netmask address for main wifi net", &save
{ NULL, NULL, NULL, (u8*)&client_part_0.vars.gate[0], 228,"gateaway address, warning!!! ","gate", U8_REGS_FLAG, 78, 428, 0x30005, 4, 5, 0x303 },//!<"gateaway address, warning!!! " &save
{ NULL, NULL, NULL, (u8*)&client_part_0.vars.slip_ip[0], 232,"ip address for local net","slip_ip", U8_REGS_FLAG, 79, 432, 0x30007, 4, 5, 0x303 },//!<"ip address for local net",&save ,
{ NULL, NULL, NULL, (u8*)&client_part_0.vars.slip_netmask[0], 236,"netmask address for local net","slip_netmask", U8_REGS_FLAG, 80, 436, 0x30009, 4, 5, 0x303 },//!<"netmask address for local net", &save ,
{ NULL, NULL, NULL, (u8*)&client_part_0.vars.slip_gate[0], 240,"gateaway address for local net","slip_gate", U8_REGS_FLAG, 81, 440, 0x3000b, 4, 5, 0x303 },//!<"gateaway address for local net", &save,
