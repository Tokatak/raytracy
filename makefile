CC      = gcc
CFLAGS  = -ffast-math -Wall -Wextra -I./ -I./external/minunit -O2
LDFLAGS =

VARIANT_scalar = -DSCALAR
VARIANT_simd   = -msse3 -DSIMD

VARIANTS = scalar simd


build:
	gcc -ffast-math -Wall -Wextra -Wpedantic -g -O0 $(VARIANT_scalar) -o main main.c && ./main.exe

build-simd:
	gcc -ffast-math -Wall -Wextra -Wpedantic -g -O0 $(VARIANT_simd) -o main main.c && ./main.exe

depth:
	gcc -ffast-math -Wall -Wextra -Wpedantic -g -O0 ${VARIANT_scalar} -o main main.c && ./main.exe -d

depth-simd:
	gcc -ffast-math -Wall -Wextra -Wpedantic -g -O0 ${VARIANT_simd} -o main main.c && ./main.exe -d

optimized:
	gcc -ffast-math  -Wall -Wextra -Wpedantic -O3  -march=native ${VARIANT_scalar} -o main main.c && ./main.exe

msvc:
	cl /Zi /DEBUG /EHsc main.c && main.exe


# TESTS
tests-clean:
	-rm -r -f ./tests/failed/*
tests-build-scalar:
	gcc -ffast-math -Wall  -I./external/minunit -I./ ./tests/main.c $(VARIANT_scalar) -o test_suite_scalar
tests-build-simd:
	gcc -ffast-math -Wall  -I./external/minunit -I./ ./tests/main.c $(VARIANT_simd) -o test_suite_simd
# todo outdated 
tests-build-cl:
	cl /Zi /DEBUG /EHsc /I ./ /I ./external/* ./tests/main.c /Fe:cl_test_suite.exe
# outdated end
tests-build: tests-build-scalar tests-build-simd
tests-run: # todo: consider smarter define itteration
	@printf "==SCALAR==\n"
	-test_suite_scalar.exe 
	@printf "==SIMD==\n"
	-test_suite_simd.exe	
test tests: tests-clean tests-build tests-run



# PROFILE
profile-clean:
	rm -f gmon.out && rm -f  executable.exe && rm -f callgraph.txt
profile-build:
	gcc -DPROFILE -ffast-math -g -pg -O2 -no-pie main.c -msse3 -o executable.exe && ./executable.exe
profile-run:
	gprof -q executable.exe gmon.out > callgraph.txt
profile: profile-clean profile-build profile-run

# vectorize
vectorize-output:
	 gcc -O3 -march=native -ftree-vectorize -fopt-info-vec-all -msse3 -o main main.c
vectorize-store:
	 gcc -O3 -march=native -ftree-vectorize -fopt-info-vec-all -msse3 -o main main.c -lm 2 > vectorize.txt
