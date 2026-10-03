#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <float.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

enum status_code {
    STATUS_OK = 0,
    ERR_INVALID_INPUT,
    ERR_CALCULATION
};

enum status_code parse_epsilon(const char *str, double *result) {
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
        return ERR_INVALID_INPUT;
    }

    if (val <= 0.0 || val >= 1.0 || val < DBL_EPSILON) {
        return ERR_INVALID_INPUT;
    }

    *result = val;
    return STATUS_OK;
}

int is_prime(long long n) {
    if (n < 2) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (long long d = 3; d <= n / d; d += 2) {
        if (n % d == 0) return 0;
    }
    return 1;
}

enum status_code calc_e_limit(double eps, double *result) {
    if (!result || eps <= 0.0) return ERR_INVALID_INPUT;

    long long n = 1;
    double prev = 0.0;
    double cur = 2.0;

    while (fabs(cur - prev) >= eps && n < 10000000LL) {
        prev = cur;
        n++;
        cur = pow(1.0 + 1.0 / (double)n, (double)n);
    }

    *result = cur;
    return STATUS_OK;
}

enum status_code calc_e_series(double eps, double *result) {
    if (!result || eps <= 0.0) return ERR_INVALID_INPUT;

    double sum = 1.0;
    double term = 1.0;
    long long n = 1;

    while (term >= eps) {
        term /= (double)n;
        sum += term;
        n++;
    }

    *result = sum;
    return STATUS_OK;
}

enum status_code calc_e_equation(double eps, double *result) {
    if (!result || eps <= 0.0) return ERR_INVALID_INPUT;

    double left = 2.0;
    double right = 3.0;

    while (right - left > eps) {
        double mid = left + (right - left) / 2.0;
        if (log(mid) - 1.0 > 0.0) {
            right = mid;
        } else {
            left = mid;
        }
    }

    *result = left + (right - left) / 2.0;
    return STATUS_OK;
}

enum status_code calc_pi_limit(double eps, double *result) {
    if (!result || eps <= 0.0) return ERR_INVALID_INPUT;

    long long n = 1;
    double cur = 4.0;
    double diff = 1.0;

    while (diff >= eps && n < 10000000LL) {
        double factor = (4.0 * (double)n * (double)(n + 1)) / ((2.0 * n + 1.0) * (2.0 * n + 1.0));
        double next = cur * factor;
        diff = fabs(next - cur);
        cur = next;
        n++;
    }

    *result = cur;
    return STATUS_OK;
}

enum status_code calc_pi_series(double eps, double *result) {
    if (!result || eps <= 0.0) return ERR_INVALID_INPUT;

    double sum = 0.0;
    long long n = 1;
    double term = 4.0;
    int sign = 1;

    while (term >= eps && n < 20000000LL) {
        sum += sign * term;
        sign = -sign;
        n++;
        term = 4.0 / (2.0 * n - 1.0);
    }

    *result = sum;
    return STATUS_OK;
}

enum status_code calc_pi_equation(double eps, double *result) {
    if (!result || eps <= 0.0) return ERR_INVALID_INPUT;

    double left = 2.0;
    double right = 4.0;

    while (right - left > eps) {
        double mid = left + (right - left) / 2.0;
        if (-sin(mid) > 0.0) {
            right = mid;
        } else {
            left = mid;
        }
    }

    *result = left + (right - left) / 2.0;
    return STATUS_OK;
}

enum status_code calc_ln2_limit(double eps, double *result) {
    if (!result || eps <= 0.0) return ERR_INVALID_INPUT;

    long long n = 1;
    double prev = 0.0;
    double cur = 1.0;

    while (fabs(cur - prev) >= eps && n < 10000000LL) {
        prev = cur;
        n++;
        cur = (double)n * (pow(2.0, 1.0 / (double)n) - 1.0);
    }

    *result = cur;
    return STATUS_OK;
}

enum status_code calc_ln2_series(double eps, double *result) {
    if (!result || eps <= 0.0) return ERR_INVALID_INPUT;

    double sum = 0.0;
    long long n = 1;
    double term = 1.0;
    int sign = 1;

    while (term >= eps && n < 20000000LL) {
        sum += sign * term;
        sign = -sign;
        n++;
        term = 1.0 / (double)n;
    }

    *result = sum;
    return STATUS_OK;
}

enum status_code calc_ln2_equation(double eps, double *result) {
    if (!result || eps <= 0.0) return ERR_INVALID_INPUT;

    double left = 0.0;
    double right = 1.0;

    while (right - left > eps) {
        double mid = left + (right - left) / 2.0;
        if (exp(mid) - 2.0 > 0.0) {
            right = mid;
        } else {
            left = mid;
        }
    }

    *result = left + (right - left) / 2.0;
    return STATUS_OK;
}

enum status_code calc_sqrt2_limit(double eps, double *result) {
    if (!result || eps <= 0.0) return ERR_INVALID_INPUT;

    double prev = -0.5;
    double cur = prev - (prev * prev) / 2.0 + 1.0;

    while (fabs(cur - prev) >= eps) {
        prev = cur;
        cur = prev - (prev * prev) / 2.0 + 1.0;
    }

    *result = cur;
    return STATUS_OK;
}

enum status_code calc_sqrt2_product(double eps, double *result) {
    if (!result || eps <= 0.0) return ERR_INVALID_INPUT;

    double factor = pow(2.0, 0.25);
    double prod = factor;

