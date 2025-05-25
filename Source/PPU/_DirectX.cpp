#include <d3d9.h>
#include "Basics/Std.hpp"

#define VERTEX_BUFFER_SIZE (1<<16)

struct dx_vertex
{
    float x, y, z, rhw;
    DWORD color;
};

struct dx_state
{
    LPDIRECT3D9 DX_State;
    LPDIRECT3DDEVICE9 DX_Device;
    D3DPRESENT_PARAMETERS DX_Params;
    LPDIRECT3DVERTEXBUFFER9 DX_VB;
    dx_vertex *Vertices;
    usz VertexCount;
};

static dx_state _DX;

static inline DWORD DX_RGBAToInternal(rgba c)
{
    return D3DCOLOR_ARGB(c&0xFF, c>>24&0xFF, c>>16&0xFF, c>>8&0xFF);
}

static void DX_Init(HWND hWnd)
{
    _DX.DX_State = Direct3DCreate9(D3D_SDK_VERSION);

    Mem::Zero(&_DX.DX_Params, sizeof(_DX.DX_Params));  
    _DX.DX_Params.Windowed = TRUE;
    _DX.DX_Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
    _DX.DX_Params.hDeviceWindow = hWnd;
    _DX.DX_Params.BackBufferWidth = 640;
    _DX.DX_Params.BackBufferHeight = 480;

    _DX.DX_State->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hWnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &_DX.DX_Params,
        &_DX.DX_Device
    );

    _DX.DX_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    _DX.DX_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    _DX.DX_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    _DX.DX_Device->CreateVertexBuffer(
        sizeof(dx_vertex) * VERTEX_BUFFER_SIZE,
        0,
        D3DFVF_XYZRHW | D3DFVF_DIFFUSE,
        D3DPOOL_MANAGED,
        &_DX.DX_VB,
        NULL
    );

    _DX.Vertices = (dx_vertex*)Mem::Alloc(sizeof(dx_vertex) * VERTEX_BUFFER_SIZE);
    _DX.VertexCount = 0;
}

static void DX_Append(dx_vertex *v, usz n)
{
    if (_DX.VertexCount + n > VERTEX_BUFFER_SIZE)
    {
        ExitProcess(1);
    }

    Mem::Copy(_DX.Vertices + _DX.VertexCount, v, sizeof(dx_vertex) * n);
    _DX.VertexCount += n;
}

static void DX_EndFrame()
{
    void* pVertices;
    _DX.DX_VB->Lock(0, sizeof(dx_vertex) * _DX.VertexCount, (void**)&pVertices, 0);
    Mem::Copy(pVertices, _DX.Vertices, sizeof(dx_vertex) * _DX.VertexCount);
    _DX.DX_VB->Unlock();

    _DX.DX_Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(127, 127, 127), 1.0f, 0);
    _DX.DX_Device->BeginScene();

    _DX.DX_Device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
    _DX.DX_Device->SetStreamSource(0, _DX.DX_VB, 0, sizeof(dx_vertex));
    _DX.DX_Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, _DX.VertexCount / 3);

    _DX.DX_Device->EndScene();
    _DX.DX_Device->Present(NULL, NULL, NULL, NULL);
    _DX.VertexCount = 0;
}

static void DX_Shutdown()
{
    _DX.DX_VB->Release();
    _DX.DX_Device->Release();
    _DX.DX_State->Release();
    Mem::Free(_DX.Vertices);
}