#include <windows.h>

#pragma comment(lib, "user32.lib")

HWND g_hWnd;

// Define function to create and show a window
bool CreateShowWindow(HINSTANCE h_inst, long width, long height, LPCWSTR name_window, LPCWSTR name_window_class,
    int cmd_show, bool windowed) {
    // Constants
    const DWORD kDwStyle = windowed ? WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX : WS_POPUP;
    const int kCmdShow = windowed ? cmd_show : SW_SHOWMAXIMIZED;

    // Create window class structure
    WNDCLASSEXW wcex = {sizeof(WNDCLASSEXW), CS_CLASSDC, DefWindowProc, 0L, 0L, h_inst, nullptr, nullptr, nullptr, nullptr,
        name_window_class, nullptr};
    // Register it
    if (!RegisterClassExW(&wcex))
        return false;

    // Adjust window size with window border
    RECT rect;
    rect = {0, 0, width, height};
    AdjustWindowRectEx(&rect, kDwStyle, false, 0);

    // Create window and get the window handle
    g_hWnd = CreateWindowExW(0, name_window_class, name_window, kDwStyle, CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, h_inst, nullptr);
    if (!g_hWnd)
        return false;

    // Visualize window
    ShowWindow(g_hWnd, kCmdShow);
    UpdateWindow(g_hWnd);
    return true;
}

int WINAPI WinMain(HINSTANCE h_inst, HINSTANCE h_pinst, LPSTR p_cmd, int cmd_show) {
    if (!CreateShowWindow(h_inst, 1280, 720, L"Window name", L"Window class name", cmd_show, true))
        return 1;
    while(true) {
    }
    return 0;
}
