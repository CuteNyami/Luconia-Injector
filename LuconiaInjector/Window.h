#pragma once

#define WIN32_LEAN_AND_MEAN            
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <CommCtrl.h>
#include <string>
#include <Vssym32.h>
#include <Uxtheme.h>
#include <WindowsX.h>
#include <shellapi.h>
#include <fstream>
#include <filesystem>
#include <commdlg.h>

#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "Uxtheme.lib")

#include "Utils.h"
#include "resource.h"

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

class Window
{
public:
	HINSTANCE hInstance;
	int nCmdShow;
	WCHAR szWindowClass[100];
public:
	Window() = default;
	Window(HINSTANCE hInstance, int nCmdShow)
	{
		this->hInstance = hInstance;
		this->nCmdShow = nCmdShow;
	}
public:
	ATOM registerClass(WNDPROC);
	WPARAM setup(MSG*, WNDPROC);
	BOOL init();
};