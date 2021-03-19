import re
import base_object
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


def short_name(name):
    return REGS_SHORT_NAME[name]


class RegsHand(base_object.Base):
    SELF_TYPE = 0x01
    RO_TYPE = 0x02
    SAVE_TYPE = 0x04
    USER_VARS = 0x08
    CREDENTIAL_FLAG = 0x10

    def __init__(self):
        self.p_default = "NULL"
        self.description = "noname"
        self.internal_name = "noname"
        self.additional_name = "noname"
        self.type = "Kod"
        self.ind = 0
        self.guid = 0
        self.saved_address = 16
        self.size_array = 0
        self.size_value = 0
        self.is_struct = 0
        self.flag = 0x00
        self.temp_description = "//"
        self.regs_num = 0
        self.is_reduced = 0
        self.dict = []

    def check(self, line, type_module):
        self.size_array = 1
        self.is_struct = 0
        self.flag = self.SELF_TYPE
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
                n_t = re.compile('\"[\s\w\d\-\(\)\[\]\,\!\;\.\>\<\*\:]+\"')
                name_rs = n_t.search(self.temp_description)
                if name_rs:
                    self.description = name_rs.group(0)[1:-1]
                if "&def" in self.temp_description:
                    self.p_default = "&def_" + self.internal_name
                else:
                    self.p_default = "NULL"
                if "&ro" in self.temp_description:
                    self.flag |= self.RO_TYPE & 0xff
                if "&credential" in self.temp_description:
                    self.flag |= self.CREDENTIAL_FLAG & 0xff
                if "&save" in self.temp_description:
                    self.flag |= self.SAVE_TYPE & 0xff
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
                    self.is_struct = 1
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

    def add_variable(self, type_module):
        flag_str = ""
        if self.flag & self.RO_TYPE:
            flag_str += "READ_ONLY"
        if self.flag & self.SAVE_TYPE:
            flag_str += ",SAVED"
        if self.flag & self.CREDENTIAL_FLAG:
            flag_str += ",CREDENTIAL"

        description = self.temp_description.replace("//", " ")
        description = description.replace("\n", " ")
        description = description.replace("!<", " ")
        description = description.replace("&save,", "")
        description = description.replace("&ro,", "")
        description = description.replace("&def,", "")
        description = description.replace("&save", "")
        description = description.replace("&ro", "")
        description = description.replace("&def", "")
        description = description.replace("&credential", "")
        template = '<!--#' + self.internal_name + '-->'
        modbus_address = self.guid//2
        return "|{}|{}|{}|{}|{}|{}|{}|{}|\n".format(str(self.ind), self.internal_name, short_name(self.type),
                                                    str(self.guid), str(modbus_address), flag_str, description, template)

    def add_variable_desc(self, type_module):
        flag_str = ""
        if self.flag & self.RO_TYPE:
            flag_str += "READ_ONLY"
        if self.flag & self.SAVE_TYPE:
            flag_str += ",SAVED"
        if self.flag & self.CREDENTIAL_FLAG:
            flag_str += ",CREDENTIAL"

        description = self.temp_description.replace("//", " ")
        description = description.replace("\n", " ")
        description = description.replace("!<", " ")
        description = description.replace("&save,", "")
        description = description.replace("&ro,", "")
        description = description.replace("&def,", "")
        description = description.replace("&save", "")
        description = description.replace("&def", "")
        description = description.replace("&credential", "")

        modbus_address = self.guid//2
        return "|{}|{}|{}|{}|{}|{}|{}|{}|\n".format(str(self.ind), self.internal_name, short_name(self.type),
                                                    str(self.size_array), str(self.guid), str(modbus_address), flag_str,
                                                    description)

    def add_variable_desc_rst(self, type_module):
        flag_str = ""
        mdb_dscrptn = ""
        modbus_address = self.guid//2
        if self.flag & self.RO_TYPE:
            flag_str += "READ_ONLY"
            mdb_dscrptn = "input_register_{} function-4".format(modbus_address)
        else:
            mdb_dscrptn = "holding_register_{} function-3,4,6,16".format(modbus_address)
        if self.flag & self.SAVE_TYPE:
            flag_str += " SAVED"
        if self.flag & self.CREDENTIAL_FLAG:
            flag_str += ",CREDENTIAL"

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

        return '    {},\"{}\",\"{}\",{},{},\"{}\",\"{}\",\"{}\"\n'.format(str(self.ind), self.internal_name, short_name(self.type),
                                                    str(self.size_array), str(self.guid), mdb_dscrptn, flag_str,
                                                    description)

    def add_variable_to_regs_description(self, regs_description_write_file, number, type_module):
        if self.flag & self.SAVE_TYPE:
            saved_address = self.saved_address
        else:
            saved_address = 0
        if self.is_struct:
            name = self.additional_name
        else:
            name = self.internal_name

        if self.size_array > 1 and self.is_reduced == 0:
            p_value = "(u8*)&regs_global.vars." + self.internal_name + "[" + str(number) + "]"
        else:
            p_value = "(u8*)&regs_global.vars." + self.internal_name

        regs_description_write_file.writelines('{' + self.p_default + ',' + p_value + ','+str(saved_address) + ',' +
                                               '\"' + self.description + '\"' + ',' + '\"' + name + '\"' + ',' +
                                               self.type + ',' + str(self.ind) + ',' + str(int(self.guid)) + ',' +
                                               str(self.size_array) + ',' + str(self.flag) + '}' + ','
                                               + self.temp_description)
        temp_description = {"regs_num": self.regs_num}
        temp_description["p_default"] = self.p_default
        temp_description["p_value"] = p_value
        temp_description["description"] = self.description
        temp_description["internal_name"] = self.internal_name
        temp_description["additional_name"] = self.additional_name
        temp_description["type"] = self.type
        temp_description["ind"] = self.ind
        temp_description["guid"] = self.guid
        temp_description["saved_address"] = self.saved_address
        temp_description["size_array"] = self.size_array
        temp_description["size_value"] = self.size_value
        temp_description["is_struct"] = self.is_struct
        temp_description["flag"] = self.flag
        temp_description["regs_num"] = self.regs_num
        self.dict.append(temp_description)
        self.regs_num += 1
        self.ind += 1

    def __del__(self):
        print("dlt regs hand")
