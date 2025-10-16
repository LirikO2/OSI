#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

int main(int argc, char *argv[]) {
    SetConsoleOutputCP(CP_UTF8);

    if (argc != 2) {
        fprintf(stderr, "Использование: %s <имя_файла>\n", argv[0]);
        return 1;
    }
    
    // Открываем файл для чтения
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        fprintf(stderr, "Ошибка открытия файла: %s\n", argv[1]);
        return 1;
    }
    
    char line[1024];
    
    // Читаем файл построчно
    while (fgets(line, sizeof(line), file) != NULL) {
        // Удаляем символ новой строки
        line[strcspn(line, "\n\r")] = '\0';
        
        // Пропускаем пустые строки
        if (strlen(line) == 0) {
            continue;
        }
        
        // Разбираем числа из строки
        char *token = strtok(line, " ");
        float sum = 0.0f;
        int count = 0;
        
        while (token != NULL) {
            float number;
            if (sscanf(token, "%f", &number) == 1) {
                sum += number;
                count++;
            }
            token = strtok(NULL, " ");
        }
        
        // Выводим результат
        if (count > 0) {
            printf("Сумма: %.2f (из %d чисел)\n", sum, count);
        } else {
            printf("Не найдено чисел в строке\n");
        }
        
        // Сбрасываем буфер вывода
        fflush(stdout);
    }
    
    fclose(file);
    return 0;
}