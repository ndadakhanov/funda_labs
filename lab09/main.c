#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <time.h>

#define FIXED_SIZE 15

enum status_code {
    STATUS_OK = 0,
    ERR_INVALID_INPUT,
    ERR_OVERFLOW,
    ERR_MEMORY_ALLOCATION
};

enum status_code parse_int(const char *str, int *result) {
    if (!str || !result || *str == '\0') {
        return ERR_INVALID_INPUT;
    }

    size_t len = strlen(str);
    if (len == 0 || len > 64) {
        return ERR_INVALID_INPUT;
    }

    for (size_t k = 0; k < len; k++) {
        if (isspace((unsigned char)str[k])) {
            return ERR_INVALID_INPUT;
        }
    }

    int i = 0;
    int sign = 1;
    if (str[i] == '+') {
        i++;
    } else if (str[i] == '-') {
        sign = -1;
        i++;
    }

    if (str[i] == '\0') {
        return ERR_INVALID_INPUT;
    }

    for (int j = i; str[j] != '\0'; j++) {
        if (!isdigit((unsigned char)str[j])) {
            return ERR_INVALID_INPUT;
        }
    }

    while (str[i] == '0') {
        i++;
    }

    if (str[i] == '\0') {
        *result = 0;
        return STATUS_OK;
    }

    unsigned int limit = (sign == 1) ? (unsigned int)INT_MAX : ((unsigned int)INT_MAX + 1U);
    unsigned int uval = 0;

    while (str[i] != '\0') {
        int digit = str[i] - '0';

        if (uval > (limit - (unsigned int)digit) / 10U) {
            return ERR_OVERFLOW;
        }

        uval = uval * 10U + (unsigned int)digit;
        i++;
    }

    if (sign == -1) {
        if (uval == (unsigned int)INT_MAX + 1U) {
            *result = INT_MIN;
        } else {
            *result = -(int)uval;
        }
    } else {
        *result = (int)uval;
    }

    return STATUS_OK;
}

enum status_code fill_array_random(int *arr, const size_t size, const int a, const int b) {
    if (!arr || size == 0 || a > b) {
        return ERR_INVALID_INPUT;
    }

    long long range = (long long)b - (long long)a + 1LL;
    if (range <= 0 || range > (long long)RAND_MAX + 1LL) {
        return ERR_INVALID_INPUT;
    }

    for (size_t i = 0; i < size; i++) {
        arr[i] = a + (int)(rand() % (int)range);
    }

    return STATUS_OK;
}

enum status_code swap_min_max(int *arr, const size_t size, size_t *min_idx_out, size_t *max_idx_out) {
    if (!arr || size == 0 || !min_idx_out || !max_idx_out) {
        return ERR_INVALID_INPUT;
    }

    size_t min_idx = 0;
    size_t max_idx = 0;

    for (size_t i = 1; i < size; i++) {
        if (arr[i] < arr[min_idx]) {
            min_idx = i;
        }
        if (arr[i] > arr[max_idx]) {
            max_idx = i;
        }
    }

    int temp = arr[min_idx];
    arr[min_idx] = arr[max_idx];
    arr[max_idx] = temp;

    *min_idx_out = min_idx;
    *max_idx_out = max_idx;

    return STATUS_OK;
}

enum status_code build_array_c(const int *a, const size_t size_a, const int *b, const size_t size_b, int *c) {
    if (!a || !b || !c || size_a == 0 || size_b == 0) {
        return ERR_INVALID_INPUT;
    }

    for (size_t i = 0; i < size_a; i++) {
        int closest = b[0];
        int min_diff = abs(a[i] - b[0]);

        for (size_t j = 1; j < size_b; j++) {
            int diff = abs(a[i] - b[j]);
            if (diff < min_diff) {
                min_diff = diff;
                closest = b[j];
                if (min_diff == 0) {
                    break;
                }
            }
        }

        c[i] = a[i] + closest;
    }

    return STATUS_OK;
}

