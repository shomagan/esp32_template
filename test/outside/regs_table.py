import re
from dataclasses import dataclass
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
REGS_H = REPO_ROOT / "main" / "regs.h"
REGS_DESCRIPTION_C = REPO_ROOT / "main" / "regs_description.c"
REGS_DESCRIPTION_H = REPO_ROOT / "main" / "regs_description.h"

READ_ONLY = 1 << 1
SAVING = 1 << 2
CREDENTIAL = 1 << 4
CRITICAL = 1 << 5
UNSAFE_TO_WRITE = {"command"}

TYPE_SIZES = {
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
    "TIME_REGS_FLAG": 4,
}

TYPE_FORMATS = {
    "U8_REGS_FLAG": "B",
    "U16_REGS_FLAG": "H",
    "U32_REGS_FLAG": "I",
    "U64_REGS_FLAG": "Q",
    "I8_REGS_FLAG": "b",
    "S16_REGS_FLAG": "h",
    "S32_REGS_FLAG": "i",
    "S64_REGS_FLAG": "q",
    "FLOAT_REGS_FLAG": "f",
    "DOUBLE_REGS_FLAG": "d",
    "INT_REGS_FLAG": "i",
    "TIME_REGS_FLAG": "I",
}

ROW_RE = re.compile(
    r'"(?P<name>\w+)",\s*(?P<guid>0x[0-9a-fA-F]+),\s*(?P<mdb>0x[0-9a-fA-F]+),\s*'
    r"(?P<type>\w+),\s*(?P<size>\d+),\s*(?P<property>\d+),\s*(?P<space>\d+)\s*\}"
)


@dataclass(frozen=True)
class Register:
    name: str
    guid: int
    modbus_address: int
    modbus_function: int
    type: str
    size: int
    property: int

    @property
    def read_only(self):
        return bool(self.property & READ_ONLY)

    @property
    def saved(self):
        return bool(self.property & SAVING)

    @property
    def safe_to_write(self):
        return self.name not in UNSAFE_TO_WRITE and not self.property & (SAVING | CREDENTIAL | CRITICAL)

    @property
    def size_in_bytes(self):
        return TYPE_SIZES[self.type] * self.size

    @property
    def word_count(self):
        return (self.size_in_bytes + 1) // 2

    @property
    def struct_format(self):
        return "<" + TYPE_FORMATS[self.type] * self.size


def load_main_table(path=REGS_DESCRIPTION_C):
    text = Path(path).read_text(encoding="utf-8-sig")
    start = re.search(r"regs_description_global\[[^\]]*\]\s*=\s*\{", text).end()
    end = text.index("};", start)
    regs = {}
    for match in ROW_RE.finditer(text[start:end]):
        mdb = int(match["mdb"], 16)
        regs[match["name"]] = Register(
            name=match["name"],
            guid=int(match["guid"], 16),
            modbus_address=mdb & 0xFFFF,
            modbus_function=(mdb >> 16) & 0xF,
            type=match["type"],
            size=int(match["size"]),
            property=int(match["property"]),
        )
    return regs


def load_fw_version(path=REGS_H):
    text = Path(path).read_text(encoding="utf-8-sig")
    match = re.search(r"#define\s+FW_VERSION\s+\{([^}]*)\}", text)
    return [int(part) for part in match[1].split(",")]


def load_num_of_main_vars(path=REGS_DESCRIPTION_H):
    text = Path(path).read_text(encoding="utf-8-sig")
    return int(re.search(r"#define\s+NUM_OF_MAIN_VARS\s+(\d+)", text)[1])
