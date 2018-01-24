#ifndef _DX11SYSTEM_H_
#define _DX11SYSTEM_H_
#pragma once
//speed up the build process, it reduces the size of the Win32 header files by excluding some of the less used APIs.
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "DX11Input.h"
#include "DX11Graphics.h"

class DX11System {
public:
    DX11System();
    DX11System(const DX11System&);
    ~DX11System();

    bool Initialize(Dungeon *dungeon);
    void Shutdown();
    void Run();

    LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
    bool Frame();
    void InitializeWindows(int&, int&);
    void ShutdownWindows();

private:
    LPCWSTR m_applicationName;
    HINSTANCE m_hinstance;
    HWND m_hwnd;

    DX11Input* m_Input;
    DX11Graphics* m_Graphics;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static DX11System* ApplicationHandle = 0;

#endif