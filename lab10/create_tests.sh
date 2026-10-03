#!/bin/bash

mkdir -p tests

cat << 'EOF' > tests/input_task10_pos_base10.txt
10
15
-250
100
-50
Stop
EOF

cat << 'EOF' > tests/input_task10_pos_base16.txt
16
000A
-00FF
10
002B
Stop
EOF

cat << 'EOF' > tests/input_task10_pos_base2.txt
2
1010
-1111
0001
0
Stop
EOF

cat << 'EOF' > tests/input_task10_pos_base36.txt
36
1Z
-Z
10
Stop
EOF

cat << 'EOF' > tests/input_task10_pos_zeros.txt
8
000
-0
0
Stop
EOF

cat << 'EOF' > tests/input_task10_err_base_too_low.txt
1
10
Stop
EOF

cat << 'EOF' > tests/input_task10_err_base_too_high.txt
37
10
Stop
EOF

cat << 'EOF' > tests/input_task10_err_digit_exceeds_base.txt
8
17
28
Stop
EOF

cat << 'EOF' > tests/input_task10_err_lowercase_digit.txt
16
FF
1a
Stop
EOF

cat << 'EOF' > tests/input_task10_err_bad_symbol.txt
10
123
45#6
Stop
EOF

cat << 'EOF' > tests/input_task10_err_empty_stop.txt
10
Stop
EOF

cat << 'EOF' > tests/input_task10_err_no_stop_eof.txt
10
12
34
EOF

cat << 'EOF' > tests/input_task10_err_overflow_number.txt
10
99999999999999999999999999999999999999999999999999999999999999999999
Stop
EOF

cat << 'EOF' > tests/input_task10_err_overflow_sum.txt
10
9000000000000000000
9000000000000000000
Stop
EOF