#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define BUFFER_SIZE 1024

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // Создаем "трубу" для общения между процессами
    HANDLE hPipeRead, hPipeWrite;
    HANDLE hChildProcess = NULL;
    SECURITY_ATTRIBUTES saAttr;
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    char filename[256];
    CHAR buffer[BUFFER_SIZE];
    DWORD dwRead, dwWritten;
    BOOL bSuccess;
    
    // Настройка атрибутов безопасности для pipe
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;
    
    // Создаем pipe
    if (!CreatePipe(&hPipeRead, &hPipeWrite, &saAttr, 0)) {
        fprintf(stderr, "Ошибка создания pipe\n");
        return 1;
    }
    
    // Получаем имя файла от пользователя
    printf("Введите имя файла: ");
    if (scanf("%255s", filename) != 1) {
        fprintf(stderr, "Ошибка чтения имени файла\n");
        CloseHandle(hPipeRead);
        CloseHandle(hPipeWrite);
        return 1;
    }

    // Подготавливаем информацию для запуска дочерней программы
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si.hStdOutput = hPipeWrite;  // Перенаправляем вывод дочернего процесса в pipe 
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    
    // Подготавливаем командную строку для дочернего процесса
    char cmdLine[512];
    snprintf(cmdLine, sizeof(cmdLine), "child.exe \"%s\"", filename);
    
    // Создаем дочерний процесс
    if (!CreateProcess(
        NULL,       // Имя программы
        cmdLine,    // Командная строка
        NULL,       // Атрибуты процесса не наследуются
        NULL,       // Атрибуты потока не наследуются
        TRUE,       // Наследуем handles
        0,          // Флаги создания
        NULL,       // Используем окружение родителя
        NULL,       // Используем текущий каталог
        &si,        // STARTUPINFO
        &pi)) {     // PROCESS_INFORMATION
        
        fprintf(stderr, "Ошибка создания дочернего процесса\n");
        CloseHandle(hPipeRead);
        CloseHandle(hPipeWrite);
        return 1;
    }
    
    // Закрываем ненужные handles
    CloseHandle(hPipeWrite);
    CloseHandle(pi.hThread);
    hChildProcess = pi.hProcess;
    
    // Читаем из pipe и выводим в консоль
    while (TRUE) {
        bSuccess = ReadFile(hPipeRead, buffer, BUFFER_SIZE - 1, &dwRead, NULL);
        if (!bSuccess || dwRead == 0) {
            break;
        }
        
        buffer[dwRead] = '\0';
        printf("%s", buffer);
    }
    
    // Ждем завершения дочернего процесса
    WaitForSingleObject(hChildProcess, INFINITE);
    
    // Закрываем handles
    CloseHandle(hPipeRead);
    CloseHandle(hChildProcess);
    
    return 0;
}