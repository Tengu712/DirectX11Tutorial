#include <windows.h>

#pragma comment(lib, "user32.lib")

HWND g_hWnd;

// Define function to create and show a window
bool CreateShowWindow(HINSTANCE h_inst, long width, long height, LPCSTR name_window, LPCSTR name_window_class,
    int cmd_show, bool windowed) {
    // Constants
    const DWORD kDwStyle = windowed ? WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX : WS_POPUP;
    const int kCmdShow = windowed ? cmd_show : SW_SHOWMAXIMIZED;

    // Adjust window size with window border
    RECT rect;
    rect = {0, 0, width, height};
    AdjustWindowRectEx(&rect, kDwStyle, false, 0);

    // Create window and get the window handle
    g_hWnd = CreateWindowExA(0, "COMBOBOX", name_window, kDwStyle, CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, h_inst, nullptr);
    if (!g_hWnd)
        return false;

    // Visualize window
    ShowWindow(g_hWnd, kCmdShow);
    UpdateWindow(g_hWnd);
    return true;
}

int WINAPI WinMain(HINSTANCE h_inst, HINSTANCE h_pinst, LPSTR p_cmd, int cmd_show) {
    bool flg = true;
    flg = flg && CreateShowWindow(h_inst, 1280, 720, "Window name", "Window class name", cmd_show, false);
    while(true) {
    }
    return 0;
}
