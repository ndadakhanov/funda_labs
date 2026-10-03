#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <ctype.h>
#include <limits.h>
#include <float.h>

enum status_code {
    STATUS_OK = 0,
    ERR_INVALID_INPUT,
    ERR_OVERFLOW,
    ERR_CALCULATION,
    ERR_NO_ROOT,
    ERR_MEMORY_ALLOCATION
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

enum status_code parse_int(const char *str, int *result) {
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
    unsigned int limit = (sign == 1) ? (unsigned int)INT_MAX : ((unsigned int)INT_MAX + 1U);
    unsigned int uval = 0;
    while (str[i] != '\0') {
        int digit = str[i] - '0';
        if (uval > (limit - digit) / 10) {
            return ERR_OVERFLOW;
        }
        uval = uval * 10 + digit;
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

enum status_code check_polygon_convex(const double eps, const int count, int *is_convex, ...) {
    if (!is_convex || eps <= 0.0 || count < 3) {
        return ERR_INVALID_INPUT;
    }
    double *x = (double *)malloc(count * sizeof(double));
    double *y = (double *)malloc(count * sizeof(double));
    if (!x || !y) {
        free(x);
        free(y);
        return ERR_MEMORY_ALLOCATION;
    }
    va_list args;
    va_start(args, is_convex);
    for (int i = 0; i < count; i++) {
        x[i] = va_arg(args, double);
        y[i] = va_arg(args, double);
    }
    va_end(args);

    int has_pos = 0;
    int has_neg = 0;
    for (int i = 0; i < count; i++) {
        int p1 = i;
        int p2 = (i + 1) % count;
        int p3 = (i + 2) % count;

        double dx1 = x[p2] - x[p1];
        double dy1 = y[p2] - y[p1];
        double dx2 = x[p3] - x[p2];
        double dy2 = y[p3] - y[p2];

        double cross = dx1 * dy2 - dy1 * dx2;
        if (cross > eps) {
            has_pos = 1;
        } else if (cross < -eps) {
            has_neg = 1;
        }
        if (has_pos && has_neg) {
            break;
        }
    }

    free(x);
    free(y);

    if (has_pos && has_neg) {
        *is_convex = 0;
    } else {
        *is_convex = 1;
    }
    return STATUS_OK;
}

enum status_code evaluate_polynomial(const double x, const int degree, double *result, ...) {
    if (!result || degree < 0) {
        return ERR_INVALID_INPUT;
    }
    va_list args;
    va_start(args, result);
    double val = va_arg(args, double);
    for (int i = 0; i < degree; i++) {
        double coeff = va_arg(args, double);
        val = val * x + coeff;
        if (isnan(val) || isinf(val)) {
            va_end(args);
            return ERR_OVERFLOW;
        }
    }
    va_end(args);
    *result = val;
    return STATUS_OK;
}

int char_to_digit(const char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
    if (c >= 'a' && c <= 'z') return c - 'a' + 10;
    return -1;
}

enum status_code parse_ull_base(const char *str, const int base, unsigned long long *result) {
    if (!str || !result || *str == '\0' || base < 2 || base > 36) {
        return ERR_INVALID_INPUT;
    }
    unsigned long long val = 0;
    int i = 0;
    while (str[i] != '\0') {
        int d = char_to_digit(str[i]);
        if (d < 0 || d >= base) {
            return ERR_INVALID_INPUT;
        }
        if (val > (ULLONG_MAX - d) / (unsigned long long)base) {
            return ERR_OVERFLOW;
        }
        val = val * (unsigned long long)base + d;
        i++;
    }
    *result = val;
    return STATUS_OK;
}

int is_kaprekar_number(const unsigned long long n, const int base) {
    if (n == 0) {
        return 0;
    }
    if (n == 1) {
        return 1;
    }
    if (ULLONG_MAX / n < n) {
        return 0;
    }
    unsigned long long sq = n * n;
    unsigned long long divisor = (unsigned long long)base;

    while (divisor <= sq) {
        unsigned long long r = sq % divisor;
        unsigned long long l = sq / divisor;
        if (r > 0 && l + r == n) {
            return 1;
        }
        if (divisor > ULLONG_MAX / (unsigned long long)base) {
            break;
        }
        divisor *= (unsigned long long)base;
    }
    return 0;
}

enum status_code find_kaprekar_numbers(const int base, const int count,
                                      char ***found_numbers, int *found_count, ...) {
    if (!found_numbers || !found_count || base < 2 || base > 36 || count <= 0) {
        return ERR_INVALID_INPUT;
    }
    char **matched = (char **)malloc(count * sizeof(char *));
    if (!matched) {
        return ERR_MEMORY_ALLOCATION;
    }
    int match_count = 0;

    va_list args;
    va_start(args, found_count);
    for (int i = 0; i < count; i++) {
        const char *s = va_arg(args, const char *);
        unsigned long long val = 0;
        if (parse_ull_base(s, base, &val) == STATUS_OK) {
            if (is_kaprekar_number(val, base)) {
                size_t slen = strlen(s);
                char *dup = (char *)malloc(slen + 1);
                if (dup) {
                    strcpy(dup, s);
                    matched[match_count++] = dup;
                }
            }
        }
    }
    va_end(args);

    *found_numbers = matched;
    *found_count = match_count;
    return STATUS_OK;
}

enum status_code geometric_mean(double *result, const int count, ...) {
    if (!result || count <= 0) {
        return ERR_INVALID_INPUT;
    }
    va_list args;
    va_start(args, count);
    double sum_log = 0.0;
    int has_zero = 0;

    for (int i = 0; i < count; i++) {
        double val = va_arg(args, double);
        if (val < 0.0) {
            va_end(args);
            return ERR_INVALID_INPUT;
        }
        if (val < 1e-15) {
            has_zero = 1;
        } else {
            sum_log += log(val);
        }
    }
    va_end(args);

    if (has_zero) {
        *result = 0.0;
        return STATUS_OK;
    }
    double res = exp(sum_log / (double)count);
    if (isnan(res) || isinf(res)) {
        return ERR_OVERFLOW;
    }
    *result = res;
    return STATUS_OK;
}

enum status_code fast_power(const double base, const int exp, double *result) {
    if (!result) {
        return ERR_INVALID_INPUT;
    }
    if (exp == 0) {
        *result = 1.0;
        return STATUS_OK;
    }
    if (exp < 0) {
        if (fabs(base) < 1e-15) {
            return ERR_CALCULATION;
        }
        if (exp == INT_MIN) {
            double temp = 0.0;
            enum status_code sc = fast_power(base, -(exp + 1), &temp);
            if (sc != STATUS_OK) return sc;
            double res = 1.0 / (base * temp);
            if (isnan(res) || isinf(res)) return ERR_OVERFLOW;
            *result = res;
            return STATUS_OK;
        }
        double temp = 0.0;
        enum status_code sc = fast_power(base, -exp, &temp);
        if (sc != STATUS_OK) return sc;
        double res = 1.0 / temp;
        if (isnan(res) || isinf(res)) return ERR_OVERFLOW;
        *result = res;
        return STATUS_OK;
    }
    if (exp % 2 == 0) {
        double half = 0.0;
        enum status_code sc = fast_power(base, exp / 2, &half);
        if (sc != STATUS_OK) return sc;
        double res = half * half;
        if (isnan(res) || isinf(res)) return ERR_OVERFLOW;
        *result = res;
        return STATUS_OK;
    } else {
        double prev = 0.0;
        enum status_code sc = fast_power(base, exp - 1, &prev);
        if (sc != STATUS_OK) return sc;
        double res = base * prev;
        if (isnan(res) || isinf(res)) return ERR_OVERFLOW;
        *result = res;
        return STATUS_OK;
    }
}

double eq_sqrt2(const double x) {
    return x * x - 2.0;
}

double eq_dottie(const double x) {
    return cos(x) - x;
}

double eq_exp3(const double x) {
    return exp(x) - 3.0;
}

double eq_cubic(const double x) {
    return x * x * x - x - 2.0;
}

enum status_code solve_bisection(const double a, const double b, const double eps,
                                double (*f)(const double), double *root) {
    if (!f || !root || eps <= 0.0 || a >= b) {
        return ERR_INVALID_INPUT;
    }
    double fa = f(a);
    double fb = f(b);
    if (fabs(fa) < eps) {
        *root = a;
        return STATUS_OK;
    }
    if (fabs(fb) < eps) {
        *root = b;
        return STATUS_OK;
    }
    if (fa * fb > 0.0) {
        return ERR_NO_ROOT;
    }

    double left = a;
    double right = b;
    int max_iters = 1000;

    while ((right - left) / 2.0 > eps && max_iters-- > 0) {
        double mid = left + (right - left) / 2.0;
        double fmid = f(mid);
        if (fabs(fmid) < eps) {
            *root = mid;
            return STATUS_OK;
        }
        if (fa * fmid < 0.0) {
            right = mid;
            fb = fmid;
        } else {
            left = mid;
            fa = fmid;
        }
    }
    *root = left + (right - left) / 2.0;
    return STATUS_OK;
}

void run_all_demos(void) {
    printf("================ ДЕМОНСТРАЦИЯ ЛАБОРАТОРНОЙ РАБОТЫ 6 ================\n\n");

    printf("--- Подпункт 1: Проверка многоугольника на выпуклость ---\n");
    int conv = 0;
    check_polygon_convex(1e-6, 4, &conv, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0);
    printf("Квадрат (0,0)-(1,0)-(1,1)-(0,1): %s\n", conv ? "ВЫПУКЛЫЙ" : "НЕВЫПУКЛЫЙ");

    check_polygon_convex(1e-6, 5, &conv, 0.0, 0.0, 2.0, 0.0, 1.0, 1.0, 2.0, 2.0, 0.0, 2.0);
    printf("Стрела (вогнутый 5-угольник): %s\n\n", conv ? "ВЫПУКЛЫЙ" : "НЕВЫПУКЛЫЙ");

    printf("--- Подпункт 2: Значение многочлена в точке (схема Горнера) ---\n");
    double p_val = 0.0;
    evaluate_polynomial(2.0, 2, &p_val, 1.0, 2.0, 1.0);
    printf("P(x) = 1*x^2 + 2*x + 1 в точке x=2.0: %.6f\n", p_val);

    evaluate_polynomial(3.0, 3, &p_val, 2.0, -1.0, 0.0, 5.0);
    printf("P(x) = 2*x^3 - 1*x^2 + 0*x + 5 в точке x=3.0: %.6f\n\n", p_val);

    printf("--- Подпункт 3: Поиск чисел Капрекара ---\n");
    char **kap_found = NULL;
    int kap_cnt = 0;
    find_kaprekar_numbers(10, 6, &kap_found, &kap_cnt, "9", "10", "45", "55", "99", "100");
    printf("Числа Капрекара в системе счисления 10: ");
    for (int i = 0; i < kap_cnt; i++) {
        printf("%s ", kap_found[i]);
        free(kap_found[i]);
    }
    free(kap_found);
    printf("\n\n");

    printf("--- Подпункт 4: Среднее геометрическое чисел ---\n");
    double gm = 0.0;
    geometric_mean(&gm, 3, 1.0, 3.0, 9.0);
    printf("Среднее геометрическое [1.0, 3.0, 9.0]: %.6f\n", gm);

    geometric_mean(&gm, 2, 2.0, 8.0);
    printf("Среднее геометрическое [2.0, 8.0]: %.6f\n\n", gm);

    printf("--- Подпункт 5: Быстрое рекурсивное возведение в степень ---\n");
    double pwr = 0.0;
    fast_power(2.0, 10, &pwr);
    printf("2.0 ^ 10 = %.6f\n", pwr);

    fast_power(2.0, -3, &pwr);
    printf("2.0 ^ (-3) = %.6f\n\n", pwr);

    printf("--- Подпункт 6: Метод дихотомии (поиск корня) ---\n");
    double root = 0.0;
    solve_bisection(1.0, 2.0, 1e-6, eq_sqrt2, &root);
    printf("Корень x^2 - 2 = 0 на [1.0, 2.0]: %.6f\n", root);

    solve_bisection(0.0, 1.0, 1e-6, eq_dottie, &root);
    printf("Корень cos(x) - x = 0 на [0.0, 1.0]: %.6f\n", root);

    solve_bisection(0.0, 2.0, 1e-6, eq_exp3, &root);
    printf("Корень exp(x) - 3 = 0 на [0.0, 2.0]: %.6f\n", root);
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        run_all_demos();
        return 0;
    }

    const char *flag_str = argv[1];
    if ((flag_str[0] != '-' && flag_str[0] != '/') || flag_str[1] == '\0' || flag_str[2] != '\0') {
        printf("Ошибка: некорректный флаг подзадачи '%s'. Допустимы: -1 .. -6 (или с префиксом '/').\n", flag_str);
        return 1;
    }

    char subtask = flag_str[1];
    switch (subtask) {
        case '1': {
            if (argc < 9 || (argc - 3) % 2 != 0) {
                printf("Ошибка: для подзадачи 1 требуется эпсилон и не менее 3 пар координат (x y).\n");
                return 1;
            }
            double eps = 0.0;
            if (parse_double(argv[2], &eps) != STATUS_OK || eps <= 0.0) {
                printf("Ошибка: эпсилон должен быть положительным вещественным числом.\n");
                return 1;
            }
            int count = (argc - 3) / 2;
            if (count > 6) {
                printf("Ошибка: в тестовом режиме CLI поддерживается до 6 вершин.\n");
                return 1;
            }
            double p[12];
            for (int i = 0; i < count * 2; i++) {
                if (parse_double(argv[3 + i], &p[i]) != STATUS_OK) {
                    printf("Ошибка: координата '%s' не является вещественным числом.\n", argv[3 + i]);
                    return 1;
                }
            }
            int is_conv = 0;
            enum status_code sc = STATUS_OK;
            switch (count) {
                case 3: sc = check_polygon_convex(eps, 3, &is_conv, p[0], p[1], p[2], p[3], p[4], p[5]); break;
                case 4: sc = check_polygon_convex(eps, 4, &is_conv, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]); break;
                case 5: sc = check_polygon_convex(eps, 5, &is_conv, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9]); break;
                case 6: sc = check_polygon_convex(eps, 6, &is_conv, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11]); break;
            }
            if (sc != STATUS_OK) {
                printf("Ошибка при вычислении выпуклости многоугольника.\n");
                return 1;
            }
            printf("Результат: многоугольник %s\n", is_conv ? "ВЫПУКЛЫЙ" : "НЕВЫПУКЛЫЙ");
            break;
        }

