import importlib
import os
import shutil
import sys
from pathlib import Path

import pytest

REPO = Path(__file__).resolve().parents[3]


def read_lf(path):
    return Path(path).read_text(encoding="utf-8").replace("\r\n", "\n")


@pytest.fixture
def tree(tmp_path, monkeypatch):
    for name in ("main", "generator", "user_tasks"):
        shutil.copytree(REPO / name, tmp_path / name, ignore=shutil.ignore_patterns("__pycache__", ".idea"))
    monkeypatch.chdir(tmp_path / "generator")
    monkeypatch.syspath_prepend(str(tmp_path / "generator"))
    for module in ("regs_generator", "regs_handl", "base_object"):
        sys.modules.pop(module, None)
    generator = importlib.import_module("regs_generator")
    monkeypatch.setattr(generator, "change_version", lambda path: None)
    return tmp_path, generator
