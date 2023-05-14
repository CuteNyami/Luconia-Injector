#include "Window.h"
#include "Minecraft.h"
#include "Discord.h"
#include "Inject.h"
#include "Utils.h"
#include "Downloader.h"

#include <iostream>
#include "Config.h"

#define ID_LAUNCH_BUTTON 100
#define ID_HIDE_MENU_BUTTON 101
#define ID_SELECT_BUTTON 102
#define ID_PATH_EDIT 103

#define ID_CHECK_CUSTOM_DLL 104
#define ID_CHECK_SHOW_DISCORD_RPC_DLL 105
#define ID_CHECK_CLOSE_AFTER_INJECT_DLL 106

#define ID_QUIT 100
#define ID_OPEN 101
#define ID_LAUNCH 102

HINSTANCE hInst;
Window window;
NOTIFYICONDATA nid;
HMENU hMenu;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void SetAccessControl(std::wstring ExecutableName, const wchar_t* AccessString);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, int nCmdShow)
{
    /*
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "r", stdin);
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    */

    Discord::init();
    Config::init();
    Downloader::init();

    if (Config::config["settings"]["show_discord_rpc"]) Discord::update();

    MSG msg{};
    window = Window(hInstance, nCmdShow);
    window.setup(&msg, WndProc);

    //delete fp;
    return (int) msg.wParam;
}

void InjectSelectedDll(HWND hWnd)
{
    ShellExecute(hWnd, L"open", L"shell:appsFolder\\Microsoft.MinecraftUWP_8wekyb3d8bbwe!App", NULL, NULL, 0);
    while (true) if (GetProcId(L"Minecraft.Windows.exe") != NULL) break;

    DWORD processId = GetProcId(L"Minecraft.Windows.exe");

    if (processId == NULL)
    {
        SetDlgItemTextA(hWnd, 5, "Can't find process! | 0");
        return;
    }

    static bool alreadyAsked = false;
    if (!Config::config["settings"]["use_custom_path"])
    {
        if (!alreadyAsked)
        {
            Downloader::checkUpdate(true);
            alreadyAsked = true;
        }
    }

    std::wstring wstring;
    if (!Config::config["settings"]["use_custom_path"])
    {
        String path = Downloader::filePath;
        wstring = path.to_wstring();
    }
    else
    {
        wchar_t wc[1024];
        GetWindowText(GetDlgItem(hWnd, ID_PATH_EDIT), wc, 1024);
        wstring = wc;
    }
    WString wstr = wstring;

    std::ifstream existsPath(wstr.to_string());

    if (!existsPath)
    {
        std::wstring text = L"Process found! | " + std::to_wstring(processId) + L" | Invalid path";
        SetDlgItemTextW(hWnd, 5, text.c_str());
        return;
    }
    existsPath.close();

    SetAccessControl(wstr.to_wstr(), L"S-1-15-2-1");
    Inject(processId, wstr.c_str());

    if (Config::config["settings"]["close_after_inject"])
    {
        Shell_NotifyIconW(NIM_DELETE, &nid);
        PostQuitMessage(0);
        FreeConsole();
    }

    std::string status = "Process found! | " + std::to_string(processId) + " | Injected";
    SetDlgItemTextA(hWnd, 5, status.c_str());
}