        case '2': {
            if (argc < 4) {
                printf("Ошибка: для подзадачи 2 требуется <x> <degree> <коэффициенты...>\n");
                return 1;
            }
            double x = 0.0;
            if (parse_double(argv[2], &x) != STATUS_OK) {
                printf("Ошибка: точка x должна быть вещественным числом.\n");
                return 1;
            }
            int degree = 0;
            if (parse_int(argv[3], &degree) != STATUS_OK || degree < 0) {
                printf("Ошибка: степень degree должна быть неотрицательным целым числом.\n");
                return 1;
            }
            if (argc != 4 + degree + 1) {
                printf("Ошибка: для степени %d требуется ровно %d коэффициентов.\n", degree, degree + 1);
                return 1;
            }
            if (degree > 5) {
                printf("Ошибка: в тестовом режиме CLI поддерживается степень не выше 5.\n");
                return 1;
            }
            double c[6];
            for (int i = 0; i <= degree; i++) {
                if (parse_double(argv[4 + i], &c[i]) != STATUS_OK) {
                    printf("Ошибка: коэффициент '%s' не является вещественным числом.\n", argv[4 + i]);
                    return 1;
                }
            }
            double res = 0.0;
            enum status_code sc = STATUS_OK;
            switch (degree) {
                case 0: sc = evaluate_polynomial(x, 0, &res, c[0]); break;
                case 1: sc = evaluate_polynomial(x, 1, &res, c[0], c[1]); break;
                case 2: sc = evaluate_polynomial(x, 2, &res, c[0], c[1], c[2]); break;
                case 3: sc = evaluate_polynomial(x, 3, &res, c[0], c[1], c[2], c[3]); break;
                case 4: sc = evaluate_polynomial(x, 4, &res, c[0], c[1], c[2], c[3], c[4]); break;
                case 5: sc = evaluate_polynomial(x, 5, &res, c[0], c[1], c[2], c[3], c[4], c[5]); break;
            }
            if (sc != STATUS_OK) {
                printf("Ошибка при вычислении многочлена.\n");
                return 1;
            }
            printf("Значение многочлена в точке %f равно: %.6f\n", x, res);
            break;
        }

