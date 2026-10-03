#!/bin/bash

gcc -std=c99 -Wall -Wextra main.c -o a.out || { echo "Ошибка компиляции!"; exit 1; }

rm -f tests/out_task08_* tests/out_bad* tests/out_overflow*

echo "================ СТАРТ ТЕСТОВ ЛАБОРАТОРНОЙ 8 ================"

for file in tests/args_task08_*; do
    [ -e "$file" ] || continue
    args=$(tr -d '\r' < "$file")
    
    echo "========================================"
    echo "Тестовый файл: $file"
    echo "Команда:       ./a.out $args"
    
    out_target=""
    for word in $args; do
        out_target="$word"
    done
    
    if [ -n "$out_target" ] && [[ "$out_target" == *"out_"* ]]; then
        rm -f "$out_target"
    fi
    
    echo "Вывод программы:"
    ./a.out $args
    exit_code=$?
    echo "Код возврата:  $exit_code"
    
    if [ $exit_code -eq 0 ] && [ -n "$out_target" ] && [ -f "$out_target" ]; then
        echo ""
        echo "Содержимое созданного файла ($out_target):"
        echo "--- НАЧАЛО ФАЙЛА ---"
        cat "$out_target"
        echo "--- КОНЕЦ ФАЙЛА ---"
    fi
    echo ""
done

echo "================ ВСЕ ТЕСТЫ ВЫПОЛНЕНЫ ================"