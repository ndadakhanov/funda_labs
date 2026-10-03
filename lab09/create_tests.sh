#!/bin/bash

mkdir -p tests

echo "10 50" > tests/args_task09_pos_regular.txt
echo "-100 100" > tests/args_task09_pos_negative_range.txt
echo "0 20" > tests/args_task09_pos_from_zero.txt
echo "7 7" > tests/args_task09_pos_same_bounds.txt
echo "-50 -10" > tests/args_task09_pos_both_negative.txt

echo "50 10" > tests/args_task09_err_a_greater_b.txt
echo "100 -100" > tests/args_task09_err_inverted.txt

echo "" > tests/args_task09_err_no_args.txt
echo "10" > tests/args_task09_err_too_few_args.txt
echo "10 20 30" > tests/args_task09_err_too_many_args.txt

echo "abc 50" > tests/args_task09_err_chars_a.txt
echo "10 xyz" > tests/args_task09_err_chars_b.txt
echo "10.5 20" > tests/args_task09_err_float.txt

echo "9999999999999999999999999999999999999999999999999999 50" > tests/args_task09_err_overflow_huge_a.txt
echo "10 9999999999999999999999999999999999999999999999999999" > tests/args_task09_err_overflow_huge_b.txt
echo "3000000000 50" > tests/args_task09_err_overflow_int.txt