    while (fabs(factor - 1.0) >= eps) {
        factor = sqrt(factor);
        prod *= factor;
    }

    *result = prod;
    return STATUS_OK;
}

enum status_code calc_sqrt2_equation(double eps, double *result) {
    if (!result || eps <= 0.0) return ERR_INVALID_INPUT;

    double left = 1.0;
    double right = 2.0;

    while (right - left > eps) {
        double mid = left + (right - left) / 2.0;
        if (mid * mid - 2.0 > 0.0) {
            right = mid;
        } else {
            left = mid;
        }
    }

    *result = left + (right - left) / 2.0;
    return STATUS_OK;
}

enum status_code calc_gamma_limit(double eps, double *result) {
    if (!result || eps <= 0.0) return ERR_INVALID_INPUT;

    double prev = 0.0;
    double cur = 0.0;

    for (int m = 1; m <= 22; ++m) {
        double sum = 0.0;
        double c_m_k = 1.0;
        double ln_fact = 0.0;

        for (int k = 1; k <= m; ++k) {
            c_m_k = c_m_k * (double)(m - k + 1) / (double)k;
            ln_fact += log((double)k);
            double term = c_m_k * (double)((k % 2 != 0) ? -1 : 1) * ln_fact / (double)k;
            sum += term;
        }

        prev = cur;
        cur = sum;

        if (m > 2 && fabs(cur - prev) < eps) {
            break;
        }
    }

    *result = cur;
    return STATUS_OK;
}

enum status_code calc_gamma_series(double eps, double *result) {
    if (!result || eps <= 0.0) return ERR_INVALID_INPUT;

    double sum = (1.0 - 0.5) + (1.0 - 1.0 / 3.0);
    long long m = 2;

    while (m < 200000LL) {
        double block_sum = 0.0;
        long long start = m * m;
        long long end = (m + 1) * (m + 1) - 1;

        for (long long k = start; k <= end; ++k) {
            block_sum += (1.0 / (double)(m * m)) - (1.0 / (double)k);
        }

        sum += block_sum;
        if (block_sum < eps) {
            break;
        }
        m++;
    }

    *result = -(M_PI * M_PI) / 6.0 + sum;
    return STATUS_OK;
}

enum status_code calc_gamma_equation(double eps, double *result) {
    if (!result || eps <= 0.0) return ERR_INVALID_INPUT;

    double prod = 1.0;
    double prev_c = 0.0;
    double c = 0.0;

    for (long long p = 2; p <= 100000LL; ++p) {
        if (is_prime(p)) {
            prod *= (double)(p - 1) / (double)p;
            prev_c = c;
            c = log((double)p) * prod;
            if (p > 50 && fabs(c - prev_c) < eps) {
                break;
            }
        }
    }

    double left = 0.0;
    double right = 1.0;

    while (right - left > eps) {
        double mid = left + (right - left) / 2.0;
        if (exp(-mid) - c < 0.0) {
            right = mid;
        } else {
            left = mid;
        }
    }

    *result = left + (right - left) / 2.0;
    return STATUS_OK;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Ошибка: неверное количество аргументов.\n");
        printf("Использование: %s <эпсилон>\n", argv[0]);
        printf("Пример: %s 0.0001\n", argv[0]);
        return 1;
    }

    double eps = 0.0;
    enum status_code st = parse_epsilon(argv[1], &eps);
    if (st != STATUS_OK) {
        printf("Ошибка: аргумент '%s' некорректен. Точность эпсилон должна быть вещественным числом в диапазоне (0; 1).\n", argv[1]);
        return 1;
    }

    double val_lim = 0.0, val_ser = 0.0, val_eq = 0.0;

    printf("Точность вычислений: %.10g\n\n", eps);
    printf("%-10s | %-18s | %-18s | %-18s\n", "Константа", "Предел", "Ряд / Произв.", "Уравнение");
    printf("-----------------------------------------------------------------------------\n");

    calc_e_limit(eps, &val_lim);
    calc_e_series(eps, &val_ser);
    calc_e_equation(eps, &val_eq);
    printf("%-10s | %-18.10f | %-18.10f | %-18.10f\n", "e", val_lim, val_ser, val_eq);

    calc_pi_limit(eps, &val_lim);
    calc_pi_series(eps, &val_ser);
    calc_pi_equation(eps, &val_eq);
    printf("%-10s | %-18.10f | %-18.10f | %-18.10f\n", "pi", val_lim, val_ser, val_eq);

    calc_ln2_limit(eps, &val_lim);
    calc_ln2_series(eps, &val_ser);
    calc_ln2_equation(eps, &val_eq);
    printf("%-10s | %-18.10f | %-18.10f | %-18.10f\n", "ln 2", val_lim, val_ser, val_eq);

    calc_sqrt2_limit(eps, &val_lim);
    calc_sqrt2_product(eps, &val_ser);
    calc_sqrt2_equation(eps, &val_eq);
    printf("%-10s | %-18.10f | %-18.10f | %-18.10f\n", "sqrt(2)", val_lim, val_ser, val_eq);

    calc_gamma_limit(eps, &val_lim);
    calc_gamma_series(eps, &val_ser);
    calc_gamma_equation(eps, &val_eq);
    printf("%-10s | %-18.10f | %-18.10f | %-18.10f\n", "gamma", val_lim, val_ser, val_eq);

    return 0;
}