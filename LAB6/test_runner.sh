#!/bin/sh
set -u

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
EXE="$ROOT/executable"
TESTS="test_01 test_02 test_03 test_04 test_05 vector_array_add vector_fir"
PASS=0
TOTAL=0
OUT="/tmp/lab6_test_output.txt"

run_cmd() {
    "$@" >"$OUT" 2>&1
    return $?
}

check_i32() {
    awk -v off="$2" -v want="$3" '
    function hb(h, i, c, d, n) {
        n = 0
        for (i = 1; i <= length(h); i++) {
            c = toupper(substr(h, i, 1))
            d = index("0123456789ABCDEF", c) - 1
            n = n * 16 + d
        }
        return n
    }
    { for (i = 1; i <= NF; i++) b[++n] = hb($i) }
    END {
        p = off + 1
        v = b[p] + b[p+1]*256 + b[p+2]*65536 + b[p+3]*16777216
        if (v >= 2147483648) v -= 4294967296
        exit(v == want ? 0 : 1)
    }' "$1"
}

check_vector_add() {
    awk '
    function hb(h, i, c, d, n) {
        n = 0
        for (i = 1; i <= length(h); i++) {
            c = toupper(substr(h, i, 1)); d = index("0123456789ABCDEF", c) - 1
            n = n * 16 + d
        }
        return n
    }
    function i32(off, p, v) {
        p = off + 1
        v = b[p] + b[p+1]*256 + b[p+2]*65536 + b[p+3]*16777216
        if (v >= 2147483648) v -= 4294967296
        return v
    }
    { for (i = 1; i <= NF; i++) b[++n] = hb($i) }
    END {
        N = i32(0)
        if (N <= 0 || N % 8 != 0) exit 1
        A = 4; B = 4 + 4*N; C = 4 + 8*N
        for (i = 0; i < N; i++) if (i32(C+4*i) != i32(A+4*i) + i32(B+4*i)) exit 1
        exit 0
    }' "$1"
}

check_fir() {
    awk '
    function hb(h, i, c, d, n) {
        n = 0
        for (i = 1; i <= length(h); i++) {
            c = toupper(substr(h, i, 1)); d = index("0123456789ABCDEF", c) - 1
            n = n * 16 + d
        }
        return n
    }
    function i32(off, p, v) {
        p = off + 1
        v = b[p] + b[p+1]*256 + b[p+2]*65536 + b[p+3]*16777216
        if (v >= 2147483648) v -= 4294967296
        return v
    }
    { for (i = 1; i <= NF; i++) b[++n] = hb($i) }
    END {
        N = i32(0)
        if (N < 8) exit 1
        for (k = 0; k < N-8; k++) {
            want = 0
            for (i = 0; i < 8; i++) want += i32(36+4*(k+i)) * i32(4+4*i)
            if (i32(256+4*k) != want) exit 1
        }
        exit 0
    }' "$1"
}

check_result() {
    case "$1" in
        test_01) check_i32 "$2" 16 55 ;;
        test_02) check_i32 "$2" 16 70 && check_i32 "$2" 20 85 ;;
        test_03) check_i32 "$2" 36 37 ;;
        test_04) check_i32 "$2" 24 150 ;;
        test_05) check_i32 "$2" 16 10 && check_i32 "$2" 20 40 && check_i32 "$2" 24 80 ;;
        vector_array_add) check_vector_add "$2" ;;
        vector_fir) check_fir "$2" ;;
        *) return 1 ;;
    esac
}

record() {
    TOTAL=$((TOTAL + 1))
    if [ "$1" -eq 0 ]; then
        PASS=$((PASS + 1))
        printf '[PASS] %s\n' "$2"
    else
        printf '[FAIL] %s\n' "$2"
        tail -80 "$OUT" 2>/dev/null
    fi
}

make clean >"$OUT" 2>&1
record $? "Clean"
make >"$OUT" 2>&1
record $? "Build"

for name in $TESTS; do
    d="$ROOT/tests/$name"
    data="$d/data.byte"
    cp "$data" "$data.lab6_backup"
    run_cmd "$EXE" "$d/program.txt" "$data" <<EOF2
exit
EOF2
    status=$?
    ok=$status
    cmp -s "$d/program.byte" "$d/expected_program.byte" || ok=1
    check_result "$name" "$data" || ok=1
    record "$ok" "$name"
    mv "$data.lab6_backup" "$data"
done

