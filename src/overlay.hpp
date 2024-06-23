#include <Windows.h>
#include <dwmapi.h>
#include <d3d11.h>
#include <stdexcept>
#include "uiaccess.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class overlay {
	bool createDeviceD3D(HWND hWnd);
	void createRenderTarget();
	void cleanupDeviceD3D();
	void cleanupRenderTarget();
	WNDCLASSEXW windowClass{};
	ID3D11Device* device{ nullptr };
	ID3D11DeviceContext* deviceContext{ nullptr };
	IDXGISwapChain* swapChain{ nullptr };
	ID3D11RenderTargetView* renderTargetView{ nullptr };
public:
	void begin();
	void beginRender();
	void endRender();
	void end();

	void streamproof(bool value);
	void clickable(bool value);

	bool rendering{};
	HWND hwnd{};

	int width{};
	int height{};
	float centerX{};
	float centerY{};
};