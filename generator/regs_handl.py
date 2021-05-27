import re
import base_object
import json

REGS_SIZE = {
    "U8_REGS_FLAG": 1,
    "U16_REGS_FLAG": 2,
    "U32_REGS_FLAG": 4,
    "U64_REGS_FLAG": 8,
    "I8_REGS_FLAG": 1,
    "S16_REGS_FLAG": 2,
    "S32_REGS_FLAG": 4,
    "S64_REGS_FLAG": 8,
    "FLOAT_REGS_FLAG": 4,
    "DOUBLE_REGS_FLAG": 8,
    "INT_REGS_FLAG": 4,
    "STRUCT_REGS_FLAG": 0,
}

REGS_BIT_SIZE = {
    "U8_REGS_FLAG": 8,
    "U16_REGS_FLAG": 16,
    "U32_REGS_FLAG": 32,
    "U64_REGS_FLAG": 64,
    "I8_REGS_FLAG": 8,
    "S16_REGS_FLAG": 16,
    "S32_REGS_FLAG": 32,
    "S64_REGS_FLAG": 64,
    "FLOAT_REGS_FLAG": 32,
    "DOUBLE_REGS_FLAG": 64,
    "INT_REGS_FLAG": 32,
    "STRUCT_REGS_FLAG": 0,
}

REGS_SHORT_NAME = {
    "U8_REGS_FLAG": "U8",
    "U16_REGS_FLAG": "U16",
    "U32_REGS_FLAG": "U32",
    "U64_REGS_FLAG": "U64",
    "I8_REGS_FLAG": "I8",
    "S16_REGS_FLAG": "S16",
    "S32_REGS_FLAG": "S32",
    "S64_REGS_FLAG": "S64",
    "FLOAT_REGS_FLAG": "FLOAT",
    "DOUBLE_REGS_FLAG": "DOUBLE",
    "INT_REGS_FLAG": "INT",
    "STRUCT_REGS_FLAG": "STRUCT",
}
MODBUS_FUNCTIONS = {
    "coils": 1,
    "input_discretes": 2,
    "holding_registers": 3,
    "input_registers": 4,
}

def short_name(name):
    return REGS_SHORT_NAME[name]


