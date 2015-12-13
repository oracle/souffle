SOUFFLE=../../src/souffle

$SOUFFLE sg.dl  -c -v --no-auto-schedule -o sg_seq
$SOUFFLE sg.dl  -c -v --no-auto-schedule -j 8 -o sg_omp

