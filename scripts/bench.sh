#!/usr/bin/env bash

set -e
set -x

declare -a SIZE=(10000 50000 100000 1000000 10000000 100000000)
# declare -a SIZE=(1024 2048 4096 8192)
# declare -a SIZE=(4)

host=$1
bench=$2
mkdir -p stat/$host

rm -f stat/$host/$bench-result.txt stat/$host/$bench-stats.csv stat/$host/$bench-performance.png

for sz in "${SIZE[@]}"
do
  echo "abs $sz"
  ./build/$bench $sz >> stat/$host/$bench-result.txt
done

cat stat/$host/$bench-result.txt | awk '                          \
  /Array dim/ {                              \
    size = $NF;                               \
  }                                           \
  /Elapsed time GOLDEN/ {                     \
    golden = $(NF-1);                         \
  }                                           \
  /Elapsed time SSE/ {                   \
    sse = $(NF-1);                            \
  }                                           \
  /Elapsed time AVX/ {                   \
    avx = $(NF-1);                            \
    printf("%s, %s, %s, %s\n", size, golden, sse, avx); \
  }                                           \
' > stat/$host/$bench-stats.csv

echo "                                            \
  reset;                                          \
  set terminal png enhanced large font \"Helvetica,10\"; \
                                                         \
  set title \"$bench Benchmark\";                        \
  set xlabel \"Matrix Dim\";                             \
  set ylabel \"Execution time(us)\";                     \
  set key left top;                                      \
  set logscale x;                                        \
  set logscale y;                                        \
                                                         \
  plot \"stat/$host/$bench-stats.csv\" using 1:2 with linespoint title \"Golden\", \
       \"stat/$host/$bench-stats.csv\" using 1:3 with linespoint title \"SSE\",    \
       \"stat/$host/$bench-stats.csv\" using 1:4 with linespoint title \"AVX\";    \
" | gnuplot > stat/$host/$bench-performance.png
