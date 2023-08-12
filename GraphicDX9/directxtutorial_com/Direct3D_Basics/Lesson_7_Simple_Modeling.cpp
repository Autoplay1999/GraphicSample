// include the basic windows header files and the Direct3D header file
#include "../../header.h"

using namespace DirectX;

// global declarations
HWND g_hWnd;
LPDIRECT3DDEVICE9 g_d3ddev;    // the pointer to the device class
LPDIRECT3DVERTEXBUFFER9 v_buffer = NULL;    // the pointer to the vertex buffer
LPDIRECT3DINDEXBUFFER9 i_buffer = NULL;    // the pointer to the index buffer

struct CUSTOMVERTEX { FLOAT X, Y, Z; DWORD COLOR; };
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)

#define MODEL 2

// this is the function used to render a single frame
void graphic_frame() {
    // select which vertex format we are using
    g_d3ddev->SetFVF(CUSTOMFVF);

    // set the view transform
    XMVECTOR cameraPos, lookAtPos, upDirection;

    cameraPos.m128_f32[0] = 0.0f;
    cameraPos.m128_f32[1] = 8.0f;
    cameraPos.m128_f32[2] = 25.0f;
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

    // set the projection transform
    auto matProjection = XMMatrixPerspectiveFovLH(
        XM_PIDIV4,    // the horizontal field of view
        1024.0f / 768.0f, // aspect ratio
        1.0f,    // the near view-plane
        100.0f
    );    // the far view-plane

    g_d3ddev->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&matProjection);     // set the projection

    // set the world transform
    static float index = 0.0f; index += 0.03f; // an ever-increasing float value
    auto matRotateY = XMMatrixRotationY(index);    // the rotation matrix
    g_d3ddev->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&matRotateY);    // set the world transform

    // select the vertex buffer to display
    g_d3ddev->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
    g_d3ddev->SetIndices(i_buffer);

    // draw the Hypercraft
#if MODEL == 0
    g_d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 10, 0, 6);
#elif MODEL == 1
    g_d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 5, 0, 6);
#elif MODEL == 2
    g_d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
#endif
}

void graphic_ui() {

}

void graphic_state() {
    g_d3ddev->SetRenderState(D3DRS_LIGHTING, FALSE);  // turn off the 3D lighting
    g_d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);    // both sides of the triangles
    g_d3ddev->SetRenderState(D3DRS_ZENABLE, TRUE);    // turn on the z-buffer
}

