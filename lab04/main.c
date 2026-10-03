#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PATH_BUFFER_SIZE 4096

enum status_code {
    STATUS_OK = 0,
    ERR_INVALID_INPUT,
    ERR_FILE_OPEN,
    ERR_FILE_READ,
    ERR_FILE_WRITE,
    ERR_SAME_FILE,
    ERR_BUFFER_OVERFLOW
};

int is_latin_letter(const char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int is_arabic_digit(const char c) {
    return (c >= '0' && c <= '9');
}

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

enum status_code construct_output_path(const char *in_path, char *out_path, const size_t out_size) {
    if (!in_path || !out_path || out_size == 0) {
        return ERR_INVALID_INPUT;
    }

    const char *last_slash = strrchr(in_path, '/');
    const char *last_backslash = strrchr(in_path, '\\');
    const char *sep = NULL;

    if (last_slash != NULL && last_backslash != NULL) {
        sep = (last_slash > last_backslash) ? last_slash : last_backslash;
    } else if (last_slash != NULL) {
        sep = last_slash;
    } else {
        sep = last_backslash;
    }

    int written = 0;
    if (sep != NULL) {
        size_t dir_len = (size_t)(sep - in_path + 1);
        written = snprintf(out_path, out_size, "%.*sout%s", (int)dir_len, in_path, sep + 1);
    } else {
        written = snprintf(out_path, out_size, "out%s", in_path);
    }

    if (written < 0 || (size_t)written >= out_size) {
        return ERR_BUFFER_OVERFLOW;
    }

    return STATUS_OK;
}

enum status_code process_exclude_digits(FILE *in, FILE *out) {
    if (!in || !out) {
        return ERR_INVALID_INPUT;
    }

    int c = 0;
    while ((c = fgetc(in)) != EOF) {
        if (!is_arabic_digit((char)c)) {
            if (fputc(c, out) == EOF) {
                return ERR_FILE_WRITE;
            }
        }
    }

    if (ferror(in)) {
        return ERR_FILE_READ;
    }

    return STATUS_OK;
}

enum status_code process_count_latin(FILE *in, FILE *out) {
    if (!in || !out) {
        return ERR_INVALID_INPUT;
    }

    int c = 0;
    size_t count = 0;
    int has_chars = 0;

    while ((c = fgetc(in)) != EOF) {
        has_chars = 1;
        if (c == '\r') {
            continue;
        }
        if (c == '\n') {
            if (fprintf(out, "%zu\n", count) < 0) {
                return ERR_FILE_WRITE;
            }
            count = 0;
            has_chars = 0;
        } else {
            if (is_latin_letter((char)c)) {
                count++;
            }
        }
    }

    if (ferror(in)) {
        return ERR_FILE_READ;
    }

    if (has_chars) {
        if (fprintf(out, "%zu\n", count) < 0) {
            return ERR_FILE_WRITE;
        }
    }

    return STATUS_OK;
}

enum status_code process_count_special(FILE *in, FILE *out) {
    if (!in || !out) {
        return ERR_INVALID_INPUT;
    }

    int c = 0;
    size_t count = 0;
    int has_chars = 0;

    while ((c = fgetc(in)) != EOF) {
        has_chars = 1;
        if (c == '\r') {
            continue;
        }
        if (c == '\n') {
            if (fprintf(out, "%zu\n", count) < 0) {
                return ERR_FILE_WRITE;
            }
            count = 0;
            has_chars = 0;
        } else {
            if (!is_latin_letter((char)c) && !is_arabic_digit((char)c) && c != ' ') {
                count++;
            }
        }
    }

    if (ferror(in)) {
        return ERR_FILE_READ;
    }

    if (has_chars) {
        if (fprintf(out, "%zu\n", count) < 0) {
            return ERR_FILE_WRITE;
        }
    }

    return STATUS_OK;
}

enum status_code process_replace_ascii(FILE *in, FILE *out) {
    if (!in || !out) {
        return ERR_INVALID_INPUT;
    }

    int c = 0;
    while ((c = fgetc(in)) != EOF) {
        if (is_arabic_digit((char)c)) {
            if (fputc(c, out) == EOF) {
                return ERR_FILE_WRITE;
            }
        } else {
            if (fprintf(out, "%X", (unsigned char)c) < 0) {
                return ERR_FILE_WRITE;
            }
        }
    }

    if (ferror(in)) {
        return ERR_FILE_READ;
    }

    return STATUS_OK;
}

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        printf("Ошибка: неверное количество аргументов.\n");
        printf("Использование:\n");
        printf("  %s <флаг> <входной_файл>\n", argv[0]);
        printf("  %s <n-флаг> <входной_файл> <выходной_файл>\n", argv[0]);
        printf("Доступные флаги: -d, -i, -s, -a (с префиксом 'n': -nd, -ni, -ns, -na, или с символом '/')\n");
        return 1;
    }

    const char *flag_str = argv[1];
    if (flag_str[0] != '-' && flag_str[0] != '/') {
        printf("Ошибка: флаг должен начинаться с '-' или '/'.\n");
        return 1;
    }

    size_t flag_len = strlen(flag_str);
    int has_n = 0;
    char action = '\0';

    if (flag_len == 2) {
        has_n = 0;
        action = flag_str[1];
    } else if (flag_len == 3 && flag_str[1] == 'n') {
        has_n = 1;
        action = flag_str[2];
    } else {
        printf("Ошибка: некорректный флаг '%s'.\n", flag_str);
        return 1;
    }

    if (action != 'd' && action != 'i' && action != 's' && action != 'a') {
        printf("Ошибка: неизвестное действие '%c'. Допустимы: d, i, s, a.\n", action);
        return 1;
    }

    if (has_n && argc != 4) {
        printf("Ошибка: флаг с префиксом 'n' требует указания выходного файла (3 аргумента).\n");
        return 1;
    }
    if (!has_n && argc != 3) {
        printf("Ошибка: стандартный флаг без 'n' принимает только входной файл (2 аргумента).\n");
        return 1;
    }

    const char *in_path = argv[2];
    char out_path_buf[PATH_BUFFER_SIZE];
    const char *out_path = NULL;

    if (has_n) {
        out_path = argv[3];
    } else {
        enum status_code sc = construct_output_path(in_path, out_path_buf, sizeof(out_path_buf));
        if (sc != STATUS_OK) {
            printf("Ошибка: путь к файлу слишком длинный.\n");
            return 1;
        }
        out_path = out_path_buf;
    }

    enum status_code val_status = validate_file_paths(in_path, out_path);
    if (val_status == ERR_SAME_FILE) {
        printf("Ошибка: пути входного и выходного файлов совпадают ('%s').\n", in_path);
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

    enum status_code status = STATUS_OK;

    switch (action) {
        case 'd':
            status = process_exclude_digits(in, out);
            break;
        case 'i':
            status = process_count_latin(in, out);
            break;
        case 's':
            status = process_count_special(in, out);
            break;
        case 'a':
            status = process_replace_ascii(in, out);
            break;
    }

    fclose(in);
    fclose(out);

    switch (status) {
        case STATUS_OK:
            printf("Обработка успешно завершена. Результат записан в: %s\n", out_path);
            break;
        case ERR_FILE_READ:
            printf("Ошибка при чтении входного файла.\n");
            return 1;
        case ERR_FILE_WRITE:
            printf("Ошибка при записи в выходной файл.\n");
            return 1;
        default:
            printf("Произошла ошибка при обработке данных.\n");
            return 1;
    }

    return 0;
}