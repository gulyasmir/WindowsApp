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

    CreateWindow(L"MainWndClass", L"Сканер отпечатка пальца", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 800, 400, NULL, NULL, NULL, NULL);

    // Run the message loop.

    while (GetMessage(&SoftwareMainMessage, NULL, NULL, NULL))
    {
        TranslateMessage(&SoftwareMainMessage);
        DispatchMessage(&SoftwareMainMessage);
    }
    TerminateThread(readThread, 0);
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

void ExitSoftware(void) {
    isConnected = false;
    isThreading = false;
    CloseHandle(connectedPort);
    CloseHandle(readThread);
    ExitThread(0);
    PostQuitMessage(0);
}



LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
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
            ConnectButtonView(hwnd);
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
            MessageBoxA(hwnd, "Это программа интерфейс для взаимодействия со сканером отпечатка пальца.", "Справка", MB_OK);
            break;   

        case OnReadField:
            CharsRead = GetWindowTextA(hEditControl, Buffer, TextBufferSize);
            SetWindowTextA(hStaticControlReadText, Buffer); 
            break;
            
        case OnReadCountText:
            CharsRead = GetWindowTextA(hEditControl, Buffer, TextBufferSize);
            SetWindowTextA(hStaticControlReadCountText, ("Количество символов:" + std::to_string(CharsRead)).c_str());
            break;

        case OnReadNumberField:
            num = GetDlgItemInt(hwnd, DlgIndexNumber, FALSE, FALSE);
            SetWindowTextA(hStaticControlReadNumber, std::to_string(num).c_str());
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

        case DisconnectThis:
            isConnected = false;
            isThreading = false;
            CloseHandle(connectedPort);
            CloseHandle(readThread);
            SetWindowStatus("Разъединено");
            DestroyWindow(ConnectedButton);
            break;

        default:  break;
        }
        break;

    case WM_CREATE:
        MainWndAddMenus(hwnd);
        MainWndAddWidgets(hwnd);
        SetOpenFileParams(hwnd);
        SerialUpdate();
        readThread = CreateThread(NULL, 0, SerialRead, NULL, 0, NULL);
        break;

    case WM_CLOSE:
        if (MessageBox(hwnd, L"Вы точно хотите закрыть программу?", L"Закрытие программы", MB_OKCANCEL) == IDOK)
        {
           // DestroyWindow(hwnd);

            isConnected = false;
            isThreading = false;
            CloseHandle(connectedPort);
            CloseHandle(readThread);

            ExitSoftware();
        }
        // Else: User canceled. Do nothing.
        return 0;

    case WM_DESTROY:
        ExitSoftware();
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

    AppendMenu(SubMenu, MF_STRING, OnSaveFile, L"Сохранить текст");
    AppendMenu(SubMenu, MF_STRING, OnLoadFile, L"Загрузить из файла");
    AppendMenu(SubMenu, MF_STRING, OnConnectRequest, L"Соединение");

    AppendMenu(SubMenu, MF_POPUP, (UINT_PTR)SubActionMenu, L"Действие");
    AppendMenu(SubActionMenu, MF_STRING, OnClearField, L"Очистить");

    AppendMenu(SubMenu, MF_SEPARATOR, NULL, NULL);
    AppendMenu(SubMenu, MF_STRING, OnExitSoftware, L"Выход");

    AppendMenu(ComPortSubMenu, MF_STRING, OnSerialRefresh, L"Обновить порты");
    AppendMenu(ComPortSubMenu, MF_POPUP, (UINT_PTR)ComPortListMenu, L"Выбрать порт");

    AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)SubMenu, L"Файл");
    AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)ComPortSubMenu, L"Соединения");
    AppendMenu(RootMenu, MF_STRING, OnMenuHelp, L"Справка");

    SetMenu(hwnd, RootMenu);
}

void MainWndAddWidgets(HWND hwnd) {

    hStaticControl = CreateWindowA("static", "Ввод текста", WS_VISIBLE | WS_CHILD | ES_CENTER, 5, 5, 500, 20, hwnd, NULL, NULL, NULL);

    hStaticControlReadText = CreateWindowA("static", " ", WS_VISIBLE | WS_CHILD | ES_CENTER, 600, 35, 200, 80, hwnd, NULL, NULL, NULL);
 
    hStaticControlReadCountText = CreateWindowA("static", " ", WS_VISIBLE | WS_CHILD | ES_CENTER, 600, 130, 200, 80, hwnd, NULL, NULL, NULL);

    hStaticControlReadNumber = CreateWindowA("static", " ", WS_VISIBLE | WS_CHILD | ES_CENTER, 350, 200, 200, 30, hwnd, NULL, NULL, NULL);

    hEditControl = CreateWindowA("edit", "Этот текст можно отредактировать", WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_VSCROLL | WS_BORDER, 5, 30, 570, 100, hwnd, NULL, NULL, NULL);

    hNumberControl = CreateWindowA("edit", "111", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER | WS_BORDER, 10, 200, 100, 30, hwnd, (HMENU)DlgIndexNumber, NULL, NULL);

    hStaticControConnected = CreateWindowA("static", "Соединение отсутствует", WS_VISIBLE | WS_CHILD | ES_CENTER, 10, 270, 200, 30, hwnd, NULL, NULL, NULL);

    CreateWindowA("button", "Очистить", WS_VISIBLE | WS_CHILD | ES_CENTER, 10, 150, 100, 30, hwnd, (HMENU)OnClearField, NULL, NULL);
    CreateWindowA("button", "Считать введенный текст", WS_VISIBLE | WS_CHILD | ES_CENTER, 130, 150, 200, 30, hwnd, (HMENU)OnReadField, NULL, NULL);
    CreateWindowA("button", "Подсчет символов", WS_VISIBLE | WS_CHILD | ES_CENTER, 350, 150, 200, 30, hwnd, (HMENU)OnReadCountText, NULL, NULL);

    CreateWindowA("button", "Считать введеные цифры", WS_VISIBLE | WS_CHILD | ES_CENTER, 130, 200, 200, 30, hwnd, (HMENU)OnReadNumberField, NULL, NULL);

   
}



void WindowHide(HWND hwnd) {
      DestroyWindow(hwnd);
}

void ConnectButtonView(HWND hwnd) {
    ConnectedButton = CreateWindowA("button", "Разъединить", WS_VISIBLE | WS_CHILD | ES_CENTER, 250, 270, 100, 30, hwnd, (HMENU)DisconnectThis, NULL, NULL);
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
    ofn.lpstrInitialDir = "C:\\";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
}

void SetWindowStatus(std::string status) {
    SetWindowTextA(hStaticControConnected, status.c_str());
}

