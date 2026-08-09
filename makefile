build:
# debug
	gcc -Wall -Wextra -Wpedantic -g -O0 -o main main.c &&  ./main.exe

# optimized
#	 gcc -Wall -Wextra -Wpedantic -O3 -march=native -o main main.c && main.exe

msvc:
	cl /Zi /DEBUG /EHsc main.c && main.exe


# TESTS
tests-clean:
	-rm -f test_suite
tests-build:
	gcc -Wall  -I./external/minunit	 -I./ ./tests/main.c -o test_suite
tests-build-cl:
	cl /Zi /DEBUG /EHsc /I ./ /I ./external/* ./tests/main.c /Fe:cl_test_suite.exe
tests-run:
	test_suite.exe
test tests: tests-clean tests-build tests-run

# PROFILE
profile-clean:
	rm gmon.out && rm executable.exe && rm callgraph.txt
profile-build:
	gcc -g -pg -O2 -no-pie main.c -o executable.exe && ./executable.exe
profile-run:
	gprof -q executable.exe gmon.out > callgraph.txt
profile: profile-clean profile-build profile-run

