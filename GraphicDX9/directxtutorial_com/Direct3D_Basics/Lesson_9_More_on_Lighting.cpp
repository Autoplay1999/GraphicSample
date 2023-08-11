// include the basic windows header files and the Direct3D header file
#include "../../header.h"

using namespace DirectX;

// global declarations
HWND g_hWnd;
LPDIRECT3DDEVICE9 g_d3ddev;    // the pointer to the device class
LPDIRECT3DVERTEXBUFFER9 v_buffer = NULL;    // the pointer to the vertex buffer
LPDIRECT3DINDEXBUFFER9 i_buffer = NULL;

struct CUSTOMVERTEX { FLOAT X, Y, Z; D3DVECTOR NORMAL; };
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_NORMAL)
#define LIGHT 1

static void set_light();

// this is the function used to render a single frame
void graphic_frame() {
    // select which vertex format we are using
    g_d3ddev->SetFVF(CUSTOMFVF);

    // set the view transform
    XMVECTOR cameraPos, lookAtPos, upDirection;

    cameraPos.m128_f32[0] = 0.0f;
    cameraPos.m128_f32[1] = 40.0f;
    cameraPos.m128_f32[2] = 30.0f;
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

    // select the vertex buffer to display
    g_d3ddev->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));

    // set the world transform
    static float index = 0.0f; index += 0.03f;    // an ever-increasing float value
    auto matTranslate = XMMatrixTranslation(sin(index) * 12.0f, 0.0f, cos(index) * 25.0f);
    g_d3ddev->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&(matTranslate));    // set the world transform

    // select the vertex and index buffers to use
    g_d3ddev->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
    g_d3ddev->SetIndices(i_buffer);

    set_light();

    // draw the cube
    g_d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);
}

void graphic_ui() {

}

void graphic_state() {
    g_d3ddev->SetRenderState(D3DRS_LIGHTING, TRUE);    // turn on the 3D lighting
    g_d3ddev->SetRenderState(D3DRS_ZENABLE, TRUE);    // turn on the z-buffer
    g_d3ddev->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(50, 50, 50));    // ambient light
    g_d3ddev->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);    // handle normals in scaling
}

