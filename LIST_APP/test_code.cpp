#include <windows.h>
#include <iostream>
#include <psapi.h>

int main()
{
    // Enumerate the top-level windows
    int cnt = 0;
    HWND hwnd = NULL;
    while ((hwnd = FindWindowEx(NULL, hwnd, NULL, NULL)) != NULL) {
        // Get the process ID of the window
        DWORD processId;
        GetWindowThreadProcessId(hwnd, &processId);

        // Get the handle to the process
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);

        // Get the executable name of the process
        if (hProcess != NULL) {
            char exeName[MAX_PATH];
            DWORD exeNameSize = sizeof(exeName);
            if (GetProcessImageFileNameA(hProcess, exeName, exeNameSize)) {
                std::cout << "Application name: " << exeName << "\n";
                cnt ++;
            }
            CloseHandle(hProcess);
        }
    }
    std::cout << cnt;
    return 0;
}

// g++ test_code.cpp -o test_code.exe -lpsapi & test_code.exe