class RegsHand(base_object.Base):
    SELF_TYPE_FLAG = 0x01
    RO_TYPE_FLAG = 0x02
    SAVE_TYPE_FLAG = 0x04
    USER_VARS_FLAG = 0x08
    CREDENTIAL_FLAG = 0x10
    CRITICAL_FLAG = 0x20
    MIN_VALUE_FLAG = 0x40
    MAX_VALUE_FLAG = 0x80

    GENERATOR_MARKER = "#generator_use_descritption"
    REGS_DESCRIPTION_SETTINGS = MIN_VALUE_FLAG | MAX_VALUE_FLAG

    def __init__(self):
        self.p_default = "NULL"
        self.p_min_value = "NULL"
        self.p_max_value = "NULL"
        self.description = "noname"
        self.internal_name = "noname"
        self.additional_name = "noname"
        self.type = "Kod"
        self.user_describe_rst = ""
        self.ind = 0
        self.guid = 0
        self.byte_number = 0
        self.saved_address = 16
        self.size_array = 0
        self.size_value = 0
        self.reg_opt_is_struct = 0
        self.flag = 0x00
        self.temp_description = "//"
        self.regs_num = 0
        self.is_reduced = 0
        self.dict = []
        self.structure_number = 0
        self.modbus_structures_description = []
        self.current_struct_json = {}
        self.modbus_areas = {}

    def regs_file_handling(self, path_to_file, regs_description):
        regs_file = open(path_to_file, 'r', errors='ignore')
        struct_started = 0
        line_number = 0
        for line_full in regs_file:
            line_number += 1
            json_str = self.check_generator_descriptions(line_full)
            if len(json_str):
                json_description = json.loads(json_str)
                self.current_struct_json = json_description
                if self.check_is_modbus_regs_description_structure(json_description):
                    struct_type = self.find_struct_type(path_to_file, line_number)
                    regs_global_name = find_global_register(path_to_file, struct_type)
                    if len(struct_type) and len(regs_global_name):
                        json_description["struct_type"] = struct_type
                        json_description["regs_global_name"] = regs_global_name
                        self.modbus_structures_description.append(json_description)
                        self.structure_number += 1
                        struct_started = 1
                        self.byte_number = 0
                        print("find global struct")
                        continue
                    else:
                        self.print_error("error struct type {} or reg_global_name {}"
                                         "".format(struct_type, regs_global_name))
            if '//' in line_full:
                start = line_full.find('//')
                line = line_full[:start]
            else:
                line = line_full
            if struct_started and '}' in line:
                struct_started = 0
            if struct_started:
                self.check(line_full)
                if type(self.size_array) == str:
                    regs_file_temp = open(path_to_file, 'r', errors='ignore')
                    self.size_array = self.find_define(self.size_array, regs_file_temp)
                    regs_file_temp.close()
                if self.size_array:
                    if self.reg_opt_is_struct:
                        struct_num = self.size_array
                        print('struct_num', struct_num)
                        description = self.description
                        for i in range(struct_num):
                            self.description = description + str(i)
                            self.additional_name = self.internal_name + str(i)
                            self.size_array = self.size_value
                            self.user_describe_rst += self.add_variable_desc_rst()
                            self.add_variable_to_regs_description(regs_description, i)
                            self.guid += self.size_value * REGS_SIZE[self.type]
                            self.byte_number += self.size_value * REGS_SIZE[self.type]
                            if self.flag & self.SAVE_TYPE_FLAG:
                                self.saved_address += self.size_value * REGS_SIZE[self.type]
                    else:
                        self.user_describe_rst += self.add_variable_desc_rst()
                        self.add_variable_to_regs_description(regs_description, 0)
                        self.guid += self.size_array * REGS_SIZE[self.type]
                        self.byte_number += self.size_array * REGS_SIZE[self.type]
                        if self.flag & self.SAVE_TYPE_FLAG:
                            self.saved_address += self.size_array * REGS_SIZE[self.type]
        regs_file.close()

    def check(self, line):
        self.size_array = 1
        self.reg_opt_is_struct = 0
        self.flag = self.SELF_TYPE_FLAG
        self.is_reduced = 0
        w = re.compile(
            '^\s*(?P<type>[\w\d]+)\s+(?P<name>[\w\d]+)\s*(\[(?P<size>[\d\w]+)\])?\s*;\s*(?P<descript>\/\/[\w\W]*$)*',
            re.ASCII)
        if w.match(line):
            l = w.search(line)
            self.description = l.group('name')
            self.internal_name = l.group('name')
            if l.group('descript'):
                self.temp_description = l.group('descript')
                n_t = re.compile('\"[\s\w\d\-\(\)\[\]\,\!\;\.\>\<\*\:\#\=]+\"')
                name_rs = n_t.search(self.temp_description)
                if name_rs:
                    self.description = name_rs.group(0)[1:-1]
                if "&def" in self.temp_description:
                    self.p_default = "&def_" + self.internal_name
                else:
                    self.p_default = "NULL"
                if "&ro" in self.temp_description:
                    self.flag |= self.RO_TYPE_FLAG & 0xff
                if "&credential" in self.temp_description:
                    self.flag |= self.CREDENTIAL_FLAG & 0xff
                if "&save" in self.temp_description:
                    self.flag |= self.SAVE_TYPE_FLAG & 0xff
                if "&max" in self.temp_description:
                    self.flag |= self.MAX_VALUE_FLAG & 0xff
                    self.p_max_value = "&def_max_" + self.internal_name
                else:
                    self.p_max_value = "NULL"
                if "&min" in self.temp_description:
                    self.flag |= self.MIN_VALUE_FLAG & 0xff
                    self.p_min_value = "&def_min_" + self.internal_name
                else:
                    self.p_min_value = "NULL"
            else:
                self.temp_description = '\n'

            u8_template = re.compile("v?u8|uint8_t|uint8", re.ASCII)
            s8_template = re.compile("v?s8", re.ASCII)
            u16_template = re.compile("v?u16|uint16_t|uint16", re.ASCII)
            s16_template = re.compile("v?s16", re.ASCII)
            u32_template = re.compile("v?u32|uint32_t|uint32", re.ASCII)
            s32_template = re.compile("v?s32", re.ASCII)
            int_template = re.compile("int", re.ASCII)
            u64_template = re.compile("v?u64|uint64_t", re.ASCII)
            s64_template = re.compile("v?s64", re.ASCII)
            float_template = re.compile("Flo32|float", re.ASCII)
            double_template = re.compile("double", re.ASCII)
            time_template = re.compile("time", re.ASCII)
            if u8_template.match(l.group('type')):
                self.type = "U8_REGS_FLAG"
            elif s8_template.match(l.group('type')):
                self.type = "I8_REGS_FLAG"
            elif u16_template.match(l.group('type')):
                self.type = "U16_REGS_FLAG"
            elif s16_template.match(l.group('type')):
                self.type = "S16_REGS_FLAG"
            elif u32_template.match(l.group('type')):
                self.type = "U32_REGS_FLAG"
            elif s32_template.match(l.group('type')):
                self.type = "S32_REGS_FLAG"
            elif int_template.match(l.group('type')):
                self.type = "S32_REGS_FLAG"
            elif u64_template.match(l.group('type')):
                self.type = "U64_REGS_FLAG"
            elif s64_template.match(l.group('type')):
                self.type = "S64_REGS_FLAG"
            elif float_template.match(l.group('type')):
                self.type = "FLOAT_REGS_FLAG"
            elif double_template.match(l.group('type')):
                self.type = "DOUBLE_REGS_FLAG"
            elif time_template.match(l.group('type')):
                self.type = "TIME_REGS_FLAG"
            else:
                '''some special structure type'''
                if re.search("structured_time_t", l.group('type')):
                    self.type = "U8_REGS_FLAG"
                    self.size_array = 10
                    self.is_reduced = 1
                elif re.search("task_info_t", l.group('type')):
                    self.type = "U8_REGS_FLAG"
                    self.size_value = 28
                    self.reg_opt_is_struct = 1
                else:
                    print("invalidate type for variable" + self.internal_name + "description "
                          + self.temp_description)
                    self.type = "INT_REGS_FLAG"
                    self.size_array = 0

            if l.group('size'):
                if l.group('size').isdecimal():
                    self.size_array = int(l.group('size'))
                else:
                    self.size_array = l.group('size')
        else:
            self.size_array = 0

    def add_variable_desc(self):
        mdb_base = self.modbus_structures_description[self.structure_number - 1]["modbus_address"]
        flag_str = ""
        if self.flag & self.RO_TYPE_FLAG:
            flag_str += "READ_ONLY"
        if self.flag & self.SAVE_TYPE_FLAG:
            flag_str += ",SAVED"
        if self.flag & self.CREDENTIAL_FLAG:
            flag_str += ",CREDENTIAL"
        if self.flag & self.CRITICAL_FLAG:
            flag_str += ",CRITICAL"
        if self.flag & self.MIN_VALUE_FLAG:
            flag_str += ",HAVE_MIN_VALUE"
        if self.flag & self.MAX_VALUE_FLAG:
            flag_str += ",HAVE_MAX_VALUE"

        description = self.temp_description.replace("//", " ")
        description = description.replace("\n", " ")
        description = description.replace("!<", " ")
        description = description.replace("&save,", "")
        description = description.replace("&ro,", "")
        description = description.replace("&def,", "")
        description = description.replace("&save", "")
        description = description.replace("&def", "")
        description = description.replace("&credential", "")
        description = description.replace("&crtcl", "")
        modbus_function = MODBUS_FUNCTIONS[self.modbus_structures_description[self.structure_number - 1]["modbus_type"]]
        modbus_address = (mdb_base + self.byte_number // 2) + (modbus_function << 16)
        return "|{}|{}|{}|{}|{}|{}|{}|{}|\n".format(str(self.ind), self.internal_name, short_name(self.type),
                                                    str(self.size_array), str(self.guid), str(hex(modbus_address)), flag_str,
                                                    description)

    def add_variable_desc_rst(self):
        flag_str = ""
        mdb_base = self.modbus_structures_description[self.structure_number - 1]["modbus_address"]
        modbus_address = mdb_base + self.byte_number // 2
        mdb_type = self.modbus_structures_description[self.structure_number - 1]["modbus_type"]
        if self.flag & self.RO_TYPE_FLAG:
            flag_str += "READ_ONLY"
            mdb_description = "{}_{} function-4".format(mdb_type, modbus_address)
        else:
            mdb_description = "{}_{} function-3,4,6,16".format(mdb_type, modbus_address)
        if self.flag & self.SAVE_TYPE_FLAG:
            flag_str += " SAVED"
        if self.flag & self.CREDENTIAL_FLAG:
            flag_str += ",CREDENTIAL"
        if self.flag & self.CRITICAL_FLAG:
            flag_str += ",CRITICAL"
        if self.flag & self.MIN_VALUE_FLAG:
            flag_str += ",HAVE_MIN_VALUE"
        if self.flag & self.MAX_VALUE_FLAG:
            flag_str += ",HAVE_MAX_VALUE"

        description = self.temp_description.replace("//", "")
        description = description.replace("\"", "")
        description = description.replace("\n", "")
        description = description.replace("!<", "")
        description = description.replace("&save,", "")
        description = description.replace("&ro,", "")
        description = description.replace("&def,", "")
        description = description.replace("&save", "")
        description = description.replace("&def", "")
        description = description.replace("&credential", "")
        description = description.replace("&crtcl", "")
        return '    {},\"{}\",\"{}\",{},{},\"{}\",\"{}\",\"{}\"\n'.format(str(self.ind), self.internal_name,
                                                                          short_name(self.type),
                                                                          str(self.size_array), str(self.guid),
                                                                          mdb_description, flag_str,
                                                                          description)

    def add_variable_to_regs_description(self, regs_description_write_file, number):
        mdb_regs_global_name = self.modbus_structures_description[self.structure_number - 1]["regs_global_name"]
        mdb_base = self.modbus_structures_description[self.structure_number - 1]["modbus_address"]
        modbus_function = MODBUS_FUNCTIONS[self.modbus_structures_description[self.structure_number - 1]["modbus_type"]]
        modbus_address = (mdb_base + self.byte_number // 2) + (modbus_function << 16)

        if self.flag & self.SAVE_TYPE_FLAG:
            saved_address = self.saved_address
        else:
            saved_address = 0
        if self.reg_opt_is_struct:
            name = self.additional_name
        else:
            name = self.internal_name
        if self.size_array > 1 and self.is_reduced == 0:
            p_value = "(u8*)&{}.vars.{}[{}]".format(mdb_regs_global_name, self.internal_name, number)
        else:
            p_value = "(u8*)&{}.vars.{}".format(mdb_regs_global_name, self.internal_name)
        regs_description_write_file.writelines('{{ {}, {}, {}, {}, {},\"{}\",\"{}\", {}, {}, {}, {}, {}, {}, {} }},'
                                               '{}'.format(self.p_default, self.p_min_value, self.p_max_value,
                                                           p_value, str(saved_address), self.description
                                                           , name, self.type, str(self.ind),
                                                           str(int(self.guid)), str(hex(modbus_address)),
                                                           str(self.size_array), str(self.flag),
                                                           (self.structure_number - 1), self.temp_description))
        temp_description = {"regs_num": self.regs_num, "p_default": self.p_default, "p_min_value": self.p_min_value,
                            "p_max_value": self.p_min_value, "p_value": p_value,
                            "description": self.description, "internal_name": self.internal_name,
                            "additional_name": self.additional_name, "type": self.type, "ind": self.ind,
                            "guid": self.guid, "saved_address": self.saved_address, "size_array": self.size_array,
                            "size_value": self.size_value, "is_struct": self.reg_opt_is_struct, "flag": self.flag,
                            "modbus_address": modbus_address, "space_name" : self.modbus_structures_description[self.structure_number - 1]["space_name"]}
        self.check_reg_name(temp_description)
        self.update_modbus_area(temp_description, int(mdb_base), modbus_function)
        self.dict.append(temp_description)
        self.regs_num += 1
        self.ind += 1

    def check_reg_name(self, reg_description):
        for reg in self.dict:
            if reg["internal_name"] == reg_description["internal_name"]:
                self.print_error("reg with name - {} already exist, current struct -{}"
                                 "preview struct - {}"
                                 "".format(reg_description["internal_name"],
                                           reg_description["space_name"],
                                           reg["space_name"]))

    def update_modbus_area(self, reg_description, mdb_base, modbus_function):
        modbus_area = {"mdb_base": mdb_base,
                       "modbus_function": modbus_function,
                       "modbus_last": reg_description["modbus_address"] & 0xffff +
                               (reg_description["size_value"] * REGS_SIZE[reg_description["type"]]) // 2}
        if reg_description["space_name"] in self.modbus_areas:
            if self.modbus_areas[reg_description["space_name"]]["mdb_base"] != mdb_base or\
               self.modbus_areas[reg_description["space_name"]]["modbus_function"] != modbus_function or \
               self.modbus_areas[reg_description["space_name"]]["modbus_last"] >= \
                    (reg_description["modbus_address"] & 0xffff + (reg_description["size_value"] *
                                                                   REGS_SIZE[reg_description["type"]])//2):
                self.print_error("area error mdb_base - {}, modbus_function - {}, size"
                                 "".format(mdb_base, modbus_function,
                                           (reg_description["modbus_address"] +
                                            (reg_description["size_value"] * REGS_SIZE[reg_description["type"]])//2)))
        start_address = reg_description["modbus_address"] & 0xffff
        last_address = reg_description["modbus_address"] & 0xffff +\
                       (reg_description["size_value"] * REGS_SIZE[reg_description["type"]]) // 2
        for area in self.modbus_areas:
            if area != reg_description["space_name"]:
                if (self.modbus_areas[area]["mdb_base"] <= start_address < self.modbus_areas[area]["modbus_last"]) or\
                 (self.modbus_areas[area]["mdb_base"] <= last_address < self.modbus_areas[area]["modbus_last"]):
                    self.print_error("modbus address space intersections "
                                     "reg - {} with area - {}".format(reg_description["internal_name"],
                                                                      self.modbus_areas[area]))
        self.modbus_areas[reg_description["space_name"]] = modbus_area

    def __del__(self):
        print("dlt regs hand")

    @staticmethod
    def find_define(name, file):
        for line in file:
            define_temp = re.compile('^\s*\#define\s+' + name + '\s+(?P<value>[\d]+).*', re.ASCII)
            match = define_temp.match(line)
            if match:
                search = define_temp.search(line)
                if search.group('value'):
                    if search.group('value').isdecimal():
                        print('find define ', name)
                        return int(search.group('value'))
                    else:
                        print("warning find not decimal define")
        print("did't find  define", name)
        return 0

    def check_generator_descriptions(self, line):
        if self.GENERATOR_MARKER in line and "{" in line and "}" in line:
            last_str = [pos for pos, char in enumerate(line) if char == "}"]
            json_part = line[line.index("{"):last_str[-1]+1]
            return json_part
        return ""

    def check_is_modbus_regs_description_structure(self, json_structure):
        res = 0
        if "address_space" in json_structure:
            if 100 > json_structure["address_space"] >= 0:
                if "modbus_type" in json_structure:
                    if json_structure["modbus_type"] == "holding_registers" or \
                            json_structure["modbus_type"] == "input_registers" or \
                            json_structure["modbus_type"] == "coil_registers" or \
                            json_structure["modbus_type"] == "discrete_registers":
                        if "modbus_address" in json_structure:
                            if 65535 > json_structure["modbus_address"] >= 0:
                                res = 1
                            else:
                                self.print_error("incorrect address space for structure {}".format(json_structure))
                    else:
                        self.print_error("modbus_type not correct in struct {}".format(json_structure))
            else:
                self.print_error("incorrect address space for structure {}".format(json_structure))
        return res

    def find_struct_type(self, path_to_file, from_line):
        struct_type = ""
        regs_file_temp = open(path_to_file, 'r', errors='ignore')
        lines = regs_file_temp.readlines()
        regs_file_temp.close()
        for line_number in range(from_line, len(lines)):
            json_str = self.check_generator_descriptions(lines[line_number])
            if len(json_str):
                print("find start of struct {} {}".format(json_str, line_number))
                json_description = json.loads(json_str)
                if "message" in json_description:
                    if json_description["message"] == "end_struct":
                        define_temp = re.compile("^\s*}\s*(?P<STRUCT_TYPE>[\w\d]+)\s*;")
                        match = define_temp.match(lines[line_number])
                        if match:
                            search = define_temp.search(lines[line_number])
                            if search.group('STRUCT_TYPE'):
                                struct_type = search.group('STRUCT_TYPE')
                                break
        return struct_type


def find_global_register(path_to_file, struct_type):
    global_register = ""
    regs_file_temp = open(path_to_file, 'r', errors='ignore')
    lines = regs_file_temp.readlines()
    for line_number in range(len(lines)):
        define_temp = re.compile("^\s*extern\s+"+ struct_type + "\s+(?P<GLOBAL_REG>[\w\d]+)\s*;\s*", re.UNICODE)
        match = define_temp.match(lines[line_number])
        if match:
            search = define_temp.search(lines[line_number])
            if search.group('GLOBAL_REG'):
                global_register = search.group('GLOBAL_REG')
                break
    regs_file_temp.close()
    return global_register


