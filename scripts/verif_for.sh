#!/usr/bin/env bash

if [ -z $NIX_ARTIFACT_ENV ]; then
  echo "Not in the suitable environment to run the artifact. Please open the required environment by typing 'nix-shell'."
  exit 1
fi

debug_level=0

if [ -z $1 -o -z $2 -o -z $3 -o -z $4 -o -z $5 -o -z $6 -o -z $7 ]; then
  echo Invalid arguments: $1, $2, $3, $4, $5, $6, $7
  echo "usage: verif_for <ntasks> <param|fixed> <scheduling> <comp> <dyn_threads> <opt flags>"
  echo "SCRIPT WENT WRONG"
  exit 2
fi

ntasks=$1
param_or_fixed=$2
scheduling=$3
compiler=$4
dyn_threads=$5
printing=$6
opt_flags=$7
if [ $scheduling = 'rr' ]; then
  export SCHEDULER=ROUND_ROBIN_SCHEDULING
elif [ $scheduling = 'fp' ]; then
  export SCHEDULER=FP_SCHEDULING
elif [ $scheduling = 'edf' ]; then
  export SCHEDULER=EDF_SCHEDULING
else
  echo "Invalid scheduling argument '$3' must be one of: rr, fp, edf"
  exit 2
fi

if test $ntasks != 1 && test $ntasks != 2 && test $ntasks != 5 && test $ntasks != 10 && test $ntasks != 50 && test $ntasks != 100 && test $ntasks != 500 && test $ntasks != 1000 && test $ntasks != 2000 && test $ntasks != 5000 && test $ntasks != 10000 && test $ntasks != 100000; then
  echo "Invalid number of tasks: should be one of: 1 2 5 10 50 100 500 1000 2000 5000 10000 100000"
  exit 2
fi

log=/tmp/log
exe=system_${ntasks}tasks.exe
educrtos_dir=$ARTIFACT_ROOT/educrtos
exe_full=$educrtos_dir/$exe
config_file=$educrtos_dir/binsec.ini
cfg_dir=$ARTIFACT_ROOT/cfg

if [ "$param_or_fixed" = "param" ]
then
  shape_arg=
elif [ $param_or_fixed = 'fixed' ]; then
  shape_arg=-codex-no-use-shape
else
  echo "Invalid shape argument '$param_or_fixed'. Must be one of: param, fixed."
  exit 2
fi

if [ $compiler = 'clang' ]; then
  use_clang='true'
elif [ $compiler = 'gcc' ]; then
  use_clang='false'
else
  echo "Invalid compiler argument '$compiler'. Must be one of: clang, gcc"
  exit 2
fi

if [ $dyn_threads = 'dyn' ]; then
  export DYNAMIC_TASK_CREATION=yes
  dyn_thread_arg=''
elif [ $dyn_threads = 'nodyn' ]; then
  export DYNAMIC_TASK_CREATION=no
  dyn_thread_arg='-codex-no-dyn-threads'
else
  echo "Invalid dyn_threads argument '$dyn_threads'. Must be one of: dyn, nodyn"
  exit 2
fi

if [ $printing = 'print' ]; then
  export DEBUG_PRINTING=yes
elif [ $printing = 'noprint' ]; then
  export DEBUG_PRINTING=no
else
  echo "Invalid printing argument '$printing'. Must be one of: print, noprint"
  exit 2
fi

if [ $opt_flags = '-O0' -o $opt_flags = '-O1' -o $opt_flags = '-O2' \
    -o $opt_flags = '-O3' -o $opt_flags = '-Os' ]; then
  export OPT_FLAGS=$opt_flags
else
  echo "Invalid printing argument '$printing'. Must be one of: -O0,-O1,-O2,-O3,-Os"
  exit 2
fi

cd $educrtos_dir && \
  nix-shell --arg clang $use_clang --command "make clean && make $exe" && \
  cd -
  if test $? != 0; then
    echo "Could not compile EducRTOS. Aborting."
    exit 3
  fi
echo binsec -config $config_file $exe_full -codex-x86-types $scheduling $shape_arg -codex-nb-tasks $ntasks $dyn_thread_arg -codex-debug-level $debug_level
binsec -config $config_file $exe_full -codex-x86-types $scheduling $shape_arg -codex-nb-tasks $ntasks $dyn_thread_arg -codex-debug-level $debug_level >& $log
exit_code=$?
if test $exit_code = 2; then
  echo "Analysis aborted due to insufficient precision or certain runtime error."
  echo "Log file: $log"
  exit 2
else
  mkdir -p $cfg_dir
  mv -f init_cpu0.dot handlers_cpu0.dot $cfg_dir/ && \
    if [ $param_or_fixed = param ]; then mv -f concrete_init_cpu0.dot $cfg_dir/ ; fi
  if test $? != 0; then
    echo "Error when moving output files."
    exit $exit_code
  fi
  echo "Log file: $log"
  if test $exit_code != 0; then
    echo "Alarms were emitted."
    exit 1
  fi
fi
