#!/bin/bash

mkdir -p tests

cat << 'EOF' > tests/input_r1_same.txt
one three five seven
EOF

cat << 'EOF' > tests/input_r2_same.txt
two four six eight
EOF

cat << 'EOF' > tests/input_r1_long.txt
word1 word3 word5 extra1 extra2 extra3
EOF

cat << 'EOF' > tests/input_r2_short.txt
word2 word4
EOF

cat << 'EOF' > tests/input_r_spaces.txt
   tokenA      tokenB
     tokenC  
EOF

touch tests/input_empty.txt

cat << 'EOF' > tests/input_tokens12.txt
First SECOND Third FOURTH Five SIXTH Seven EIGHTH Nine TEN Eleven TWELVE
EOF

echo "-r tests/input_r1_same.txt tests/input_r2_same.txt tests/out_r_same.txt" > tests/args_task07_r_balanced.txt
echo "/r tests/input_r1_long.txt tests/input_r2_short.txt tests/out_r_long.txt" > tests/args_task07_r_slash_unbalanced.txt
echo "-r tests/input_empty.txt tests/input_r2_same.txt tests/out_r_empty1.txt" > tests/args_task07_r_one_empty.txt
echo "-r tests/input_r_spaces.txt tests/input_r2_short.txt tests/out_r_spaces.txt" > tests/args_task07_r_many_spaces.txt

echo "-a tests/input_tokens12.txt tests/out_a_transformed.txt" > tests/args_task07_a_regular.txt
echo "/a tests/input_tokens12.txt tests/out_a_slash.txt" > tests/args_task07_a_slash.txt
echo "-a tests/input_empty.txt tests/out_a_empty.txt" > tests/args_task07_a_empty.txt

echo "-" > tests/args_task07_err_single_dash.txt
echo "-x tests/input_tokens12.txt tests/out_err.txt" > tests/args_task07_err_unknown_action.txt
echo "-r tests/input_r1_same.txt tests/input_r2_same.txt" > tests/args_task07_err_r_too_few_args.txt
echo "-r tests/input_r1_same.txt tests/input_r2_same.txt tests/out1.txt extra.txt" > tests/args_task07_err_r_too_many_args.txt
echo "-a tests/input_tokens12.txt" > tests/args_task07_err_a_too_few_args.txt
echo "-a tests/input_tokens12.txt tests/out.txt extra.txt" > tests/args_task07_err_a_too_many_args.txt
echo "-r tests/input_r1_same.txt tests/input_r2_same.txt tests/input_r1_same.txt" > tests/args_task07_err_r_same_file1.txt
echo "-r tests/input_r1_same.txt tests/input_r2_same.txt ./tests/input_r2_same.txt" > tests/args_task07_err_r_same_file2.txt
echo "-a tests/input_tokens12.txt tests/input_tokens12.txt" > tests/args_task07_err_a_same_file.txt
echo "-a tests/input_tokens12.txt ./tests/input_tokens12.txt" > tests/args_task07_err_a_same_file_dot.txt
echo "-a tests/non_existent_file_999.txt tests/out.txt" > tests/args_task07_err_file_not_found.txt