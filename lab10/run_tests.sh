#!/bin/bash

gcc -std=c99 -Wall -Wextra main.c -o a.out || { echo "Ошибка компиляции!"; exit 1; }

echo "================ СТАРТ ТЕСТОВ ЛАБОРАТОРНОЙ 10 ================"

for file in tests/input_task10_*; do
    [ -e "$file" ] || continue
    
    echo "========================================"
    echo "Тестовый файл: $file"
    echo "Команда:       ./a.out < $file"
    echo "Вывод программы:"
    ./a.out < "$file"
    exit_code=$?
    echo "Код возврата:  $exit_code"
    echo ""
done

echo "================ ВСЕ ТЕСТЫ ВЫПОЛНЕНЫ ================"