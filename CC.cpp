#include <windows.h>
#include <string>
#include <fstream>
#include <sstream>
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")
#define IDC_COMBO_LANG  101
#define IDC_RADIO_MODE1 102
#define IDC_RADIO_MODE2 103
#define IDC_RADIO_MODE3 104
#define IDC_BTN_START   105
#define IDC_STATIC_DROP 106
#define BG_DARK      RGB(30, 30, 30)    
#define BG_LIGHT     RGB(45, 45, 48)    
#define TEXT_WHITE   RGB(240, 240, 240) 
HWND hComboLang, hRadioMode1, hRadioMode2, hRadioMode3, hBtnStart, hStaticDrop;
std::wstring g_filePath = L"";
HBRUSH hbrBkgnd = NULL;
HBRUSH hbrCard = NULL;
HFONT hFontMain = NULL;
HFONT hFontDrop = NULL;
bool isLineEmpty(const std::string& line) {
    for (char c : line) {
        if (c != ' ' && c != '\t' && c != '\r' && c != '\n' && (unsigned char)c != 13) {
            return false; 
        }
    }
    return true; 
}
void cleanCodeFile(const std::wstring& path, int langIndex, int mode) {
    std::ifstream inFile(path, std::ios::binary);
    if (!inFile.is_open()) {
        MessageBoxW(NULL, L"Не удалось открыть выбранный файл!", L"Ошибка", MB_OK | MB_ICONERROR);
        return;
    }
    std::stringstream buffer;
    buffer << inFile.rdbuf();
    std::string content = buffer.str();
    inFile.close();
    std::stringstream ss(content);
    std::string line;
    std::string result = "";
    bool inBlockComment = false;
    while (std::getline(ss, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (langIndex != 5 && (mode == 0 || mode == 2)) {
            bool inString = false;
            char stringChar = 0;
            size_t commentPos = std::string::npos;
            for (size_t i = 0; i < line.length(); ++i) {
                if ((line[i] == '"' || line[i] == '\'') && (i == 0 || line[i - 1] != '\\')) {
                    if (!inString) {
                        inString = true;
                        stringChar = line[i];
                    }
                    else if (line[i] == stringChar) {
                        inString = false;
                    }
                }
                if (!inString) {
                    if (langIndex >= 1 && langIndex <= 4) {
                        if (line[i] == '/' && i + 1 < line.length() && line[i + 1] == '/') {
                            commentPos = i;
                            break; 
                        }
                    }
                    else if (langIndex == 0) {
                        if (line[i] == '#') {
                            commentPos = i;
                            break;
                        }
                    }
                }
            }
            if (commentPos != std::string::npos) {
                line = line.substr(0, commentPos);
            }
        }
        if (mode == 1 || mode == 2 || langIndex == 5) {
            if (!isLineEmpty(line)) {
                result += line + "\n";
            }
        }
        else {
            result += line + "\n";
        }
    }
    std::wstring outPath = path;
    size_t dotPos = outPath.find_last_of(L'.');
    if (dotPos != std::wstring::npos) outPath.insert(dotPos, L"_cleaned");
    else outPath += L"_cleaned";
    std::ofstream outFile(outPath, std::ios::binary);
    if (outFile.is_open()) {
        outFile.write(result.c_str(), result.length());
        outFile.close();
        MessageBoxW(NULL, L"Очистка успешно завершена!", L"Успех!", MB_OK | MB_ICONINFORMATION);
    }
    else {
        MessageBoxW(NULL, L"Не удалось записать очищенный файл!", L"Ошибка", MB_OK | MB_ICONERROR);
    }
}
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        ChangeWindowMessageFilterEx(hwnd, WM_DROPFILES, MSGFLT_ALLOW, NULL);
        ChangeWindowMessageFilterEx(hwnd, 0x0049, MSGFLT_ALLOW, NULL);
        hFontMain = CreateFontW(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");
        hFontDrop = CreateFontW(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");
        hStaticDrop = CreateWindowW(L"STATIC", L"\n\nПЕРЕТАЩИТЕ СЮДА ФАЙЛ С КОДОМ", WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER, 20, 20, 350, 75, hwnd, (HMENU)IDC_STATIC_DROP, NULL, NULL);
        SendMessageW(hStaticDrop, WM_SETFONT, (WPARAM)hFontDrop, TRUE);
        HWND hLabel = CreateWindowW(L"STATIC", L"Выберите язык или тип файла:", WS_CHILD | WS_VISIBLE, 20, 105, 250, 20, hwnd, NULL, NULL, NULL);
        SendMessageW(hLabel, WM_SETFONT, (WPARAM)hFontMain, TRUE);
        hComboLang = CreateWindowW(L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL, 20, 125, 350, 200, hwnd, (HMENU)IDC_COMBO_LANG, NULL, NULL);
        SendMessageW(hComboLang, WM_SETFONT, (WPARAM)hFontMain, TRUE);
        SendMessageW(hComboLang, CB_ADDSTRING, 0, (LPARAM)L"Python (.py)");
        SendMessageW(hComboLang, CB_ADDSTRING, 0, (LPARAM)L"C++ / C (.cpp, .c, .h)");
        SendMessageW(hComboLang, CB_ADDSTRING, 0, (LPARAM)L"Rust (.rs)");
        SendMessageW(hComboLang, CB_ADDSTRING, 0, (LPARAM)L"JavaScript / TypeScript (.js, .ts)");
        SendMessageW(hComboLang, CB_ADDSTRING, 0, (LPARAM)L"Go (.go)");
        SendMessageW(hComboLang, CB_ADDSTRING, 0, (LPARAM)L"Любой другой текстовый файл (Только пустые строки)");
        SendMessageW(hComboLang, CB_SETCURSEL, 0, 0);
        hRadioMode1 = CreateWindowW(L"BUTTON", L"Удалить только комментарии", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP, 20, 165, 350, 20, hwnd, (HMENU)IDC_RADIO_MODE1, NULL, NULL);
        hRadioMode2 = CreateWindowW(L"BUTTON", L"Удалить только ПОЛНОСТЬЮ пустые строки", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 20, 190, 350, 20, hwnd, (HMENU)IDC_RADIO_MODE2, NULL, NULL);
        hRadioMode3 = CreateWindowW(L"BUTTON", L"Удалить комментарии и пустые строки", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 20, 215, 350, 20, hwnd, (HMENU)IDC_RADIO_MODE3, NULL, NULL);
        SendMessageW(hRadioMode1, WM_SETFONT, (WPARAM)hFontMain, TRUE);
        SendMessageW(hRadioMode2, WM_SETFONT, (WPARAM)hFontMain, TRUE);
        SendMessageW(hRadioMode3, WM_SETFONT, (WPARAM)hFontMain, TRUE);
        SendMessageW(hRadioMode3, BM_SETCHECK, BST_CHECKED, 0); 
        hBtnStart = CreateWindowW(L"BUTTON", L"ОЧИСТИТЬ ФАЙЛ", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 20, 250, 350, 42, hwnd, (HMENU)IDC_BTN_START, NULL, NULL);
        SendMessageW(hBtnStart, WM_SETFONT, (WPARAM)hFontDrop, TRUE);
        DragAcceptFiles(hwnd, TRUE);
        return 0;
    }
    case WM_CTLCOLORSTATIC: {
        HDC hdcStatic = (HDC)wParam;
        HWND hwndStatic = (HWND)lParam;
        SetTextColor(hdcStatic, TEXT_WHITE);
        SetBkMode(hdcStatic, TRANSPARENT);
        if (hwndStatic == hStaticDrop) return (INT_PTR)hbrCard;
        return (INT_PTR)hbrBkgnd;
    }
    case WM_DROPFILES: {
        HDROP hDrop = (HDROP)wParam;
        wchar_t fileBuffer[MAX_PATH];
        if (DragQueryFileW(hDrop, 0, fileBuffer, MAX_PATH)) {
            g_filePath = fileBuffer;
            size_t lastSlash = g_filePath.find_last_of(L"\\/");
            std::wstring fileName = (lastSlash != std::wstring::npos) ? g_filePath.substr(lastSlash + 1) : g_filePath;
            SetWindowTextW(hStaticDrop, (L"\nФайл добавлен:\n" + fileName).c_str());
            size_t dotPos = fileName.find_last_of(L'.');
            if (dotPos != std::wstring::npos) {
                std::wstring ext = fileName.substr(dotPos);
                if (ext == L".py") SendMessageW(hComboLang, CB_SETCURSEL, 0, 0);
                else if (ext == L".cpp" || ext == L".c" || ext == L".h") SendMessageW(hComboLang, CB_SETCURSEL, 1, 0);
                else if (ext == L".rs") SendMessageW(hComboLang, CB_SETCURSEL, 2, 0);
                else if (ext == L".js" || ext == L".ts") SendMessageW(hComboLang, CB_SETCURSEL, 3, 0);
                else if (ext == L".go") SendMessageW(hComboLang, CB_SETCURSEL, 4, 0);
                else SendMessageW(hComboLang, CB_SETCURSEL, 5, 0); 
            }
        }
        DragFinish(hDrop);
        return 0;
    }
    case WM_COMMAND: {
        if (LOWORD(wParam) == IDC_BTN_START) {
            if (g_filePath.empty()) {
                MessageBoxW(hwnd, L"Перетащите файл с кодом в верхнее поле окна!", L"Внимание", MB_OK | MB_ICONWARNING);
                return 0;
            }
            int langIndex = (int)SendMessageW(hComboLang, CB_GETCURSEL, 0, 0);
            int mode = 0; 
            if (SendMessageW(hRadioMode2, BM_GETCHECK, 0, 0) == BST_CHECKED) mode = 1;
            if (SendMessageW(hRadioMode3, BM_GETCHECK, 0, 0) == BST_CHECKED) mode = 2;
            cleanCodeFile(g_filePath, langIndex, mode);
        }
        return 0;
    }
    case WM_DESTROY:
        DeleteObject(hbrBkgnd);
        DeleteObject(hbrCard);
        DeleteObject(hFontMain);
        DeleteObject(hFontDrop);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    InitCommonControls();
    hbrBkgnd = CreateSolidBrush(BG_DARK);
    hbrCard = CreateSolidBrush(BG_LIGHT);
    const wchar_t CLASS_NAME[] = L"UltimateCodeCleanerClass";
    WNDCLASSW wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = hbrBkgnd;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassW(&wc);
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int winWidth = 405;
    int winHeight = 350; 
    int posX = (screenWidth - winWidth) / 2;
    int posY = (screenHeight - winHeight) / 2;
    HWND hwnd = CreateWindowExW(
        0, CLASS_NAME, L"CleanCode Tool",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        posX, posY, winWidth, winHeight, NULL, NULL, hInstance, NULL
    );
    if (hwnd == NULL) return 0;
    ShowWindow(hwnd, nCmdShow);
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
