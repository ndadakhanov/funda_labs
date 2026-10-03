#!/bin/bash

mkdir -p tests

cat << 'EOF' > tests/input_task08_basic.txt
000101 00FF Z 10 0000 45
EOF

cat << 'EOF' > tests/input_task08_multiline.txt
   0007   
-001A     +003C   
  0      99
EOF

cat << 'EOF' > tests/input_task08_case_mixed.txt
1a2B 3c4D 000ff 000FF
EOF

touch tests/input_task08_empty.txt

cat << 'EOF' > tests/input_task08_bad_chars.txt
101 25 FF 12#45 67
EOF

cat << 'EOF' > tests/input_task08_overflow.txt
101 9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999 55
EOF

echo "tests/input_task08_basic.txt tests/out_task08_basic.txt" > tests/args_task08_basic.txt
echo "tests/input_task08_multiline.txt tests/out_task08_multiline.txt" > tests/args_task08_multiline.txt
echo "tests/input_task08_case_mixed.txt tests/out_task08_case_mixed.txt" > tests/args_task08_case_mixed.txt
echo "tests/input_task08_empty.txt tests/out_task08_empty.txt" > tests/args_task08_empty.txt

echo "" > tests/args_task08_err_no_args.txt
echo "tests/input_task08_basic.txt" > tests/args_task08_err_too_few_args.txt
echo "tests/input_task08_basic.txt tests/out.txt extra_arg" > tests/args_task08_err_too_many_args.txt
echo "tests/input_task08_basic.txt tests/input_task08_basic.txt" > tests/args_task08_err_same_file.txt
echo "tests/input_task08_basic.txt ./tests/input_task08_basic.txt" > tests/args_task08_err_same_file_dot.txt
echo "tests/non_existent_file_777.txt tests/out.txt" > tests/args_task08_err_file_not_found.txt
echo "tests/input_task08_bad_chars.txt tests/out_bad.txt" > tests/args_task08_err_bad_chars.txt
echo "tests/input_task08_overflow.txt tests/out_overflow.txt" > tests/args_task08_err_overflow.txt