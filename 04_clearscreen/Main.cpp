#include <d3d11.h>
#include <windows.h>

// d3d11.dll
HMODULE g_hD3D11 = nullptr;
typedef HRESULT (*ImprtdD3D11CreateDevice)(IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT, const D3D_FEATURE_LEVEL*,
    UINT, UINT, ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext**);

// dxgi.dll
HMODULE g_hDXGI = nullptr;
typedef HRESULT (*ImprtdCreateDXGIFactory)(REFIID, void**);

HWND g_hWnd = nullptr;
ID3D11Device* g_pDevice = nullptr;
ID3D11DeviceContext* g_pContext = nullptr;
IDXGISwapChain* g_pSwapchain = nullptr;
ID3D11RenderTargetView* g_pBackbuffer = nullptr;

LRESULT WINAPI WndProc(HWND h_wnd, unsigned int msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(h_wnd, msg, wparam, lparam);
}

// Define function to load dll
bool LoadDLL(HMODULE* ph_module, const char* name) {
    if (ph_module == nullptr) {
        MessageBoxA(nullptr, "Nullptr gained when loading dll.", "Error", MB_OK);
        return false;
    }
    *ph_module = LoadLibraryA(name);
    if (g_hD3D11 == nullptr) {
        MessageBoxA(nullptr, "Failed to load dll.", "Error", MB_OK);
        return false;
    }
    return true;
}

bool CreateShowWindow(HINSTANCE h_inst, long width, long height, LPCWSTR name_window, LPCWSTR name_window_class,
    int cmdshow, bool windowed) {
    const DWORD kDwStyle = windowed ? WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX : WS_POPUP;
    const int kCmdShow = windowed ? cmdshow : SW_SHOWMAXIMIZED;

    WNDCLASSEXW wcex = {sizeof(WNDCLASSEXW), CS_CLASSDC, WndProc, 0L, 0L, h_inst, nullptr, nullptr, nullptr, nullptr,
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

// Define function to create Direct3D device and context
bool CreateDrawingDevices(unsigned int width, unsigned int height) {
    // Create device and context
    ImprtdD3D11CreateDevice func_create_device = (ImprtdD3D11CreateDevice)GetProcAddress(g_hD3D11, "D3D11CreateDevice");
    if (func_create_device == nullptr) {
        MessageBoxA(nullptr, "Failed to get function 'D3D11CreateDevice' from d3d11.dll.", "Error", MB_OK);
        return false;
    }
    D3D_FEATURE_LEVEL feature_levels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };
    if (FAILED(func_create_device(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
            feature_levels, ARRAYSIZE(feature_levels), D3D11_SDK_VERSION, &g_pDevice, nullptr, &g_pContext))) {
        MessageBoxA(nullptr, "Failed to create D3D11Device.", "Error", MB_OK);
        return false;
    }

    // Create factory to create swapchain
    ImprtdCreateDXGIFactory func_create_dxgifactory =
        (ImprtdCreateDXGIFactory)GetProcAddress(g_hDXGI, "CreateDXGIFactory");
    if (func_create_device == nullptr) {
        MessageBoxA(nullptr, "Failed to get function 'CreateDXGIFactory' from dxgi.dll.", "Error", MB_OK);
        return false;
    }
    const IID iid_idxgifactory = {0x7b7166ec, 0x21c7, 0x44ae, {0xb2, 0x1a, 0xc9, 0xae, 0x32, 0x1a, 0xe3, 0x69}};
    IDXGIFactory* p_factory = nullptr;
    if (FAILED(func_create_dxgifactory(iid_idxgifactory, (void**)&p_factory))) {
        MessageBoxA(nullptr, "Failed to create DXGIFactory.", "Error", MB_OK);
        return false;
    }

    // Create swapchain
    DXGI_SWAP_CHAIN_DESC desc_swapchain = {{width, height, {60U, 1U}, DXGI_FORMAT_R8G8B8A8_UNORM,
                                               DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED, DXGI_MODE_SCALING_UNSPECIFIED},
        {1, 0}, DXGI_USAGE_RENDER_TARGET_OUTPUT, 1U, g_hWnd, true, DXGI_SWAP_EFFECT_DISCARD,
        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH};
    if (FAILED(p_factory->CreateSwapChain(g_pDevice, &desc_swapchain, &g_pSwapchain))) {
        MessageBoxA(nullptr, "Failed to create swapchain.", "Error", MB_OK);
        return false;
    }

    // Create backbuffer(render target view)
    const IID iid_id3d11texture2d = {0x6f15aaf2, 0xd208, 0x4e89, {0x9a, 0xb4, 0x48, 0x95, 0x35, 0xd3, 0x4f, 0x9c}};
    ID3D11Texture2D* p_tex_backbuffer = nullptr;
    if (FAILED(g_pSwapchain->GetBuffer(0, iid_id3d11texture2d, (void**)&p_tex_backbuffer))) {
        MessageBoxA(nullptr, "Failed to get backbuffer.", "Error", MB_OK);
        return false;
    }
    if (FAILED(g_pDevice->CreateRenderTargetView(p_tex_backbuffer, nullptr, &g_pBackbuffer))) {
        MessageBoxA(nullptr, "Failed to create render target view of backbuffer.", "Error", MB_OK);
        return false;
    }

    p_factory->Release();
    p_tex_backbuffer->Release();
    return true;
}

// Define functions to clear screen

void ClearBackBuffer(float r, float g, float b, float a) {
    float clearColor[4] = {r, g, b, a};
    g_pContext->ClearRenderTargetView(g_pBackbuffer, clearColor);
}

void SetBackBuffer() {
    g_pContext->OMSetRenderTargets(1U, &g_pBackbuffer, nullptr);
}

void Swap() {
    g_pSwapchain->Present(1U, 0U);
}

int WINAPI WinMain(HINSTANCE h_inst, HINSTANCE h_pinst, LPSTR cmdline, int cmdshow) {
    if (!LoadDLL(&g_hD3D11, "d3d11.dll") || !LoadDLL(&g_hDXGI, "dxgi.dll"))
        return 1;
    if (!CreateShowWindow(h_inst, 1280, 720, L"Window name", L"WINDOWCLASNAME", cmdshow, true))
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
        ClearBackBuffer(0.0f, 0.0f, 0.0f, 1.0f);
        SetBackBuffer();
        Swap();
    }
    return 0;
}
