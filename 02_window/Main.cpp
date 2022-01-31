#include <windows.h>

// Global variables
HWND g_hWnd = nullptr;

// Define function to create and show a window
bool CreateShowWindow(HINSTANCE h_inst, long width, long height, LPCWSTR name_window, LPCWSTR name_window_class,
    int cmdshow, bool windowed) {
    const DWORD kDwStyle = windowed ? WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX : WS_POPUP;
    const int kCmdShow = windowed ? cmdshow : SW_SHOWMAXIMIZED;

    WNDCLASSEXW wcex = {sizeof(WNDCLASSEXW), CS_CLASSDC, DefWindowProcW, 0L, 0L, h_inst, nullptr, nullptr, nullptr, nullptr,
        name_window_class, nullptr};
    if (!RegisterClassExW(&wcex)) {
        MessageBoxA(nullptr, "Failed to register window class.", "Error", MB_OK);
        return false;
    }

    RECT rect;
    rect = {0, 0, width, height};
    if (!AdjustWindowRectEx(&rect, kDwStyle, false, 0)) {
        MessageBoxA(nullptr, "Failed to adjust window size.", "Error", MB_OK);
        return false;
    }

    g_hWnd = CreateWindowExW(0, name_window_class, name_window, kDwStyle, CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, h_inst, nullptr);
    if (g_hWnd == nullptr) {
        MessageBoxA(nullptr, "Failed to create window.", "Error", MB_OK);
        return false;
    }

    ShowWindow(g_hWnd, kCmdShow);
    return true;
}

int WINAPI WinMain(HINSTANCE h_inst, HINSTANCE h_pinst, LPSTR cmdline, int cmdshow) {
    if (!CreateShowWindow(h_inst, 1280, 720, L"Window name", L"Window class name", cmdshow, true))
        return 1;
    while(true) {
    }
    return 0;
}