void pathEditDisabled(HWND hWnd)
{
    if (!Config::config["settings"]["use_custom_path"])
    {
        wchar_t wc[1024];
        GetWindowText(GetDlgItem(hWnd, ID_PATH_EDIT), wc, 1024);
        WString wstr = wc;
        String str = (std::string)Config::config["settings"]["custom_path"];

        if (Config::config["settings"]["custom_path"] == "")
        {
            if (wstr.to_string() == "") SetWindowText(GetDlgItem(hWnd, ID_PATH_EDIT), L"Enable custom dll to change the path");
            else SetWindowText(GetDlgItem(hWnd, ID_PATH_EDIT), wc);
        }
        else SetWindowText(GetDlgItem(hWnd, ID_PATH_EDIT), str.to_wstring().c_str());
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)                                                                                                                                                              
{                                                                                                                                                                                                                                            
    POINT pt;

    switch (message)
    {
    case WM_CREATE:
    {
        SystemParametersInfo(SPI_SETBEEP, FALSE, NULL, 0);

        CreateWindowW(L"BUTTON", L"Launch", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 10, 125, 40, hWnd, (HMENU)ID_LAUNCH_BUTTON, NULL, NULL);
        CreateWindowW(L"BUTTON", L"Hide Menu", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 10, 55, 125, 20, hWnd, (HMENU)ID_HIDE_MENU_BUTTON, NULL, NULL);
        CreateWindowW(L"BUTTON", L"Select", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 10, 80, 62, 20, hWnd, (HMENU)ID_SELECT_BUTTON, NULL, NULL);
        
        CreateWindowW(L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_WANTRETURN, 78, 80, 195, 20, hWnd, (HMENU)ID_PATH_EDIT, hInst, NULL);
        WCHAR placeholderText[] = L"Click \"Select\" to select the dll file";

        SendMessage(GetDlgItem(hWnd, ID_PATH_EDIT), EM_SETCUEBANNER, FALSE, (LPARAM)placeholderText);
        
        CreateWindow(L"BUTTON", TEXT("Custom dll"), WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 143, 10, 1000, 20, hWnd, (HMENU)ID_CHECK_CUSTOM_DLL, NULL, NULL);
        CreateWindow(L"BUTTON", TEXT("Show Discord RPC"), WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 143, 30, 1000, 20, hWnd, (HMENU)ID_CHECK_SHOW_DISCORD_RPC_DLL, NULL, NULL);
        CreateWindow(L"BUTTON", TEXT("Close after inject"), WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 143, 50, 1000, 20, hWnd, (HMENU)ID_CHECK_CLOSE_AFTER_INJECT_DLL, NULL, NULL);

        CheckDlgButton(hWnd, ID_CHECK_CUSTOM_DLL, Config::config["settings"]["use_custom_path"] ? BST_CHECKED : BST_UNCHECKED);
        CheckDlgButton(hWnd, ID_CHECK_SHOW_DISCORD_RPC_DLL, Config::config["settings"]["show_discord_rpc"] ? BST_CHECKED : BST_UNCHECKED);
        CheckDlgButton(hWnd, ID_CHECK_CLOSE_AFTER_INJECT_DLL, Config::config["settings"]["close_after_inject"] ? BST_CHECKED : BST_UNCHECKED);
        //CheckDlgButton(hWnd, 2, BST_CHECKED);

        EnableWindow(GetDlgItem(hWnd, ID_SELECT_BUTTON), Config::config["settings"]["use_custom_path"]);
        EnableWindow(GetDlgItem(hWnd, ID_PATH_EDIT), Config::config["settings"]["use_custom_path"]);

        pathEditDisabled(hWnd);

        std::string str = "Version 1.0 | Minecraft Version " + Minecraft::getVersion();
        std::wstring wstr = std::wstring(str.begin(), str.end());

        CreateWindowEx(0, STATUSCLASSNAME, wstr.c_str(), WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWnd, (HMENU)5, NULL, NULL);

        nid.cbSize = sizeof(nid);
        nid.hWnd = hWnd;
        nid.uID = 0;
        nid.uVersion = NOTIFYICON_VERSION;
        nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        nid.uCallbackMessage = WM_USER;
        nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
        lstrcpy(nid.szTip, L"Luconia Injector");

        Shell_NotifyIcon(NIM_ADD, &nid);
        hMenu = CreatePopupMenu();
        AppendMenu(hMenu, MF_STRING, ID_LAUNCH, L"Launch");
        AppendMenu(hMenu, MF_STRING, ID_OPEN, L"Open");
        AppendMenu(hMenu, MF_STRING, ID_QUIT, L"Quit");
        break;
    }
    case WM_USER:
    {
        switch (lParam)
        {
            case WM_LBUTTONDBLCLK: 
            {
                ShowWindow(hWnd, SW_SHOW);
                break;
            }
            case WM_RBUTTONDOWN: 
            {
                GetCursorPos(&pt);
                SetForegroundWindow(hWnd);
                auto iconSelection = TrackPopupMenu(hMenu, TPM_RETURNCMD, pt.x, pt.y, NULL, hWnd, NULL);

                switch (iconSelection) 
                {
                    case ID_QUIT: 
                        PostQuitMessage(0);
                        Shell_NotifyIcon(NIM_DELETE, &nid);
                        break;
                    case ID_OPEN: 
                        ShowWindow(hWnd, SW_SHOW);
                        break;
                    case ID_LAUNCH:
                        InjectSelectedDll(hWnd);
                        break;
                }
                break;
            }
        }
    }
    case WM_COMMAND:
    {
        int id = LOWORD(wParam);
        switch (id)
        {
        case ID_CHECK_CUSTOM_DLL:
        {
            BOOL checked = IsDlgButtonChecked(hWnd, ID_CHECK_CUSTOM_DLL);
            if (checked)
            {
                CheckDlgButton(hWnd, ID_CHECK_CUSTOM_DLL, BST_UNCHECKED);
                EnableWindow(GetDlgItem(hWnd, ID_SELECT_BUTTON), FALSE);
                EnableWindow(GetDlgItem(hWnd, ID_PATH_EDIT), FALSE);
                Config::config["settings"]["use_custom_path"] = false;
                Config::save();
                
                pathEditDisabled(hWnd);
            }
            else
            {
                CheckDlgButton(hWnd, ID_CHECK_CUSTOM_DLL, BST_CHECKED);
                EnableWindow(GetDlgItem(hWnd, ID_SELECT_BUTTON), TRUE);
                EnableWindow(GetDlgItem(hWnd, ID_PATH_EDIT), TRUE);
                Config::config["settings"]["use_custom_path"] = true;
                Config::save();

                SetWindowText(GetDlgItem(hWnd, ID_PATH_EDIT), NULL);
            }
            break;
        }
        case ID_CHECK_CLOSE_AFTER_INJECT_DLL:
        {
            BOOL checked = IsDlgButtonChecked(hWnd, ID_CHECK_CLOSE_AFTER_INJECT_DLL);
            if (checked)
            {
                CheckDlgButton(hWnd, ID_CHECK_CLOSE_AFTER_INJECT_DLL, BST_UNCHECKED);
                Config::config["settings"]["close_after_inject"] = false;
                Config::save();
            }
            else
            {
                CheckDlgButton(hWnd, ID_CHECK_CLOSE_AFTER_INJECT_DLL, BST_CHECKED);
                Config::config["settings"]["close_after_inject"] = true;
                Config::save();
            }
            break;
        }
        case ID_CHECK_SHOW_DISCORD_RPC_DLL:
        {
            BOOL checked = IsDlgButtonChecked(hWnd, ID_CHECK_SHOW_DISCORD_RPC_DLL);
            if (checked)
            {
                CheckDlgButton(hWnd, ID_CHECK_SHOW_DISCORD_RPC_DLL, BST_UNCHECKED);
                Config::config["settings"]["show_discord_rpc"] = false;
                Config::save();
                Discord_ClearPresence();
            }
            else
            {
                CheckDlgButton(hWnd, ID_CHECK_SHOW_DISCORD_RPC_DLL, BST_CHECKED);
                Config::config["settings"]["show_discord_rpc"] = true;
                Config::save();
                Discord::update();
            }
            break;
        }
        case ID_LAUNCH_BUTTON:
        {
            InjectSelectedDll(hWnd);
            break;
        }
        case ID_SELECT_BUTTON:
        {
            wchar_t path[1024] = {};
            wchar_t name[1024] = {};

            OPENFILENAME openDialog = {};
            openDialog.lStructSize = sizeof(OPENFILENAME);
            openDialog.hwndOwner = hWnd;
            openDialog.lpstrFile = path;
            openDialog.nMaxFile = sizeof(path);
            openDialog.lpstrFilter = L"Dynamic link library (*.dll)\0*.dll\0Show all files (*.*)\0*.*\0";

            if (GetOpenFileName(&openDialog) != 0) 
            {
                SetDlgItemTextW(hWnd, ID_PATH_EDIT, path);
            }

            break;
        }
        case ID_HIDE_MENU_BUTTON:
            ShowWindow(hWnd, SW_HIDE);
            break;
        }
        break;
    }
    case WM_CTLCOLORSTATIC:
    {
        HDC hdc = (HDC)wParam;
        int id = LOWORD(wParam);
        break;
    }
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            HFONT hFont = CreateFont(13, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma"));
            
            SelectObject(hdc, hFont);
            DeleteObject(hFont);

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        Shell_NotifyIconW(NIM_DELETE, &nid);
        PostQuitMessage(0);
        FreeConsole();
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void SetAccessControl(std::wstring ExecutableName, LPCWSTR AccessString)
{
    PSECURITY_DESCRIPTOR SecurityDescriptor;
    EXPLICIT_ACCESSW ExplicitAccess{};

    ACL* AccessControlCurrent = nullptr;
    ACL* AccessControlNew = nullptr;

    SECURITY_INFORMATION SecurityInfo = DACL_SECURITY_INFORMATION;
    PSID SecurityIdentifier;

    if (GetNamedSecurityInfoW(ExecutableName.c_str(), SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, &AccessControlCurrent, NULL, &SecurityDescriptor) == ERROR_SUCCESS)
    {
        ConvertStringSidToSidW(AccessString, &SecurityIdentifier);
        if (SecurityIdentifier != NULL)
        {
            ExplicitAccess.grfAccessPermissions = GENERIC_READ | GENERIC_EXECUTE;
            ExplicitAccess.grfAccessMode = SET_ACCESS;
            ExplicitAccess.grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
            ExplicitAccess.Trustee.TrusteeForm = TRUSTEE_IS_SID;
            ExplicitAccess.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
            ExplicitAccess.Trustee.ptstrName = (LPWSTR)SecurityIdentifier;

            if (SetEntriesInAclW(1, &ExplicitAccess, AccessControlCurrent, &AccessControlNew) == ERROR_SUCCESS) SetNamedSecurityInfoW((LPWSTR)ExecutableName.c_str(), SE_FILE_OBJECT, SecurityInfo, NULL, NULL, AccessControlNew, NULL);
        }
    }
    if (SecurityDescriptor) LocalFree((HLOCAL)SecurityDescriptor);
    if (AccessControlNew) LocalFree((HLOCAL)AccessControlNew);
}