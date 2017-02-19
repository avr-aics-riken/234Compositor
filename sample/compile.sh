mpicc -std=gnu99 -Wall -I../include -o test_234byte_mandel test_234byte_mandel.c -lm ../lib/lib234comp.a
mpicc -std=gnu99 -Wall -I../include -o test_234float_mandel test_234float_mandel.c -lm ../lib/lib234comp.a

# OpenMP enabled version 
#mpicc -std=gnu99 -Wall -fopenmp -I../include -o test_234byte_mandel test_234byte_mandel.c -lm ../lib/lib234comp.a
#mpicc -std=gnu99 -Wall -fopenmp -I../include -o test_234float_mandel test_234float_mandel.c -lm ../lib/lib234comp.a

