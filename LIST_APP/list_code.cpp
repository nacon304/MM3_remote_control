#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <stdlib.h>

#pragma comment(lib, "advapi32.lib")

using namespace std;

void printMenu()
{
    cout << "+----------------------------------------+\n";
    cout << "| Menu                                   |\n";
    cout << "+----------------------------------------+\n";
    cout << "| 0. Liet ke cac app                     |\n";
    cout << "| 1. Bat dau chay app                    |\n";
    cout << "| 2. Ket thuc app                        |\n";
    cout << "+----------------------------------------+\n";
}
vector<string> listAllAppDemo()
{
    std::vector<std::string> installed_programs;

    HKEY hKey;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths", 0, KEY_READ | KEY_WOW64_32KEY, &hKey) != ERROR_SUCCESS) {
        std::cerr << "Failed to open registry key" << std::endl;
        return installed_programs;
    }

    char szDisplayName[1024];
    char szKeyName[1024];
    DWORD dwSize = sizeof(szDisplayName);

    for (DWORD i = 0, retValue = ERROR_SUCCESS; retValue == ERROR_SUCCESS; ++i) {
        retValue = RegEnumKeyEx(hKey, i, szKeyName, &dwSize, NULL, NULL, NULL, NULL);
        dwSize = sizeof(szKeyName);

        if (retValue == ERROR_SUCCESS) {
            HKEY hSubKey;
            if (RegOpenKeyEx(hKey, szKeyName, 0, KEY_READ | KEY_WOW64_32KEY, &hSubKey) == ERROR_SUCCESS) {
                dwSize = sizeof(szDisplayName);
                if (RegQueryValueEx(hSubKey, NULL, NULL, NULL, (LPBYTE)szDisplayName, &dwSize) == ERROR_SUCCESS) {
                    installed_programs.push_back(szDisplayName);
                }
                RegCloseKey(hSubKey);
            }
        }
    }

    RegCloseKey(hKey);

    installed_programs.erase(unique(installed_programs.begin(), installed_programs.end()), installed_programs.end());
    
    int cnt = 0;
    for (;cnt < installed_programs.size();)
    {
        installed_programs[cnt].erase(remove(installed_programs[cnt].begin(), installed_programs[cnt].end(), '\"'), installed_programs[cnt].end());
        string program = installed_programs[cnt];
        std::cout << ++cnt << ". " << program << std::endl;
    }

    return installed_programs;
}
void listAllApp()
{
    std::vector<std::string> installed_programs;

    HKEY hKey;
    LSTATUS result;

    // Mở registry key
    result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall", 0, KEY_READ, &hKey);
    if (result != ERROR_SUCCESS) {
        std::cout << "Không thể mở registry key." << std::endl;
        return ;
    }

    // Lấy số lượng subkey
    DWORD subkeyCount;
    result = RegQueryInfoKey(hKey, NULL, NULL, NULL, &subkeyCount, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    if (result != ERROR_SUCCESS) {
        std::cout << "Không thể truy vấn thông tin registry key." << std::endl;
        return ;
    }

    // Liệt kê các subkey và in ra tên ứng dụng
    for (DWORD i = 0; i < subkeyCount; i++) {
        char subkeyName[256];
        DWORD subkeyNameLength = 256;
        result = RegEnumKeyEx(hKey, i, subkeyName, &subkeyNameLength, NULL, NULL, NULL, NULL);
        if (result != ERROR_SUCCESS) {
            std::cout << "Không thể liệt kê subkey." << std::endl;
            return ;
        }

        HKEY appKey;
        result = RegOpenKeyEx(hKey, subkeyName, 0, KEY_READ, &appKey);
        if (result != ERROR_SUCCESS) {
            continue;
        }

        DWORD valueSize;
        char displayName[256];
        valueSize = 256;
        result = RegQueryValueEx(appKey, "DisplayName", NULL, NULL, (LPBYTE)displayName, &valueSize);
        if (result != ERROR_SUCCESS) {
            continue;
        }

        installed_programs.push_back(displayName);

        RegCloseKey(appKey);
    }

    // Đóng registry key
    RegCloseKey(hKey);

    installed_programs.erase(unique(installed_programs.begin(), installed_programs.end()), installed_programs.end());
    sort(installed_programs.begin(), installed_programs.end());

    int cnt = 0;
    for (auto const & program : installed_programs)
    {
        if(program == "${{arpDisplayName}}")
            continue;
        std::cout << ++cnt << ". " << program << std::endl;
    }
}
bool isPositiveInteger(const string& s)
{
    if (s.empty() || (!isdigit(s[0])))
        return false;
    for (char c : s) {
        if (!isdigit(c))
            return false;
    }
    if (s.size() > 1 && s[0] == '0')
        return false;
    return true;
}
string findChoice(string choice, vector<string> pathApps)
{
    string pathApp = choice;
    if (isPositiveInteger(choice))
    {
        int idx = stoi(choice);
        if (idx > pathApps.size())
            return "ERROR";

        pathApp = pathApps[idx - 1];
    }

    return pathApp;
}
string escapePath(string path) 
{
    string escapedPath;
    for (char c : path) {
        if (c == '\\' || c == '\"') {
            escapedPath += '\\';
        }
        escapedPath += c;
    }
    return escapedPath;
}
string ConvertExePath(const string& path)
{
    string convertedPath;
    char expandedPath[MAX_PATH];
    ExpandEnvironmentStrings(path.c_str(), expandedPath, MAX_PATH);
    string newPath = string(expandedPath);
    for (char c : newPath)
        if (c == '\\')
            convertedPath += "\\\\";
        else
            convertedPath += c;

    return convertedPath;
}
void runApp(string exePath)
{
    std::string convertedPath = ConvertExePath(exePath);
    cout<<convertedPath<<endl;

    ShellExecute(NULL, "open", convertedPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
}
void processRunApp()
{
    cout << "Cac app co the chay: \n";
    vector<string> appsDemo = listAllAppDemo();
    cout << endl;

    fflush(stdin);
    cout << "App ban muon chay (nhap <so thu tu> hoac <dia chi>): ";
    string choice;
    cin >> choice;

    string path = findChoice(choice, appsDemo);
    if (path == "ERROR")
        cout << "Nhap khong hop le!\n";
    cout << "Chay app: " << path << endl;
    runApp(path);
}
void solve()
{
    char continue_choice;
    do {
        printMenu();

        cout << endl;
        int opt;
        cout << "Thao tac ban chon (0, 1, 2): ";
        cin >> opt;
        cout << endl;

        if (opt == 0) {
            cout << "Ban da chon thao tac 0" << endl;
            cout << endl;

            cout << "Day la cac app co tren may tinh:\n";
            listAllApp();
            cout << endl;
        }
        else if (opt == 1) {
            // Xử lý thao tác 1
            cout << "Ban da chon thao tac 1" << endl;
            cout << endl;

            processRunApp();
            cout << endl;
        } 
        else {
            // Xử lý thao tác 2
            cout << "Ban da chon thao tac 2" << endl;
            cout << endl;
            
            // processCloseApp();
            cout << endl;
        }

        cout << "Ban co muon tiep tuc thao tac voi App? (nhap 'y' de tiep tuc): ";
        cin >> continue_choice;
    } while (continue_choice == 'y');
}
int main()
{
    solve();
}

// run: g++ list_code.cpp -o list_code.exe & list_code.exe