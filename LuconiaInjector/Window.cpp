#include "Window.h"

ATOM Window::registerClass(WNDPROC wndProc)
{
	WNDCLASSEX wcex{};
	
	wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = wndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"LuconiaInjector";
    wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2)); //LoadIcon(NULL, IDI_APPLICATION)

    return RegisterClassExW(&wcex);
}

WPARAM Window::setup(MSG* msg, WNDPROC wndProc)
{
    registerClass(wndProc);

    if (!init()) return FALSE;

    while (GetMessage(msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(msg);
        DispatchMessage(msg);
    }

    return msg->wParam;
}

BOOL Window::init()
{
    HWND hWnd = CreateWindowW(L"LuconiaInjector", TEXT("Luconia Injector"), WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, 0, 300, 170, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd) return FALSE;

    EnumChildWindows(hWnd, (WNDENUMPROC)Utils::SetFont, (LPARAM)GetStockObject(DEFAULT_GUI_FONT));

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}