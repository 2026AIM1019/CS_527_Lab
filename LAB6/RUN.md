chmod +x test_runner.sh
chmod +x run_multiprocess_demo.sh

make clean
make

make test
make

./executable
------------Simulation start here-----------------
tests/test_01/program.txt tests/test_01/data.byte
tests/test_02/program.txt tests/test_02/data.byte
tests/test_03/program.txt tests/test_03/data.byte
tests/test_04/program.txt tests/test_04/data.byte
tests/test_05/program.txt tests/test_05/data.byte
exit
--------------------------------------------------
