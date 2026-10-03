#!/bin/bash

mkdir -p tests

cat << 'EOF' > tests/input_text.txt
Hello 123 World!
This is a test line: 42.
Final 999 END.
EOF

cat << 'EOF' > tests/input_special.txt
Hello, World! 123
@#$% special & symbols
    just spaces and 777
EOF

cat << 'EOF' > tests/input_no_newline.txt
Line without newline at the end 123
EOF

touch tests/input_empty.txt

echo "-d tests/input_text.txt" > tests/args_task04_d_auto.txt
echo "/i tests/input_text.txt" > tests/args_task04_i_slash_auto.txt
echo "-s tests/input_special.txt" > tests/args_task04_s_auto.txt
echo "-a tests/input_text.txt" > tests/args_task04_a_auto.txt

echo "-nd tests/input_text.txt tests/custom_out_d.txt" > tests/args_task04_nd_custom.txt
echo "-ni tests/input_text.txt tests/custom_out_i.txt" > tests/args_task04_ni_custom.txt
echo "/ns tests/input_special.txt tests/custom_out_s.txt" > tests/args_task04_ns_slash_custom.txt
echo "-na tests/input_text.txt tests/custom_out_a.txt" > tests/args_task04_na_custom.txt

echo "-i tests/input_empty.txt" > tests/args_task04_empty_file.txt
echo "-i tests/input_no_newline.txt" > tests/args_task04_no_trailing_newline.txt

echo "-" > tests/args_task04_err_single_dash.txt
echo "-n" > tests/args_task04_err_dash_n.txt
echo "-d" > tests/args_task04_err_no_input_file.txt
echo "-nd tests/input_text.txt" > tests/args_task04_err_nd_without_out.txt
echo "-d tests/input_text.txt extra_file.txt" > tests/args_task04_err_d_too_many_args.txt
echo "-q tests/input_text.txt" > tests/args_task04_err_unknown_action.txt
echo "unknown tests/input_text.txt" > tests/args_task04_err_no_prefix.txt
echo "-nd tests/input_text.txt tests/input_text.txt" > tests/args_task04_err_same_file.txt
echo "-nd tests/input_text.txt ./tests/input_text.txt" > tests/args_task04_err_same_file_dot.txt
echo "-d tests/non_existent_file_12345.txt" > tests/args_task04_err_file_not_found.txt