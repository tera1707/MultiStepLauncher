// WindowsProject1.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "WindowsProject1.h"
#include <vector>

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// Raw Input (タッチデジタイザ) 関連
BOOL                RegisterRawTouchInput(HWND hWnd);
void                HandleRawInputTouch(HRAWINPUT hRawInput);

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
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

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

    return (int) msg.wParam;
}



//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
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

   // 他アプリ上のタッチ操作も含め、システム全体のタッチデジタイザ入力を
   // バックグラウンドで監視するために Raw Input を登録します。
   if (!RegisterRawTouchInput(hWnd))
   {
       OutputDebugStringW(L"Raw Input (タッチデジタイザ) の登録に失敗しました。\n");
   }

   return TRUE;
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウを描画する
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//
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
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            BeginPaint(hWnd, &ps);
            // TODO: 描画コードをここに追加してください...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_INPUT:
        // システム全体（他アプリ上も含む）のタッチデジタイザ入力。
        // RIDEV_INPUTSINK によりフォーカスの有無に関わらず受信します。
        HandleRawInputTouch(reinterpret_cast<HRAWINPUT>(lParam));
        return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

//
//  関数: RegisterRawTouchInput(HWND)
//
//  目的: タッチデジタイザ (Usage Page 0x0D / Usage 0x04) を RIDEV_INPUTSINK 付きで
//        登録します。これにより、フォーカスの有無やウィンドウの所有者に関わらず、
//        システム全体のタッチ入力ストリーム（他アプリ上の操作を含む）を
//        このウィンドウで WM_INPUT として受信できます。
//
BOOL RegisterRawTouchInput(HWND hWnd)
{
    RAWINPUTDEVICE rid{};
    rid.usUsagePage = HID_USAGE_PAGE_DIGITIZER;
    rid.usUsage     = HID_USAGE_DIGITIZER_TOUCH_SCREEN;
    rid.dwFlags     = RIDEV_INPUTSINK;
    rid.hwndTarget  = hWnd;

    return RegisterRawInputDevices(&rid, 1, sizeof(rid));
}

//
//  関数: HandleRawInputTouch(HRAWINPUT)
//
//  目的: WM_INPUT で届いた生の HID レポートを解析し、同時接触数 (Contact Count) を
//        取得します。2 点以上の同時タッチを検知した場合にログを出力します。
//
void HandleRawInputTouch(HRAWINPUT hRawInput)
{
    UINT dwSize = 0;

    // 必要なバッファサイズを取得します。
    if (GetRawInputData(hRawInput, RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER)) != 0)
    {
        return;
    }

    if (dwSize == 0)
    {
        return;
    }

    std::vector<BYTE> buffer(dwSize);
    if (GetRawInputData(hRawInput, RID_INPUT, buffer.data(), &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
    {
        return;
    }

    RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(buffer.data());
    if (raw->header.dwType != RIM_TYPEHID)
    {
        return;
    }

    // プリパースドデータ（レポート構造の解析情報）のサイズを取得します。
    UINT dwPreparsedSize = 0;
    if (GetRawInputDeviceInfoW(raw->header.hDevice, RIDI_PREPARSEDDATA, nullptr, &dwPreparsedSize) != 0
        || dwPreparsedSize == 0)
    {
        return;
    }

    std::vector<BYTE> preparsedBuffer(dwPreparsedSize);
    PHIDP_PREPARSED_DATA pPreparsedData = reinterpret_cast<PHIDP_PREPARSED_DATA>(preparsedBuffer.data());
    if (GetRawInputDeviceInfoW(raw->header.hDevice, RIDI_PREPARSEDDATA, pPreparsedData, &dwPreparsedSize)
        != static_cast<UINT>(dwPreparsedSize))
    {
        return;
    }

    HIDP_CAPS caps{};
    if (HidP_GetCaps(pPreparsedData, &caps) != HIDP_STATUS_SUCCESS)
    {
        return;
    }

    // レポート内の全 Value（符号なし整数値を持つ項目）を取得します。
    // タッチデジタイザのレポートには Contact Count（接触数）と
    // Contact Identifier（接触点 ID）が Value として含まれます。
    USHORT valueCapsCount = caps.NumberInputValueCaps;
    if (valueCapsCount == 0)
    {
        return;
    }

    std::vector<HIDP_VALUE_CAPS> valueCaps(valueCapsCount);
    if (HidP_GetValueCaps(HidP_Input, valueCaps.data(), &valueCapsCount, pPreparsedData) != HIDP_STATUS_SUCCESS)
    {
        return;
    }

    BYTE* pReport = raw->data.hid.bRawData;
    ULONG contactCount = 0;
    BOOL  foundContactCount = FALSE;

    for (USHORT i = 0; i < valueCapsCount; i++)
    {
        const HIDP_VALUE_CAPS& vc = valueCaps[i];

        // Digitizer ページの Contact Count Usage (0x54) を探します。
        if (vc.UsagePage == HID_USAGE_PAGE_DIGITIZER && !vc.IsRange && vc.NotRange.Usage == 0x54 /* Contact Count */)
        {
            ULONG value = 0;
            if (HidP_GetUsageValue(HidP_Input, vc.UsagePage, 0, vc.NotRange.Usage, &value, pPreparsedData,
                    reinterpret_cast<PCHAR>(pReport), raw->data.hid.dwSizeHid) == HIDP_STATUS_SUCCESS)
            {
                contactCount = value;
                foundContactCount = TRUE;
            }
            break;
        }
    }

    if (!foundContactCount)
    {
        return;
    }

    WCHAR szLog[128];
    swprintf_s(szLog, L"RawInput Touch ContactCount: %lu\n", contactCount);
    OutputDebugStringW(szLog);

    if (contactCount >= 2)
    {
        OutputDebugStringW(L"2点以上の同時タッチを検知しました。\n");
    }
}

// バージョン情報ボックスのメッセージ ハンドラーです。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
