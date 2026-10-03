#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

enum status_code {
    STATUS_OK = 0,
    ERR_INVALID_INPUT,
    ERR_FILE_OPEN,
    ERR_FILE_READ,
    ERR_FILE_WRITE,
    ERR_SAME_FILE,
    ERR_MEMORY_ALLOCATION
};

int is_latin_letter(const char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
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

enum status_code write_char_base(FILE *out, const unsigned char c, const int base) {
    if (!out || base < 2 || base > 36) {
        return ERR_INVALID_INPUT;
    }

    char buf[32];
    int len = 0;
    unsigned int val = (unsigned int)c;

    if (val == 0) {
        buf[len++] = '0';
    } else {
        while (val > 0) {
            int rem = val % base;
            if (rem < 10) {
                buf[len++] = (char)('0' + rem);
            } else {
                buf[len++] = (char)('A' + (rem - 10));
            }
            val /= base;
        }
    }

    for (int i = len - 1; i >= 0; i--) {
        if (fputc(buf[i], out) == EOF) {
            return ERR_FILE_WRITE;
        }
    }

    return STATUS_OK;
}

enum status_code process_merge_files(FILE *in1, FILE *in2, FILE *out) {
    if (!in1 || !in2 || !out) {
        return ERR_INVALID_INPUT;
    }

    int first_token = 1;
    int has1 = 1;
    int has2 = 1;

    while (has1 || has2) {
        if (has1) {
            char *token1 = NULL;
            enum status_code sc = read_next_token(in1, &token1, &has1);
            if (sc != STATUS_OK) {
                return sc;
            }
            if (has1 && token1) {
                if (!first_token) {
                    if (fputc(' ', out) == EOF) {
                        free(token1);
                        return ERR_FILE_WRITE;
                    }
                }
                if (fputs(token1, out) == EOF) {
                    free(token1);
                    return ERR_FILE_WRITE;
                }
                first_token = 0;
                free(token1);
            }
        }

        if (has2) {
            char *token2 = NULL;
            enum status_code sc = read_next_token(in2, &token2, &has2);
            if (sc != STATUS_OK) {
                return sc;
            }
            if (has2 && token2) {
                if (!first_token) {
                    if (fputc(' ', out) == EOF) {
                        free(token2);
                        return ERR_FILE_WRITE;
                    }
                }
                if (fputs(token2, out) == EOF) {
                    free(token2);
                    return ERR_FILE_WRITE;
                }
                first_token = 0;
                free(token2);
            }
        }
    }

    return STATUS_OK;
}

enum status_code process_transform_file(FILE *in, FILE *out) {
    if (!in || !out) {
        return ERR_INVALID_INPUT;
    }

    int first_token = 1;
    int has_token = 1;
    size_t token_index = 1;

    while (1) {
        char *token = NULL;
        enum status_code sc = read_next_token(in, &token, &has_token);
        if (sc != STATUS_OK) {
            return sc;
        }
        if (!has_token || !token) {
            break;
        }

        if (!first_token) {
            if (fputc(' ', out) == EOF) {
                free(token);
                return ERR_FILE_WRITE;
            }
        }
        first_token = 0;

        if (token_index % 10 == 0) {
            for (size_t i = 0; token[i] != '\0'; i++) {
                char ch = token[i];
                if (is_latin_letter(ch)) {
                    ch = (char)tolower((unsigned char)ch);
                }
                sc = write_char_base(out, (unsigned char)ch, 4);
                if (sc != STATUS_OK) {
                    free(token);
                    return sc;
                }
            }
        } else if (token_index % 2 == 0) {
            for (size_t i = 0; token[i] != '\0'; i++) {
                char ch = token[i];
                if (is_latin_letter(ch)) {
                    ch = (char)tolower((unsigned char)ch);
                }
                if (fputc(ch, out) == EOF) {
                    free(token);
                    return ERR_FILE_WRITE;
                }
            }
        } else if (token_index % 5 == 0) {
            for (size_t i = 0; token[i] != '\0'; i++) {
                sc = write_char_base(out, (unsigned char)token[i], 8);
                if (sc != STATUS_OK) {
                    free(token);
                    return sc;
                }
            }
        } else {
            if (fputs(token, out) == EOF) {
                free(token);
                return ERR_FILE_WRITE;
            }
        }

        free(token);
        token_index++;
    }

    return STATUS_OK;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Ошибка: не указан флаг действия.\n");
        printf("Использование:\n");
        printf("  %s -r <file1> <file2> <out_file>\n", argv[0]);
        printf("  %s -a <in_file> <out_file>\n", argv[0]);
        printf("Допускается использование символа '/' вместо '-'.\n");
        return 1;
    }

    const char *flag_str = argv[1];
    if ((flag_str[0] != '-' && flag_str[0] != '/') || flag_str[1] == '\0' || flag_str[2] != '\0') {
        printf("Ошибка: некорректный флаг '%s'.\n", flag_str);
        return 1;
    }

    char action = flag_str[1];

    if (action == 'r') {
        if (argc != 5) {
            printf("Ошибка: для флага '%s' требуется ровно 3 пути к файлам: <file1> <file2> <out_file> (передано: %d).\n", flag_str, argc - 2);
            return 1;
        }

        const char *in_path1 = argv[2];
        const char *in_path2 = argv[3];
        const char *out_path = argv[4];

        if (validate_file_paths(in_path1, out_path) == ERR_SAME_FILE) {
            printf("Ошибка: путь первого входного файла совпадает с выходным файлом ('%s').\n", in_path1);
            return 1;
        }
        if (validate_file_paths(in_path2, out_path) == ERR_SAME_FILE) {
            printf("Ошибка: путь второго входного файла совпадает с выходным файлом ('%s').\n", in_path2);
            return 1;
        }

        FILE *in1 = fopen(in_path1, "r");
        if (!in1) {
            printf("Ошибка: невозможно открыть файл '%s' для чтения.\n", in_path1);
            return 1;
        }

        FILE *in2 = fopen(in_path2, "r");
        if (!in2) {
            printf("Ошибка: невозможно открыть файл '%s' для чтения.\n", in_path2);
            fclose(in1);
            return 1;
        }

        FILE *out = fopen(out_path, "w");
        if (!out) {
            printf("Ошибка: невозможно создать выходной файл '%s' для записи.\n", out_path);
            fclose(in1);
            fclose(in2);
            return 1;
        }

        enum status_code sc = process_merge_files(in1, in2, out);

        fclose(in1);
        fclose(in2);
        fclose(out);

        if (sc != STATUS_OK) {
            if (sc == ERR_FILE_READ) {
                printf("Ошибка при чтении данных из входного файла.\n");
            } else if (sc == ERR_FILE_WRITE) {
                printf("Ошибка при записи в выходной файл.\n");
            } else if (sc == ERR_MEMORY_ALLOCATION) {
                printf("Ошибка выделения динамической памяти под лексему.\n");
            } else {
                printf("Произошла ошибка при обработке файлов.\n");
            }
            return 1;
        }

        printf("Объединение лексем успешно завершено. Результат записан в: %s\n", out_path);
        return 0;

    } else if (action == 'a') {
        if (argc != 4) {
            printf("Ошибка: для флага '%s' требуется ровно 2 пути к файлам: <in_file> <out_file> (передано: %d).\n", flag_str, argc - 2);
            return 1;
        }

        const char *in_path = argv[2];
        const char *out_path = argv[3];

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

        enum status_code sc = process_transform_file(in, out);

        fclose(in);
        fclose(out);

        if (sc != STATUS_OK) {
            if (sc == ERR_FILE_READ) {
                printf("Ошибка при чтении данных из входного файла.\n");
            } else if (sc == ERR_FILE_WRITE) {
                printf("Ошибка при записи в выходной файл.\n");
            } else if (sc == ERR_MEMORY_ALLOCATION) {
                printf("Ошибка выделения динамической памяти под лексему.\n");
            } else {
                printf("Произошла ошибка при обработке файла.\n");
            }
            return 1;
        }

        printf("Преобразование лексем успешно завершено. Результат записан в: %s\n", out_path);
        return 0;

    } else {
        printf("Ошибка: неизвестное действие '%c'. Допустимы: r, a.\n", action);
        return 1;
    }
}