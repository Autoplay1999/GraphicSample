#include "header.h"
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

using namespace DirectX;
using namespace DirectX::PackedVector;

// Data
static LPDIRECT3D9              g_pD3D = nullptr;
static LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};
static WNDCLASSEXW              g_wc;
static HWND                     g_hwnd;
static LPDIRECT3DVERTEXBUFFER9  g_buffer = NULL;    // the pointer to the grid's vertex buffer


// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
HWND MakeWindow();
void CleanupWindow();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void render_grid();

bool graphic_init(HWND, IDirect3DDevice9*);
void graphic_frame();
void graphic_ui();
void graphic_state();
void graphic_cleanup();
void graphic_wndproc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Main code
int main(int, char**) {
    if (!MakeWindow())
        return 1;

    if (!CreateDeviceD3D(g_hwnd)) {
        CleanupDeviceD3D();
        ::UnregisterClassW(g_wc.lpszClassName, g_wc.hInstance);
        return 2;
    }

    ::ShowWindow(g_hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(g_hwnd);

    if (!ImGui_Init_DX9(g_hwnd, g_pd3dDevice))
        return 3;

    if (!graphic_init(g_hwnd, g_pd3dDevice))
        return 4;

    graphic_state();

    MSG msg;
    bool done = false;
    // ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    while (!done) {
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }

        if (done)
            break;

        if (g_ResizeWidth != 0 && g_ResizeHeight != 0) {
            g_d3dpp.BackBufferWidth = g_ResizeWidth;
            g_d3dpp.BackBufferHeight = g_ResizeHeight;
            g_ResizeWidth = g_ResizeHeight = 0;
            ResetDevice();
        }

        ImGui_BeginFrame();
        graphic_ui();
        ImGui_EndFrame();

        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        graphic_state();

        // D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
        g_pd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFF202020 /*clear_col_dx*/, 1.0f, 0);

        if (g_pd3dDevice->BeginScene() >= 0) {
            render_grid();
            graphic_frame();
            ImGui_Render();
            g_pd3dDevice->EndScene();
        }

        HRESULT result = g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);

        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();

        static std::chrono::system_clock::time_point nextFrame;
        static std::chrono::system_clock::time_point curFrame;
        static std::chrono::microseconds limitFrame;

        limitFrame = std::chrono::microseconds(time_t(std::micro::den / 60));
        curFrame = std::chrono::system_clock::now();
        nextFrame += limitFrame;

        if (curFrame - nextFrame >= limitFrame)
            nextFrame = curFrame + limitFrame;

        std::this_thread::sleep_until(nextFrame);
    }

    graphic_cleanup();
    ImGui_Cleanup();
    CleanupDeviceD3D();
    CleanupWindow();

    return 0;
}

// Helper functions

HWND MakeWindow() {
    RECT rct{0, 0, 1024, 768};
    WNDCLASSEXW wc = {sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"DX9 Example", nullptr};
    g_wc = wc;
    ::RegisterClassExW(&wc);
    AdjustWindowRect(&rct, WS_OVERLAPPEDWINDOW, false);
    return g_hwnd = ::CreateWindowW(wc.lpszClassName, L"DirectX9 Example", WS_OVERLAPPEDWINDOW, 100, 100, rct.right, rct.bottom, nullptr, nullptr, wc.hInstance, nullptr);
}

void CleanupWindow() {
    ::DestroyWindow(g_hwnd);
    ::UnregisterClassW(g_wc.lpszClassName, g_wc.hInstance);
}

bool CreateDeviceD3D(HWND hWnd) {
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D() {
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = nullptr; }
}

void ResetDevice() {
    ImGui_OnDeviceLost();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_OnDeviceReset();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    graphic_wndproc(hWnd, msg, wParam, lParam);
    
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

struct GRIDVERTEX { D3DVECTOR position; DWORD color; };
#define GRIDFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)

void render_grid() {
    static bool InitNeeded = true;
    static int GridSize = 100;
    static XMMATRIX matIdentity;

    g_pd3dDevice->SetFVF(GRIDFVF);

    if (InitNeeded) {
        g_pd3dDevice->CreateVertexBuffer(sizeof(GRIDVERTEX) * GridSize * 100,
            0,
            GRIDFVF,
            D3DPOOL_MANAGED,
            &g_buffer,
            0);

        GRIDVERTEX* pGridData = 0;
        g_buffer->Lock(0, 0, (void**)&pGridData, 0);

        int index = 0;
        for (; index <= GridSize * 4 + 1; index++) {
            float x = float((index % 2) ? GridSize : -GridSize);
            float y = 0.0f;
            float z = float(index / 2 - GridSize);

            pGridData[index].position = D3DVECTOR(x, y, z);
            pGridData[index].color = XMCOLOR(0.4f * 255.0f, 0.4f * 255.0f, 0.4f * 255.0f, 1.0f * 255.0f);
        }

        for (; index <= GridSize * 8 + 4; index++) {
            static int half = index;
            float x = float((index - half) / 2 - GridSize);
            float y = 0.0f;
            float z = float((index % 2) ? -GridSize : GridSize);

            pGridData[index].position = D3DVECTOR(x, y, z);
            pGridData[index].color = XMCOLOR(0.4f * 255.0f, 0.4f * 255.0f, 0.4f * 255.0f, 1.0f * 255.0f);
        }

        g_buffer->Unlock();

        matIdentity = XMMatrixIdentity();
        InitNeeded = false;
    }

    DWORD oldState[3];
    g_pd3dDevice->GetRenderState(D3DRS_LIGHTING, oldState + 0);
    g_pd3dDevice->GetRenderState(D3DRS_ZENABLE, oldState + 1);
    g_pd3dDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, oldState + 2);

    g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
    g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    g_pd3dDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&matIdentity);
    g_pd3dDevice->SetStreamSource(0, g_buffer, 0, sizeof(GRIDVERTEX));
    g_pd3dDevice->SetTexture(0, NULL);
    g_pd3dDevice->DrawPrimitive(D3DPT_LINELIST, 0, GridSize * 4 + 2);

    g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, oldState[0]);
    g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, oldState[1]);
    g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, oldState[2]);
}
