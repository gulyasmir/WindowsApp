#pragma once
#define OnMenuAction1 1
#define OnMenuAction2 2
#define OnMenuAction3 3
#define OnExitSoftware 4
#define OnClearField 5
#define OnMenuHelp 6
#define OnReadField 7
#define OnSaveFile 8
#define OnLoadFile 9
#define OnReadNumberField 11

#define DlgIndexNumber 256


#define	TextBufferSize 256

char Buffer[TextBufferSize];
int CharsRead;
unsigned num;

HWND hEditControl;
HWND hStaticControl;
HWND hNumberControl;

char filename[260];
OPENFILENAMEA ofn;

WNDCLASS NewWindowClass(HBRUSH BGColor, HCURSOR Cursor, HINSTANCE hInstance, HICON Icon, LPCWSTR Name, WNDPROC WindowProc);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void MainWndAddMenus(HWND hwnd);
void MainWndAddWidgets(HWND hwnd);
void SaveData(LPCSTR path);
void LoadData(LPCSTR path);
void SetOpenFileParams(HWND hwnd);