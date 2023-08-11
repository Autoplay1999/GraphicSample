// include the basic windows header files and the Direct3D header file
#include "../../header.h"

// namespace dx = DirectX;
using namespace DirectX;

// global declarations
HWND g_hWnd;
LPDIRECT3DDEVICE9 g_d3ddev;    // the pointer to the device class
LPDIRECT3DVERTEXBUFFER9 v_buffer = NULL;    // the pointer to the vertex buffer

struct CUSTOMVERTEX { FLOAT X, Y, Z; DWORD COLOR; };
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)

// this is the function used to render a single frame
void graphic_frame() {
    // select which vertex format we are using
    g_d3ddev->SetFVF(CUSTOMFVF);

    // SET UP THE PIPELINE
    // XMMATRIX matRotateY;    // a matrix to store the rotation information

    static float index = 0.0f; index += 0.05f;    // an ever-increasing float value

    // build a matrix to rotate the model based on the increasing float value
    auto matRotateY = XMMatrixRotationY(index);

    // tell Direct3D about our matrix
    g_d3ddev->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&matRotateY);

    XMVECTOR cameraPos, lookAtPos, upDirection;

    cameraPos.m128_f32[0] = 0.0f;
    cameraPos.m128_f32[1] = 0.0f;
    cameraPos.m128_f32[2] = 10.0f;
    cameraPos.m128_f32[3] = 1.0f;

    lookAtPos.m128_f32[0] = 0.0f;
    lookAtPos.m128_f32[1] = 0.0f;
    lookAtPos.m128_f32[2] = 0.0f;
    lookAtPos.m128_f32[3] = 1.0f;

    upDirection.m128_f32[0] = 0.0f;
    upDirection.m128_f32[1] = 1.0f;
    upDirection.m128_f32[2] = 0.0f;
    upDirection.m128_f32[3] = 1.0f;

    auto matView = XMMatrixLookAtLH(
        cameraPos,    // the camera position
        lookAtPos,    // the look-at position
        upDirection   // the up direction
    );    

    g_d3ddev->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&matView);    // set the view transform to matView

    auto matProjection = XMMatrixPerspectiveFovLH(
        XM_PIDIV4,      // the horizontal field of view
        1024.0f / 768.0f,   // aspect ratio
        1.0f,               // the near view-plane
        100.0f              // the far view-plane
    );   

    g_d3ddev->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&matProjection);    // set the projection

    // select the vertex buffer to display
    g_d3ddev->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));

    // copy the vertex buffer to the back buffer
    g_d3ddev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
}

void graphic_ui() {

}

void graphic_state() {
    g_d3ddev->SetRenderState(D3DRS_LIGHTING, FALSE);    // turn off the 3D lighting
}

// this is the function that puts the 3D models into video RAM
bool graphic_init(HWND hWnd, IDirect3DDevice9* device) {
    g_hWnd = hWnd;
    g_d3ddev = device;

    // create the vertices using the CUSTOMVERTEX struct
    CUSTOMVERTEX vertices[] =
    {
        { 3.0f, -3.0f, 0.0f, D3DCOLOR_XRGB(0, 0, 255), },
        { 0.0f, 3.0f, 0.0f, D3DCOLOR_XRGB(0, 255, 0), },
        { -3.0f, -3.0f, 0.0f, D3DCOLOR_XRGB(255, 0, 0), },
    };

    // create a vertex buffer interface called v_buffer
    g_d3ddev->CreateVertexBuffer(3 * sizeof(CUSTOMVERTEX),
        0,
        CUSTOMFVF,
        D3DPOOL_MANAGED,
        &v_buffer,
        NULL);

    VOID* pVoid;    // a void pointer

    // lock v_buffer and load the vertices into it
    v_buffer->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, vertices, sizeof(vertices));
    v_buffer->Unlock();

    return true;
}

void graphic_wndproc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

}

// this is the function that cleans up Direct3D and COM
void graphic_cleanup() {
    v_buffer->Release();    // close and release the vertex buffer
}