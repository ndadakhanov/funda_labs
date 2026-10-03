#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

enum status_code {
    STATUS_OK = 0,
    ERR_INVALID_ARGUMENTS,
    ERR_OVERFLOW,
    ERR_DIVERGES,
    ERR_MAX_ITERATIONS
};

enum status_code parse_double(const char *str, double *result) {
    if (str == NULL || result == NULL) {
        return ERR_INVALID_ARGUMENTS;
    }
    size_t len = strlen(str);
    if (len == 0 || len > 64) {
        return ERR_INVALID_ARGUMENTS;
    }
    for (size_t i = 0; i < len; i++) {
        if (isspace((unsigned char)str[i])) {
            return ERR_INVALID_ARGUMENTS;
        }
    }
    char *endptr = NULL;
    double val = strtod(str, &endptr);
    if (endptr == str || *endptr != '\0') {
        return ERR_INVALID_ARGUMENTS;
    }
    if (isnan(val) || isinf(val)) {
        return ERR_OVERFLOW;
    }
    *result = val;
    return STATUS_OK;
}

enum status_code calculate_sum_a(const double eps, const double x, double *result) {
    if (result == NULL || eps <= 0.0) {
        return ERR_INVALID_ARGUMENTS;
    }
    double term = 1.0;
    double sum = term;
    int n = 1;
    while (fabs(term) >= eps && n < 1000000) {
        term = term * x / (double)n;
        sum += term;
        n++;
    }
    if (n >= 1000000) {
        return ERR_MAX_ITERATIONS;
    }
    *result = sum;
    return STATUS_OK;
}

enum status_code calculate_sum_b(const double eps, const double x, double *result) {
    if (result == NULL || eps <= 0.0) {
        return ERR_INVALID_ARGUMENTS;
    }
    double term = 1.0;
    double sum = term;
    int n = 1;
    const double x2 = x * x;
    while (fabs(term) >= eps && n < 1000000) {
        term = -term * x2 / ((2.0 * n - 1.0) * (2.0 * n));
        sum += term;
        n++;
    }
    if (n >= 1000000) {
        return ERR_MAX_ITERATIONS;
    }
    *result = sum;
    return STATUS_OK;
}

enum status_code calculate_sum_c(const double eps, const double x, double *result) {
    if (result == NULL || eps <= 0.0) {
        return ERR_INVALID_ARGUMENTS;
    }
    if (fabs(x) >= 1.0) {
        return ERR_DIVERGES;
    }
    double term = 1.0;
    double sum = term;
    int n = 1;
    const double x2 = x * x;
    while (fabs(term) >= eps && n < 1000000) {
        double factor = (9.0 * n * n) / ((3.0 * n - 1.0) * (3.0 * n - 2.0));
        term = term * factor * x2;
        sum += term;
        n++;
    }
    if (n >= 1000000) {
        return ERR_MAX_ITERATIONS;
    }
    *result = sum;
    return STATUS_OK;
}

enum status_code calculate_sum_d(const double eps, const double x, double *result) {
    if (result == NULL || eps <= 0.0) {
        return ERR_INVALID_ARGUMENTS;
    }
    if (fabs(x) > 1.0) {
        return ERR_DIVERGES;
    }
    const double x2 = x * x;
    double term = -0.5 * x2;
    double sum = term;
    int n = 2;
    while (fabs(term) >= eps && n < 1000000) {
        term = -term * (2.0 * n - 1.0) / (2.0 * n) * x2;
        sum += term;
        n++;
    }
    if (n >= 1000000) {
        return ERR_MAX_ITERATIONS;
    }
    *result = sum;
    return STATUS_OK;
}

double func_a(const double x) {
    if (fabs(x) < 1e-15) {
        return 1.0;
    }
    return log(1.0 + x) / x;
}

double func_b(const double x) {
    return exp(-(x * x) / 2.0);
}

double func_c(const double x) {
    double diff = 1.0 - x;
    if (diff <= 1e-15) {
        diff = 1e-15;
    }
    return log(1.0 / diff);
}

double func_d(const double x) {
    if (fabs(x) < 1e-15) {
        return 1.0;
    }
    return pow(x, x);
}