// this is the function that puts the 3D models into video RAM
bool graphic_init(HWND hWnd, IDirect3DDevice9* device) {
    g_hWnd = hWnd;
    g_d3ddev = device;

    // create the vertices using the CUSTOMVERTEX struct
    CUSTOMVERTEX vertices[] =
    {
        { -3.0f, -3.0f, 3.0f, 0.0f, 0.0f, 1.0f, },    // side 1
        { 3.0f, -3.0f, 3.0f, 0.0f, 0.0f, 1.0f, },
        { -3.0f, 3.0f, 3.0f, 0.0f, 0.0f, 1.0f, },
        { 3.0f, 3.0f, 3.0f, 0.0f, 0.0f, 1.0f, },

        { -3.0f, -3.0f, -3.0f, 0.0f, 0.0f, -1.0f, },    // side 2
        { -3.0f, 3.0f, -3.0f, 0.0f, 0.0f, -1.0f, },
        { 3.0f, -3.0f, -3.0f, 0.0f, 0.0f, -1.0f, },
        { 3.0f, 3.0f, -3.0f, 0.0f, 0.0f, -1.0f, },

        { -3.0f, 3.0f, -3.0f, 0.0f, 1.0f, 0.0f, },    // side 3
        { -3.0f, 3.0f, 3.0f, 0.0f, 1.0f, 0.0f, },
        { 3.0f, 3.0f, -3.0f, 0.0f, 1.0f, 0.0f, },
        { 3.0f, 3.0f, 3.0f, 0.0f, 1.0f, 0.0f, },

        { -3.0f, -3.0f, -3.0f, 0.0f, -1.0f, 0.0f, },    // side 4
        { 3.0f, -3.0f, -3.0f, 0.0f, -1.0f, 0.0f, },
        { -3.0f, -3.0f, 3.0f, 0.0f, -1.0f, 0.0f, },
        { 3.0f, -3.0f, 3.0f, 0.0f, -1.0f, 0.0f, },

        { 3.0f, -3.0f, -3.0f, 1.0f, 0.0f, 0.0f, },    // side 5
        { 3.0f, 3.0f, -3.0f, 1.0f, 0.0f, 0.0f, },
        { 3.0f, -3.0f, 3.0f, 1.0f, 0.0f, 0.0f, },
        { 3.0f, 3.0f, 3.0f, 1.0f, 0.0f, 0.0f, },

        { -3.0f, -3.0f, -3.0f, -1.0f, 0.0f, 0.0f, },    // side 6
        { -3.0f, -3.0f, 3.0f, -1.0f, 0.0f, 0.0f, },
        { -3.0f, 3.0f, -3.0f, -1.0f, 0.0f, 0.0f, },
        { -3.0f, 3.0f, 3.0f, -1.0f, 0.0f, 0.0f, },
    };

    // create a vertex buffer interface called v_buffer
    g_d3ddev->CreateVertexBuffer(24 * sizeof(CUSTOMVERTEX),
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

    // create the indices using an int array
    short indices[] =
    {
        0, 1, 2,    // side 1
        2, 1, 3,
        4, 5, 6,    // side 2
        6, 5, 7,
        8, 9, 10,    // side 3
        10, 9, 11,
        12, 13, 14,    // side 4
        14, 13, 15,
        16, 17, 18,    // side 5
        18, 17, 19,
        20, 21, 22,    // side 6
        22, 21, 23,
    };

    // create an index buffer interface called i_buffer
    g_d3ddev->CreateIndexBuffer(36 * sizeof(short),
        0,
        D3DFMT_INDEX16,
        D3DPOOL_MANAGED,
        &i_buffer,
        NULL);

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
}

void set_light() {
    D3DLIGHT9 light;    // create the light struct
    D3DMATERIAL9 material;    // create the material struct

    ZeroMemory(&light, sizeof(light));    // clear out the light struct for use
#if LIGHT == 0
    light.Type = D3DLIGHT_POINT;    // make the light type 'point light'
    light.Diffuse = D3DCOLORVALUE(0.5f, 0.5f, 0.5f, 1.0f);
    light.Position = D3DVECTOR(0.0f, 5.0f, 0.0f);
    light.Range = 100.0f;    // a range of 100
    light.Attenuation0 = 0.0f;    // no constant inverse attenuation
    light.Attenuation1 = 0.125f;    // only .125 inverse attenuation
    light.Attenuation2 = 0.0f;    // no square inverse attenuation
#elif LIGHT == 1
    light.Type = D3DLIGHT_SPOT;    // make the light type 'spot light'
    light.Diffuse = D3DCOLORVALUE(0.5f, 0.5f, 0.5f, 1.0f);
    light.Position = D3DVECTOR(-12.0f, 0.0f, 30.0f);
    light.Direction = D3DVECTOR(0.0f, 0.0f, -1.0f);
    light.Range = 100.0f;    // a range of 100
    light.Attenuation0 = 0.0f;    // no constant inverse attenuation
    light.Attenuation1 = 0.125f;    // only .125 inverse attenuation
    light.Attenuation2 = 0.0f;    // no square inverse attenuation
    light.Phi = (XM_PI / 180.0f) * 40.0f;    // set the outer cone to 30 degrees
    light.Theta = (XM_PI / 180.0f) * 20.0f;    // set the inner cone to 10 degrees
    light.Falloff = 1.0f;    // use the typical falloff
#endif

    g_d3ddev->SetLight(0, &light);    // send the light struct properties to light #0
    g_d3ddev->LightEnable(0, TRUE);    // turn on light #0

    ZeroMemory(&material, sizeof(D3DMATERIAL9));    // clear out the struct for use
    material.Diffuse = D3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);    // set diffuse color to white
    material.Ambient = D3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);    // set ambient color to white

    g_d3ddev->SetMaterial(&material);    // set the globably-used material to &material
}