#ifdef IMGUI_API
#ifndef __IMGUI_WRAPPER_H__
#define __IMGUI_WRAPPER_H__
#pragma once
#define IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
#define IMGUI_DEFINE_MATH_OPERATORS

#include <Windows.h>

static void ImGui_BeginFrame() {
#if defined(IMGUI_BACKEND_DX9)
    ImGui_ImplDX9_NewFrame();
#endif
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

static void ImGui_EndFrame() {
    ImGui::EndFrame();
}

static void ImGui_Render() {
    ImGui::Render();
#if defined(IMGUI_BACKEND_DX9)
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
#endif

#ifdef IMGUI_HAS_DOCK
    // Update and Render additional Platform Windows
    auto& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
#endif
}

static void ImGui_Cleanup() {
#if defined(IMGUI_BACKEND_DX9)
    ImGui_ImplDX9_Shutdown();
#endif
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

#if defined(IMGUI_BACKEND_DX9)
#include <d3d9.h>
static bool ImGui_Init_DX9(HWND hWnd, LPDIRECT3DDEVICE9 pDevice) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

#ifdef IMGUI_HAS_DOCK
    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
#endif

    if (!ImGui_ImplWin32_Init(hWnd))
        return false;
    
    if (!ImGui_ImplDX9_Init(pDevice))
        return false;

    return true;
}

static void ImGui_OnDeviceLost() {
    ImGui_ImplDX9_InvalidateDeviceObjects();
}

static void ImGui_OnDeviceReset() {
    ImGui_ImplDX9_CreateDeviceObjects();
}
#endif

#endif
#endif