enum status_code integrate_midpoint(double (*f)(const double), const double a, const double b, const double eps, double *result) {
    if (f == NULL || result == NULL || eps <= 0.0 || a >= b) {
        return ERR_INVALID_ARGUMENTS;
    }
    int n = 8;
    double h = (b - a) / (double)n;
    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        double x = a + (i + 0.5) * h;
        sum += f(x);
    }
    double current = sum * h;
    double prev = 0.0;
    int iterations = 0;
    const int max_iterations = 22;

    do {
        prev = current;
        n *= 2;
        h = (b - a) / (double)n;
        sum = 0.0;
        for (int i = 0; i < n; i++) {
            double x = a + (i + 0.5) * h;
            sum += f(x);
        }
        current = sum * h;
        iterations++;
    } while (fabs(current - prev) / 3.0 >= eps && iterations < max_iterations);

    *result = current;
    return STATUS_OK;
}

void print_sum_result(const char *name, enum status_code code, double res) {
    switch (code) {
        case STATUS_OK:
            printf("Сумма %s: %.10f\n", name, res);
            break;
        case ERR_DIVERGES:
            printf("Сумма %s: ряд расходится при данном x\n", name);
            break;
        case ERR_MAX_ITERATIONS:
            printf("Сумма %s: превышено максимальное число итераций\n", name);
            break;
        case ERR_INVALID_ARGUMENTS:
            printf("Сумма %s: некорректные аргументы\n", name);
            break;
        default:
            printf("Сумма %s: неизвестная ошибка\n", name);
            break;
    }
}

void print_integral_result(const char *name, enum status_code code, double res) {
    switch (code) {
        case STATUS_OK:
            printf("Интеграл %s: %.10f\n", name, res);
            break;
        case ERR_MAX_ITERATIONS:
            printf("Интеграл %s: не удалось достичь точности за допустимое число шагов\n", name);
            break;
        case ERR_INVALID_ARGUMENTS:
            printf("Интеграл %s: некорректные аргументы\n", name);
            break;
        default:
            printf("Интеграл %s: неизвестная ошибка\n", name);
            break;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Ошибка: неверное количество аргументов!\n");
        printf("Использование: %s <epsilon> <x>\n", argv[0]);
        return 1;
    }

    double eps = 0.0;
    enum status_code sc = parse_double(argv[1], &eps);
    if (sc == ERR_OVERFLOW) {
        printf("Ошибка: переполнение при чтении epsilon!\n");
        return 1;
    }
    if (sc != STATUS_OK) {
        printf("Ошибка: параметр epsilon должен быть вещественным числом!\n");
        return 1;
    }

    if (eps <= 0.0 || eps >= 1.0) {
        printf("Ошибка: точность epsilon должна быть в диапазоне (0, 1)!\n");
        return 1;
    }
    if (eps < 1e-14) {
        printf("Ошибка: заданная точность epsilon слишком мала (меньше машинной точности double)!\n");
        return 1;
    }

    double x = 0.0;
    sc = parse_double(argv[2], &x);
    if (sc == ERR_OVERFLOW) {
        printf("Ошибка: переполнение при чтении x!\n");
        return 1;
    }
    if (sc != STATUS_OK) {
        printf("Ошибка: параметр x должен быть вещественным числом!\n");
        return 1;
    }

    printf("Параметры: epsilon = %e, x = %f\n", eps, x);
    printf("--- ЧАСТЬ 1: ВЫЧИСЛЕНИЕ СУММ ---\n");

    double res = 0.0;
    sc = calculate_sum_a(eps, x, &res);
    print_sum_result("a", sc, res);

    sc = calculate_sum_b(eps, x, &res);
    print_sum_result("b", sc, res);

    sc = calculate_sum_c(eps, x, &res);
    print_sum_result("c", sc, res);

    sc = calculate_sum_d(eps, x, &res);
    print_sum_result("d", sc, res);

    printf("--- ЧАСТЬ 2: ВЫЧИСЛЕНИЕ ИНТЕГРАЛОВ ---\n");

    sc = integrate_midpoint(func_a, 0.0, 1.0, eps, &res);
    print_integral_result("a", sc, res);

    sc = integrate_midpoint(func_b, 0.0, 1.0, eps, &res);
    print_integral_result("b", sc, res);

    sc = integrate_midpoint(func_c, 0.0, 1.0, eps, &res);
    print_integral_result("c", sc, res);

    sc = integrate_midpoint(func_d, 0.0, 1.0, eps, &res);
    print_integral_result("d", sc, res);

    return 0;
}