        case '3': {
            if (argc < 4) {
                printf("Ошибка: для подзадачи 3 требуется <base> <числа...>\n");
                return 1;
            }
            int base = 0;
            if (parse_int(argv[2], &base) != STATUS_OK || base < 2 || base > 36) {
                printf("Ошибка: основание системы счисления base должно лежать в диапазоне [2..36].\n");
                return 1;
            }
            int count = argc - 3;
            if (count > 6) {
                printf("Ошибка: в тестовом режиме CLI поддерживается до 6 чисел.\n");
                return 1;
            }
            const char *s[6];
            for (int i = 0; i < count; i++) {
                s[i] = argv[3 + i];
            }
            char **matched = NULL;
            int matched_cnt = 0;
            enum status_code sc = STATUS_OK;
            switch (count) {
                case 1: sc = find_kaprekar_numbers(base, 1, &matched, &matched_cnt, s[0]); break;
                case 2: sc = find_kaprekar_numbers(base, 2, &matched, &matched_cnt, s[0], s[1]); break;
                case 3: sc = find_kaprekar_numbers(base, 3, &matched, &matched_cnt, s[0], s[1], s[2]); break;
                case 4: sc = find_kaprekar_numbers(base, 4, &matched, &matched_cnt, s[0], s[1], s[2], s[3]); break;
                case 5: sc = find_kaprekar_numbers(base, 5, &matched, &matched_cnt, s[0], s[1], s[2], s[3], s[4]); break;
                case 6: sc = find_kaprekar_numbers(base, 6, &matched, &matched_cnt, s[0], s[1], s[2], s[3], s[4], s[5]); break;
            }
            if (sc != STATUS_OK) {
                printf("Ошибка при поиске чисел Капрекара.\n");
                return 1;
            }
            printf("Найденные числа Капрекара в с/с %d (%d шт.): ", base, matched_cnt);
            for (int i = 0; i < matched_cnt; i++) {
                printf("%s ", matched[i]);
                free(matched[i]);
            }
            free(matched);
            printf("\n");
            break;
        }

