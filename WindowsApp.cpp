#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include "Resource.h"
#include "SoftwareDefinitions.h"



int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{

    // Register the window class.

    WNDCLASS SoftwareMainClass = NewWindowClass((HBRUSH)COLOR_WINDOW, LoadCursor(NULL, IDC_ARROW), hInstance, LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)), L"MainWndClass", WindowProc);
   // RegisterClass(&SoftwareMainClass);

    if (!RegisterClassW(&SoftwareMainClass)) {return -1;}
    MSG SoftwareMainMessage = { 0 };

    CreateWindow(L"MainWndClass", L"Окно-болванка", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, NULL, NULL);

    // Create the window.
/*
    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        L"MainWndClass",                     // Window class
        L"Окно-болванка",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    */
    // Run the message loop.

    while (GetMessage(&SoftwareMainMessage, NULL, NULL, NULL))
    {
        TranslateMessage(&SoftwareMainMessage);
        DispatchMessage(&SoftwareMainMessage);
    }

    return 0;
}

WNDCLASS NewWindowClass(HBRUSH BGColor, HCURSOR Cursor, HINSTANCE hInstance, HICON Icon, LPCWSTR Name, WNDPROC WindowProc) {
  //  const wchar_t CLASS_NAME[] = L"Sample Window Class";

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
    switch (uMsg)
    {
    case WM_COMMAND:
        switch (wParam) {
        case OnMenuAction1:
            MessageBoxA(hwnd, "menu 1 has clicked!", "menu 1 worked", MB_OK);
            break;
        case OnMenuAction2:
            MessageBoxA(hwnd, "menu 2 has clicked!", "menu 2 worked", MB_OK);
            break;
        case OnMenuAction3:
            MessageBoxA(hwnd, "menu 3 has clicked!", "menu 3 worked", MB_OK);
            break;
        case OnExitSoftware:
            PostQuitMessage(0);
            break;

        case OnButtonClear:
            SetWindowTextA(hEditControl, "");
            break;

        case OnMenuHelp:
            MessageBoxA(hwnd, "Текст справки!", "Справка", MB_OK);
            break;   

        default:  break;
        }
        break;

    case WM_CREATE:
        MainWndAddMenus(hwnd);
        MainWndAddWidgets(hwnd);
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

    AppendMenu(SubActionMenu, MF_STRING, OnMenuAction1, L"Меню1");
    AppendMenu(SubActionMenu, MF_STRING, OnMenuAction2, L"Меню2");
    AppendMenu(SubActionMenu, MF_STRING, OnMenuAction3, L"Меню3");

    AppendMenu(SubMenu, MF_POPUP, (UINT_PTR)SubActionMenu, L"Action");
    AppendMenu(SubMenu, MF_SEPARATOR, NULL, NULL);
    AppendMenu(SubMenu, MF_STRING, OnExitSoftware, L"Выход");

    AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)SubMenu, L"File");
    AppendMenu(RootMenu, MF_STRING, OnMenuHelp, L"Справка");

    SetMenu(hwnd, RootMenu);
}

void MainWndAddWidgets(HWND hwnd) {

    CreateWindowA("static", "Ниже можете написать текст", WS_VISIBLE | WS_CHILD | ES_CENTER, 5, 5, 490, 20, hwnd, NULL, NULL, NULL);

    hEditControl = CreateWindowA("edit", "this is edit control", WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_VSCROLL, 5, 30, 490, 100, hwnd, NULL, NULL, NULL);

    CreateWindowA("button", "Очистить", WS_VISIBLE | WS_CHILD | ES_CENTER, 50, 150, 100, 30, hwnd, (HMENU)OnButtonClear, NULL, NULL);

}