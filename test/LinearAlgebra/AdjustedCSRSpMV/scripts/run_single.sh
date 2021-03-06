#!/bin/bash

txtred='\e[0;31m'
txtgreen='\e[0;32m'
txtrst='\e[0m'

run_sim=./run-sim.sh
max_time=120

f=$1
sim_bin=$2

if [[ $f != *"$1"* ]]; then
  continue
fi
abs_f=`readlink -f $f`

start_time=`date +%s`
timeout --signal=KILL ${max_time} ${run_sim} ${sim_bin} ${abs_f} 1 > /dev/null
status=$?
end_time=`date +%s`
exec_time=`expr $end_time - $start_time`
if [[ $status == 0 ]]; then
  echo -e -n "${txtgreen}Passed${txtrst} ($exec_time s) "
else
  echo -e -n "${txtred}Failed${txtrst} ($exec_time s)"
fi
echo `basename $abs_f`
exit ${status}
