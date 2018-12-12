mpicc -std=gnu99 -Wall -I../include -o test_234byte_mandel  test_234byte_mandel.c  ../lib/lib234comp.a -lm
mpicc -std=gnu99 -Wall -I../include -o test_234float_mandel test_234float_mandel.c ../lib/lib234comp.a -lm