        case '4': {
            if (argc < 3) {
                printf("Ошибка: для подзадачи 4 требуется передать числа.\n");
                return 1;
            }
            int count = argc - 2;
            if (count > 6) {
                printf("Ошибка: в тестовом режиме CLI поддерживается до 6 чисел.\n");
                return 1;
            }
            double nums[6];
            for (int i = 0; i < count; i++) {
                if (parse_double(argv[2 + i], &nums[i]) != STATUS_OK) {
                    printf("Ошибка: аргумент '%s' не является вещественным числом.\n", argv[2 + i]);
                    return 1;
                }
            }
            double res = 0.0;
            enum status_code sc = STATUS_OK;
            switch (count) {
                case 1: sc = geometric_mean(&res, 1, nums[0]); break;
                case 2: sc = geometric_mean(&res, 2, nums[0], nums[1]); break;
                case 3: sc = geometric_mean(&res, 3, nums[0], nums[1], nums[2]); break;
                case 4: sc = geometric_mean(&res, 4, nums[0], nums[1], nums[2], nums[3]); break;
                case 5: sc = geometric_mean(&res, 5, nums[0], nums[1], nums[2], nums[3], nums[4]); break;
                case 6: sc = geometric_mean(&res, 6, nums[0], nums[1], nums[2], nums[3], nums[4], nums[5]); break;
            }
            if (sc != STATUS_OK) {
                printf("Ошибка при вычислении среднего геометрического (числа должны быть неотрицательными).\n");
                return 1;
            }
            printf("Среднее геометрическое: %.6f\n", res);
            break;
        }

