// PickFont.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "PickFont.h"
#include <commdlg.h>

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


// Structure shared between main window and dialog box
typedef struct
{
	int iDevice, iMapMode;
	BOOL fMatchAspect;
	BOOL fAdvGraphics;
	LOGFONT lf;
	TEXTMETRIC tm;
	TCHAR szFaceName[LF_FULLFACESIZE];
}
DLGPARAMS;

// Formatting for BCHAR fields of TEXTMETRIC structure
#ifdef UNICODE
#define BCHARFORM TEXT ("0x%04X")
#else
#define BCHARFORM TEXT ("0x%02X")
#endif

// Global variables
HWND hdlg;
TCHAR szAppName[] = TEXT("PickFont");

// Forward declarations of functions
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
void SetLogFontFromFields(HWND hdlg, DLGPARAMS * pdp);
void SetFieldsFromTextMetric(HWND hdlg, DLGPARAMS * pdp);
void MySetMapMode(HDC hdc, int iMapMode);


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
    LoadStringW(hInstance, IDC_PICKFONT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PICKFONT));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PICKFONT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PICKFONT);
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
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static DLGPARAMS dp;
	static TCHAR szText[] = TEXT("\x41\x42\x43\x44\x45 ")
		TEXT("\x61\x62\x63\x64\x65 ")
		TEXT("\xC0\xC1\xC2\xC3\xC4\xC5 ")
		TEXT("\xE0\xE1\xE2\xE3\xE4\xE5 ")
#ifdef UNICODE
		TEXT("\x0390\x0391\x0392\x0393\x0394\x0395 ")
		TEXT("\x03B0\x03B1\x03B2\x03B3\x03B4\x03B5 ")
		TEXT("\x0410\x0411\x0412\x0413\x0414\x0415 ")
		TEXT("\x0430\x0431\x0432\x0433\x0434\x0435 ")
		TEXT("\x5000\x5001\x5002\x5003\x5004")
