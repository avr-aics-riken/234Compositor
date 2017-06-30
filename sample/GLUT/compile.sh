mpicc -std=gnu99 -Wall -fopenmp -I../../include -o test_234byte  test_234byte.c  ../../lib/lib234comp.a -lm
mpicc -std=gnu99 -Wall -fopenmp -I../../include -o test_234float test_234float.c ../../lib/lib234comp.a -lm

