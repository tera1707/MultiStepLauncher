#include "framework.h"
#include "GlobalHookCpp.h"

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

HHOOK g_hMouseHook = nullptr;                   // グローバル低レベルマウスフック

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ここにコードを挿入してください。

    // グローバル文字列を初期化する
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GLOBALHOOKCPP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    // 他のウィンドウのマウスメッセージ（上下・移動）を取得するための
    // 低レベルマウスフックをインストールします。
    g_hMouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, hInstance, 0);
    if (!g_hMouseHook)
    {
        MessageBoxW(nullptr, L"マウスフックのインストールに失敗しました。", szTitle, MB_OK | MB_ICONWARNING);
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GLOBALHOOKCPP));

    MSG msg;

    // メイン メッセージ ループ:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    if (g_hMouseHook)
    {
        UnhookWindowsHookEx(g_hMouseHook);
        g_hMouseHook = nullptr;
    }

    return (int) msg.wParam;
}

//
//  関数: LowLevelMouseProc(int, WPARAM, LPARAM)
//
//  目的: 他のウィンドウを含むシステム全体のマウスメッセージ（移動・左右ボタンの
//        押下/解放・ホイール）を監視するための低レベルフックプロシージャです。
//
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        MSLLHOOKSTRUCT* pMouse = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
        WCHAR szLog[128];

        switch (wParam)
        {
        case WM_MOUSEMOVE:
            // pMouse->pt にスクリーン座標でのマウス位置が入っています。
            swprintf_s(szLog, L"MouseMove: (%ld, %ld)\n", pMouse->pt.x, pMouse->pt.y);
            OutputDebugStringW(szLog);
            break;
        case WM_LBUTTONDOWN:
            swprintf_s(szLog, L"LButtonDown: (%ld, %ld)\n", pMouse->pt.x, pMouse->pt.y);
            OutputDebugStringW(szLog);
            return 1;
            break;
        case WM_RBUTTONDOWN:
            swprintf_s(szLog, L"RButtonDown: (%ld, %ld)\n", pMouse->pt.x, pMouse->pt.y);
            OutputDebugStringW(szLog);
            break;
        case WM_MBUTTONDOWN:
            swprintf_s(szLog, L"MButtonDown: (%ld, %ld)\n", pMouse->pt.x, pMouse->pt.y);
            OutputDebugStringW(szLog);
            break;
        case WM_LBUTTONUP:
            swprintf_s(szLog, L"LButtonUp: (%ld, %ld)\n", pMouse->pt.x, pMouse->pt.y);
            OutputDebugStringW(szLog);
            break;
        case WM_RBUTTONUP:
            swprintf_s(szLog, L"RButtonUp: (%ld, %ld)\n", pMouse->pt.x, pMouse->pt.y);
            OutputDebugStringW(szLog);
            break;
        case WM_MBUTTONUP:
            swprintf_s(szLog, L"MButtonUp: (%ld, %ld)\n", pMouse->pt.x, pMouse->pt.y);
            OutputDebugStringW(szLog);
            break;
        default:
            break;
        }
    }

    // 必ず次のフックへメッセージを渡します。
    return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GLOBALHOOKCPP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GLOBALHOOKCPP);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 選択されたメニューの解析:
            switch (wmId)
            {
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
