#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

enum status_code {
    STATUS_OK = 0,
    ERR_INVALID_INPUT,
    ERR_OVERFLOW,
    ERR_NO_NUMBERS,
    ERR_MEMORY_ALLOCATION
};

int char_to_digit(const char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'A' && c <= 'Z') {
        return c - 'A' + 10;
    }
    return -1;
}

char digit_to_char(const int d) {
    if (d >= 0 && d <= 9) {
        return (char)('0' + d);
    }
    return (char)('A' + (d - 10));
}

enum status_code read_next_token(FILE *in, char **token, int *has_token) {
    if (!in || !token || !has_token) {
        return ERR_INVALID_INPUT;
    }

    int c = 0;
    while ((c = fgetc(in)) != EOF && isspace((unsigned char)c)) {
    }

    if (c == EOF) {
        *has_token = 0;
        *token = NULL;
        return STATUS_OK;
    }

    size_t capacity = 16;
    size_t length = 0;
    char *buf = (char *)malloc(capacity);
    if (!buf) {
        return ERR_MEMORY_ALLOCATION;
    }

    buf[length++] = (char)c;

    while ((c = fgetc(in)) != EOF && !isspace((unsigned char)c)) {
        if (length + 1 >= capacity) {
            capacity *= 2;
            char *new_buf = (char *)realloc(buf, capacity);
            if (!new_buf) {
                free(buf);
                return ERR_MEMORY_ALLOCATION;
            }
            buf = new_buf;
        }
        buf[length++] = (char)c;
    }

    buf[length] = '\0';
    *token = buf;
    *has_token = 1;

    return STATUS_OK;
}

enum status_code parse_base(const char *str, int *base) {
    if (!str || !base || *str == '\0') {
        return ERR_INVALID_INPUT;
    }

    int i = 0;
    if (str[i] == '+') {
        i++;
    }
    if (str[i] == '\0') {
        return ERR_INVALID_INPUT;
    }

    int val = 0;
    while (str[i] != '\0') {
        if (!isdigit((unsigned char)str[i])) {
            return ERR_INVALID_INPUT;
        }
        int d = str[i] - '0';
        if (val > (INT_MAX - d) / 10) {
            return ERR_OVERFLOW;
        }
        val = val * 10 + d;
        i++;
    }

    if (val < 2 || val > 36) {
        return ERR_INVALID_INPUT;
    }

    *base = val;
    return STATUS_OK;
}

enum status_code parse_number_in_base(const char *str, const int base, long long *result, unsigned long long *abs_val) {
    if (!str || !result || !abs_val || base < 2 || base > 36 || *str == '\0') {
        return ERR_INVALID_INPUT;
    }

    int is_neg = 0;
    const char *p = str;

    if (*p == '+') {
        p++;
    } else if (*p == '-') {
        is_neg = 1;
        p++;
    }

    if (*p == '\0') {
        return ERR_INVALID_INPUT;
    }

    while (*p == '0') {
        p++;
    }

    if (*p == '\0') {
        *result = 0;
        *abs_val = 0;
        return STATUS_OK;
    }

    unsigned long long limit = is_neg ? ((unsigned long long)LLONG_MAX + 1ULL) : (unsigned long long)LLONG_MAX;
    unsigned long long uval = 0;

    while (*p != '\0') {
        int d = char_to_digit(*p);
        if (d < 0 || d >= base) {
            return ERR_INVALID_INPUT;
        }

        if (uval > (limit - (unsigned long long)d) / (unsigned long long)base) {
            return ERR_OVERFLOW;
        }

        uval = uval * (unsigned long long)base + (unsigned long long)d;
        p++;
    }

    *abs_val = uval;
    if (is_neg) {
        if (uval == (unsigned long long)LLONG_MAX + 1ULL) {
            *result = LLONG_MIN;
        } else {
            *result = -(long long)uval;
        }
    } else {
        *result = (long long)uval;
    }

    return STATUS_OK;
}

enum status_code safe_add(const long long a, const long long b, long long *res) {
    if (!res) {
        return ERR_INVALID_INPUT;
    }

    if (b > 0 && a > LLONG_MAX - b) {
        return ERR_OVERFLOW;
    }
    if (b < 0 && a < LLONG_MIN - b) {
        return ERR_OVERFLOW;
    }

    *res = a + b;
    return STATUS_OK;
}

