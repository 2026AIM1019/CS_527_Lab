#!/usr/bin/env python3
import subprocess, struct, shutil, re
from pathlib import Path

ROOT = Path(__file__).resolve().parent
EXE = ROOT / "executable"
TESTS = ["test_01", "test_02", "test_03", "test_04", "test_05", "vector_array_add", "vector_fir"]

def bytes_of(p):
    return bytes(int(x, 16) & 255 for x in p.read_text().split())

def i32(b, a):
    return struct.unpack_from("<i", b, a)[0]

def run(cmd, inp=b"", timeout=10):
    return subprocess.run(cmd, cwd=ROOT, input=inp, text=False,
                          stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                          timeout=timeout)

def build():
    r = run(["make", "clean"])
    if r.returncode: print(r.stdout.decode()); return False
    r = run(["make"])
    if r.returncode: print(r.stdout.decode()); return False
    print("[PASS] Build")
    return True

def check_bytecode(d):
    a, e = d / "program.byte", d / "expected_program.byte"
    return a.exists() and e.exists() and a.read_bytes() == e.read_bytes()

def check_result(name, d):
    b = bytes_of(d / "data.byte")
    if name.startswith("test_"):
        ex = {"test_01":[(16,55)], "test_02":[(16,70),(20,85)],
              "test_03":[(36,37)], "test_04":[(24,150)],
              "test_05":[(16,10),(20,40),(24,80)]}[name]
        return all(i32(b, a) == v for a, v in ex)
    n = i32(b, 0)
    if name == "vector_array_add":
        if n <= 0 or n % 8: return False
        A, B, C = 4, 4 + 4*n, 4 + 8*n
        return all(i32(b, C+4*i) == i32(b, A+4*i) + i32(b, B+4*i) for i in range(n))
    weights = [i32(b, 4+4*i) for i in range(8)]
    inp = [i32(b, 36+4*i) for i in range(n)]
    return all(i32(b, 0x100+4*k) == sum(inp[k+i]*weights[i] for i in range(8)) for k in range(max(0,n-8)))

def run_regression():
    passed = 0
    for name in TESTS:
        d = ROOT / "tests" / name
        data = d / "data.byte"
        original = data.read_bytes()
        try:
            r = run([str(EXE), str(d/"program.txt"), str(data)], b"exit\n")
            ok = r.returncode == 0 and check_bytecode(d) and check_result(name, d)
            print(f"[PASS] {name}" if ok else f"[FAIL] {name}")
            if not ok: print(r.stdout.decode()[-2000:])
            passed += int(ok)
        finally:
            data.write_bytes(original)
    return passed

def lab4_print():
    d = ROOT / "tests" / "lab4_print"
    data = d / "data.byte"
    original = data.read_bytes()
    log = ROOT / "process.log"
    if log.exists(): log.unlink()
    try:
        r = run([str(EXE), str(d/"program.txt"), str(data)], b"exit\n")
        text = log.read_text() if log.exists() else ""
        good = (r.returncode == 0 and "Process id:" in text and
                "x1 : 0000002A" in text and "x2 : 000000FF" in text)
        print("[PASS] lab4_print" if good else "[FAIL] lab4_print")
        if not good: print(r.stdout.decode()[-2000:])
        return good
    finally:
        data.write_bytes(original)

def lab4_multiprocess():
    d = ROOT / "tests" / "lab4_multiprocess"
    files = []
    originals = []
    for i in range(1, 7):
        files += [str(d/f"program{i}.txt"), str(d/f"data{i}.byte")]
        originals.append((d/f"data{i}.byte").read_bytes())
    log = ROOT / "process.log"
    if log.exists(): log.unlink()
    try:
        # All six tasks are loaded before the scheduler starts. This makes
        # four immediate processor assignments and two waiting tasks deterministic.
        r = run([str(EXE)] + files, b"exit\n", timeout=15)
        out = r.stdout.decode()
        logs = log.read_text() if log.exists() else ""
        loaded = all(f"Loaded PID {i} on processor {i-1}" in out for i in range(1,5))
        waiting = ("PID 5 placed in WAITING queue" in out and
                   "PID 6 placed in WAITING queue" in out)
        dispatched = ("Dispatched PID 5 from WAITING queue" in out and
                      "Dispatched PID 6 from WAITING queue" in out)
        printed = all(f"x1 : {0xC8+i:08X}" in logs for i in range(1,7))
        good = r.returncode == 0 and loaded and waiting and dispatched and printed
        print("[PASS] lab4_multiprocess" if good else "[FAIL] lab4_multiprocess")
        if not good:
            print(out[-5000:])
            print("--- process.log ---")
            print(logs[-3000:])
        return good
    finally:
        for i, original in enumerate(originals, 1):
            (d/f"data{i}.byte").write_bytes(original)

def main():
    if not build(): return 1
    passed = run_regression()
    passed += int(lab4_print())
    passed += int(lab4_multiprocess())
    total = len(TESTS) + 2
    print(f"\n{passed}/{total} tests passed")
    print("ALL TESTS PASSED" if passed == total else "SOME TESTS FAILED")
    return 0 if passed == total else 1

if __name__ == "__main__":
    raise SystemExit(main())
