// include the basic windows header files and the Direct3D header file
#include "../../header.h"

using namespace DirectX;
using namespace DirectX::PackedVector;
namespace fs = std::filesystem;

// global declarations
HWND g_hWnd;
LPDIRECT3DDEVICE9 g_d3ddev;    // the pointer to the device class
LPDIRECT3DVERTEXBUFFER9 t_buffer = NULL;    // the pointer to the particle's vertex buffer
LPDIRECT3DVERTEXBUFFER9 g_buffer = NULL;    // the pointer to the grid's vertex buffer
float camx, camy, camz;    // camera position
XMMATRIX matRotateX;
XMMATRIX matRotateY;

struct CUSTOMVERTEX { FLOAT X, Y, Z; DWORD COLOR; FLOAT U, V; };
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)
struct GRIDVERTEX { D3DVECTOR position; DWORD color; };
#define GRIDFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)

// g_texture declarations
LPDIRECT3DTEXTURE9 g_texture;

// function prototypes
void set_particle(float x, float y, float z, float r);
void render_particle();
static void render_grid();

// this is the function used to render a single frame
void graphic_frame() {
    // calculate the camera position
    static float index = 0.0f; index += 0.01f;
    camx = cos(index) * 5.0f; camy = 2.0f; camz = sin(index) * 5.0f;

    // set the view transform
    XMVECTOR cameraPos, lookAtPos, upDirection;

    cameraPos.m128_f32[0] = camx;
    cameraPos.m128_f32[1] = camy;
    cameraPos.m128_f32[2] = camz;
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

    // render the grid
    // g_d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    // render_grid();

    g_d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    render_particle();
}

void graphic_ui() {

}

void graphic_state() {
    g_d3ddev->SetRenderState(D3DRS_LIGHTING, FALSE);
    g_d3ddev->SetRenderState(D3DRS_ZENABLE, TRUE);

    g_d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);    // turn on the color blending
    g_d3ddev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);    // set source factor
    g_d3ddev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);    // set dest factor
    g_d3ddev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);    // set the operation
}

// this is the function that puts the 3D models into video RAM
bool graphic_init(HWND hWnd, IDirect3DDevice9* device) {
    // load the g_texture we will use
    g_hWnd = hWnd;
    g_d3ddev = device;

    auto resPath = fs::current_path() / "directxtutorial_com" / "Particle_Engines" / "Assets" / "fire.png";

    if (!fs::exists(resPath))
        return false;

    int imageWidth, imageHeight, comp;
    unsigned char* imageData = stbi_load(resPath.string().c_str(), &imageWidth, &imageHeight, &comp, STBI_rgb_alpha);

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

    // create the vertices using the CUSTOMVERTEX
    struct CUSTOMVERTEX t_vert[] =
    {
            {-1.0f, 1.0f, 0.0f, D3DCOLOR_XRGB(118, 89, 55), 1, 0,},
        {-1.0f, -1.0f, 0.0f, D3DCOLOR_XRGB(118, 89, 55), 0, 0,},
        {1.0f, 1.0f, 0.0f, D3DCOLOR_XRGB(118, 89, 55), 1, 1,},
        {1.0f, -1.0f, 0.0f, D3DCOLOR_XRGB(118, 89, 55), 0, 1,},
    };

    // create a vertex buffer interface called t_buffer
    g_d3ddev->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX),
        0,
        CUSTOMFVF,
        D3DPOOL_MANAGED,
        &t_buffer,
        NULL);

    VOID* pVoid;    // a void pointer

    // lock t_buffer and load the vertices into it
    t_buffer->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, t_vert, sizeof(t_vert));
    t_buffer->Unlock();

    return true;
}

void graphic_wndproc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

}

// this is the function that cleans up Direct3D and COM
void graphic_cleanup() {
    t_buffer->Release();    // close and release the particle vertex buffer
    g_buffer->Release();    // close and release the grid vertex buffer
    g_texture->Release();    // close and release the g_texture
}

// this is the function that positions, rotates, scales and renders the particle
void set_particle(float x, float y, float z, float r) {
    // Before setting the world transform, do the intense mathematics!
    // a. Calculate the Differences
    static float difx, dify, difz;
    difx = camx - x;
    dify = camy - y;
    difz = camz - z;
    // ooh, intense!

    // b. Calculate the Distances
    static float FlatDist, TotalDist;
    FlatDist = sqrt(difx * difx + difz * difz);
    TotalDist = sqrt(FlatDist * FlatDist + dify * dify);

    // c. Y Rotation
    auto matRotateY = XMMatrixIdentity();
    matRotateY.r[0].m128_f32[0] = matRotateY.r[2].m128_f32[2] = difz / FlatDist;    // cosY
    matRotateY.r[2].m128_f32[0] = difx / FlatDist;    // sinY
    matRotateY.r[0].m128_f32[2] = -matRotateY.r[2].m128_f32[0];    // -sinY

    // d. X Rotation
    auto matRotateX = XMMatrixIdentity();
    matRotateX.r[1].m128_f32[1] = matRotateX.r[2].m128_f32[2] = FlatDist / TotalDist;    // cosX
    matRotateX.r[2].m128_f32[1] = dify / TotalDist;    // sinX
    matRotateX.r[1].m128_f32[2] = -matRotateX.r[2].m128_f32[1];    // -sinX

    // e. Tranlation
    static XMMATRIX matTranslate;
    matTranslate = XMMatrixTranslation(x, y, z);

    // f. Scaling
    static XMMATRIX matScale;
    matScale = XMMatrixIdentity();
    matScale.r[0].m128_f32[0] = matScale.r[1].m128_f32[1] = matScale.r[2].m128_f32[2] = r;

    // Now build the world matrix and set it
    matScale *= matRotateX * matRotateY * matTranslate;
    g_d3ddev->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&matScale);
}

// this function renders the particle
void render_particle() {
    // set the vertex format back to custom
    g_d3ddev->SetFVF(CUSTOMFVF);

    // reposition the particle
    set_particle(0.0f, 0.0f, 0.0f, 1.0f);

    // select the vertex buffer to display
    g_d3ddev->SetStreamSource(0, t_buffer, 0, sizeof(CUSTOMVERTEX));

    // set the g_texture
    g_d3ddev->SetTexture(0, g_texture);

    // draw the g_textured square
    g_d3ddev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

    return;
}

// this function renders the grid
void render_grid() {
    static bool InitNeeded = true;
    static int GridSize = 10;
    static XMMATRIX matIdentity;

    g_d3ddev->SetFVF(GRIDFVF);

    if (InitNeeded) {
        g_d3ddev->CreateVertexBuffer(sizeof(GRIDVERTEX) * GridSize * 9,
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

    g_d3ddev->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&matIdentity);
    g_d3ddev->SetStreamSource(0, g_buffer, 0, sizeof(GRIDVERTEX));
    g_d3ddev->SetTexture(0, NULL);
    g_d3ddev->DrawPrimitive(D3DPT_LINELIST, 0, GridSize * 4 + 2);
}
