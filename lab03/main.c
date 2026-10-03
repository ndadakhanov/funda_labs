#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <limits.h>
#include <float.h>

enum status_code {
    STATUS_OK = 0,
    ERR_INVALID_INPUT,
    ERR_OVERFLOW
};

enum quad_solution_type {
    QUAD_INF_ROOTS = 0,
    QUAD_NO_ROOTS,
    QUAD_ONE_ROOT,
    QUAD_TWO_ROOTS
};

struct quad_solution {
    double a;
    double b;
    double c;
    enum quad_solution_type type;
    double root1;
    double root2;
};

enum status_code parse_double(const char *str, double *result) {
    if (!str || !result || *str == '\0') {
        return ERR_INVALID_INPUT;
    }

    size_t len = strlen(str);
    if (len == 0 || len > 64) {
        return ERR_INVALID_INPUT;
    }

    for (size_t i = 0; i < len; i++) {
        if (isspace((unsigned char)str[i])) {
            return ERR_INVALID_INPUT;
        }
    }

    char *endptr = NULL;
    double val = strtod(str, &endptr);

    if (endptr == str || *endptr != '\0') {
        return ERR_INVALID_INPUT;
    }

    if (isnan(val) || isinf(val)) {
        return ERR_OVERFLOW;
    }

    *result = val;
    return STATUS_OK;
}

enum status_code parse_epsilon(const char *str, double *result) {
    if (!str || !result) {
        return ERR_INVALID_INPUT;
    }

    double val = 0.0;
    enum status_code st = parse_double(str, &val);
    if (st != STATUS_OK) {
        return st;
    }
    if (val <= 0.0 || val >= 1.0 || val < DBL_EPSILON) {
        return ERR_INVALID_INPUT;
    }

    *result = val;
    return STATUS_OK;
}

enum status_code parse_long_long(const char *str, long long *result) {
    if (!str || !result || *str == '\0') {
        return ERR_INVALID_INPUT;
    }

