#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>

enum status_code {
    STATUS_OK = 0,
    ERR_INVALID_INPUT,
    ERR_OVERFLOW
};

enum prime_status {
    STATUS_NEITHER = 0,
    STATUS_PRIME,
    STATUS_COMPOSITE
};

enum status_code parse_long_long(const char *str, long long *result) {
    if (!str || !result || *str == '\0') {
        return ERR_INVALID_INPUT;
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

    unsigned long long limit = (sign == 1) ? (unsigned long long)LLONG_MAX : ((unsigned long long)LLONG_MAX + 1ULL);
    unsigned long long uval = 0;

    while (str[i] != '\0') {
        int digit = str[i] - '0';

        if (uval > (limit - digit) / 10) {
            return ERR_OVERFLOW;
        }

        uval = uval * 10 + digit;
        i++;
    }

    if (sign == -1) {
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

int is_flag(const char *str) {
    if (!str) {
        return 0;
    }
    if ((str[0] == '-' || str[0] == '/') && str[1] != '\0' && str[2] == '\0') {
        char c = str[1];
        if (c == 'h' || c == 'p' || c == 's' || c == 'e' || c == 'a' || c == 'f') {
            return 1;
        }
    }
    return 0;
}

enum status_code find_multiples(long long x, int *multiples, int *count) {
    if (!multiples || !count) {
        return ERR_INVALID_INPUT;
    }
    if (x <= 0) {
        return ERR_INVALID_INPUT;
    }

    *count = 0;
    if (x > 100) {
        return STATUS_OK;
    }

    for (long long i = x; i <= 100; i += x) {
        multiples[(*count)++] = (int)i;
    }

    return STATUS_OK;
}

enum status_code check_prime(long long x, enum prime_status *status) {
    if (!status) {
        return ERR_INVALID_INPUT;
    }
    if (x <= 1) {
        *status = STATUS_NEITHER;
        return STATUS_OK;
    }
    if (x == 2) {
        *status = STATUS_PRIME;
        return STATUS_OK;
    }
    if (x % 2 == 0) {
        *status = STATUS_COMPOSITE;
        return STATUS_OK;
    }

    for (long long i = 3; i <= x / i; i += 2) {
        if (x % i == 0) {
            *status = STATUS_COMPOSITE;
            return STATUS_OK;
        }
    }

    *status = STATUS_PRIME;
    return STATUS_OK;
}

enum status_code get_hex_digits(long long x, int *is_negative, char *digits, int *digit_count) {
    if (!is_negative || !digits || !digit_count) {
        return ERR_INVALID_INPUT;
    }

    unsigned long long u;
    if (x < 0) {
        *is_negative = 1;
        u = 0ULL - (unsigned long long)x;
    } else {
        *is_negative = 0;
        u = (unsigned long long)x;
    }

    char temp[32];
    int temp_len = 0;

    if (u == 0) {
        temp[temp_len++] = '0';
    } else {
        while (u > 0) {
            int rem = (int)(u % 16);
            if (rem < 10) {
                temp[temp_len++] = (char)('0' + rem);
            } else {
                temp[temp_len++] = (char)('A' + (rem - 10));
            }
            u /= 16;
        }
    }

    for (int i = 0; i < temp_len; ++i) {
        digits[i] = temp[temp_len - 1 - i];
    }
    digits[temp_len] = '\0';
    *digit_count = temp_len;

    return STATUS_OK;
}

enum status_code compute_powers(int max_exp, long long table[10][10]) {
    if (!table || max_exp < 1 || max_exp > 10) {
        return ERR_INVALID_INPUT;
    }

    for (int b = 1; b <= 10; ++b) {
        long long val = 1;
        for (int p = 1; p <= max_exp; ++p) {
            val *= b;
            table[b - 1][p - 1] = val;
        }
    }

    return STATUS_OK;
}

enum status_code compute_sum_natural(long long x, long long *result) {
    if (!result) {
        return ERR_INVALID_INPUT;
    }
    if (x < 1) {
        return ERR_INVALID_INPUT;
    }

    if (x % 2 == 0) {
        long long half = x / 2;
        if (LLONG_MAX / half < (x + 1)) {
            return ERR_OVERFLOW;
        }
        *result = half * (x + 1);
    } else {
        long long half = (x + 1) / 2;
        if (LLONG_MAX / half < x) {
            return ERR_OVERFLOW;
        }
        *result = x * half;
    }

    return STATUS_OK;
}

enum status_code compute_factorial(long long x, unsigned long long *result) {
    if (!result) {
        return ERR_INVALID_INPUT;
    }
    if (x < 0) {
        return ERR_INVALID_INPUT;
    }
    if (x > 20) {
        return ERR_OVERFLOW;
    }

    unsigned long long res = 1;
    for (long long i = 2; i <= x; ++i) {
        res *= (unsigned long long)i;
    }

    *result = res;
    return STATUS_OK;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Ошибка: неверное количество аргументов.\n");
        printf("Использование: %s <число> <флаг>  ИЛИ  %s <флаг> <число>\n", argv[0], argv[0]);
        printf("Флаги: -h, -p, -s, -e, -a, -f (допускается префикс '/' вместо '-')\n");
        return 1;
    }

    const char *flag_str = NULL;
    const char *num_str = NULL;

    int arg1_is_flag = is_flag(argv[1]);
    int arg2_is_flag = is_flag(argv[2]);

    if (arg1_is_flag && !arg2_is_flag) {
        flag_str = argv[1];
        num_str = argv[2];
    } else if (!arg1_is_flag && arg2_is_flag) {
        flag_str = argv[2];
        num_str = argv[1];
    } else if (arg1_is_flag && arg2_is_flag) {
        printf("Ошибка: передано два флага. Требуется передать ровно одно число и один флаг.\n");
        return 1;
    } else {
        printf("Ошибка: не указан корректный флаг действия (-h, -p, -s, -e, -a, -f или с префиксом '/').\n");
        return 1;
    }

    long long x = 0;
    enum status_code parse_status = parse_long_long(num_str, &x);
    if (parse_status == ERR_INVALID_INPUT) {
        printf("Ошибка: аргумент '%s' не является корректным целым числом.\n", num_str);
        return 1;
    } else if (parse_status == ERR_OVERFLOW) {
        printf("Ошибка: число '%s' выходит за пределы диапазона допустимых значений 64-битного числа.\n", num_str);
        return 1;
    }

    char flag = flag_str[1];

    switch (flag) {
        case 'h': {
            int multiples[100];
            int count = 0;
            enum status_code status = find_multiples(x, multiples, &count);
            if (status == ERR_INVALID_INPUT) {
                printf("Ошибка: для флага -h число x должно быть натуральным (x > 0).\n");
                return 1;
            }
            if (count == 0) {
                printf("В диапазоне [1..100] нет натуральных чисел, кратных %lld.\n", x);
            } else {
                printf("Натуральные числа в пределах 100, кратные %lld:\n", x);
                for (int i = 0; i < count; ++i) {
                    printf("%d%c", multiples[i], (i + 1 < count) ? ' ' : '\n');
                }
            }
            break;
        }

        case 'p': {
            enum prime_status p_status;
            check_prime(x, &p_status);
            if (p_status == STATUS_PRIME) {
                printf("Число %lld является простым.\n", x);
            } else if (p_status == STATUS_COMPOSITE) {
                printf("Число %lld является составным.\n", x);
            } else {
                printf("Число %lld не является ни простым, ни составным (определено для чисел больше 1).\n", x);
            }
            break;
        }

        case 's': {
            int is_negative = 0;
            char digits[32];
            int count = 0;
            get_hex_digits(x, &is_negative, digits, &count);
            printf("Шестнадцатеричные цифры числа %lld (от старших к младшим):\n", x);
            if (is_negative) {
                printf("- ");
            }
            for (int i = 0; i < count; ++i) {
                printf("%c%c", digits[i], (i + 1 < count) ? ' ' : '\n');
            }
            break;
        }

        case 'e': {
            long long table[10][10];
            enum status_code status = compute_powers((int)x, table);
            if (status == ERR_INVALID_INPUT) {
                printf("Ошибка: для флага -e показатель степени x должен лежать в диапазоне от 1 до 10 включительно.\n");
                return 1;
            }
            printf("Таблица степеней для оснований [1..10] и показателей [1..%lld]:\n", x);
            printf("%-10s", "Основание");
            for (int p = 1; p <= (int)x; ++p) {
                printf(" | ^%-10d", p);
            }
            printf("\n");
            for (int b = 1; b <= 10; ++b) {
                printf("%-10d", b);
                for (int p = 1; p <= (int)x; ++p) {
                    printf(" | %-11lld", table[b - 1][p - 1]);
                }
                printf("\n");
            }
            break;
        }

        case 'a': {
            long long sum = 0;
            enum status_code status = compute_sum_natural(x, &sum);
            if (status == ERR_INVALID_INPUT) {
                printf("Ошибка: для флага -a число x должно быть натуральным (x >= 1).\n");
                return 1;
            } else if (status == ERR_OVERFLOW) {
                printf("Ошибка: сумма натуральных чисел для x = %lld вызывает переполнение диапазона long long.\n", x);
                return 1;
            }
            printf("Сумма всех натуральных чисел от 1 до %lld равна %lld.\n", x, sum);
            break;
        }

        case 'f': {
            unsigned long long fact = 0;
            enum status_code status = compute_factorial(x, &fact);
            if (status == ERR_INVALID_INPUT) {
                printf("Ошибка: факториал определен только для неотрицательных целых чисел (x >= 0).\n");
                return 1;
            } else if (status == ERR_OVERFLOW) {
                printf("Ошибка: факториал для x = %lld вызывает переполнение 64-битного целого (максимум x = 20).\n", x);
                return 1;
            }
            printf("Факториал %lld! равен %llu.\n", x, fact);
            break;
        }

        default:
            printf("Ошибка: непредвиденный флаг.\n");
            return 1;
    }

    return 0;
}