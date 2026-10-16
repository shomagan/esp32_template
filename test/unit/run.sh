#!/bin/bash
set -u
source "$IDF_PATH/export.sh" > /dev/null 2>&1

unit_dir=$(cd "$(dirname "$0")" && pwd)
build_dir=/tmp/regs_app_build

echo "== generator tests"
python -m pytest "$unit_dir/generator"
generator_result=$?

echo "== regs_description tests"
idf.py -C "$unit_dir/regs_app" -B "$build_dir" -DSDKCONFIG="$build_dir/sdkconfig" build > "$build_dir.log" 2>&1
build_result=$?
if [ $build_result -ne 0 ]; then
   tail -n 40 "$build_dir.log"
   c_result=$build_result
else
   "$build_dir/regs_unit_test.elf"
   c_result=$?
fi

echo "== generator: $([ $generator_result -eq 0 ] && echo PASS || echo FAIL), regs_description: $([ $c_result -eq 0 ] && echo PASS || echo FAIL)"
[ $generator_result -eq 0 ] && [ $c_result -eq 0 ]
