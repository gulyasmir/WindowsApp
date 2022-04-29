#pragma once
#define OnSerialRefresh 1

#define OnExitSoftware 4
#define OnClearField 5
#define OnMenuHelp 6
#define OnReadField 7
#define OnSaveFile 8
#define OnLoadFile 9
#define OnReadNumberField 11
#define OnConnectRequest 12

#define DlgIndexNumber 256
#define	TextBufferSize 256
#define ComPortAmount 50
#define ComSelectIndex 120

char Buffer[TextBufferSize];
int CharsRead;
unsigned num;

HWND hEditControl;
HWND hStaticControl;
HWND hNumberControl;

HMENU ComPortSubMenu;
HMENU ComPortListMenu;

char filename[260];
OPENFILENAMEA ofn;

bool isConnected = false;
int selectedPort = 1;
int targetBoudRate;

HANDLE connectedPort;

WNDCLASS NewWindowClass(HBRUSH BGColor, HCURSOR Cursor, HINSTANCE hInstance, HICON Icon, LPCWSTR Name, WNDPROC WindowProc);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void MainWndAddMenus(HWND hwnd);
void MainWndAddWidgets(HWND hwnd);
void SaveData(LPCSTR path);
void LoadData(LPCSTR path);
void SetOpenFileParams(HWND hwnd);

int SerialBegin(int BaudRate, int Comport);
void SetWindowStatus(std::string status);
void ConnectRequest(void);
void SerialRead(void);