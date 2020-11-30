#!/bin/bash
printf "Ensuring X system is up before launching..."
printf "
This script file formatted to run in Darwin"
xterm -e exit
sleep 1.0
uMissionTester regtest_1_targ_archie.moos >& /dev/null &
sleep 0.25
uMissionTester regtest_1_targ_betty.moos >& /dev/null &
sleep 0.25
uMissionTester regtest_1_targ_shoreside.moos >& /dev/null&
wait $!
printf "Simulation script complete.
"
mykill
