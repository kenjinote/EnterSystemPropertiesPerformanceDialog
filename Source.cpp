#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>
#include <vector>
#include <string>

struct Window
{
    HWND hWnd;
    WCHAR szTitle[256];
    WCHAR szClass[256];
};

struct ProcessWindowsInfo
{
    DWORD ProcessID;
    std::vector<Window> Windows;
};

BOOL __stdcall EnumProcessWindowsProc(HWND hWnd, LPARAM lParam)
{
    ProcessWindowsInfo* Info = reinterpret_cast<ProcessWindowsInfo*>(lParam);
    DWORD WindowProcessID;
    GetWindowThreadProcessId(hWnd, &WindowProcessID);
    if (WindowProcessID == Info->ProcessID)
    {
        Window w;
        w.hWnd = hWnd;
		GetWindowText(hWnd, w.szTitle, 256);
        GetClassName(hWnd, w.szClass, 256);
        Info->Windows.push_back(w);
    }
    return true;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPWSTR pCmdLine, int nCmdShow)
{
    SHELLEXECUTEINFO sei = { 0 };
    sei.cbSize = sizeof(SHELLEXECUTEINFO);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.hwnd = GetDesktopWindow();
    sei.lpVerb = L"open";
    sei.lpFile = L"systempropertiesperformance.exe";
    sei.lpParameters = 0;
    sei.lpDirectory = NULL;
    sei.nShow = SW_SHOW;
    sei.hInstApp = NULL;
    if (ShellExecuteEx(&sei))
    {
        WaitForInputIdle(sei.hProcess, INFINITE);
        ProcessWindowsInfo info = {0};
        info.ProcessID = GetProcessId(sei.hProcess);
        Sleep(1000);
        EnumWindows((WNDENUMPROC)EnumProcessWindowsProc, reinterpret_cast<LPARAM>(&info));
        if (info.Windows.size() > 0) {
            for (auto w : info.Windows) {
                if (lstrcmpi(w.szTitle, L"パフォーマンス オプション") == 0) {

                    SendMessage(w.hWnd, WM_COMMAND, IDOK, 0);

                    keybd_event(VK_RETURN, 0, 0, 0);
                    keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0);

                    SendMessage(w.hWnd, WM_CLOSE, 0, 0);

                    HWND hOKButton = FindWindowEx(w.hWnd, NULL, L"Button", L"OK");

                    if (hOKButton != NULL) {
						SendMessage(hOKButton, BM_CLICK, 0, 0);

                        RECT rect;
                        GetWindowRect(hOKButton, &rect);
                        int x = rect.left + (rect.right - rect.left) / 2 - 100;
                        int y = rect.top + (rect.bottom - rect.top) / 2;
                        mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, x, y, 0, 0);
                        mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN, x, y, 0, 0);
                        mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP, x, y, 0, 0);
                    }

                    DestroyWindow(w.hWnd);
                }
            }
		}
		CloseHandle(sei.hProcess);
    }
	return 0;
}
