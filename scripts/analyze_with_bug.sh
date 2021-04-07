#! /usr/bin/env bash

bug_number=$1

export INJECT_BUG=INJECT_BUG$bug_number

log_file=$(mktemp)
echo "Analysis log file: $analysis_log, script log file: $log_file"
echo $ARTIFACT_ROOT/scripts/verif_for.sh 10 param rr clang nodyn noprint -Os
$ARTIFACT_ROOT/scripts/verif_for.sh 10 param rr clang nodyn noprint -Os >& $log_file
exit_code=$?
if [ $exit_code = 0 ]; then
  echo "Analysis succeeded, but it shouldn't have."
  exit 1
elif [ $exit_code = 1 ]; then
  echo "Analysis emitted alarms (as expected)."
  grep 'Total alarms:' $analysis_log
elif [ $exit_code = 2 ]; then
  echo "Analysis aborted due to insufficient precision or certain runtime error (as expected)."
elif [ $exit_code = 3 ]; then
  echo "Error during EducRTOS compilation."
  echo "Build log: $log_file"
else
  echo "Unknown script error (exit code $exit_code)"
  exit $exit_code
fi
echo # Newline
