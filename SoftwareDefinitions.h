#pragma once
#define OnMenuAction1 1
#define OnMenuAction2 2
#define OnMenuAction3 3
#define OnExitSoftware 4
#define OnButtonClear 5
#define OnMenuHelp 6

HWND hEditControl;
WNDCLASS NewWindowClass(HBRUSH BGColor, HCURSOR Cursor, HINSTANCE hInstance, HICON Icon, LPCWSTR Name, WNDPROC WindowProc);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void MainWndAddMenus(HWND hwnd);
void MainWndAddWidgets(HWND hwnd);