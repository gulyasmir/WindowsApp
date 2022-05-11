#pragma once
#define OnSerialRefresh 1
#define OnReadCountText 2
#define DisconnectThis 3
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
HWND ConnectedButton;

HWND hStaticControlReadText;
HWND hStaticControlReadCountText;
HWND hStaticControlReadNumber;
HWND hStaticControConnected;
HMENU ComPortSubMenu;
HMENU ComPortListMenu;

char filename[260];
OPENFILENAMEA ofn;

volatile bool isConnected = false;
volatile bool isThreading = true;

int selectedPort = 1;
int targetBoudRate = 9600;

HANDLE connectedPort;
HANDLE readThread;

WNDCLASS NewWindowClass(HBRUSH BGColor, HCURSOR Cursor, HINSTANCE hInstance, HICON Icon, LPCWSTR Name, WNDPROC WindowProc);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void MainWndAddMenus(HWND hwnd);
void MainWndAddWidgets(HWND hwnd);
void WindowHide(HWND hwnd);
void ConnectButtonView(HWND hwnd);

void SaveData(LPCSTR path);
void LoadData(LPCSTR path);
void SetOpenFileParams(HWND hwnd);

int SerialBegin(int BaudRate, int Comport);
void SetWindowStatus(std::string status);
void ConnectRequest(void);
void SerialRead(void);

