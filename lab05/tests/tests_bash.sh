#!/bin/bash

echo "0.0001 0.5" > args_task05_pos_regular.txt
echo "0.00001 0.0" > args_task05_pos_zero_x.txt
echo "0.0001 -0.5" > args_task05_pos_neg_x.txt
echo "1e-6 0.2" > args_task05_pos_high_prec.txt

echo "0.0001 1.0" > args_task05_diverge_c_boundary.txt
echo "0.0001 2.0" > args_task05_diverge_c_and_d.txt
echo "0.0001 -1.5" > args_task05_diverge_negative_large.txt

echo "0.0001" > args_task05_err_too_few_args.txt
echo "0.0001 0.5 10" > args_task05_err_too_many_args.txt

echo "-0.0001 0.5" > args_task05_err_eps_neg.txt
echo "0 0.5" > args_task05_err_eps_zero.txt
echo "1.5 0.5" > args_task05_err_eps_greater_one.txt
echo "1e-25 0.5" > args_task05_err_eps_too_small.txt

echo "abc 0.5" > args_task05_err_eps_chars.txt
echo "0.0001 xyz" > args_task05_err_x_chars.txt
echo "0.0001abc 0.5" > args_task05_err_trailing_junk.txt
echo "1.2.3.4 0.5" > args_task05_err_multiple_dots.txt

echo "9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999 0.5" > args_task05_err_overflow_long_string.txt
echo "1e9999 0.5" > args_task05_err_overflow_exp.txt