import os

import pytest
from pymodbus.client import ModbusTcpClient

from regs_table import load_fw_version, load_main_table

try:
    from config import ip_address as CONFIG_IP
except ImportError:
    CONFIG_IP = None


def pytest_addoption(parser):
    parser.addoption("--device-ip", default=os.environ.get("MODBUS_IP", CONFIG_IP), help="device IP (env MODBUS_IP)")
    parser.addoption("--device-port", type=int, default=int(os.environ.get("MODBUS_PORT", "502")), help="env MODBUS_PORT")
    parser.addoption("--unit-id", type=int, default=int(os.environ.get("MODBUS_UNIT", "14")), help="mdb_addr, env MODBUS_UNIT")


@pytest.fixture(scope="session")
def regs():
    return load_main_table()


@pytest.fixture(scope="session")
def fw_version():
    return load_fw_version()


@pytest.fixture(scope="session")
def unit_id(request):
    return request.config.getoption("--unit-id")


@pytest.fixture(scope="session")
def client(request):
    host = request.config.getoption("--device-ip")
    if not host:
        pytest.exit("device IP not set: use --device-ip or MODBUS_IP", returncode=2)
    client = ModbusTcpClient(host, port=request.config.getoption("--device-port"), timeout=3, retries=1)
    client.connect()
    yield client
    client.close()