// this is the function that puts the 3D models into video RAM
bool graphic_init(HWND hWnd, IDirect3DDevice9* device) {
    g_hWnd = hWnd;
    g_d3ddev = device;

#if MODEL == 0
    // create the vertices using the CUSTOMVERTEX
    struct CUSTOMVERTEX vertices[] =
    {
        // fuselage
        { 3.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(0, 255, 0), },
        { 0.0f, 3.0f, -3.0f, D3DCOLOR_XRGB(0, 0, 255), },
        { 0.0f, 0.0f, 10.0f, D3DCOLOR_XRGB(255, 0, 0), },
        { -3.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(0, 255, 255), },

        // left gun
        { 3.2f, -1.0f, -3.0f, D3DCOLOR_XRGB(0, 0, 255), },
        { 3.2f, -1.0f, 11.0f, D3DCOLOR_XRGB(0, 255, 0), },
        { 2.0f, 1.0f, 2.0f, D3DCOLOR_XRGB(255, 0, 0), },

        // right gun
        { -3.2f, -1.0f, -3.0f, D3DCOLOR_XRGB(0, 0, 255), },
        { -3.2f, -1.0f, 11.0f, D3DCOLOR_XRGB(0, 255, 0), },
        { -2.0f, 1.0f, 2.0f, D3DCOLOR_XRGB(255, 0, 0), },
    };

    // create a vertex buffer interface called v_buffer
    g_d3ddev->CreateVertexBuffer(10 * sizeof(CUSTOMVERTEX),
        0,
        CUSTOMFVF,
        D3DPOOL_MANAGED,
        &v_buffer,
        NULL);

    // create the indices using an int array
    short indices[] =
    {
        0, 1, 2,    // fuselage
        2, 1, 3,
        3, 1, 0,
        0, 2, 3,
        4, 5, 6,    // wings
        7, 8, 9,
    };

    // create a index buffer interface called i_buffer
    g_d3ddev->CreateIndexBuffer(18 * sizeof(short),
        0,
        D3DFMT_INDEX16,
        D3DPOOL_MANAGED,
        &i_buffer,
        NULL);
#elif MODEL == 1
    // create the vertices using the CUSTOMVERTEX
    struct CUSTOMVERTEX vertices[] =
    {
        // base
        { -3.0f, 0.0f, 3.0f, D3DCOLOR_XRGB(0, 255, 0), },
        { 3.0f, 0.0f, 3.0f, D3DCOLOR_XRGB(0, 0, 255), },
        { -3.0f, 0.0f, -3.0f, D3DCOLOR_XRGB(255, 0, 0), },
        { 3.0f, 0.0f, -3.0f, D3DCOLOR_XRGB(0, 255, 255), },

        // top
        { 0.0f, 7.0f, 0.0f, D3DCOLOR_XRGB(0, 255, 0), },
    };

    // create a vertex buffer interface called v_buffer
    g_d3ddev->CreateVertexBuffer(5 * sizeof(CUSTOMVERTEX),
        0,
        CUSTOMFVF,
        D3DPOOL_MANAGED,
        &v_buffer,
        NULL);

    // create the indices using an int array
    short indices[] =
    {
        0, 2, 1,    // base
        1, 2, 3,
        0, 1, 4,    // sides
        1, 3, 4,
        3, 2, 4,
        2, 0, 4,
    };

    // create a index buffer interface called i_buffer
    g_d3ddev->CreateIndexBuffer(18 * sizeof(short),
        0,
        D3DFMT_INDEX16,
        D3DPOOL_MANAGED,
        &i_buffer,
        NULL);
#elif MODEL == 2
    // create the vertices using the CUSTOMVERTEX struct
    CUSTOMVERTEX vertices[] =
    {
        { -3.0f, 3.0f, -3.0f, D3DCOLOR_XRGB(0, 0, 255), },
        { 3.0f, 3.0f, -3.0f, D3DCOLOR_XRGB(0, 255, 0), },
        { -3.0f, -3.0f, -3.0f, D3DCOLOR_XRGB(255, 0, 0), },
        { 3.0f, -3.0f, -3.0f, D3DCOLOR_XRGB(0, 255, 255), },
        { -3.0f, 3.0f, 3.0f, D3DCOLOR_XRGB(0, 0, 255), },
        { 3.0f, 3.0f, 3.0f, D3DCOLOR_XRGB(255, 0, 0), },
        { -3.0f, -3.0f, 3.0f, D3DCOLOR_XRGB(0, 255, 0), },
        { 3.0f, -3.0f, 3.0f, D3DCOLOR_XRGB(0, 255, 255), },
    };

    // create a vertex buffer interface called v_buffer
    g_d3ddev->CreateVertexBuffer(8 * sizeof(CUSTOMVERTEX),
        0,
        CUSTOMFVF,
        D3DPOOL_MANAGED,
        &v_buffer,
        NULL);

    // create the indices using an int array
    short indices[] =
    {
        0, 1, 2,    // side 1
        2, 1, 3,
        4, 0, 6,    // side 2
        6, 0, 2,
        7, 5, 6,    // side 3
        6, 5, 4,
        3, 1, 7,    // side 4
        7, 1, 5,
        4, 5, 0,    // side 5
        0, 5, 1,
        3, 7, 2,    // side 6
        2, 7, 6,
    };

    // create an index buffer interface called i_buffer
    g_d3ddev->CreateIndexBuffer(36 * sizeof(short),
        0,
        D3DFMT_INDEX16,
        D3DPOOL_MANAGED,
        &i_buffer,
        NULL);
#endif

    VOID* pVoid;    // a void pointer

    // lock v_buffer and load the vertices into it
    v_buffer->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, vertices, sizeof(vertices));
    v_buffer->Unlock();

    // lock i_buffer and load the indices into it
    i_buffer->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, indices, sizeof(indices));
    i_buffer->Unlock();

    return true;
}

void graphic_wndproc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

}

// this is the function that cleans up Direct3D and COM
void graphic_cleanup(void) {
    v_buffer->Release();    // close and release the vertex buffer
    i_buffer->Release();
}