    for (size_t k = 0; str[k] != '\0'; k++) {
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

void solve_single_equation(const double a, const double b, const double c, const double eps,
                           enum quad_solution_type *type, double *r1, double *r2) {
    if (fabs(a) < eps) {
        if (fabs(b) < eps) {
            if (fabs(c) < eps) {
                *type = QUAD_INF_ROOTS;
            } else {
                *type = QUAD_NO_ROOTS;
            }
        } else {
            *type = QUAD_ONE_ROOT;
            *r1 = -c / b;
            if (fabs(*r1) < eps) {
                *r1 = 0.0;
            }
        }
    } else {
        double d = b * b - 4.0 * a * c;
        if (fabs(d) < eps) {
            *type = QUAD_ONE_ROOT;
            *r1 = -b / (2.0 * a);
            if (fabs(*r1) < eps) {
                *r1 = 0.0;
            }
        } else if (d > eps) {
            *type = QUAD_TWO_ROOTS;
            double sqrt_d = sqrt(d);
            *r1 = (-b - sqrt_d) / (2.0 * a);
            *r2 = (-b + sqrt_d) / (2.0 * a);
            if (fabs(*r1) < eps) *r1 = 0.0;
            if (fabs(*r2) < eps) *r2 = 0.0;
        } else {
            *type = QUAD_NO_ROOTS;
        }
    }
}

enum status_code solve_quadratic_permutations(const double c1, const double c2, const double c3, const double eps,
                                              struct quad_solution *solutions, int *count) {
    if (!solutions || !count || eps <= 0.0) {
        return ERR_INVALID_INPUT;
    }

    double perms[6][3] = {
        {c1, c2, c3},
        {c1, c3, c2},
        {c2, c1, c3},
        {c2, c3, c1},
        {c3, c1, c2},
        {c3, c2, c1}
    };

    *count = 0;

    for (int i = 0; i < 6; ++i) {
        double a = perms[i][0];
        double b = perms[i][1];
        double c = perms[i][2];

        int is_duplicate = 0;
        for (int j = 0; j < *count; ++j) {
            if (fabs(solutions[j].a - a) < eps &&
                fabs(solutions[j].b - b) < eps &&
                fabs(solutions[j].c - c) < eps) {
                is_duplicate = 1;
                break;
            }
        }

        if (!is_duplicate) {
            solutions[*count].a = a;
            solutions[*count].b = b;
            solutions[*count].c = c;
            solutions[*count].root1 = 0.0;
            solutions[*count].root2 = 0.0;
            solve_single_equation(a, b, c, eps,
                                  &solutions[*count].type,
                                  &solutions[*count].root1,
                                  &solutions[*count].root2);
            (*count)++;
        }
    }

    return STATUS_OK;
}

enum status_code check_multiplicity(const long long a, const long long b, int *is_multiple) {
    if (!is_multiple || a == 0 || b == 0) {
        return ERR_INVALID_INPUT;
    }

    if (a == LLONG_MIN && b == -1) {
        *is_multiple = 1;
        return STATUS_OK;
    }

    *is_multiple = (a % b == 0);
    return STATUS_OK;
}

enum status_code check_right_triangle(const double a, const double b, const double c, const double eps, int *is_right) {
    if (!is_right || eps <= 0.0 || a <= eps || b <= eps || c <= eps) {
        return ERR_INVALID_INPUT;
    }

    double x = a;
    double y = b;
    double z = c;

    if (x > y) { double t = x; x = y; y = t; }
    if (y > z) { double t = y; y = z; z = t; }
    if (x > y) { double t = x; x = y; y = t; }

    if (fabs(z - sqrt(x * x + y * y)) < eps) {
        *is_right = 1;
    } else {
        *is_right = 0;
    }

    return STATUS_OK;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Ошибка: не передан флаг действия.\n");
        printf("Использование:\n");
        printf("  %s -q <эпсилон> <a> <b> <c>\n", argv[0]);
        printf("  %s -m <целое1> <целое2>\n", argv[0]);
        printf("  %s -t <эпсилон> <сторона1> <сторона2> <сторона3>\n", argv[0]);
        printf("Допускается использование символа '/' вместо '-'.\n");
        return 1;
    }

    const char *flag_arg = argv[1];
    if ((flag_arg[0] != '-' && flag_arg[0] != '/') || flag_arg[1] == '\0' || flag_arg[2] != '\0') {
        printf("Ошибка: некорректный флаг '%s'. Флаг должен начинаться с '-' или '/' и состоять из одного символа действия.\n", flag_arg);
        return 1;
    }

    char flag = flag_arg[1];

    switch (flag) {
        case 'q': {
            if (argc != 6) {
                printf("Ошибка: для флага '%s' требуется ровно 4 параметра: <эпсилон> <a> <b> <c> (передано: %d).\n", flag_arg, argc - 2);
                return 1;
            }

            double eps = 0.0;
            if (parse_epsilon(argv[2], &eps) != STATUS_OK) {
                printf("Ошибка: параметр эпсилон '%s' некорректен. Требуется вещественное число в интервале (0; 1).\n", argv[2]);
                return 1;
            }

            double a = 0.0, b = 0.0, c = 0.0;
            if (parse_double(argv[3], &a) != STATUS_OK) {
                printf("Ошибка: коэффициент '%s' не является корректным вещественным числом.\n", argv[3]);
                return 1;
            }
            if (parse_double(argv[4], &b) != STATUS_OK) {
                printf("Ошибка: коэффициент '%s' не является корректным вещественным числом.\n", argv[4]);
                return 1;
            }
            if (parse_double(argv[5], &c) != STATUS_OK) {
                printf("Ошибка: коэффициент '%s' не является корректным вещественным числом.\n", argv[5]);
                return 1;
            }

            struct quad_solution solutions[6];
            int count = 0;
            solve_quadratic_permutations(a, b, c, eps, solutions, &count);

            printf("Уникальных перестановок коэффициентов: %d\n", count);
            for (int i = 0; i < count; ++i) {
                printf("[%d] Уравнение: (%.6g)*x^2 + (%.6g)*x + (%.6g) = 0\n",
                       i + 1, solutions[i].a, solutions[i].b, solutions[i].c);
                switch (solutions[i].type) {
                    case QUAD_INF_ROOTS:
                        printf("    Решение: бесконечно много решений (0 = 0).\n");
                        break;
                    case QUAD_NO_ROOTS:
                        printf("    Решение: действительных корней нет.\n");
                        break;
                    case QUAD_ONE_ROOT:
                        printf("    Решение: один корень x = %.10f\n", solutions[i].root1);
                        break;
                    case QUAD_TWO_ROOTS:
                        printf("    Решение: два корня x1 = %.10f, x2 = %.10f\n",
                               solutions[i].root1, solutions[i].root2);
                        break;
                }
            }
            break;
        }

        case 'm': {
            if (argc != 4) {
                printf("Ошибка: для флага '%s' требуется ровно 2 параметра: <целое1> <целое2> (передано: %d).\n", flag_arg, argc - 2);
                return 1;
            }

            long long num1 = 0, num2 = 0;
            enum status_code st1 = parse_long_long(argv[2], &num1);
            if (st1 == ERR_INVALID_INPUT) {
                printf("Ошибка: аргумент '%s' не является целым числом.\n", argv[2]);
                return 1;
            } else if (st1 == ERR_OVERFLOW) {
                printf("Ошибка: аргумент '%s' выходит за пределы диапазона допустимых целых чисел.\n", argv[2]);
                return 1;
            }

            enum status_code st2 = parse_long_long(argv[3], &num2);
            if (st2 == ERR_INVALID_INPUT) {
                printf("Ошибка: аргумент '%s' не является целым числом.\n", argv[3]);
                return 1;
            } else if (st2 == ERR_OVERFLOW) {
                printf("Ошибка: аргумент '%s' выходит за пределы диапазона допустимых целых чисел.\n", argv[3]);
                return 1;
            }

            if (num1 == 0 || num2 == 0) {
                printf("Ошибка: оба числа должны быть строго ненулевыми.\n");
                return 1;
            }

            int is_mult = 0;
            check_multiplicity(num1, num2, &is_mult);
            if (is_mult) {
                printf("Число %lld кратно числу %lld.\n", num1, num2);
            } else {
                printf("Число %lld НЕ кратно числу %lld (остаток: %lld).\n", num1, num2, num1 % num2);
            }
            break;
        }

        case 't': {
            if (argc != 6) {
                printf("Ошибка: для флага '%s' требуется ровно 4 параметра: <эпсилон> <сторона1> <сторона2> <сторона3> (передано: %d).\n", flag_arg, argc - 2);
                return 1;
            }

            double eps = 0.0;
            if (parse_epsilon(argv[2], &eps) != STATUS_OK) {
                printf("Ошибка: параметр эпсилон '%s' некорректен. Требуется вещественное число в интервале (0; 1).\n", argv[2]);
                return 1;
            }

            double s1 = 0.0, s2 = 0.0, s3 = 0.0;
            if (parse_double(argv[3], &s1) != STATUS_OK ||
                parse_double(argv[4], &s2) != STATUS_OK ||
                parse_double(argv[5], &s3) != STATUS_OK) {
                printf("Ошибка: длины сторон треугольника должны быть корректными вещественными числами.\n");
                return 1;
            }

            if (s1 <= eps || s2 <= eps || s3 <= eps) {
                printf("Ошибка: длины сторон должны быть строго положительными числами (больше эпсилон).\n");
                return 1;
            }

            int is_right = 0;
            check_right_triangle(s1, s2, s3, eps, &is_right);
            if (is_right) {
                printf("Числа %.6g, %.6g, %.6g МОГУТ являться длинами сторон прямоугольного треугольника.\n", s1, s2, s3);
            } else {
                printf("Числа %.6g, %.6g, %.6g НЕ МОГУТ являться длинами сторон прямоугольного треугольника.\n", s1, s2, s3);
            }
            break;
        }

        default:
            printf("Ошибка: неизвестный флаг '%s'. Допустимые флаги: -q, -m, -t (или с префиксом '/').\n", flag_arg);
            return 1;
    }

    return 0;
}