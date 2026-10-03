#!/bin/bash

gcc -std=c99 -Wall -Wextra main.c -o a.out || { echo "Ошибка компиляции!"; exit 1; }

echo "================ СТАРТ ТЕСТОВ ЛАБОРАТОРНОЙ 9 ================"

for file in tests/args_task09_*; do
    [ -e "$file" ] || continue
    args=$(tr -d '\r' < "$file")
    
    echo "========================================"
    echo "Тестовый файл: $file"
    echo "Команда:       ./a.out $args"
    echo "Вывод программы:"
    ./a.out $args
    exit_code=$?
    echo "Код возврата:  $exit_code"
    echo ""
done

echo "================ ВСЕ ТЕСТЫ ВЫПОЛНЕНЫ ================"