enum status_code convert_to_base_str(const long long num, const int base, char *buffer, const size_t buf_size) {
    if (!buffer || buf_size < 70 || base < 2 || base > 36) {
        return ERR_INVALID_INPUT;
    }

    if (num == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return STATUS_OK;
    }

    int is_neg = 0;
    unsigned long long uval = 0;

    if (num < 0) {
        is_neg = 1;
        uval = 0ULL - (unsigned long long)num;
    } else {
        uval = (unsigned long long)num;
    }

    char temp[68];
    int len = 0;

    while (uval > 0) {
        int rem = (int)(uval % (unsigned long long)base);
        temp[len++] = digit_to_char(rem);
        uval /= (unsigned long long)base;
    }

    size_t out_idx = 0;
    if (is_neg) {
        buffer[out_idx++] = '-';
    }

    for (int i = len - 1; i >= 0; i--) {
        buffer[out_idx++] = temp[i];
    }
    buffer[out_idx] = '\0';

    return STATUS_OK;
}

int main(void) {
    char *token = NULL;
    int has_token = 0;

    enum status_code sc = read_next_token(stdin, &token, &has_token);
    if (sc != STATUS_OK || !has_token) {
        printf("Ошибка: не удалось прочитать основание системы счисления.\n");
        free(token);
        return 1;
    }

    int base = 0;
    sc = parse_base(token, &base);
    free(token);

    if (sc == ERR_OVERFLOW) {
        printf("Ошибка: основание системы счисления вызывает переполнение.\n");
        return 1;
    } else if (sc != STATUS_OK) {
        printf("Ошибка: основание системы счисления должно быть целым числом в диапазоне [2..36].\n");
        return 1;
    }

    long long sum = 0;
    long long max_elem = 0;
    unsigned long long max_abs_val = 0;
    int numbers_count = 0;

    while (1) {
        token = NULL;
        has_token = 0;

        sc = read_next_token(stdin, &token, &has_token);
        if (sc != STATUS_OK) {
            printf("Ошибка чтения из входного потока.\n");
            free(token);
            return 1;
        }

        if (!has_token || !token) {
            printf("Ошибка: ввод завершился до появления команды Stop.\n");
            return 1;
        }

        if (strcmp(token, "Stop") == 0) {
            free(token);
            break;
        }

        long long current_val = 0;
        unsigned long long current_abs = 0;

        sc = parse_number_in_base(token, base, &current_val, &current_abs);
        if (sc == ERR_OVERFLOW) {
            printf("Ошибка: число '%s' вызывает переполнение 64-битного целого.\n", token);
            free(token);
            return 1;
        } else if (sc != STATUS_OK) {
            printf("Ошибка: лексема '%s' содержит недопустимые цифры для основания %d (буквы должны быть строго ПРОПИСНЫМИ).\n", token, base);
            free(token);
            return 1;
        }

        free(token);

        if (numbers_count == 0) {
            max_elem = current_val;
            max_abs_val = current_abs;
        } else {
            if (current_abs > max_abs_val) {
                max_abs_val = current_abs;
                max_elem = current_val;
            }
        }

        sc = safe_add(sum, current_val, &sum);
        if (sc == ERR_OVERFLOW) {
            printf("Ошибка: сумма чисел вызывает переполнение диапазона long long.\n");
            return 1;
        }

        numbers_count++;
    }

    if (numbers_count == 0) {
        printf("Ошибка: не было введено ни одного числа перед словом Stop.\n");
        return 1;
    }

    printf("================ РЕЗУЛЬТАТЫ ВЫЧИСЛЕНИЙ ================\n");
    printf("Обработано чисел: %d (в с/с %d)\n\n", numbers_count, base);

    printf("1. Максимальное по модулю число: %lld\n", max_elem);
    printf("2. Сумма всех введённых чисел:  %lld\n\n", sum);

    int target_bases[4] = {9, 18, 27, 36};
    char buf[80];

    printf("--- Представление максимального по модулю числа (%lld) ---\n", max_elem);
    for (int i = 0; i < 4; i++) {
        convert_to_base_str(max_elem, target_bases[i], buf, sizeof(buf));
        printf("  Основание %2d: %s\n", target_bases[i], buf);
    }

    printf("\n--- Представление суммы чисел (%lld) ---\n", sum);
    for (int i = 0; i < 4; i++) {
        convert_to_base_str(sum, target_bases[i], buf, sizeof(buf));
        printf("  Основание %2d: %s\n", target_bases[i], buf);
    }

    return 0;
}