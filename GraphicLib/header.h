#pragma once
#define BACKEND_DX9  0
#define BACKEND_DX10 1
#define BACKEND_DX11 2
#define BACKEND_DX12 3
#define BACKEND_GL   4

#include "framework.h"

//#ifndef STB_IMAGE_IMPLEMENTATION
//#   define STB_IMAGE_STATIC
//#endif

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "stb/stb_image.h"

#include <DirectXMath.h>
#include <DirectXPackedVector.h>

//#ifdef INC_DIRECTXMESH
#   include "DirectXMesh/DirectXMesh/DirectXMesh.h"
#   include "DirectXMesh/Meshconvert/Mesh.h"
//#endif

#ifdef BACKEND
#   if BACKEND >= BACKEND_DX9 && BACKEND <= BACKEND_DX12
#       include <dxgi.h>
#   endif
#   if BACKEND == BACKEND_DX9
#       pragma comment (lib, "d3d9.lib")
#       include <d3d9.h>
#   elif BACKEND == BACKEND_DX10
#       pragma comment (lib, "d3d10.lib")
#       include <d3d10.h>
#   elif BACKEND == BACKEND_DX11
#       pragma comment (lib, "d3d11.lib")
#       include <d3d11.h>
#   elif BACKEND == BACKEND_DX12
#       pragma comment (lib, "d3d12.lib")
#       include <d3d12.h>
#   endif
#endif

#ifdef INC_IMGUI
#   include "imgui/imgui.h"
#   include "imgui/imgui_internal.h"
#   ifdef BACKEND
#       if BACKEND >= BACKEND_DX9 && BACKEND <= BACKEND_DX12
#           include "imgui/backends/imgui_impl_win32.h"
#       endif
#       if BACKEND == BACKEND_DX9
#           include "imgui/backends/imgui_impl_dx9.h"
#           define IMGUI_BACKEND_DX9
#       elif BACKEND == BACKEND_DX10
#           include "imgui/backends/imgui_impl_dx10.h"
#       elif BACKEND == BACKEND_DX11
#           include "imgui/backends/imgui_impl_dx11.h"
#       elif BACKEND == BACKEND_DX12
#           include "imgui/backends/imgui_impl_dx12.h"
#       endif
#   endif
#   include "imgui_wrapper/imgui_wrapper.h"
#endif

