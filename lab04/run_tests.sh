#!/bin/bash

gcc -std=c99 -Wall -Wextra main.c -o a.out || { echo "Ошибка компиляции!"; exit 1; }

rm -f tests/out* tests/custom_out*

echo "================ СТАРТ ТЕСТОВ ================"

for file in tests/args_task04_*; do
    [ -e "$file" ] || continue
    args=$(tr -d '\r' < "$file")
    
    echo "========================================"
    echo "Тестовый файл: $file"
    echo "Команда:       ./a.out $args"
    
    read -r flag in_f out_f _ <<< "$args"
    target_file=""
    
    if [ -n "$out_f" ]; then
        target_file="$out_f"
    elif [ -n "$in_f" ]; then
        dir=$(dirname "$in_f")
        base=$(basename "$in_f")
        if [ "$dir" = "." ]; then
            target_file="out$base"
        else
            target_file="$dir/out$base"
        fi
    fi
    
    if [ -n "$target_file" ] && [ "$target_file" != "$in_f" ] && [ "$target_file" != "./$in_f" ]; then
        rm -f "$target_file"
    fi
    
    echo "Вывод программы:"
    ./a.out $args
    exit_code=$?
    echo "Код возврата:  $exit_code"
    
    if [ $exit_code -eq 0 ] && [ -n "$target_file" ] && [ -f "$target_file" ]; then
        echo ""
        echo "Содержимое созданного файла ($target_file):"
        echo "--- НАЧАЛО ФАЙЛА ---"
        cat "$target_file"
        echo ""
        echo "--- КОНЕЦ ФАЙЛА ---"
    fi
    echo ""
done

echo "================ ВСЕ ТЕСТЫ ВЫПОЛНЕНЫ ================"