#!/bin/bash

mkdir -p tests

echo "-1 0.0001 0 0 1 0 1 1 0 1" > tests/args_task06_1_square.txt
echo "-1 0.0001 0 0 4 0 2 3" > tests/args_task06_1_triangle.txt
echo "/1 0.0001 0 0 2 0 1 1 2 2 0 2" > tests/args_task06_1_concave.txt
echo "-1 0.0001 0 0 1 1" > tests/args_task06_1_err_too_few_pts.txt

echo "-2 2.0 2 1.0 2.0 1.0" > tests/args_task06_2_quadratic.txt
echo "-2 3.0 3 2.0 -1.0 0.0 5.0" > tests/args_task06_2_cubic.txt
echo "-2 1.0 -1 2.0" > tests/args_task06_2_err_neg_degree.txt

echo "-3 10 9 10 45 55 99 100" > tests/args_task06_3_kaprekar_base10.txt
echo "-3 16 A 1F 55 56 A0" > tests/args_task06_3_kaprekar_base16.txt
echo "-3 40 10 20" > tests/args_task06_3_err_base_overflow.txt

echo "-4 2.0 8.0" > tests/args_task06_4_geom_two.txt
echo "-4 1.0 3.0 9.0" > tests/args_task06_4_geom_three.txt
echo "-4 2.0 -4.0 8.0" > tests/args_task06_4_err_neg_num.txt

echo "-5 2.0 10" > tests/args_task06_5_pow_pos.txt
echo "-5 2.0 -3" > tests/args_task06_5_pow_neg.txt
echo "-5 5.5 0" > tests/args_task06_5_pow_zero.txt
echo "-5 0.0 -2" > tests/args_task06_5_err_div_zero.txt

echo "-6 1 1.0 2.0 0.0001" > tests/args_task06_6_root_sqrt2.txt
echo "-6 2 0.0 1.0 0.0001" > tests/args_task06_6_root_dottie.txt
echo "-6 1 2.0 3.0 0.0001" > tests/args_task06_6_err_no_root.txt

echo "-" > tests/args_task06_err_single_dash.txt
echo "-9 1 2 3" > tests/args_task06_err_unknown_subtask.txt
echo "-5 99999999999999999999999999999999999999999999999999999999999999999999 2" > tests/args_task06_err_huge_overflow.txt
echo "-5 abc xyz" > tests/args_task06_err_not_numbers.txt