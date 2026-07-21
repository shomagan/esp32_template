# Builds and flashes the updater (recovery) app into its own "updater"
# partition via parttool.py.
#
# Do NOT use "idf.py flash" from updater/ for this: ESP-IDF's flash/app-flash
# targets always resolve their offset via --partition-boot-default (see
# tools/cmake/flash_targets.cmake), which is the factory partition's offset
# regardless of which project you built - so a plain "idf.py flash" run from
# updater/ would silently overwrite factory with the recovery app's binary.
# parttool.py writes to a partition by name instead, avoiding that.
#
# Usage: .\flash_updater.ps1 COMx

param(
    [Parameter(Mandatory = $true)]
    [string]$Port
)

$ErrorActionPreference = "Stop"

Push-Location (Join-Path $PSScriptRoot "updater")
try {
    idf.py build
    if ($LASTEXITCODE -ne 0) {
        throw "idf.py build failed with exit code $LASTEXITCODE"
    }

    # Use the IDF-provided interpreter explicitly (parttool.py re-invokes
    # itself via sys.executable to run esptool) - plain "python" can resolve
    # to some other environment on PATH, e.g. a project-local .venv, which
    # won't have esptool installed and fails with "No module named esptool".
    & "$env:IDF_PYTHON_ENV_PATH\Scripts\python.exe" "$env:IDF_PATH\components\partition_table\parttool.py" `
        --port $Port `
        --partition-table-file build\partition_table\partition-table.bin `
        write_partition --partition-name updater --input build\updater.bin
    if ($LASTEXITCODE -ne 0) {
        throw "parttool.py write_partition failed with exit code $LASTEXITCODE"
    }
}
finally {
    Pop-Location
}
