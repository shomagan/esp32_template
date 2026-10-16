import re

from conftest import REPO, read_lf

FIXTURE_H = """\
#ifndef FIXTURE_H
#define FIXTURE_H 1
#define FIXTURE_NAME_LEN 6
/** #generator_use_description {"space_name" :"fixture_reg_t",  "address_space" :30, "modbus_type" :"server", "modbus_function" :"holding_registers", "register_start_address" :9000}*/
typedef union{
    struct MCU_PACK{
        u16 plain;            //!<"plain register"
        u16 read_only;        //!<"read only register" &ro
        u32 saved;            //!<"saved with default" &save &def
        u8 name[FIXTURE_NAME_LEN]; //!<"array sized by define" &credential
        u16 limited;          //!<"limited register" &min &max
    }vars;
    u32 bytes[8];
}fixture_reg_t;// #generator_use_description {"message":"end_struct"}
extern fixture_reg_t * const fixture_reg;
#endif
"""

FIXTURE_C = """\
#include "fixture.h"
/*#generator_use_description {"user_task_regs":"start_struct"}*/
/*#generator_use_description {"user_task_regs":"end_struct"}*/
"""

ENTRY = re.compile(
    r'\{ (?P<def>[^,]+), (?P<min>[^,]+), (?P<max>[^,]+), (?P<value>[^,]+), (?P<saved>\d+),'
    r'"(?P<desc>[^"]*)","(?P<name>\w+)", (?P<guid>0x[0-9a-f]+), (?P<mdb>0x[0-9a-f]+), (?P<type>\w+), '
    r'(?P<size>\d+), (?P<prop>\d+), (?P<space>\d+) \}'
)


def generated_entries(c_text):
    return {m.group("name"): m.groupdict() for m in ENTRY.finditer(c_text)}


def test_main_generation_reproduces_committed_files(tree):
    root, generator = tree
    (root / "main" / "http").mkdir(exist_ok=True)
    generator.generate("isimfw400", skip_user_tasks=True)
    for rel in ("main/regs_description.c", "main/regs_description.h", "main/regs.h", "main/regs.c"):
        assert read_lf(root / rel) == read_lf(REPO / rel), rel


def test_user_task_generation_reproduces_committed_files(tree):
    root, generator = tree
    generator.generate_user_tasks()
    for c_file in sorted((REPO / "user_tasks").glob("*.c")):
        rel = f"user_tasks/{c_file.name}"
        assert read_lf(root / rel) == read_lf(c_file), rel


def test_flags_types_and_addresses_in_user_task_table(tree):
    root, generator = tree
    for path in (root / "user_tasks").iterdir():
        path.unlink()
    (root / "user_tasks" / "fixture.h").write_text(FIXTURE_H, encoding="utf-8")
    (root / "user_tasks" / "fixture.c").write_text(FIXTURE_C, encoding="utf-8")

    generator.generate_user_tasks(["fixture.h"])
    text = read_lf(root / "user_tasks" / "fixture.c")
    entries = generated_entries(text)

    assert list(entries) == ["plain", "read_only", "saved", "name", "limited"]
    assert "#define NUM_OF_FIXTURE_REG_VARS 5" in text

    self_flag, read_only, saving, credential, have_min, have_max = 1, 2, 4, 16, 64, 128
    assert int(entries["plain"]["prop"]) == self_flag
    assert int(entries["read_only"]["prop"]) == self_flag | read_only
    assert int(entries["saved"]["prop"]) == self_flag | saving
    assert int(entries["name"]["prop"]) == self_flag | credential
    assert int(entries["limited"]["prop"]) == self_flag | have_min | have_max

    assert entries["saved"]["def"] == "&def_saved"
    assert entries["plain"]["def"] == "NULL"
    assert (entries["limited"]["min"], entries["limited"]["max"]) == ("&def_min_limited", "&def_max_limited")

    assert entries["name"]["type"] == "U8_REGS_FLAG"
    assert entries["name"]["size"] == "6"
    assert entries["saved"]["type"] == "U32_REGS_FLAG"

    holding_registers = 3
    byte_offsets = {"plain": 0, "read_only": 2, "saved": 4, "name": 8, "limited": 14}
    for name, offset in byte_offsets.items():
        assert int(entries[name]["mdb"], 16) == (holding_registers << 16) + 9000 + offset // 2, name
        assert entries[name]["space"] == "30"

    guids = [int(entry["guid"], 16) for entry in entries.values()]
    assert guids == sorted(guids) and len(set(guids)) == len(guids)
