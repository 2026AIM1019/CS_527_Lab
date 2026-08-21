#!/usr/bin/env python3
"""Automatic CS527 Lab 3 test runner.

Checks:
1. The project builds successfully.
2. Each source program compiles and its generated program.byte matches the
   checked-in expected_program.byte.
3. The simulator executes successfully.
4. The resulting data.byte contains the expected 32-bit values.
5. Lab 3 vector tests use the required vector opcodes.

The runner restores every test's original data.byte after execution.
"""

from __future__ import annotations

import shutil
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
EXECUTABLE = ROOT / "executable"


def read_bytes(path: Path) -> bytes:
    tokens = path.read_text().split()
    return bytes(int(t, 16) & 0xFF for t in tokens)


def read_i32(data: bytes, address: int) -> int:
    if address < 0 or address + 4 > len(data):
        raise ValueError(f"address {address} is outside test memory")
    return struct.unpack_from("<i", data, address)[0]


def run(cmd: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        cmd,
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
    )


def check_build() -> bool:
    print("[BUILD] make clean && make")
    clean = run(["make", "clean"])
    if clean.returncode != 0:
        print(clean.stdout)
        return False
    build = run(["make"])
    if build.returncode != 0:
        print(build.stdout)
        return False
    print("[PASS] Build")
    return True


def compare_program_byte(test_dir: Path) -> tuple[bool, str]:
    actual = test_dir / "program.byte"
    expected = test_dir / "expected_program.byte"
    if not actual.exists():
        return False, "program.byte was not generated"
    if not expected.exists():
        return False, "expected_program.byte is missing"
    if actual.read_bytes() != expected.read_bytes():
        return False, "generated program.byte differs from expected_program.byte"
    return True, ""


def scalar_expectations(test_name: str) -> list[tuple[int, int]]:
    return {
        "test_01": [(16, 55)],
        "test_02": [(16, 70), (20, 85)],
        "test_03": [(36, 37)],
        "test_04": [(24, 150)],
        "test_05": [(16, 10), (20, 40), (24, 80)],
    }[test_name]


def check_scalar_result(test_dir: Path) -> tuple[bool, str]:
    data = read_bytes(test_dir / "data.byte")
    for address, expected in scalar_expectations(test_dir.name):
        actual = read_i32(data, address)
        if actual != expected:
            return False, f"Data[{address}] expected {expected}, got {actual}"
    return True, ""


def check_array_result(test_dir: Path) -> tuple[bool, str]:
    data = read_bytes(test_dir / "data.byte")
    n = read_i32(data, 0)
    if n <= 0 or n % 8 != 0:
        return False, f"N={n} is not a positive multiple of 8"

    a_base = 4
    b_base = 4 + 4 * n
    c_base = 4 + 8 * n
    expected = [
        read_i32(data, a_base + 4 * i) + read_i32(data, b_base + 4 * i)
        for i in range(n)
    ]
    actual = [read_i32(data, c_base + 4 * i) for i in range(n)]
    if actual != expected:
        for i, (e, a) in enumerate(zip(expected, actual)):
            if e != a:
                return False, f"C[{i}] expected {e}, got {a}"
        return False, "result array differs from expected"

    opcodes = list(read_bytes(test_dir / "program.byte"))[0::4]
    required = {0x21, 0x25, 0x26}
    missing = required - set(opcodes)
    if missing:
        return False, "missing required vector opcode(s): " + ", ".join(f"0x{x:02X}" for x in sorted(missing))
    return True, ""


def check_fir_result(test_dir: Path) -> tuple[bool, str]:
    data = read_bytes(test_dir / "data.byte")
    n = read_i32(data, 0)
    if n > 64 or n <= 0 or n % 8 != 0:
        return False, f"N={n} violates the assignment constraint (1..64, multiple of 8)"

    weights = [read_i32(data, 4 + 4 * i) for i in range(8)]
    input_base = 36
    inputs = [read_i32(data, input_base + 4 * i) for i in range(n)]
    expected = [
        sum(inputs[k + i] * weights[i] for i in range(8))
        for k in range(n - 8)
    ]
    actual = [read_i32(data, 0x100 + 4 * k) for k in range(n - 8)]

    if actual != expected:
        for i, (e, a) in enumerate(zip(expected, actual)):
            if e != a:
                return False, f"output[{i}] expected {e}, got {a}"
        return False, "FIR output differs from expected"

    opcodes = list(read_bytes(test_dir / "program.byte"))[0::4]
    required = {0x23, 0x25, 0x26}
    missing = required - set(opcodes)
    if missing:
        return False, "missing required vector opcode(s): " + ", ".join(f"0x{x:02X}" for x in sorted(missing))
    return True, ""


def run_test(test_name: str, result_checker) -> bool:
    test_dir = ROOT / "tests" / test_name
    source = test_dir / "program.txt"
    data_file = test_dir / "data.byte"

    original_data = data_file.read_bytes()
    try:
        result = run([str(EXECUTABLE), str(source)])
        if result.returncode != 0:
            print(f"[FAIL] {test_name}: simulator returned {result.returncode}")
            print(result.stdout)
            return False

        ok, msg = compare_program_byte(test_dir)
        if not ok:
            print(f"[FAIL] {test_name}: {msg}")
            return False
        print(f"[PASS] {test_name}: program.byte")

        ok, msg = result_checker(test_dir)
        if not ok:
            print(f"[FAIL] {test_name}: {msg}")
            return False
        print(f"[PASS] {test_name}: data.byte/result")
        return True
    finally:
        data_file.write_bytes(original_data)


def main() -> int:
    if not check_build():
        return 1

    tests = [
        ("test_01", check_scalar_result),
        ("test_02", check_scalar_result),
        ("test_03", check_scalar_result),
        ("test_04", check_scalar_result),
        ("test_05", check_scalar_result),
        ("vector_array_add", check_array_result),
        ("vector_fir", check_fir_result),
    ]

    passed = 0
    print("\nCS527 Lab 3 automatic test suite")
    print("=" * 44)
    for name, checker in tests:
        if run_test(name, checker):
            passed += 1

    print("=" * 44)
    print(f"{passed}/{len(tests)} tests passed")

    if passed == len(tests):
        print("ALL TESTS PASSED")
        return 0

    print("SOME TESTS FAILED")
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
