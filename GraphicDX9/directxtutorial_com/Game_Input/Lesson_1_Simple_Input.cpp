// include the basic windows header files and the Direct3D header file
#include "../../header.h"

using namespace DirectX;

// define the keyboard macro
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

// global declarations
HWND g_hWnd;
LPDIRECT3DDEVICE9 g_d3ddev;
LPD3DXMESH meshTeapot;
POINT MousePos;    // mouse position
bool g_IsActive;
XMFLOAT4 g_WndRect;

// function prototypes
void init_light(void);
void update_window_rect();

// this is the function used to render a single frame
void graphic_frame() {
    update_window_rect();
    init_light();

    // set the view transform
    auto cameraPos = XMVectorSet(0.0f, 2.0f, 6.0f, 1.0f);
    auto lookAtPos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
    auto upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
    auto matView = XMMatrixLookAtLH(
        cameraPos,    // the camera position
        lookAtPos,      // the look-at position
        upDirection // the up direction
    );    
    g_d3ddev->SetTransform(D3DTS_VIEW, (D3DXMATRIX*)&matView);

    // set the projection transform
    auto matProjection = XMMatrixPerspectiveFovLH(
        XM_PIDIV4,    // the horizontal field of view
        g_WndRect.z / g_WndRect.w, // aspect ratio
        1.0f,    // the near view-plane
        100.0f
    );    // the far view-plane

    g_d3ddev->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&matProjection);     // set the projection

    static float index = 0.0f; // index+=0.03f;

    if (g_IsActive) {
        // collect keyboard data
        if (KEY_DOWN(VK_LEFT)) index -= 0.03f;
        if (KEY_DOWN(VK_RIGHT)) index += 0.03f;

        // collect mouse data
        POINT zero_point{};
        ClientToScreen(g_hWnd, &zero_point);
        GetCursorPos(&MousePos);    // get the mouse data
        SetCursorPos(zero_point.x + (int)g_WndRect.z / 2, zero_point.y + (int)g_WndRect.w / 2);    // move the mouse to the center
        MousePos.x -= zero_point.x + g_WndRect.z / 2;
        MousePos.y -= zero_point.y + g_WndRect.w / 2;
        index += MousePos.x * 0.01f;
    }

    // set the world transform
    auto matRotateY = XMMatrixRotationY(index);
    g_d3ddev->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&matRotateY);

    // draw the teapot
    meshTeapot->DrawSubset(0);
}

void graphic_ui() {

}

void graphic_state() {
    g_d3ddev->SetRenderState(D3DRS_LIGHTING, TRUE);
    g_d3ddev->SetRenderState(D3DRS_ZENABLE, TRUE);
    g_d3ddev->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(50, 50, 50));

    g_d3ddev->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 8);    // anisotropic level
    g_d3ddev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);    // minification
    g_d3ddev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);    // magnification
    g_d3ddev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);    // mipmap
}

// this is the function that cleans up Direct3D and COM
void graphic_cleanup() {
    meshTeapot->Release();
    g_d3ddev->Release();
}

// this is the function that puts the 3D models into video RAM
bool graphic_init(HWND hWnd, LPDIRECT3DDEVICE9 device) {
    g_hWnd = hWnd;
    g_d3ddev = device;

    D3DXCreateTeapot(g_d3ddev, &meshTeapot, NULL);    // create the teapot
    return true;
}

bool graphic_wndproc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_KEYUP:
        if (wParam == VK_ESCAPE)
            PostQuitMessage(0);
        break;

    case WM_SETFOCUS:
        ShowCursor(FALSE);
        g_IsActive = true;
        break;

    case WM_KILLFOCUS:
        ShowCursor(TRUE);
        g_IsActive = false;
        break;

    case WM_SETCURSOR:
        if (LOWORD(lParam) == HTCLIENT) {
            // If cursor is inside our client area, prevent changing the cursor
            return FALSE;
        }
        break;
    }

    return TRUE;
}

// this is the function that sets up the lights and materials
void init_light(void) {
    D3DLIGHT9 light;    // create the light struct
    D3DMATERIAL9 material;    // create the material struct

    ZeroMemory(&light, sizeof(light));    // clear out the light struct for use
    light.Type = D3DLIGHT_DIRECTIONAL;    // make the light type 'directional light'
    light.Diffuse = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);    // set the light's color
    light.Direction = D3DXVECTOR3(-1.0f, -0.3f, -1.0f);

    g_d3ddev->SetLight(0, &light);    // send the light struct properties to light #0
    g_d3ddev->LightEnable(0, TRUE);    // turn on light #0

    ZeroMemory(&material, sizeof(D3DMATERIAL9));    // clear out the struct for use
    material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);    // set diffuse color to white
    material.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);    // set ambient color to white

    g_d3ddev->SetMaterial(&material);    // set the globably-used material to &material
}

void update_window_rect() {
    RECT wndRect;
    GetClientRect(g_hWnd, &wndRect);
    g_WndRect.x = wndRect.left;
    g_WndRect.y = wndRect.top;
    g_WndRect.z = wndRect.right;
    g_WndRect.w = wndRect.bottom;
}