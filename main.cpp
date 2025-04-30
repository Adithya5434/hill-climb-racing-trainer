#define UNICODE
#define _UNICODE

#include <windows.h>
#include <psapi.h>
#include <tchar.h>
#include <string>
#include <sstream>

#define ID_SET_MONEY     1
#define ID_SET_DIAMONDS  2

HWND hMoneyEdit, hDiamondEdit;
DWORD_PTR moneyAddress = 0;
DWORD_PTR diamondsAddress = 0;
HANDLE hProcess = nullptr;

// Helper to show message box
void ShowError(const std::wstring& msg) {
    MessageBoxW(nullptr, msg.c_str(), L"Error", MB_ICONERROR);
}

// Get process ID by name
DWORD GetProcessIdByName(const wchar_t* processName) {
    DWORD pids[1024], needed;
    if (!EnumProcesses(pids, sizeof(pids), &needed)) return 0;

    DWORD count = needed / sizeof(DWORD);
    for (DWORD i = 0; i < count; i++) {
        if (pids[i] == 0) continue;

        HANDLE h = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pids[i]);
        if (h) {
            HMODULE hMod;
            DWORD cbNeeded;
            if (EnumProcessModules(h, &hMod, sizeof(hMod), &cbNeeded)) {
                TCHAR szName[MAX_PATH];
                if (GetModuleBaseName(h, hMod, szName, MAX_PATH)) {
                    if (_wcsicmp(szName, processName) == 0) {
                        CloseHandle(h);
                        return pids[i];
                    }
                }
            }
            CloseHandle(h);
        }
    }
    return 0;
}

DWORD_PTR GetModuleBaseAddress(DWORD pid, const wchar_t* moduleName) {
    HANDLE h = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (!h) return 0;

    HMODULE hMods[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(h, hMods, sizeof(hMods), &cbNeeded)) {
        for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
            TCHAR szModName[MAX_PATH];
            if (GetModuleBaseName(h, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR))) {
                if (_wcsicmp(szModName, moduleName) == 0) {
                    DWORD_PTR base = (DWORD_PTR)hMods[i];
                    CloseHandle(h);
                    return base;
                }
            }
        }
    }
    CloseHandle(h);
    return 0;
}

void WriteIntToAddress(DWORD_PTR addr, int value) {
    WriteProcessMemory(hProcess, (LPVOID)addr, &value, sizeof(value), nullptr);
}

void OnSetMoney() {
    wchar_t buffer[32];
    GetWindowText(hMoneyEdit, buffer, 32);
    int money = _wtoi(buffer);
    WriteIntToAddress(moneyAddress, money);
}

void OnSetDiamonds() {
    wchar_t buffer[32];
    GetWindowText(hDiamondEdit, buffer, 32);
    int diamonds = _wtoi(buffer);
    WriteIntToAddress(diamondsAddress, diamonds);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_COMMAND:
        if (LOWORD(wParam) == ID_SET_MONEY) OnSetMoney();
        else if (LOWORD(wParam) == ID_SET_DIAMONDS) OnSetDiamonds();
        break;

    case WM_DESTROY:
        if (hProcess) CloseHandle(hProcess);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    DWORD pid = GetProcessIdByName(L"HillClimbRacing.exe");
    if (!pid) {
        ShowError(L"Hill Climb Racing is not running.");
        return 1;
    }

    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hProcess) {
        ShowError(L"Cannot open process.");
        return 1;
    }

    DWORD_PTR base = GetModuleBaseAddress(pid, L"HillClimbRacing.exe");
    if (!base) {
        ShowError(L"Could not get module base address.");
        return 1;
    }

    moneyAddress = base + 0x28CAD4;
    diamondsAddress = base + 0x28CAEC;

    const wchar_t CLASS_NAME[] = L"HCRTrainerWindow";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, L"HCR Trainer", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 180,
        nullptr, nullptr, hInstance, nullptr
    );

    CreateWindowW(L"STATIC", L"Money:", WS_VISIBLE | WS_CHILD,
        10, 10, 80, 20, hwnd, nullptr, hInstance, nullptr);

    hMoneyEdit = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
        90, 10, 100, 20, hwnd, nullptr, hInstance, nullptr);

    CreateWindowW(L"BUTTON", L"Set", WS_VISIBLE | WS_CHILD,
        200, 10, 50, 20, hwnd, (HMENU)ID_SET_MONEY, hInstance, nullptr);

    CreateWindowW(L"STATIC", L"Diamonds:", WS_VISIBLE | WS_CHILD,
        10, 40, 80, 20, hwnd, nullptr, hInstance, nullptr);

    hDiamondEdit = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
        90, 40, 100, 20, hwnd, nullptr, hInstance, nullptr);

    CreateWindowW(L"BUTTON", L"Set", WS_VISIBLE | WS_CHILD,
        200, 40, 50, 20, hwnd, (HMENU)ID_SET_DIAMONDS, hInstance, nullptr);

    CreateWindowW(L"STATIC", L"Note: value only updates on main menu.", WS_VISIBLE | WS_CHILD,
        10, 70, 280, 20, hwnd, nullptr, hInstance, nullptr);

    CreateWindowW(L"STATIC", L"Made by: github.com/Advik", WS_VISIBLE | WS_CHILD,
        10, 90, 280, 20, hwnd, nullptr, hInstance, nullptr);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
