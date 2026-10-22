#!/bin/sh
# Lab 4 scheduler demonstration retained under Lab 5's shared physical memory.
# Six tasks, four processors, two waiting tasks. Each demo task uses:
#   1 instruction page + 1 data page = 2 frames.
# Frame 0 is reserved, so 15 frames are available.
set -eu
cd "$(dirname "$0")"

tmpdir="$(mktemp -d)"
cleanup() {
    for i in 1 2 3 4 5 6; do
        if [ -f "$tmpdir/data${i}.byte" ]; then
            cp "$tmpdir/data${i}.byte" "tests/lab4_demo/data${i}.byte"
        fi
    done
    rm -rf "$tmpdir"
}
trap cleanup EXIT INT TERM

for i in 1 2 3 4 5 6; do
    cp "tests/lab4_demo/data${i}.byte" "$tmpdir/data${i}.byte"
done

rm -f process.log
OS_DEMO=1 ./executable \
 tests/lab4_demo/program1.txt tests/lab4_demo/data1.byte \
 tests/lab4_demo/program2.txt tests/lab4_demo/data2.byte \
 tests/lab4_demo/program3.txt tests/lab4_demo/data3.byte \
 tests/lab4_demo/program4.txt tests/lab4_demo/data4.byte \
 tests/lab4_demo/program5.txt tests/lab4_demo/data5.byte \
 tests/lab4_demo/program6.txt tests/lab4_demo/data6.byte <<'EOF'
exit
EOF

printf '\n===== process.log (Print instruction output) =====\n'
cat process.log
printf '\n===== Lab 4 scheduler demo complete =====\n'
