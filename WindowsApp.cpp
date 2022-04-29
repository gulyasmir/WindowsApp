#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <string>
#include "Resource.h"
#include "SoftwareDefinitions.h"
#include "SoftwareCommunication.h"


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{

    // Register the window class.

    WNDCLASS SoftwareMainClass = NewWindowClass((HBRUSH)COLOR_WINDOW, LoadCursor(NULL, IDC_ARROW), hInstance, LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)), L"MainWndClass", WindowProc);

    if (!RegisterClassW(&SoftwareMainClass)) {return -1;}
    MSG SoftwareMainMessage = { 0 };

    CreateWindow(L"MainWndClass", L"Окно-болванка", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, NULL, NULL);

    // Run the message loop.

    while (GetMessage(&SoftwareMainMessage, NULL, NULL, NULL))
    {
        TranslateMessage(&SoftwareMainMessage);
        DispatchMessage(&SoftwareMainMessage);
    }

    return 0;
}

WNDCLASS NewWindowClass(HBRUSH BGColor, HCURSOR Cursor, HINSTANCE hInstance, HICON Icon, LPCWSTR Name, WNDPROC WindowProc) {

    WNDCLASS wc = { };

    wc.hIcon = Icon;
    wc.hCursor = Cursor;
    wc.hInstance = hInstance;
    wc.lpszClassName = Name;
    wc.hbrBackground = BGColor;
    wc.lpfnWndProc = WindowProc;

    return wc;
}



LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (isConnected) { SerialRead(); }
    switch (uMsg)
    {
    case WM_COMMAND:

        if ((wParam >= ComSelectIndex) && (wParam < ComSelectIndex + ComPortAmount)) {
            selectedPort = wParam - ComSelectIndex; 
            SetWindowStatus("PORT: " + std::to_string(selectedPort));
            SerialUpdate(); 
            break;
        }

        switch (wParam) {
        case OnConnectRequest:
            ConnectRequest();
            break;
 
        case OnSerialRefresh:
            SerialUpdate();
            break;

        case OnExitSoftware:
            PostQuitMessage(0);
            break;

        case OnClearField:
            SetWindowTextA(hEditControl, "");
            break;

        case OnMenuHelp:
            MessageBoxA(hwnd, "Текст справки!", "Справка", MB_OK);
            break;   

        case OnReadField:
            CharsRead = GetWindowTextA(hEditControl, Buffer, TextBufferSize);
            SetWindowTextA(hStaticControl, Buffer);
            SetWindowTextA(hStaticControl, ("Прочитано символов:" + std::to_string(CharsRead)).c_str());

            break;
            
        case OnReadNumberField:
            num = GetDlgItemInt(hwnd, DlgIndexNumber, FALSE, FALSE);
            SetWindowTextA(hStaticControl, std::to_string(num).c_str());
            break;

        case OnSaveFile:
            if (GetSaveFileNameA(&ofn)) {
                SaveData(filename);
           }

            break;

        case OnLoadFile:
            if (GetSaveFileNameA(&ofn)) {
                LoadData(filename);
            }
            break;

        default:  break;
        }
        break;

    case WM_CREATE:
        MainWndAddMenus(hwnd);
        MainWndAddWidgets(hwnd);
        SetOpenFileParams(hwnd);
        SerialUpdate();
        break;

    case WM_CLOSE:
        if (MessageBox(hwnd, L"Вы точно хотите закрыть программу?", L"Форточка с вопросом", MB_OKCANCEL) == IDOK)
        {
            DestroyWindow(hwnd);
        }
        // Else: User canceled. Do nothing.
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // All painting occurs here, between BeginPaint and EndPaint.
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        EndPaint(hwnd, &ps);
    }
    return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


void MainWndAddMenus(HWND hwnd) {
    HMENU RootMenu = CreateMenu();
    HMENU SubMenu = CreateMenu();
    HMENU SubActionMenu = CreateMenu();

    ComPortSubMenu = CreateMenu();
    ComPortListMenu = CreateMenu();

    AppendMenu(SubMenu, MF_STRING, OnSaveFile, L"Save");
    AppendMenu(SubMenu, MF_STRING, OnLoadFile, L"Load");
    AppendMenu(SubMenu, MF_STRING, OnConnectRequest, L"Connect");

    AppendMenu(SubMenu, MF_POPUP, (UINT_PTR)SubActionMenu, L"Action");
    AppendMenu(SubActionMenu, MF_STRING, OnClearField, L"Clear");

    AppendMenu(SubMenu, MF_SEPARATOR, NULL, NULL);
    AppendMenu(SubMenu, MF_STRING, OnExitSoftware, L"Выход");

    AppendMenu(ComPortSubMenu, MF_STRING, OnSerialRefresh, L"Refresh ports");
    AppendMenu(ComPortSubMenu, MF_POPUP, (UINT_PTR)ComPortListMenu, L"Selected port");

    AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)SubMenu, L"File");
    AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)ComPortSubMenu, L"Connection");
    AppendMenu(RootMenu, MF_STRING, OnMenuHelp, L"Справка");

    SetMenu(hwnd, RootMenu);
}

void MainWndAddWidgets(HWND hwnd) {

    hStaticControl = CreateWindowA("static", "Ниже можете написать текст", WS_VISIBLE | WS_CHILD | ES_CENTER, 5, 5, 490, 20, hwnd, NULL, NULL, NULL);

    hEditControl = CreateWindowA("edit", "this is edit control", WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_VSCROLL, 5, 30, 490, 100, hwnd, NULL, NULL, NULL);

    hNumberControl = CreateWindowA("edit", "111", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 5, 200, 150, 30, hwnd, (HMENU)DlgIndexNumber, NULL, NULL);

    CreateWindowA("button", "Очистить", WS_VISIBLE | WS_CHILD | ES_CENTER, 50, 150, 100, 30, hwnd, (HMENU)OnClearField, NULL, NULL);
    CreateWindowA("button", "Прочитать", WS_VISIBLE | WS_CHILD | ES_CENTER, 150, 150, 100, 30, hwnd, (HMENU)OnReadField, NULL, NULL);
    CreateWindowA("button", "Прочитать цифры", WS_VISIBLE | WS_CHILD | ES_CENTER, 150, 250, 150, 30, hwnd, (HMENU)OnReadNumberField, NULL, NULL);
}

void SaveData(LPCSTR path) {
    HANDLE FileToSave = CreateFileA(
        path,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    int saveLength = GetWindowTextLength(hEditControl);
    char* data = new char[saveLength];
    saveLength = GetWindowTextA(hEditControl, data, saveLength);

    DWORD bytesIterated;
    WriteFile(FileToSave, data, saveLength, &bytesIterated, NULL);

    CloseHandle(FileToSave);
    delete[] data;
}

void LoadData(LPCSTR path) {
    HANDLE FileToLoad = CreateFileA(
        path,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    DWORD bytesIterated;
    ReadFile(FileToLoad, Buffer, TextBufferSize, &bytesIterated, NULL);

    SetWindowTextA(hEditControl, Buffer);

    CloseHandle(FileToLoad);
}

void SetOpenFileParams(HWND hwnd) {
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = sizeof(filename);
    ofn.lpstrFilter = "*.txt";
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = "C:\\source\\";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
}

void SetWindowStatus(std::string status) {
    SetWindowTextA(hStaticControl, status.c_str());
}