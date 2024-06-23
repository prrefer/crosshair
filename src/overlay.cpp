#include "overlay.hpp"

LRESULT WINAPI windowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;
    if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return EXIT_SUCCESS;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

void overlay::createRenderTarget()
{
    ID3D11Texture2D* pBackBuffer{};
    swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    device->CreateRenderTargetView(pBackBuffer, nullptr, &renderTargetView);
    pBackBuffer->Release();
}

bool overlay::createDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &swapChain, &device, &featureLevel, &deviceContext);

    overlay::createRenderTarget();
    return true;
}

void overlay::cleanupRenderTarget()
{
    if (renderTargetView) { renderTargetView->Release(); renderTargetView = nullptr; }
}

void overlay::cleanupDeviceD3D()
{
    overlay::cleanupRenderTarget();
    if (swapChain) { swapChain->Release(); swapChain = nullptr; }
    if (deviceContext) { deviceContext->Release(); deviceContext = nullptr; }
    if (device) { device->Release(); device = nullptr; }
}

void overlay::begin() {
    if (PrepareForUIAccess() != ERROR_SUCCESS) throw std::invalid_argument("could not prepare for window bands, try running as admin");

    width = GetSystemMetrics(SM_CXSCREEN);
    height = GetSystemMetrics(SM_CYSCREEN);
    centerX = width / 2;
    centerY = height / 2;

    typedef HWND(WINAPI* CreateWindowInBand)(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, _In_opt_ HWND hWndParent, _In_opt_ HMENU hMenu, _In_opt_ HINSTANCE hInstance, _In_opt_ LPVOID lpParam, DWORD band);
    CreateWindowInBand createWindowInBand = reinterpret_cast<CreateWindowInBand>(GetProcAddress(LoadLibraryA("user32.dll"), "CreateWindowInBand"));

    windowClass.cbSize = sizeof(WNDCLASSEXW);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = windowProc;
    windowClass.hInstance = GetModuleHandle(nullptr);
    windowClass.lpszClassName = L" ";
    RegisterClassExW(&windowClass);

    hwnd = createWindowInBand(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_NOACTIVATE, windowClass.lpszClassName, L"", WS_POPUP, 0, 0, width, height, nullptr, nullptr, windowClass.hInstance, nullptr, 2);
    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);
    SetWindowDisplayAffinity(hwnd, WDA_EXCLUDEFROMCAPTURE);
    const MARGINS margins{ 0, 0, width, height };
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    if (!overlay::createDeviceD3D(hwnd)) {
        overlay::cleanupDeviceD3D();
        UnregisterClassW(windowClass.lpszClassName, windowClass.hInstance);
        throw std::invalid_argument("could not create d3d device");
    }

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(device, deviceContext);
    rendering = true;
}

void overlay::beginRender() {
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (msg.message == WM_QUIT) rendering = false;
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void overlay::endRender() {
    ImGui::Render();
    constexpr float clear_color_with_alpha[4] = { 0.f, 0.f, 0.f, 0.f };
    deviceContext->OMSetRenderTargets(1, &renderTargetView, nullptr);
    deviceContext->ClearRenderTargetView(renderTargetView, clear_color_with_alpha);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    swapChain->Present(0, 0);
}

void overlay::end() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    cleanupDeviceD3D();
    DestroyWindow(hwnd);
    UnregisterClassW(windowClass.lpszClassName, windowClass.hInstance);
}

void overlay::streamproof(bool value) {
    SetWindowDisplayAffinity(hwnd, value ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);
}

void overlay::clickable(bool value) {
    SetWindowLong(hwnd, GWL_EXSTYLE, value ? WS_EX_TOPMOST | WS_EX_TRANSPARENT : WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED);
}