d="$ROOT/tests/lab4_print"
data="$d/data.byte"
cp "$data" "$data.lab6_backup"
rm -f "$ROOT/process.log"
run_cmd "$EXE" "$d/program.txt" "$data" <<EOF2
exit
EOF2
status=$?
ok=$status
[ -f "$ROOT/process.log" ] || ok=1
grep -q "Process id:" "$ROOT/process.log" 2>/dev/null || ok=1
grep -q "x1 : 0000002A" "$ROOT/process.log" 2>/dev/null || ok=1
grep -q "x2 : 000000FF" "$ROOT/process.log" 2>/dev/null || ok=1
record "$ok" "lab4_print"
mv "$data.lab6_backup" "$data"

d="$ROOT/tests/lab4_multiprocess"
tmp="$ROOT/.lab6_mp_tmp"
rm -rf "$tmp"
mkdir -p "$tmp"
rm -f "$ROOT/process.log"
set --
for i in 1 2 3 4 5 6; do
    dst="$tmp/data$i.byte"
    awk 'BEGIN { for (i=1; i<=512; i++) printf "00%s", (i%16==0?"\n":" ") }' > "$dst"
    set -- "$@" "$d/program$i.txt" "$dst"
done
run_cmd "$EXE" "$@" <<EOF2
exit
EOF2
status=$?
ok=$status
for i in 1 2 3 4; do grep -q "Loaded PID $i on processor $((i-1))" "$OUT" || ok=1; done
grep -q "PID 5 placed in WAITING queue" "$OUT" || ok=1
grep -q "PID 6 placed in WAITING queue" "$OUT" || ok=1
grep -q "Dispatched PID 5 from WAITING queue" "$OUT" || ok=1
grep -q "Dispatched PID 6 from WAITING queue" "$OUT" || ok=1
[ -f "$ROOT/process.log" ] || ok=1
for i in 1 2 3 4 5 6; do expected=$(printf '%08X' $((200+i))); grep -q "x1 : $expected" "$ROOT/process.log" 2>/dev/null || ok=1; done
record "$ok" "lab4_multiprocess"
rm -rf "$tmp"

d="$ROOT/tests/lab5_mmu"
data="$d/data.byte"
cp "$data" "$data.lab6_backup"
rm -f "$ROOT/process.log"
run_cmd "$EXE" "$d/program.txt" "$data" <<EOF2
exit
EOF2
status=$?
ok=$status
cmp -s "$d/program.byte" "$d/expected_program.byte" || ok=1
check_i32 "$data" 508 123 || ok=1
check_i32 "$data" 512 200 || ok=1
[ -f "$ROOT/process.log" ] || ok=1
grep -q "x7 : 0000007B" "$ROOT/process.log" 2>/dev/null || ok=1
grep -q "x8 : 000000C8" "$ROOT/process.log" 2>/dev/null || ok=1
record "$ok" "lab5_mmu"
mv "$data.lab6_backup" "$data"

rm -f "$ROOT/program_*.freq" "$ROOT/program*.map" "$ROOT/tests/test_01/program.byte.map"
d="$ROOT/tests/test_01"
data="$d/data.byte"
cp "$data" "$data.lab6_backup"
run_cmd "$EXE" "$d/program.txt" "$data" <<EOF2
exit
EOF2
status=$?
ok=$status
freq=$(find "$ROOT" -maxdepth 1 -type f -name 'program_*.freq' | head -1)
[ -n "$freq" ] || ok=1
if [ -n "$freq" ]; then
    awk 'NF >= 2 && $1 ~ /^[0-9][0-9]*$/ {found=1} END {exit(found ? 0 : 1)}' "$freq" || ok=1
    grep -q 'x5 = x1 + x2' "$freq" || ok=1
    grep -q 'x7 = x5 + x6' "$freq" || ok=1
fi
record "$ok" "lab6_frequency"
mv "$data.lab6_backup" "$data"
rm -f "$ROOT/program_*.freq" "$ROOT/program*.map" "$ROOT/tests/test_01/program.byte.map"

if grep -q 'pageTable\[' "$ROOT/memory.c"; then
    record 1 "lab6_page_table_storage"
elif grep -q 'memory\[PTBR\[proc_id\]' "$ROOT/memory.c" && grep -q 'PTBR\[proc_id\] = proc_id \* NUM_LOGICAL_PAGES' "$ROOT/memory.c"; then
    record 0 "lab6_page_table_storage"
else
    record 1 "lab6_page_table_storage"
fi

rm -f "$ROOT/process.log" "$OUT"
make clean >/dev/null 2>&1
printf '\n%d/%d tests passed\n' "$PASS" "$TOTAL"
if [ "$PASS" -eq "$TOTAL" ]; then
    printf 'ALL TESTS PASSED\n'
    exit 0
fi
printf 'SOME TESTS FAILED\n'
exit 1
