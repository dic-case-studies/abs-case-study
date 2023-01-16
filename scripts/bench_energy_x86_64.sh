#!/usr/bin/env bash

set -e
set -x

declare -a CASES=(abs)
# declare -a CASES=(min-max-bench)
declare -a METHODS=(GOLDEN SSE AVX)

declare -a SIZE=(10000 50000 75001 100000 500001 1000000 10000000 100000000)
# declare -a SIZE=(1024 2048 4096 8192)
# declare -a SIZE=(4096)

host=$1
mkdir -p stats/$host/energy

for case in ${CASES[@]}
do  
  for method in ${METHODS[@]}
  do
      rm -f build/${case} stats/$host/energy/${case}-${method}-result.txt

      make build/${case} OPT="-UGOLDEN -USSE -UAVX -UASSERT -D$method"
      for sz in "${SIZE[@]}"
      do
      echo "Running ${case} ${method} ${sz}" >> stats/$host/energy/${case}-${method}-result.txt 
      perf stats -e power/energy-pkg/ ./build/${case} ${sz} &>> stats/$host/energy/${case}-${method}-result.txt 
      echo "----------------------------" >> stats/$host/energy/${case}-${method}-result.txt 
      done

      cat stats/$host/energy/${case}-${method}-result.txt   | awk '    \
        /Array dim/ {                              \
          size = $NF;                               \
        }                                           \
        /Elapsed time/ {                   \
          time = $(NF-1);                            \
        }                                           \
        /energy-pkg/ {                              \
          energy = $1;                            \
          printf("%s, %s, %s\n", size, energy, time); \
        }                                           \
      ' > stats/$host/energy/${case}-${method}-stats.csv
  done

  echo "                                            \
    reset;                                          \
    set terminal png enhanced large; \
                                                          \
    set title \"$bench Benchmark\";                        \
    set xlabel \"Matrix Dim\";                             \
    set ylabel \"Joules\";                     \
    set key left top;                                      \
    set logscale x;                                        \
                                                          \
    plot \"stats/$host/energy/${case}-GOLDEN-stats.csv\" using 1:2 with linespoint title \"Golden\", \
        \"stats/$host/energy/${case}-SSE-stats.csv\" using 1:2 with linespoint title \"SSE\",    \
        \"stats/$host/energy/${case}-AVX-stats.csv\" using 1:2 with linespoint title \"AVX\";    \
  " | gnuplot > stats/$host/energy/${case}-performance.png


done
