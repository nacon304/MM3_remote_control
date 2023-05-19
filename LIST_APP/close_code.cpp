// #include <cstdio>

// int main() {
//     // FILE* file = fopen("\\Device\\HarddiskVolume3\\Windows\\explorer.exe", "w");
//     FILE* file = fopen("\\Device\\HarddiskVolume3\\Program Files (x86)\\Windows Media Player\\wmplayer.exe", "r");
//     if (file != nullptr) {
//         // Thực hiện các thao tác cần thiết trên tệp tin ở đây
        
//         fclose(file); // Đóng tệp tin
//         printf("File closed successfully.\n");
//     } else {
//         printf("Failed to open the file.\n");
//     }
    
//     return 0;
// }
#include <iostream>
#include <windows.h>
#include <tlhelp32.h>

bool TerminateProcessByName(const char* processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return false;
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hSnapshot, &pe)) {
        CloseHandle(hSnapshot);
        return false;
    }

    bool found = false;
    do {
        if (_stricmp(pe.szExeFile, processName) == 0) {
            found = true;
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0, pe.th32ProcessID);
            if (hProcess != NULL) {
                TerminateProcess(hProcess, 0);
                CloseHandle(hProcess);
            }
        }
    } while (Process32Next(hSnapshot, &pe));

    CloseHandle(hSnapshot);
    return found;
}

int main() {
    const char* processName = "WINWORD.EXE";
    if (TerminateProcessByName(processName)) {
        std::cout << "Process terminated successfully." << std::endl;
    } else {
        std::cout << "Failed to terminate the process." << std::endl;
    }

    return 0;
}