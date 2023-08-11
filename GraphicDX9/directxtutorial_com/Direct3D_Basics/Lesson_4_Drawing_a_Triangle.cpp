// include the basic windows header files and the Direct3D header file
#include "../../header.h"

// global declarations
HWND g_hWnd;
LPDIRECT3DDEVICE9 g_d3ddev;    // the pointer to the device class
LPDIRECT3DVERTEXBUFFER9 v_buffer = NULL;    // the pointer to the vertex buffer

struct CUSTOMVERTEX { FLOAT X, Y, Z, RHW; DWORD COLOR; };
#define CUSTOMFVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

// this is the function used to render a single frame
void graphic_frame() {
    RECT wndRect;
    GetClientRect(g_hWnd, &wndRect);
    float width = (float)wndRect.right;
    float height = (float)wndRect.bottom;

    // create the vertices using the CUSTOMVERTEX struct
    CUSTOMVERTEX vertices[] =
    {
        { width * .5f, height * .1f, 0.5f, 1.0f, D3DCOLOR_XRGB(0, 0, 255), },
        { width * .9f, height * .9f, 0.5f, 1.0f, D3DCOLOR_XRGB(0, 255, 0), },
        { width * .1f, height * .9f, 0.5f, 1.0f, D3DCOLOR_XRGB(255, 0, 0), },
    };

    VOID* pVoid;    // a void pointer

    // lock v_buffer and load the vertices into it
    v_buffer->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, vertices, sizeof(vertices));
    v_buffer->Unlock();

    // select which vertex format we are using
    g_d3ddev->SetFVF(CUSTOMFVF);

    // select the vertex buffer to display
    g_d3ddev->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));

    // copy the vertex buffer to the back buffer
    g_d3ddev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
}

void graphic_ui() {

}

void graphic_state() {

}

// this is the function that puts the 3D models into video RAM
bool graphic_init(HWND hWnd, IDirect3DDevice9* device) {
    g_hWnd = hWnd;
    g_d3ddev = device;

    // create a vertex buffer interface called v_buffer
    g_d3ddev->CreateVertexBuffer(3 * sizeof(CUSTOMVERTEX),
        0,
        CUSTOMFVF,
        D3DPOOL_MANAGED,
        &v_buffer,
        NULL);
    return true;
}

void graphic_wndproc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

}

// this is the function that cleans up Direct3D and COM
void graphic_cleanup() {
    v_buffer->Release();    // close and release the vertex buffer
}