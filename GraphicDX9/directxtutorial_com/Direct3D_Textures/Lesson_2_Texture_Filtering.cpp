// include the basic windows header files and the Direct3D header file
#include "../../header.h"
#include <DirectXMath.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../../stb_image.h"

using namespace DirectX;
namespace fs = std::filesystem;

// global declarations
HWND g_hWnd;
LPDIRECT3DDEVICE9 g_d3ddev;
LPDIRECT3DVERTEXBUFFER9 v_buffer = NULL;
LPDIRECT3DINDEXBUFFER9 i_buffer = NULL;
LPDIRECT3DTEXTURE9 g_texture = NULL;    // our first g_texture

struct CUSTOMVERTEX { FLOAT X, Y, Z; D3DVECTOR NORMAL; FLOAT U, V; };
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

static void set_light();

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
    static float index = 0.0f; index += 0.03f;
    auto matRotateY = XMMatrixRotationY(index);
    g_d3ddev->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&matRotateY);

    // select the vertex and index buffers to use
    g_d3ddev->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
    g_d3ddev->SetIndices(i_buffer);

    // set the g_texture
    g_d3ddev->SetTexture(0, g_texture);

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

    g_d3ddev->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 8);    // anisotropic level
    g_d3ddev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);    // minification
    g_d3ddev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);    // magnification
    g_d3ddev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);    // mipmap
}

// this is the function that puts the 3D models into video RAM
bool graphic_init(HWND hWnd, IDirect3DDevice9* device) {
    g_hWnd = hWnd;
    g_d3ddev = device;

    auto resPath = fs::current_path() / "directxtutorial_com" / "Direct3D_Textures" / "Assets" / "bricks.png";

    if (!fs::exists(resPath))
        return false;

    int imageWidth, imageHeight, comp;
    unsigned char* imageData = stbi_load(resPath.string().c_str(), &imageWidth, &imageHeight, &comp, 4);

    if (D3D_OK != g_d3ddev->CreateTexture(imageWidth, imageHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &g_texture, nullptr))
        return false;

    for (int i = 0; i < imageWidth * imageHeight; ++i) {
        auto tmp = imageData[i * 4];
        imageData[i * 4] = imageData[i * 4 + 2];
        imageData[i * 4 + 2] = tmp;
    }

    D3DLOCKED_RECT lockedRect;
    g_texture->LockRect(0, &lockedRect, nullptr, 0);
    unsigned char* dest = static_cast<unsigned char*>(lockedRect.pBits);
    memcpy(dest, imageData, imageWidth * imageHeight * 4);
    g_texture->UnlockRect(0);

    stbi_image_free(imageData);

    // create the vertices using the CUSTOMVERTEX struct
    CUSTOMVERTEX vertices[] =
    {
        { -3.0f, -3.0f, 3.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, },    // side 1
        { 3.0f, -3.0f, 3.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, },
        { -3.0f, 3.0f, 3.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f, },
        { 3.0f, 3.0f, 3.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, },

        { -3.0f, -3.0f, -3.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, },    // side 2
        { -3.0f, 3.0f, -3.0f,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f, },
        { 3.0f, -3.0f, -3.0f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f, },
        { 3.0f, 3.0f, -3.0f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f, },

        { -3.0f, 3.0f, -3.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, },    // side 3
        { -3.0f, 3.0f, 3.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, },
        { 3.0f, 3.0f, -3.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, },
        { 3.0f, 3.0f, 3.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, },

        { -3.0f, -3.0f, -3.0f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f, },    // side 4
        { 3.0f, -3.0f, -3.0f,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f, },
        { -3.0f, -3.0f, 3.0f,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f, },
        { 3.0f, -3.0f, 3.0f,  0.0f, -1.0f, 0.0f,  1.0f, 1.0f, },

        { 3.0f, -3.0f, -3.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, },    // side 5
        { 3.0f, 3.0f, -3.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, },
        { 3.0f, -3.0f, 3.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, },
        { 3.0f, 3.0f, 3.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, },

        { -3.0f, -3.0f, -3.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, },    // side 6
        { -3.0f, -3.0f, 3.0f,  -1.0f, 0.0f, 0.0f,  1.0f, 0.0f, },
        { -3.0f, 3.0f, -3.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f, },
        { -3.0f, 3.0f, 3.0f,  -1.0f, 0.0f, 0.0f,  1.0f, 1.0f, },
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
    v_buffer->Release();
    i_buffer->Release();
    g_texture->Release();    // close and release the g_texture
}

// this is the function that sets up the lights and materials
void set_light() {
    D3DLIGHT9 light;    // create the light struct
    D3DMATERIAL9 material;    // create the material struct

    ZeroMemory(&light, sizeof(light));    // clear out the light struct for use
    light.Type = D3DLIGHT_DIRECTIONAL;    // make the light type 'directional light'
    light.Diffuse = D3DCOLORVALUE(0.5f, 0.5f, 0.5f, 1.0f);    // set the light's color
    light.Direction = D3DVECTOR(-1.0f, -0.3f, -1.0f);

    g_d3ddev->SetLight(0, &light);    // send the light struct properties to light #0
    g_d3ddev->LightEnable(0, TRUE);    // turn on light #0

    ZeroMemory(&material, sizeof(D3DMATERIAL9));    // clear out the struct for use
    material.Diffuse = D3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);    // set diffuse color to white
    material.Ambient = D3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);    // set ambient color to white

    g_d3ddev->SetMaterial(&material);    // set the globably-used material to &material
}
