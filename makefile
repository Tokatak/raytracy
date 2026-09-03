CFLAGS = -msse3

build:
# debug
	gcc -ffast-math -Wall -Wextra -Wpedantic -g -O0 $(CFLAGS) -o main main.c && ./main.exe

optimized:
	gcc -ffast-math  -Wall -Wextra -Wpedantic -O3  -march=native $(CFLAGS) -o main main.c && ./main.exe

msvc:
	cl /Zi /DEBUG /EHsc main.c && main.exe


# TESTS
tests-clean:
	-rm -f ./tests/failed/*
tests-build:
	gcc -ffast-math -Wall  -I./external/minunit	 -I./ ./tests/main.c $(CFLAGS) -o test_suite
tests-build-cl:
	cl /Zi /DEBUG /EHsc /I ./ /I ./external/* ./tests/main.c /Fe:cl_test_suite.exe
tests-run:
	test_suite.exe
test tests: tests-clean tests-build tests-run

# PROFILE
profile-clean:
	rm -f gmon.out && rm -f  executable.exe && rm -f callgraph.txt
profile-build:
	gcc -DPROFILE -ffast-math -g -pg -O2 -no-pie main.c $(CFLAGS) -o executable.exe && ./executable.exe
profile-run:
	gprof -q executable.exe gmon.out > callgraph.txt
profile: profile-clean profile-build profile-run

# vectorize
vectorize-output:
	 gcc -O3 -march=native -ftree-vectorize -fopt-info-vec-all $(CFLAGS) -o main main.c
vectorize-store:
	 gcc -O3 -march=native -ftree-vectorize -fopt-info-vec-all $(CFLAGS) -o main main.c -lm 2 > vectorize.txt
