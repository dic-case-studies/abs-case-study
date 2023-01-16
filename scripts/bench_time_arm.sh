#!/usr/bin/env bash

declare -a SIZE=(10000 50000 75001 100000 500001 1000000 10000000 100000000)

host=$1
bench=abs
mkdir -p stats/$host/time

rm -f stats/$host/time/$bench-result.txt stats/$host/time/$bench-stats.csv stats/$host/time/$bench-performance.png

for sz in "${SIZE[@]}"
do
  echo "abs $sz"
  ./build/$bench $sz >> stats/$host/time/$bench-result.txt
done

echo "SIZE, GOLDEN, OMP, SSE2NEON, NEON" > stats/$host/time/$bench-stats.csv;
cat stats/$host/time/$bench-result.txt | awk '                          \
  /Array dim/ {                              \
    size = $NF;                               \
  }                                           \
  /Elapsed time GOLDEN/ {                     \
    golden = $(NF-1);                         \
  }                                           \
  /Elapsed time OpenMP/ {                     \
    omp = $(NF-1);                         \
  }                                           \
  /Elapsed time SSE/ {                   \
    sse = $(NF-1);                            \
  }                                           \
  /Elapsed time NEON/ {                   \
    neon = $(NF-1);                            \
    printf("%s, %s, %s, %s, %s\n", size, golden, omp, sse, neon); \
  }                                           \
' >> stats/$host/time/$bench-stats.csv

echo "                                            \
  reset;                                          \
  set terminal png enhanced large font \"Helvetica,10\"; \
                                                         \
  set title \"$bench Benchmark\";                        \
  set xlabel \"Matrix Dim\";                             \
  set ylabel \"Execution time(us)\";                     \
  set datafile separator ',';                           \
  set key autotitle columnhead;                         \
  set logscale x;                                        \
  set logscale y;                                        \
                                                         \
  plot \"stats/$host/time/$bench-stats.csv\" using 1:2 with linespoint, \
       \"stats/$host/time/$bench-stats.csv\" using 1:3 with linespoint ,   \
       \"stats/$host/time/$bench-stats.csv\" using 1:4 with linespoint,   \
       \"stats/$host/time/$bench-stats.csv\" using 1:5 with linespoint;   \
" | gnuplot > stats/$host/time/$bench-performance.png
