#include <d3d11.h>
#include <windows.h>
#include <wrl/client.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;

HWND g_hWnd = nullptr;
ComPtr<ID3D11Device> g_pDevice = nullptr;
ComPtr<ID3D11DeviceContext> g_pContext = nullptr;
ComPtr<IDXGISwapChain> g_pSwapchain = nullptr;
ComPtr<ID3D11RenderTargetView> g_pBackbuffer = nullptr;

LRESULT WINAPI WndProc(HWND h_wnd, unsigned int msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(h_wnd, msg, wParam, lParam);
}

bool CreateShowWindow(HINSTANCE h_inst, long width, long height, LPCWSTR name_window, LPCWSTR name_window_class,
    int cmd_show, bool windowed) {
    const DWORD kDwStyle = windowed ? WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX : WS_POPUP;
    const int kCmdShow = windowed ? cmd_show : SW_SHOWMAXIMIZED;

    WNDCLASSEXW wcex = {sizeof(WNDCLASSEXW), CS_CLASSDC, WndProc, 0L, 0L, h_inst, nullptr, nullptr, nullptr, nullptr,
        name_window_class, nullptr};
    if (!RegisterClassExW(&wcex))
        return false;

    RECT rect;
    rect = {0, 0, width, height};
    AdjustWindowRectEx(&rect, kDwStyle, false, 0);

    g_hWnd = CreateWindowExW(0, name_window_class, name_window, kDwStyle, CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, h_inst, nullptr);
    if (!g_hWnd)
        return false;

    ShowWindow(g_hWnd, kCmdShow);
    UpdateWindow(g_hWnd);
    return true;
}

// Define function to create Direct3D device and context
bool CreateDrawingDevices(unsigned int width, unsigned int height) {
    // Create device and context
    D3D_FEATURE_LEVEL feature_levels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };
    if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
            feature_levels, ARRAYSIZE(feature_levels), D3D11_SDK_VERSION, g_pDevice.GetAddressOf(), nullptr,
            g_pContext.GetAddressOf())))
        return false;

    // Create factory to create swapchain
    ComPtr<IDXGIFactory> p_factory = nullptr;
    if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)p_factory.GetAddressOf())))
        return false;

    // Create swapchain
    DXGI_SWAP_CHAIN_DESC desc_swapchain =
        {{width, height, {60U, 1U}, DXGI_FORMAT_R8G8B8A8_UNORM,
          DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED, DXGI_MODE_SCALING_UNSPECIFIED},
        {1, 0}, DXGI_USAGE_RENDER_TARGET_OUTPUT, 1U, g_hWnd, true, DXGI_SWAP_EFFECT_DISCARD,
        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH};
    if (FAILED(p_factory->CreateSwapChain(g_pDevice.Get(), &desc_swapchain, g_pSwapchain.GetAddressOf())))
        return false;

    // Create backbuffer(render target view)
    ComPtr<ID3D11Texture2D> p_tex_backbuffer = nullptr;
    if (FAILED(g_pSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)p_tex_backbuffer.GetAddressOf())))
        return false;
    if (FAILED(g_pDevice->CreateRenderTargetView(p_tex_backbuffer.Get(), nullptr, g_pBackbuffer.GetAddressOf())))
        return false;

    return true;
}

void ClearBackBuffer() {
    float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    g_pContext->ClearRenderTargetView(g_pBackbuffer.Get(), clearColor);
}

void SetBackBuffer() {
    g_pContext->OMSetRenderTargets(1U, g_pBackbuffer.GetAddressOf(), nullptr);
}

void Swap() {
    g_pSwapchain->Present(1U, 0U);
}

int WINAPI WinMain(HINSTANCE h_inst, HINSTANCE h_pinst, LPSTR p_cmd, int cmd_show) {
    if (!CreateShowWindow(h_inst, 1280, 720, L"Window name", L"WINDOWCLASNAME", cmd_show, true))
        return 1;
    if (!CreateDrawingDevices(1280, 720))
        return 1;
    MSG msg;
    while (true) {
        if (PeekMessageW(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                break;
            DispatchMessageW(&msg);
            continue;
        }
        ClearBackBuffer();
        SetBackBuffer();
        Swap();
    }
    return 0;
}
