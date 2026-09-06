#!/bin/sh
# Lab 4 scheduler demonstration: 6 tasks, 4 processors, 2 waiting tasks.
# Uses dedicated short-running programs so queue/slice transitions are easy to see.
set -eu
cd "$(dirname "$0")"
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