        case '5': {
            if (argc != 4) {
                printf("Ошибка: для подзадачи 5 требуется ровно 2 аргумента: <base> <exp>\n");
                return 1;
            }
            double base_val = 0.0;
            if (parse_double(argv[2], &base_val) != STATUS_OK) {
                printf("Ошибка: основание степени должно быть вещественным числом.\n");
                return 1;
            }
            int exp_val = 0;
            if (parse_int(argv[3], &exp_val) != STATUS_OK) {
                printf("Ошибка: показатель степени должен быть целым числом.\n");
                return 1;
            }
            double res = 0.0;
            enum status_code sc = fast_power(base_val, exp_val, &res);
            if (sc == ERR_CALCULATION) {
                printf("Ошибка: деление на ноль (0 в отрицательной степени).\n");
                return 1;
            } else if (sc == ERR_OVERFLOW) {
                printf("Ошибка: переполнение при возведении в степень.\n");
                return 1;
            } else if (sc != STATUS_OK) {
                printf("Ошибка при возведении в степень.\n");
                return 1;
            }
            printf("Результат возведения %.6f ^ %d: %.6f\n", base_val, exp_val, res);
            break;
        }

        case '6': {
            if (argc != 6) {
                printf("Ошибка: для подзадачи 6 требуется <eq_num: 1..4> <a> <b> <eps>\n");
                return 1;
            }
            int eq_num = 0;
            if (parse_int(argv[2], &eq_num) != STATUS_OK || eq_num < 1 || eq_num > 4) {
                printf("Ошибка: номер уравнения должен быть от 1 до 4.\n");
                printf("1: x^2 - 2 = 0\n2: cos(x) - x = 0\n3: exp(x) - 3 = 0\n4: x^3 - x - 2 = 0\n");
                return 1;
            }
            double a = 0.0, b = 0.0, eps = 0.0;
            if (parse_double(argv[3], &a) != STATUS_OK ||
                parse_double(argv[4], &b) != STATUS_OK ||
                parse_double(argv[5], &eps) != STATUS_OK) {
                printf("Ошибка: границы интервала и эпсилон должны быть вещественными числами.\n");
                return 1;
            }
            if (eps <= 0.0 || a >= b) {
                printf("Ошибка: требуется a < b и eps > 0.\n");
                return 1;
            }
            double (*funcs[4])(const double) = {eq_sqrt2, eq_dottie, eq_exp3, eq_cubic};
            double root = 0.0;
            enum status_code sc = solve_bisection(a, b, eps, funcs[eq_num - 1], &root);
            if (sc == ERR_NO_ROOT) {
                printf("Ошибка: на концах интервала функция не меняет знак (f(a) * f(b) > 0).\n");
                return 1;
            } else if (sc != STATUS_OK) {
                printf("Ошибка при поиске корня.\n");
                return 1;
            }
            printf("Найденный корень уравнения на интервале [%.6f, %.6f]: %.6f\n", a, b, root);
            break;
        }

        default:
            printf("Ошибка: неизвестный флаг '%s'. Допустимы: -1 .. -6 (или с префиксом '/').\n", flag_str);
            return 1;
    }

    return 0;
}