#include <windows.h>
#include <string>

#define TEXTEDITBOX_MULT_WIDTH 450
#define TEXTEDITBOX_MULT_HEIGHT 300
#define TEXTEDITBOX_SING_WIDTH 260
#define TEXTEDITBOX_SING_HEIGHT 120

struct _TextEditBoxParams
{
    bool mult;
    HINSTANCE instance;
    HWND hw_edit;
    HWND hw_btn;
    HFONT font;
    std::wstring* result;
};

static LRESULT CALLBACK _TextEditBoxProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    constexpr int edit_id = 100;
    constexpr int button_id = 200;
    constexpr int margin = 10;
    constexpr int edit_height = 32;
    constexpr int button_width = 75;
    constexpr int button_height = 23;

    _TextEditBoxParams* pParams = nullptr;
    if (msg == WM_CREATE)
    {
        LPCREATESTRUCT pCreateStruct = (LPCREATESTRUCT)lParam;
        _TextEditBoxParams* lpParams = (_TextEditBoxParams*)pCreateStruct->lpCreateParams;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)lpParams);
        pParams = lpParams;
    }
    else
    {
        pParams = (_TextEditBoxParams*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    }

    const bool& mult = pParams->mult;
    HWND& hw_edit = pParams->hw_edit;
    HWND& hw_btn = pParams->hw_btn;

    switch (msg)
    {
    case WM_CREATE:
    {
        hw_edit = CreateWindowExW(NULL, L"EDIT", L"", mult ? (WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL) : (WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL), 1, 1, 1, 1, hwnd, (HMENU)edit_id, pParams->instance, NULL);
        hw_btn = CreateWindowExW(NULL, L"BUTTON", L"确认", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 1, 1, 1, 1, hwnd, (HMENU)button_id, pParams->instance, NULL);
        pParams->font = CreateFontW(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Microsoft YaHei");
        SendMessageW(hw_edit, WM_SETFONT, (WPARAM)pParams->font, TRUE);
        SendMessageW(hw_btn, WM_SETFONT, (WPARAM)pParams->font, TRUE);
        SetWindowPos(hwnd, NULL, 0, 0, mult ? TEXTEDITBOX_MULT_WIDTH : TEXTEDITBOX_SING_WIDTH, mult ? TEXTEDITBOX_MULT_HEIGHT : TEXTEDITBOX_SING_HEIGHT, SWP_NOMOVE | SWP_NOZORDER);
        break;
    }
    case WM_GETMINMAXINFO:
    {
        if (pParams)
        {
            MINMAXINFO* mmi = (MINMAXINFO*)lParam;
            mmi->ptMinTrackSize.x = 300;
            mmi->ptMinTrackSize.y = mult ? 200 : 150;
            if (!mult) mmi->ptMaxTrackSize.y = 150;
        }
        break;
    }
    case WM_SIZE:
    {
        RECT client_rect;
        GetClientRect(hwnd, &client_rect);
        SetWindowPos(hw_edit, NULL, margin, margin, client_rect.right - margin * 2, mult ? (client_rect.bottom - button_height - margin * 3) : edit_height, SWP_NOZORDER);
        SetWindowPos(hw_btn, NULL, client_rect.right - button_width - margin, client_rect.bottom - button_height - margin, button_width, button_height, SWP_NOZORDER);
        break;
    }
    case WM_SETTEXT:
    {
        SetWindowTextW(hw_edit, (LPCWSTR)lParam);
        return 0;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == button_id)
        {
            wchar_t buffer[4096];
            GetWindowTextW(hw_edit, buffer, 4096);
            *(pParams->result) = buffer;
            DestroyWindow(hwnd);
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        _TextEditBoxParams* params = (_TextEditBoxParams*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
        DeleteObject(params->font);
        PostQuitMessage(0);
        break;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static std::wstring TextEditBox(HWND parent, const wchar_t* title, const wchar_t* text = nullptr, bool multiLine = false, DWORD exStyle = WS_EX_TOPMOST, HICON icon = nullptr)
{
    _TextEditBoxParams params;
    std::wstring result;
    if (text) result = text;
    params.instance = GetModuleHandleW(NULL);
    params.mult = multiLine;
    params.result = &result;

    WNDCLASSEXW wc = { sizeof(WNDCLASSEX) };
    wc.hIcon = icon;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpszClassName = L"TextEditBox";
    wc.lpfnWndProc = _TextEditBoxProc;
    wc.hInstance = params.instance;
    RegisterClassExW(&wc);

    SIZE size = { multiLine ? TEXTEDITBOX_MULT_WIDTH : TEXTEDITBOX_SING_WIDTH, multiLine ? TEXTEDITBOX_MULT_HEIGHT : TEXTEDITBOX_SING_HEIGHT };
    HWND hwnd = CreateWindowExW(exStyle, L"TextEditBox", title, WS_SYSMENU | WS_MAXIMIZEBOX | WS_THICKFRAME, (GetSystemMetrics(SM_CXSCREEN) >> 1) - (size.cx >> 1), (GetSystemMetrics(SM_CYSCREEN) >> 1) - (size.cy >> 1), size.cx, size.cy, parent, NULL, wc.hInstance, &params);
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    if (text) SendMessageW(hwnd, WM_SETTEXT, wcslen(text), (LPARAM)text);

    if (parent && IsWindow(parent)) EnableWindow(parent, FALSE), SetForegroundWindow(parent);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (parent && IsWindow(parent)) EnableWindow(parent, TRUE);

    return result;
}