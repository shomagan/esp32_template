import struct
import time

from regs_table import load_num_of_main_vars

ILLEGAL_DATA_ADDRESS = 2
SCRATCH_REGISTER = "table_id"
READ_ONLY_REGISTER = "unused_3"


def read_value(client, unit_id, reg):
    response = client.read_holding_registers(reg.modbus_address, count=reg.word_count, device_id=unit_id)
    assert not response.isError(), f"{reg.name}: {response}"
    raw = b"".join(word.to_bytes(2, "little") for word in response.registers)
    values = struct.unpack(reg.struct_format, raw[: reg.size_in_bytes])
    return values[0] if reg.size == 1 else list(values)


def write_register(client, unit_id, reg, value):
    assert reg.safe_to_write, f"{reg.name} is saved, critical, credential or a command"
    return client.write_register(reg.modbus_address, value, device_id=unit_id)


def write_registers(client, unit_id, reg, values):
    assert reg.safe_to_write, f"{reg.name} is saved, critical, credential or a command"
    return client.write_registers(reg.modbus_address, values, device_id=unit_id)


def test_connect(client):
    assert client.connected


def test_table_matches_register_flags(regs):
    assert len(regs) == load_num_of_main_vars()
    assert regs["mdb_addr"].modbus_address == 0
    assert regs["fw_version"].read_only
    assert not regs[SCRATCH_REGISTER].read_only
    assert regs[SCRATCH_REGISTER].safe_to_write
    assert regs[READ_ONLY_REGISTER].read_only
    assert regs[READ_ONLY_REGISTER].safe_to_write
    assert not regs["command"].safe_to_write


def test_mdb_addr_matches_unit_id(client, unit_id, regs):
    assert read_value(client, unit_id, regs["mdb_addr"]) == unit_id


def test_fw_version_matches_source(client, unit_id, regs, fw_version):
    assert read_value(client, unit_id, regs["fw_version"]) == fw_version


def test_sys_tick_counter_advances(client, unit_id, regs):
    first = read_value(client, unit_id, regs["sys_tick_counter"])
    time.sleep(0.5)
    second = read_value(client, unit_id, regs["sys_tick_counter"])
    assert second > first


def test_write_read_only_rejected(client, unit_id, regs):
    reg = regs[READ_ONLY_REGISTER]
    before = read_value(client, unit_id, reg)
    response = write_register(client, unit_id, reg, (before + 1) & 0xFFFF)
    if not response.isError():
        write_register(client, unit_id, reg, before)
    assert response.isError()
    assert response.exception_code == ILLEGAL_DATA_ADDRESS
    assert read_value(client, unit_id, reg) == before


def test_write_read_back_and_restore(client, unit_id, regs):
    reg = regs[SCRATCH_REGISTER]
    original = read_value(client, unit_id, reg)
    probe = original ^ 0x5A5A
    try:
        response = write_register(client, unit_id, reg, probe)
        assert not response.isError(), str(response)
        assert read_value(client, unit_id, reg) == probe
    finally:
        response = write_registers(client, unit_id, reg, [original])
    assert not response.isError(), str(response)
    assert read_value(client, unit_id, reg) == original
