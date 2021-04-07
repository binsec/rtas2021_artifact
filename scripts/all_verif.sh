#!/usr/bin/env bash

if [ -z $NIX_ARTIFACT_ENV ]; then
  echo "Not in the suitable environment to run the artifact. Please open the required environment by typing 'nix-shell'."
  exit 1;
fi

if test -z ${ntasks_args+x} || test -z ${param_or_fixed+x} || test -z ${opt_flags+x} \
    || test -z ${compilers+x} || test -z ${scheduling_algs+x} || test -z ${dyn_threads_args+x} \
    || test -z ${printing_args+x}; then
  echo "This script has been called with some environment variables missing. Please be aware that it was designed to be called from the artifact Makefile."
  exit 1
fi

IFS=' ' read -ar ntasks_arg <<< $ntasks_arg
IFS=' ' read -ar param_or_fixed <<< $param_or_fixed
IFS=' ' read -ar opt_flags <<< $opt_flags
IFS=' ' read -ar compilers <<< $compilers
IFS=' ' read -ar scheduling_algs <<< $scheduling_algs
IFS=' ' read -ar dyn_threads_args <<< $dyn_threads_args
IFS=' ' read -ar printing_args <<< $printing_args

log_file=$(mktemp)
echo "Script log file: " $log_file

csv_file=$(mktemp).csv
echo "Result CSV: $csv_file"
echo 'Ntasks,compiler,opt_flag,scheduling,dyn_thread,printing,time(s),mem(kB),Ninstr' > $csv_file

time_cmd=$(whereis time | cut -d ' ' -f 2)

verif_cmd=$ARTIFACT_ROOT/scripts/verif_for.sh

for ntasks in ${ntasks_args[@]}; do
  for OPT_FLAGS in ${opt_flags[@]}; do
    for comp in ${compilers[@]}; do
      for scheduling_alg in ${scheduling_algs[@]}; do
        for dyn_threads in ${dyn_threads_args[@]}; do
          export DYNAMIC_TASK_CREATION=$dyn_threads
          for printing in ${printing_args[@]}; do
            echo -n "$ntasks,$comp,$OPT_FLAGS,$scheduling_alg,$dyn_threads,$printing" >> $csv_file
            echo "Verifying configuration comp=$comp OPT_FLAGS=$OPT_FLAGS ntasks=$ntasks param_or_fixed=$param_or_fixed scheduling=$scheduling_alg dyn_threads=$dyn_threads printing=$printing"
            echo $verif_cmd $ntasks $param_or_fixed $scheduling_alg $comp $dyn_threads $printing $OPT_FLAGS
            $time_cmd $verif_cmd $ntasks $param_or_fixed $scheduling_alg $comp $dyn_threads $printing $OPT_FLAGS >& $log_file
            exit_code=$?
            if [ $exit_code -le 1 ]; then
              if [ $exit_code = 0 ]; then
                echo "OK!"
              elif [ $exit_code = 1 ]; then
                echo "Failed."
                grep 'Total alarms:' /tmp/log
              fi
              time=$(egrep -A1 'Total time:$' /tmp/log | tail -n 1 | sed 's/^ \?\([.0-9]\+\).*$/\1/')
              echo "Total time: $time"
              echo -n ",$time" >> $csv_file
              mem=$(egrep maxresident $log_file | sed 's/^.* \([0-9]\+\)maxresident)k$/\1/')
              echo -n ",$mem" >> $csv_file
              ninstr=$(egrep 'Unique visited instructions:' /tmp/log | cut -d ' ' -f 4)
              echo ",$ninstr" >> $csv_file
            elif [ $exit_code = 2 ]; then
              echo "Analysis aborted due to insufficient precision or certain runtime error."
            elif [ $exit_code = 3 ]; then
              echo "Error during EducRTOS compilation."
              echo "Build log: $log_file"
              exit 3
            else
              echo "Unknown script error (exit code $exit_code)"
              exit $exit_code
            fi
            echo # Newline
          done
        done
      done
    done
  done
done
