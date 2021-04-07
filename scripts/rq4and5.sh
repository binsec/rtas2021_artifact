#! /usr/bin/env bash

n=$1

if test $n != 1 && test $n != 2 && test $n != 5 && test $n != 10 && test $n != 50 && test $n != 100 && test $n != 500 && test $n != 1000 && test $n != 2000 && test $n != 5000 && test $n != 10000 && test $n != 100000; then
  echo "Invalid number of tasks $n: should be one of: 1 2 5 10 50 100 500 1000 2000 5000 10000 100000"
  exit 2
fi

time_cmd=$(whereis time | cut -d ' ' -f 2)
log_file=$(mktemp)
echo "Script log file: " $log_file
verif_cmd=$ARTIFACT_ROOT/scripts/verif_for.sh

if [ $n -gt 1000 ]; then
  verifs=(param)
else
  verifs=(param fixed)
fi

for param_or_fixed in ${verifs[@]}; do
  if [ $param_or_fixed = param ]; then
    echo "Parameterized verification..."
  else
    echo "In-context verification..."
  fi
  echo $verif_cmd $n $param_or_fixed rr clang nodyn noprint -O3
  $time_cmd $verif_cmd $n $param_or_fixed rr clang nodyn noprint -O3 >& $log_file
  exit_code=$?
  if [ $exit_code -le 1 ]; then
    if [ $exit_code = 0 ]; then
      echo "OK!"
    elif [ $exit_code = 1 ]; then
      grep 'Total alarms:' /tmp/log
    fi
    time=$(egrep -A1 'Total time:$' /tmp/log | tail -n 1 | sed 's/^ \?\([.0-9]\+\).*$/\1/')
    echo "Analysis time (s): $time"
    mem=$(egrep maxresident $log_file | sed 's/^.* \([0-9]\+\)maxresident)k$/\1/')
    echo "Max mem. usage (kB): $mem"
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
