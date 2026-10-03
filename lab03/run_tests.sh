#!/bin/bash

gcc -std=c99 -Wall -Wextra main.c -lm -o a.out || { echo "Ошибка компиляции!"; exit 1; }

echo "================ СТАРТ ТЕСТОВ ================"

for file in tests/args_task03_q_*; do
    [ -e "$file" ] || continue
    args=$(tr -d '\r' < "$file")
    echo "----------------------------------------"
    echo "Тест:    $file"
    echo "Команда: ./a.out -q $args"
    echo "Вывод:"
    ./a.out -q $args
    echo ""
done

for file in tests/args_task03_m_*; do
    [ -e "$file" ] || continue
    args=$(tr -d '\r' < "$file")
    echo "----------------------------------------"
    echo "Тест:    $file"
    echo "Команда: ./a.out -m $args"
    echo "Вывод:"
    ./a.out -m $args
    echo ""
done

for file in tests/args_task03_t_*; do
    [ -e "$file" ] || continue
    args=$(tr -d '\r' < "$file")
    echo "----------------------------------------"
    echo "Тест:    $file"
    echo "Команда: ./a.out -t $args"
    echo "Вывод:"
    ./a.out -t $args
    echo ""
done

for file in tests/args_task03_err_*; do
    [ -e "$file" ] || continue
    args=$(tr -d '\r' < "$file")
    echo "----------------------------------------"
    echo "Тест:    $file"
    flag="-q"
    if [[ "$file" == *"_zero"* || "$file" == *"_overflow"* ]]; then
        flag="-m"
    fi
    echo "Команда: ./a.out $flag $args"
    echo "Вывод:"
    ./a.out $flag $args
    echo ""
done

echo "================ ВСЕ ТЕСТЫ ВЫПОЛНЕНЫ ================"