void print_array(const char *name, const int *arr, const size_t size) {
    printf("%s (размер %zu):\n[ ", name, size);
    for (size_t i = 0; i < size; i++) {
        printf("%d%s", arr[i], (i + 1 < size) ? ", " : " ]\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Ошибка: неверное количество аргументов.\n");
        printf("Использование: %s <a> <b>\n", argv[0]);
        return 1;
    }

    int a = 0;
    int b = 0;

    enum status_code sc = parse_int(argv[1], &a);
    if (sc == ERR_OVERFLOW) {
        printf("Ошибка: значение параметра a выходит за пределы диапазона int.\n");
        return 1;
    } else if (sc != STATUS_OK) {
        printf("Ошибка: параметр a '%s' не является корректным целым числом.\n", argv[1]);
        return 1;
    }

    sc = parse_int(argv[2], &b);
    if (sc == ERR_OVERFLOW) {
        printf("Ошибка: значение параметра b выходит за пределы диапазона int.\n");
        return 1;
    } else if (sc != STATUS_OK) {
        printf("Ошибка: параметр b '%s' не является корректным целым числом.\n", argv[2]);
        return 1;
    }

    if (a > b) {
        printf("Ошибка: левая граница диапазона a (%d) не может быть больше правой границы b (%d).\n", a, b);
        return 1;
    }

    srand((unsigned int)time(NULL));

    printf("================ ЧАСТЬ 1: ФИКСИРОВАННЫЙ МАССИВ ================\n");
    printf("Диапазон генерации: [%d..%d], фиксированный размер: %d\n", a, b, FIXED_SIZE);

    int fixed_arr[FIXED_SIZE];
    sc = fill_array_random(fixed_arr, FIXED_SIZE, a, b);
    if (sc != STATUS_OK) {
        printf("Ошибка при заполнении массива.\n");
        return 1;
    }

    print_array("Исходный массив", fixed_arr, FIXED_SIZE);

    size_t min_pos = 0;
    size_t max_pos = 0;
    sc = swap_min_max(fixed_arr, FIXED_SIZE, &min_pos, &max_pos);
    if (sc != STATUS_OK) {
        printf("Ошибка при поиске и обмене экстремумов.\n");
        return 1;
    }

    printf("Минимум найден на позиции [%zu] (значение: %d)\n", min_pos, fixed_arr[max_pos]);
    printf("Максимум найден на позиции [%zu] (значение: %d)\n", max_pos, fixed_arr[min_pos]);
    printf("Элементы успешно поменяны местами за 1 проход.\n");

    print_array("Массив после обмена", fixed_arr, FIXED_SIZE);

    printf("\n================ ЧАСТЬ 2: ДИНАМИЧЕСКИЕ МАССИВЫ ================\n");

    size_t size_a = (size_t)(10 + rand() % (10000 - 10 + 1));
    size_t size_b = (size_t)(10 + rand() % (10000 - 10 + 1));

    printf("Сгенерированы размеры массивов: |A| = %zu, |B| = %zu\n", size_a, size_b);
    printf("Диапазон значений для A и B: [-1000..1000]\n");

    int *arr_a = (int *)malloc(size_a * sizeof(int));
    int *arr_b = (int *)malloc(size_b * sizeof(int));
    int *arr_c = (int *)malloc(size_a * sizeof(int));

    if (!arr_a || !arr_b || !arr_c) {
        printf("Ошибка: не удалось выделить динамическую память.\n");
        free(arr_a);
        free(arr_b);
        free(arr_c);
        return 1;
    }

    sc = fill_array_random(arr_a, size_a, -1000, 1000);
    if (sc != STATUS_OK) {
        free(arr_a);
        free(arr_b);
        free(arr_c);
        return 1;
    }

    sc = fill_array_random(arr_b, size_b, -1000, 1000);
    if (sc != STATUS_OK) {
        free(arr_a);
        free(arr_b);
        free(arr_c);
        return 1;
    }

    sc = build_array_c(arr_a, size_a, arr_b, size_b, arr_c);
    if (sc != STATUS_OK) {
        printf("Ошибка при формировании массива C.\n");
        free(arr_a);
        free(arr_b);
        free(arr_c);
        return 1;
    }

    printf("Массив C (размер %zu) успешно сформирован.\n", size_a);
    printf("Первые 10 элементов сформированного массива C:\n");
    size_t preview_count = (size_a < 10) ? size_a : 10;
    for (size_t i = 0; i < preview_count; i++) {
        int added_val = arr_c[i] - arr_a[i];
        printf("  C[%zu] = A[%zu] (%d) + closest(B) (%d) = %d\n", i, i, arr_a[i], added_val, arr_c[i]);
    }

    free(arr_a);
    free(arr_b);
    free(arr_c);

    printf("Динамическая память всех массивов успешно освобождена.\n");
    return 0;
}