#endif
		;
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;

	switch (message)
	{
	case WM_CREATE:
		dp.iDevice = IDM_DEVICE_SCREEN;
		hdlg = CreateDialogParam(((LPCREATESTRUCT)lParam)->hInstance, szWindowClass, hwnd, DlgProc, (LPARAM)&dp);
		return 0;
	case WM_SETFOCUS:
		SetFocus(hdlg);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_DEVICE_SCREEN:
		case IDM_DEVICE_PRINTER:
			CheckMenuItem(GetMenu(hwnd), dp.iDevice, MF_UNCHECKED);
			dp.iDevice = LOWORD(wParam);
			CheckMenuItem(GetMenu(hwnd), dp.iDevice, MF_CHECKED);
			SendMessage(hwnd, WM_COMMAND, IDOK, 0);
			return 0;
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		// Set graphics mode so escapement works in Windows NT
		SetGraphicsMode(hdc, dp.fAdvGraphics ? GM_ADVANCED : GM_COMPATIBLE);
		// Set the mapping mode and the mapper flag
		MySetMapMode(hdc, dp.iMapMode);
		SetMapperFlags(hdc, dp.fMatchAspect);
		// Find the point to begin drawing text
		GetClientRect(hdlg, &rect);
		rect.bottom += 1;
		DPtoLP(hdc, (PPOINT)&rect, 2);
		// Create and select the font; display the text
		SelectObject(hdc, CreateFontIndirect(&dp.lf));
		TextOut(hdc, rect.left, rect.bottom, szText, lstrlen(szText));
		DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
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

BOOL CALLBACK DlgProc(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static DLGPARAMS * pdp;
	static PRINTDLG pd = { sizeof(PRINTDLG) };
	HDC hdcDevice;
	HFONT hFont;

	switch (message)
	{
	case WM_INITDIALOG:
		// Save pointer to dialog-parameters structure in WndProc
		pdp = (DLGPARAMS *)lParam;
		SendDlgItemMessage(hdlg, IDC_LF_FACENAME, EM_LIMITTEXT, LF_FACESIZE - 1, 0);
		CheckRadioButton(hdlg, IDC_OUT_DEFAULT, IDC_OUT_OUTLINE, IDC_OUT_DEFAULT);
		CheckRadioButton(hdlg, IDC_DEFAULT_QUALITY, IDC_PROOF_QUALITY, IDC_DEFAULT_QUALITY);
		CheckRadioButton(hdlg, IDC_DEFAULT_PITCH, IDC_VARIABLE_PITCH, IDC_DEFAULT_PITCH);
		CheckRadioButton(hdlg, IDC_FF_DONTCARE, IDC_FF_DECORATIVE, IDC_FF_DONTCARE);
		CheckRadioButton(hdlg, IDC_MM_TEXT, IDC_MM_LOGTWIPS, IDC_MM_TEXT);
		SendMessage(hdlg, WM_COMMAND, IDOK, 0);
		// fall through
	case WM_SETFOCUS:
		SetFocus(GetDlgItem(hdlg, IDC_LF_HEIGHT));
		return FALSE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_CHARSET_HELP:
			MessageBox(hdlg,
				TEXT("0 = Ansi\n")
				TEXT("1 = Default\n")
				TEXT("2 = Symbol\n")
				TEXT("128 = Shift JIS (Japanese)\n")
				TEXT("129 = Hangul (Korean)\n")
				TEXT("130 = Johab (Korean)\n")
				TEXT("134 = GB 2312 (Simplified Chinese)\n")
				TEXT("136 = Chinese Big 5 (Traditional Chinese)\n")
				TEXT("177 = Hebrew\n")
				TEXT("178 = Arabic\n")
				TEXT("161 = Greek\n")
				TEXT("162 = Turkish\n")
				TEXT("163 = Vietnamese\n")
				TEXT("204 = Russian\n")
				TEXT("222 = Thai\n")
				TEXT("238 = East European\n")
				TEXT("255 = OEM"),
				szWindowClass, MB_OK | MB_ICONINFORMATION);
			return TRUE;
		// These radio buttons set the lfOutPrecision field
		case IDC_OUT_DEFAULT:
			pdp->lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
			return TRUE;
		case IDC_OUT_STRING:
			pdp->lf.lfOutPrecision = OUT_STRING_PRECIS;
			return TRUE;
		case IDC_OUT_CHARACTER:
			pdp->lf.lfOutPrecision = OUT_CHARACTER_PRECIS;
			return TRUE;
		case IDC_OUT_STROKE:
			pdp->lf.lfOutPrecision = OUT_STROKE_PRECIS;
			return TRUE;
		case IDC_OUT_TT:
			pdp->lf.lfOutPrecision = OUT_TT_PRECIS;
			return TRUE;
		case IDC_OUT_DEVICE:
			pdp->lf.lfOutPrecision = OUT_DEVICE_PRECIS;
			return TRUE;
		case IDC_OUT_RASTER:
			pdp->lf.lfOutPrecision = OUT_RASTER_PRECIS;
			return TRUE;
		case IDC_OUT_TT_ONLY:
			pdp->lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;
			return TRUE;
		case IDC_OUT_OUTLINE:
			pdp->lf.lfOutPrecision = OUT_OUTLINE_PRECIS;
			return TRUE;
		// These three radio buttons set the lfQuality field
		case IDC_DEFAULT_QUALITY:
			pdp->lf.lfQuality = DEFAULT_QUALITY;
			return TRUE;
		case IDC_DRAFT_QUALITY:
			pdp->lf.lfQuality = DRAFT_QUALITY;
			return TRUE;
		case IDC_PROOF_QUALITY:
			pdp->lf.lfQuality = PROOF_QUALITY;
			return TRUE;
		// These three radio buttons set the lower nibble
		// of the lfPitchAndFamily field
		case IDC_DEFAULT_PITCH:
			pdp->lf.lfPitchAndFamily = (0xF0 & pdp->lf.lfPitchAndFamily) | DEFAULT_PITCH;
			return TRUE;
		case IDC_FIXED_PITCH:
			pdp->lf.lfPitchAndFamily = (0xF0 & pdp->lf.lfPitchAndFamily) | FIXED_PITCH;
			return TRUE;
		case IDC_VARIABLE_PITCH:
			pdp->lf.lfPitchAndFamily = (0xF0 & pdp->lf.lfPitchAndFamily) | VARIABLE_PITCH;
			return TRUE;
		// These six radio buttons set the upper nibble 
		// of the lfPitchAndFamily field
		case IDC_FF_DONTCARE:
			pdp->lf.lfPitchAndFamily = (0x0F & pdp->lf.lfPitchAndFamily) | FF_DONTCARE;
			return TRUE;
		case IDC_FF_ROMAN:
			pdp->lf.lfPitchAndFamily = (0x0F & pdp->lf.lfPitchAndFamily) | FF_ROMAN;
			return TRUE;
		case IDC_FF_SWISS:
			pdp->lf.lfPitchAndFamily = (0x0F & pdp->lf.lfPitchAndFamily) | FF_SWISS;
			return TRUE;
		case IDC_FF_MODERN:
			pdp->lf.lfPitchAndFamily = (0x0F & pdp->lf.lfPitchAndFamily) | FF_MODERN;
			return TRUE;
		case IDC_FF_SCRIPT:
			pdp->lf.lfPitchAndFamily = (0x0F & pdp->lf.lfPitchAndFamily) | FF_SCRIPT;
			return TRUE;
		case IDC_FF_DECORATIVE:
			pdp->lf.lfPitchAndFamily = (0x0F & pdp->lf.lfPitchAndFamily) | FF_DECORATIVE;
			return TRUE;
		// Mapping mode:
		case IDC_MM_TEXT:
		case IDC_MM_LOMETRIC:
		case IDC_MM_HIMETRIC:
		case IDC_MM_LOENGLISH:
		case IDC_MM_HIENGLISH:
		case IDC_MM_TWIPS:
		case IDC_MM_LOGTWIPS:
			pdp->iMapMode = LOWORD(wParam);
			return TRUE;
		// OK button pressed
		// -----------------
		case IDOK:
			// Get LOGFONT structure
			SetLogFontFromFields(hdlg, pdp);
			// Set Match-Aspect and Advanced Graphics flags
			pdp->fMatchAspect = IsDlgButtonChecked(hdlg, IDC_MATCH_ASPECT);
			pdp->fAdvGraphics = IsDlgButtonChecked(hdlg, IDC_ADV_GRAPHICS);
			// Get Information Context
			if (pdp->iDevice == IDM_DEVICE_SCREEN)
			{
				hdcDevice = CreateIC(TEXT("DISPLAY"), NULL, NULL, NULL);
			}
			else
			{
				pd.hwndOwner = hdlg;
				pd.Flags = PD_RETURNDEFAULT | PD_RETURNIC;
				pd.hDevNames = NULL;
				pd.hDevMode = NULL;
				PrintDlg(&pd);
				hdcDevice = pd.hDC;
			}
			// Set the mapping mode and the mapper flag
			MySetMapMode(hdcDevice, pdp->iMapMode);
			SetMapperFlags(hdcDevice, pdp->fMatchAspect);
			// Create font and select it into IC
			hFont = CreateFontIndirect(&pdp->lf);
			SelectObject(hdcDevice, hFont);
			// Get the text metrics and face name
			GetTextMetrics(hdcDevice, &pdp->tm);
			GetTextFace(hdcDevice, LF_FULLFACESIZE, pdp->szFaceName);
			DeleteDC(hdcDevice);
			DeleteObject(hFont);
			// Update dialog fields and invalidate main window
			SetFieldsFromTextMetric(hdlg, pdp);
			InvalidateRect(GetParent(hdlg), NULL, TRUE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}
void SetLogFontFromFields(HWND hdlg, DLGPARAMS * pdp)
{
	pdp->lf.lfHeight = GetDlgItemInt(hdlg, IDC_LF_HEIGHT, NULL, TRUE);
	pdp->lf.lfWidth = GetDlgItemInt(hdlg, IDC_LF_WIDTH, NULL, TRUE);
	pdp->lf.lfEscapement = GetDlgItemInt(hdlg, IDC_LF_ESCAPE, NULL, TRUE);
	pdp->lf.lfOrientation = GetDlgItemInt(hdlg, IDC_LF_ORIENT, NULL, TRUE);
	pdp->lf.lfWeight = GetDlgItemInt(hdlg, IDC_LF_WEIGHT, NULL, TRUE);
	pdp->lf.lfCharSet = GetDlgItemInt(hdlg, IDC_LF_CHARSET, NULL, FALSE);
	pdp->lf.lfItalic = IsDlgButtonChecked(hdlg, IDC_LF_ITALIC) == BST_CHECKED;
	pdp->lf.lfUnderline = IsDlgButtonChecked(hdlg, IDC_LF_UNDER) == BST_CHECKED;
	pdp->lf.lfStrikeOut = IsDlgButtonChecked(hdlg, IDC_LF_STRIKE) == BST_CHECKED;
	GetDlgItemText(hdlg, IDC_LF_FACENAME, pdp->lf.lfFaceName, LF_FACESIZE);
}
void SetFieldsFromTextMetric(HWND hdlg, DLGPARAMS * pdp)
{
	TCHAR szBuffer[10];
	const TCHAR * szYes = TEXT("Yes");
	const TCHAR * szNo = TEXT("No");
	const TCHAR * szFamily[] = { TEXT("Don't Know"), TEXT("Roman"), TEXT("Swiss"), TEXT("Modern"), TEXT("Script"), TEXT("Decorative"), TEXT("Undefined") };
	SetDlgItemInt(hdlg, IDC_TM_HEIGHT, pdp->tm.tmHeight, TRUE);
	SetDlgItemInt(hdlg, IDC_TM_ASCENT, pdp->tm.tmAscent, TRUE);
	SetDlgItemInt(hdlg, IDC_TM_DESCENT, pdp->tm.tmDescent, TRUE);
	SetDlgItemInt(hdlg, IDC_TM_INTLEAD, pdp->tm.tmInternalLeading, TRUE);
	SetDlgItemInt(hdlg, IDC_TM_EXTLEAD, pdp->tm.tmExternalLeading, TRUE);
	SetDlgItemInt(hdlg, IDC_TM_AVECHAR, pdp->tm.tmAveCharWidth, TRUE);
	SetDlgItemInt(hdlg, IDC_TM_MAXCHAR, pdp->tm.tmMaxCharWidth, TRUE);
	SetDlgItemInt(hdlg, IDC_TM_WEIGHT, pdp->tm.tmWeight, TRUE);
	SetDlgItemInt(hdlg, IDC_TM_OVERHANG, pdp->tm.tmOverhang, TRUE);
	SetDlgItemInt(hdlg, IDC_TM_DIGASPX, pdp->tm.tmDigitizedAspectX, TRUE);
	SetDlgItemInt(hdlg, IDC_TM_DIGASPY, pdp->tm.tmDigitizedAspectY, TRUE);
	wsprintf(szBuffer, BCHARFORM, pdp->tm.tmFirstChar);
	SetDlgItemText(hdlg, IDC_TM_FIRSTCHAR, szBuffer);
	wsprintf(szBuffer, BCHARFORM, pdp->tm.tmLastChar);
	SetDlgItemText(hdlg, IDC_TM_LASTCHAR, szBuffer);
	wsprintf(szBuffer, BCHARFORM, pdp->tm.tmDefaultChar);
	SetDlgItemText(hdlg, IDC_TM_DEFCHAR, szBuffer);
	wsprintf(szBuffer, BCHARFORM, pdp->tm.tmBreakChar);
	SetDlgItemText(hdlg, IDC_TM_BREAKCHAR, szBuffer);
	SetDlgItemText(hdlg, IDC_TM_ITALIC, pdp->tm.tmItalic ? szYes : szNo);
	SetDlgItemText(hdlg, IDC_TM_UNDER, pdp->tm.tmUnderlined ? szYes : szNo);
	SetDlgItemText(hdlg, IDC_TM_STRUCK, pdp->tm.tmStruckOut ? szYes : szNo);
	SetDlgItemText(hdlg, IDC_TM_VARIABLE, TMPF_FIXED_PITCH & pdp->tm.tmPitchAndFamily ? szYes : szNo);
	SetDlgItemText(hdlg, IDC_TM_VECTOR, TMPF_VECTOR & pdp->tm.tmPitchAndFamily ? szYes : szNo);
	SetDlgItemText(hdlg, IDC_TM_TRUETYPE, TMPF_TRUETYPE & pdp->tm.tmPitchAndFamily ? szYes : szNo);
	SetDlgItemText(hdlg, IDC_TM_DEVICE, TMPF_DEVICE & pdp->tm.tmPitchAndFamily ? szYes : szNo);
	SetDlgItemText(hdlg, IDC_TM_FAMILY, szFamily[min(6, pdp->tm.tmPitchAndFamily >> 4)]);
	SetDlgItemInt(hdlg, IDC_TM_CHARSET, pdp->tm.tmCharSet, FALSE);
	SetDlgItemText(hdlg, IDC_TM_FACENAME, pdp->szFaceName);
}
void MySetMapMode(HDC hdc, int iMapMode)
{
	switch (iMapMode)
	{
	case IDC_MM_TEXT: SetMapMode(hdc, MM_TEXT); break;
	case IDC_MM_LOMETRIC: SetMapMode(hdc, MM_LOMETRIC); break;
	case IDC_MM_HIMETRIC: SetMapMode(hdc, MM_HIMETRIC); break;
	case IDC_MM_LOENGLISH: SetMapMode(hdc, MM_LOENGLISH); break;
	case IDC_MM_HIENGLISH: SetMapMode(hdc, MM_HIENGLISH); break;
	case IDC_MM_TWIPS: SetMapMode(hdc, MM_TWIPS);
		break;
	case IDC_MM_LOGTWIPS:
		SetMapMode(hdc, MM_ANISOTROPIC);
		SetWindowExtEx(hdc, 1440, 1440, NULL);
		SetViewportExtEx(hdc, GetDeviceCaps(hdc, LOGPIXELSX), GetDeviceCaps(hdc, LOGPIXELSY), NULL);
		break;
	}
}


