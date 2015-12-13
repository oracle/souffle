SOUFFLE=../../src/souffle

$SOUFFLE tc_left.dl  -c -v --no-auto-schedule -o tc_left_seq
$SOUFFLE tc_right.dl -c -v --no-auto-schedule -o tc_right_seq
$SOUFFLE tc_quad.dl  -c -v --no-auto-schedule -o tc_quad_seq

$SOUFFLE tc_left.dl  -c -v --no-auto-schedule -j 8 -o tc_left_omp
$SOUFFLE tc_right.dl -c -v --no-auto-schedule -j 8 -o tc_right_omp
$SOUFFLE tc_quad.dl  -c -v --no-auto-schedule -j 8 -o tc_quad_omp

