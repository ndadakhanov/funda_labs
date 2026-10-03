#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

enum status_code {
    STATUS_OK = 0,
    ERR_INVALID_INPUT,
    ERR_FILE_OPEN,
    ERR_FILE_READ,
    ERR_FILE_WRITE,
    ERR_SAME_FILE,
    ERR_OVERFLOW,
    ERR_MEMORY_ALLOCATION
};

enum status_code validate_file_paths(const char *in_path, const char *out_path) {
    if (!in_path || !out_path) {
        return ERR_INVALID_INPUT;
    }

    const char *p1 = in_path;
    const char *p2 = out_path;

    while (p1[0] == '.' && (p1[1] == '/' || p1[1] == '\\')) {
        p1 += 2;
    }
    while (p2[0] == '.' && (p2[1] == '/' || p2[1] == '\\')) {
        p2 += 2;
    }

    if (strcmp(p1, p2) == 0) {
        return ERR_SAME_FILE;
    }

    return STATUS_OK;
}

int char_to_digit(const char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'A' && c <= 'Z') {
        return c - 'A' + 10;
    }
    if (c >= 'a' && c <= 'z') {
        return c - 'a' + 10;
    }
    return -1;
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
        if (ferror(in)) {
            return ERR_FILE_READ;
        }
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

    if (ferror(in)) {
        free(buf);
        return ERR_FILE_READ;
    }

    buf[length] = '\0';
    *token = buf;
    *has_token = 1;

    return STATUS_OK;
}

enum status_code process_number_token(const char *token, char **stripped, int *min_base, long long *val_10) {
    if (!token || !stripped || !min_base || !val_10) {
        return ERR_INVALID_INPUT;
    }

    int is_negative = 0;
    const char *p = token;

    if (*p == '+') {
        p++;
    } else if (*p == '-') {
        is_negative = 1;
        p++;
    }

    if (*p == '\0') {
        return ERR_INVALID_INPUT;
    }

    const char *digits_start = p;
    int max_digit = 0;

    while (*p != '\0') {
        int d = char_to_digit(*p);
        if (d < 0) {
            return ERR_INVALID_INPUT;
        }
        if (d > max_digit) {
            max_digit = d;
        }
        p++;
    }

    int base = max_digit + 1;
    if (base < 2) {
        base = 2;
    }
    *min_base = base;

    const char *nz = digits_start;
    while (*nz == '0') {
        nz++;
    }

    size_t token_len = strlen(token);
    char *res_stripped = (char *)malloc(token_len + 2);
    if (!res_stripped) {
        return ERR_MEMORY_ALLOCATION;
    }

    if (*nz == '\0') {
        strcpy(res_stripped, "0");
    } else {
        size_t idx = 0;
        if (is_negative) {
            res_stripped[idx++] = '-';
        }
        while (*nz != '\0') {
            res_stripped[idx++] = *nz++;
        }
        res_stripped[idx] = '\0';
    }
    *stripped = res_stripped;

    unsigned long long limit = is_negative ? ((unsigned long long)LLONG_MAX + 1ULL) : (unsigned long long)LLONG_MAX;
    unsigned long long uval = 0;
    p = digits_start;

    while (*p != '\0') {
        int d = char_to_digit(*p);
        if (uval > (limit - (unsigned long long)d) / (unsigned long long)base) {
            return ERR_OVERFLOW;
        }
        uval = uval * (unsigned long long)base + (unsigned long long)d;
        p++;
    }

    if (is_negative) {
        if (uval == (unsigned long long)LLONG_MAX + 1ULL) {
            *val_10 = LLONG_MIN;
        } else {
            *val_10 = -(long long)uval;
        }
    } else {
        *val_10 = (long long)uval;
    }

    return STATUS_OK;
}

enum status_code process_file(FILE *in, FILE *out) {
    if (!in || !out) {
        return ERR_INVALID_INPUT;
    }

    int has_token = 1;

    while (1) {
        char *token = NULL;
        enum status_code sc = read_next_token(in, &token, &has_token);
        if (sc != STATUS_OK) {
            return sc;
        }
        if (!has_token || !token) {
            break;
        }

        char *stripped = NULL;
        int min_base = 0;
        long long val_10 = 0;

        sc = process_number_token(token, &stripped, &min_base, &val_10);
        if (sc != STATUS_OK) {
            free(token);
            free(stripped);
            return sc;
        }

        if (fprintf(out, "%s %d %lld\n", stripped, min_base, val_10) < 0) {
            free(token);
            free(stripped);
            return ERR_FILE_WRITE;
        }

        free(token);
        free(stripped);
    }

    return STATUS_OK;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Ошибка: неверное количество аргументов.\n");
        printf("Использование: %s <входной_файл> <выходной_файл>\n", argv[0]);
        return 1;
    }

    const char *in_path = argv[1];
    const char *out_path = argv[2];

    if (validate_file_paths(in_path, out_path) == ERR_SAME_FILE) {
        printf("Ошибка: путь входного файла совпадает с выходным файлом ('%s').\n", in_path);
        return 1;
    }

    FILE *in = fopen(in_path, "r");
    if (!in) {
        printf("Ошибка: невозможно открыть входной файл '%s' для чтения.\n", in_path);
        return 1;
    }

    FILE *out = fopen(out_path, "w");
    if (!out) {
        printf("Ошибка: невозможно создать выходной файл '%s' для записи.\n", out_path);
        fclose(in);
        return 1;
    }

    enum status_code sc = process_file(in, out);

    fclose(in);
    fclose(out);

    switch (sc) {
        case STATUS_OK:
            printf("Обработка успешно завершена. Результат записан в: %s\n", out_path);
            break;
        case ERR_INVALID_INPUT:
            printf("Ошибка: входной файл содержит некорректные символы (не являющиеся цифрами систем счисления [2..36]).\n");
            return 1;
        case ERR_OVERFLOW:
            printf("Ошибка: обнаружено число, выходящее за пределы диапазона 64-битного целого числа.\n");
            return 1;
        case ERR_FILE_READ:
            printf("Ошибка при чтении входного файла.\n");
            return 1;
        case ERR_FILE_WRITE:
            printf("Ошибка при записи в выходной файл.\n");
            return 1;
        case ERR_MEMORY_ALLOCATION:
            printf("Ошибка выделения динамической памяти.\n");
            return 1;
        default:
            printf("Произошла неизвестная ошибка при обработке данных.\n");
            return 1;
    }

    return 0;
}