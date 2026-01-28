// AnsysLicenseChecker.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "AnsysLicenseChecker.h"

#include <windowsx.h>
#include <commctrl.h>
#include <string>
#include <vector>

#pragma comment(lib, "Comctl32.lib")

// Simple stub — replace with real license discovery logic.
std::vector<std::wstring> GetAvailableLicenses()
{
    std::system("lmutil.exe lmstat -a -c ");

    return {
        L"ANSYS License - Structural (node-locked)",
        L"ANSYS License - CFD (shared)",
        L"ANSYS License - Electromagnetics",
        L"FeaturePack: HighPerformance",
        L"Floating: Workstation-1234"
    };
}

void KickLicense(HWND hListView, int index)
{
    if (!IsWindow(hListView) || index < 0) return;

    wchar_t buffer[512]{};
    ListView_GetItemText(hListView, index, 0, buffer, _countof(buffer));

    std::wstring msg = L"'Kick' action invoked for:\n";
    msg += buffer;

    // Replace this MessageBox with your actual kick logic.
    MessageBoxW(hListView, msg.c_str(), L"Kick", MB_OK | MB_ICONINFORMATION);
    printf( "%s", msg);

}

void PopulateLicenseList(HWND hListBox)
{
    if (!IsWindow(hListBox)) return;


    ListView_DeleteAllItems(hListBox);

    if (ListView_GetColumnWidth(hListBox, 0) == 0)
    {
        LVCOLUMNW col{};
        col.mask = LVCF_TEXT | LVCF_WIDTH;
        col.cx = 360;
        col.pszText = const_cast<LPWSTR>(L"License");
        ListView_InsertColumn(hListBox, 0, &col);

        col.cx = 80;
        col.pszText = const_cast<LPWSTR>(L"Kick");
        ListView_InsertColumn(hListBox, 1, &col);
    }

    auto licenses = GetAvailableLicenses();

    for (int i = 0; i < static_cast<int>(licenses.size()); ++i)
    {
        LVITEMW item{};
        item.mask = LVIF_TEXT;
        item.iItem = i;

        item.pszText = const_cast<LPWSTR>(licenses[i].c_str());
        ListView_InsertItem(hListBox, &item);

        ListView_SetItemText(hListBox, i, 1, const_cast<LPWSTR>(L"Kick"));

    }

    if (!licenses.empty())
    {
        ListView_SetItemState(hListBox, 0, LVIS_SELECTED, LVIS_SELECTED);
    }
}

// Helper: test whether a click was in the "Action" column for a given item
static bool IsPointInActionSubItem(HWND hListView, int item, POINT ptClient)
{
    RECT rcSub{};
    // Get bounds for subitem (column 1).
    // ListView_GetSubItemRect returns nonzero on success.
    if (!ListView_GetSubItemRect(hListView, item, 1, LVIR_BOUNDS, &rcSub))
        return false;
    return PtInRect(&rcSub, ptClient) != 0;
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        
        CreateWindowW(
            L"BUTTON",
            L"Check Licenses",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            10, 10, 140, 28,
            hwnd,
            reinterpret_cast<HMENU>(static_cast<INT_PTR>(ID_CHECK_BUTTON)),
            reinterpret_cast<LPCREATESTRUCT>(lParam)->hInstance,
            nullptr);

        
        HWND hListView = CreateWindowExW(
            0,
            WC_LISTVIEW,
            nullptr,
            WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL | WS_BORDER | WS_VSCROLL,
            10, 48, 460, 240,
            hwnd,
            reinterpret_cast<HMENU>(static_cast<INT_PTR>(ID_LISTBOX)),
            reinterpret_cast<LPCREATESTRUCT>(lParam)->hInstance,
            nullptr);

        // Initialize columns
        LVCOLUMNW col{};
        col.mask = LVCF_TEXT | LVCF_WIDTH;
        col.cx = 360;
        col.pszText = const_cast<LPWSTR>(L"License");
        ListView_InsertColumn(hListView, 0, &col);

        col.cx = 80;
        col.pszText = const_cast<LPWSTR>(L"Kick User");
        ListView_InsertColumn(hListView, 1, &col);

        ListView_SetExtendedListViewStyle(hListView, LVS_EX_FULLROWSELECT);

        return 0;
    }
    case WM_COMMAND:
    {
        int id = LOWORD(wParam);
        int code = HIWORD(wParam);
        if (id == ID_CHECK_BUTTON && code == BN_CLICKED)
        {
            HWND hListView = GetDlgItem(hwnd, ID_LISTBOX);
            PopulateLicenseList(hListView);
            return 0;
        }
        break;
    }
    case WM_NOTIFY:
    {
        LPNMHDR nm = reinterpret_cast<LPNMHDR>(lParam);
        if (nm->idFrom == ID_LISTBOX)
        {
            if (nm->code == NM_CLICK || nm->code == NM_DBLCLK)
            {
                HWND hListView = GetDlgItem(hwnd, ID_LISTBOX);
                // Get cursor position and map to list view client coordinates
                POINT pt;
                GetCursorPos(&pt);
                ScreenToClient(hListView, &pt);

                
                LVHITTESTINFO hit{};
                hit.pt = pt;
                int idx = ListView_HitTest(hListView, &hit);
                if (idx != -1 && hit.iItem != -1)
                {
                    
                    if (IsPointInActionSubItem(hListView, idx, pt))
                    {
                        // Invoke Kick action
                        KickLicense(hListView, idx);
						ListView_DeleteItem(hListView, idx);
                        return 0;
                    }
                    else if (nm->code == NM_DBLCLK)
                    {
                        // Could be useful for later ideas if I want to maybe open teams or something 


                        wchar_t buffer[512]{};
                        ListView_GetItemText(hListView, idx, 0, buffer, _countof(buffer));
                        std::wstring msg = L"Selected User:\n";
                        msg += buffer;
                        MessageBoxW(hwnd, msg.c_str(), L"User Selected", MB_OK | MB_ICONINFORMATION);
                        return 0;
                    }
                }
            }
        }
        break;
    }
    case WM_SIZE:
    {
        HWND hListView = GetDlgItem(hwnd, ID_LISTBOX);
        if (IsWindow(hListView))
        {
            RECT rc;
            GetClientRect(hwnd, &rc);
            // Keep same margins as before: left 10, top 48, right margin 10, bottom margin 58
            SetWindowPos(hListView, nullptr, 10, 48, rc.right - 20, rc.bottom - 58, SWP_NOZORDER);
        }
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR    lpCmdLine,
                      _In_ int       nCmdShow)
{
    // Initialize common controls for ListView
    INITCOMMONCONTROLSEX icc{};
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icc);

    const wchar_t CLASS_NAME[] = L"AnsysLicenseCheckerMain";

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = MainWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    RegisterClassExW(&wc);

    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"Ansys License Checker",
        WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME),
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 360,
        nullptr, nullptr, hInstance, nullptr);

    if (!hwnd)
        return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return static_cast<int>(msg.wParam);
}

// If the project is built as a Console subsystem (CRT expects `main`), provide
// a simple `main` that forwards to the GUI entry point.
extern "C" int __cdecl main()
{
    HINSTANCE hInstance = GetModuleHandleW(nullptr);
    LPWSTR cmdLine = GetCommandLineW();
    return wWinMain(hInstance, nullptr, cmdLine, SW_SHOWDEFAULT);
}

