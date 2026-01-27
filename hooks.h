#include "thememgr.h"

/**
This variable can be used as `get_value()` pointer to store some value as buffer.
I believe this may increase an average performance of dll injection.
*/
std::string buffer = "";

// theme values to be inited once and then accessable
COLORREF main_color		= RGB(0, 0, 0);
COLORREF accent_color	= RGB(0, 0, 0);
COLORREF text_color		= RGB(0, 0, 0);
COLORREF title_color	= RGB(0, 0, 0);
COLORREF header_color	= RGB(0, 0, 0);
float font_size_offset  = 1;
std::string font_family	= "Tahoma";


HWND current_hwnd{ 0 };
LPRECT current_lprect{ 0 };
LPRECT menu_lprect{ 0 }; // try to get a menu item rectangle for this. like.. hook something like CreateRect() for this.

/**
Initializes theme values, reading `.theme` file.
*/
void init_theme() {
	if (ThemeMgr::get_value("main_color", buffer)) {
		main_color = Helper::hex_to_colorref(buffer);
	}

	if (ThemeMgr::get_value("accent_color", buffer)) {
		accent_color = Helper::hex_to_colorref(buffer);
	}

	if (ThemeMgr::get_value("text_color", buffer)) {
		text_color = Helper::hex_to_colorref(buffer);
	}

	if (ThemeMgr::get_value("title_color", buffer)) {
		title_color = Helper::hex_to_colorref(buffer);
	}

	if (ThemeMgr::get_value("header_color", buffer)) {
		header_color = Helper::hex_to_colorref(buffer);
	}

	if (ThemeMgr::get_value("font_size_offset", buffer)) {
		font_size_offset = Helper::str_to_float(buffer);
	}

	if (ThemeMgr::get_value("font_family", buffer)) {
		font_family = buffer;
	}
}

/**
Hooks color of all non-bitmap elements.
*/
HBRUSH hCreateSolidBrush(COLORREF color) {
	// update main colors of application
	if (color == 0x00D0D0D0 || color == 0x00F8F8F8) {
		color = main_color;
	}

	// keep other colors unchanged
	return oCreateSolidBrush(color);
}

/**
Hooks text color in application.
*/
int hDrawTextW(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format) {
	COLORREF orig_col = GetTextColor(hdc);

	// update text color before rendering it
	if (orig_col == 0x00000000) {
		SetTextColor(hdc, text_color);

		//HDC _hdc = GetDC(current_hwnd);
		HBRUSH _hbr = CreateSolidBrush(main_color);

		FillRect(hdc, lprc, _hbr);

		DeleteObject(_hbr);
		//DeleteDC(_hdc);
	}
	if (orig_col == 0x00FFFFFF) {
		SetTextColor(hdc, title_color);
	}
	if (orig_col == 0x00204080) {
		SetTextColor(hdc, header_color);
	}

	return oDrawTextW(hdc, lpchText, cchText, lprc, format);
}

/**
Hooks font family and size.
*/
HFONT hCreateFontIndirectW(const LOGFONTW* lplf) {
	// replace only the GUI font
	if (Helper::lpcwstr_includes(lplf->lfFaceName, "Tahoma")) {
		return CreateFontW(
			long(float(lplf->lfHeight) * font_size_offset),
			lplf->lfWidth,
			lplf->lfEscapement,
			lplf->lfOrientation,
			lplf->lfWeight,
			lplf->lfItalic,
			lplf->lfUnderline,
			lplf->lfStrikeOut,
			lplf->lfCharSet,
			lplf->lfOutPrecision,
			lplf->lfClipPrecision,
			lplf->lfQuality,
			lplf->lfPitchAndFamily,
			Helper::str_to_wstr(font_family).c_str()
		);
	}
	else { // other fonts keep unchanged
		return oCreateFontIndirectW(lplf);
	}
}

/**
Hooks window creation.
*/
HWND hCreateWindowExW(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
	// replace a window title containing software name with a custom one
	if (Helper::lpcwstr_includes(lpWindowName, "PaintTool SAI")) {
		lpWindowName = L"PaintTool SAI 2 (Colored Edition)";
	}

	// keep other data unchanged
	return oCreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

/**
Hooks window title.
*/
BOOL hSetWindowTextW(HWND hWnd, LPCWSTR lpString) {
	// replace a window title containing software name with a custom one
	if (Helper::lpcwstr_includes(lpString, "PaintTool SAI")) {
		lpString = L"PaintTool SAI 2 (Colored Edition)";
	}

	// keep other data unchanged
	return oSetWindowTextW(hWnd, lpString);
}

/**
Hooks window class data such as main bg color, icon, size and etc.
*/
ATOM hRegisterClassExW(const WNDCLASSEXW* unnamedParam1) {
	WNDCLASSEXW wndclass = { sizeof(wndclass) };

	wndclass.cbClsExtra = unnamedParam1->cbClsExtra;
	wndclass.cbSize = unnamedParam1->cbSize;
	wndclass.cbWndExtra = unnamedParam1->cbWndExtra;
	wndclass.hCursor = unnamedParam1->hCursor;
	wndclass.hIcon = unnamedParam1->hIcon;
	wndclass.hIconSm = unnamedParam1->hIconSm;
	wndclass.hInstance = unnamedParam1->hInstance;
	wndclass.lpfnWndProc = unnamedParam1->lpfnWndProc;
	wndclass.lpszClassName = unnamedParam1->lpszClassName;
	wndclass.lpszMenuName = unnamedParam1->lpszMenuName;
	wndclass.style = unnamedParam1->style;

	HBRUSH hb = CreateSolidBrush(main_color);
	wndclass.hbrBackground = hb;
	DeleteObject(hb);
	
	return oRegisterClassExW(&wndclass);
}

// dull
BOOL hAppendMenuW(HMENU hMenu, UINT uFlags, UINT_PTR uIDNewItem, LPCWSTR lpNewItem) {
	return oAppendMenuW(hMenu, uFlags, uIDNewItem, lpNewItem);
}

// useless
BOOL hSetMenuInfo(HMENU unnamedParam1, LPCMENUINFO unnamedParam2) {
	return oSetMenuInfo(unnamedParam1, unnamedParam2);
}

// dull
BOOL hSetMenuItemInfoW(HMENU hmenu, UINT item, BOOL fByPositon, LPCMENUITEMINFOW lpmii) {
	return oSetMenuItemInfoW(hmenu, item, fByPositon, lpmii);
}

// useless unless try to handle LPCMENUITEMINFOW->hbmpItem and other bitmaps (replace them with other ones with custom colors)
BOOL hInsertMenuItemW(HMENU hmenu, UINT item, BOOL fByPositon, LPCMENUITEMINFOW lpmi) {
	return oInsertMenuItemW(hmenu, item, fByPositon, lpmi);
}

// useless (it assigns 0x00FFFFFF by default, which is not needed)
COLORREF hSetBkColor(HDC hdc, COLORREF color) {
	return oSetBkColor(hdc, color);
}

// useless, since it's complicated to modify colors of pjBits (too slow).
HBITMAP hCreateDIBitmap(HDC hdc, const BITMAPINFOHEADER* pbmih, DWORD flInit, const VOID* pjBits, const BITMAPINFO* pbmi, UINT iUsage) {
	/*BITMAPINFO bmi = { sizeof(bmi) };
	bmi.bmiHeader = pbmi->bmiHeader;

	for (int i = 0; i < 256; i++) {
		bmi.bmiColors[i].rgbRed			= (BYTE)i;
		bmi.bmiColors[i].rgbGreen		= (BYTE)i;
		bmi.bmiColors[i].rgbBlue		= (BYTE)i;
		bmi.bmiColors[i].rgbReserved	= (BYTE)0;
	}*/
	return oCreateDIBitmap(hdc, pbmih, flInit, pjBits, pbmi, iUsage);
}

// useless
int hSetWindowRgn(HWND hWnd, HRGN hRgn, BOOL bRedraw) {
	return oSetWindowRgn(hWnd, hRgn, bRedraw);
}

// useless, since win32 draws graphics layer by layer, 
// so it's pointless to draw like a background of an element, 
// before the original background hasn't been drawn yet,
// which will replace the custom one in the end...
HDC hBeginPaint(HWND hWnd, const LPPAINTSTRUCT lpPaint) {
	current_hwnd	= hWnd;
	current_lprect	= &lpPaint->rcPaint;

	return oBeginPaint(hWnd, lpPaint);
}

// TODO:
// an interesting one.
// can be used to overdraw already drawn elements
// with a content specified.
// probably use to fix "menu_items / text" background rendering problem.
BOOL hEndPaint(HWND hWnd, const PAINTSTRUCT* lpPaint) {
	//HDC hdc = GetDC(hWnd);

	//SetBkMode(hdc, OPAQUE);

	//RECT rect = lpPaint->rcPaint;

	//FillRect(hdc, &rect, CreateSolidBrush(RGB(255, 255, 0)));

	//ReleaseDC(hWnd, hdc);
	return oEndPaint(hWnd, lpPaint);
}

/**
Hooks several rectangle elements such as empty canvas background.
*/
int hFillRect(HDC hDC, const RECT* lprc, HBRUSH hbr) {
	if (Helper::hbr_to_colorref(hbr) == 0x00C0C0C0) { // Empty canvas color
		HBRUSH _hbr = CreateSolidBrush(accent_color);

		int callback = oFillRect(hDC, lprc, _hbr);

		DeleteObject(_hbr);

		return callback;
	}

	// TODO:
	// TEST OTHER RECTANGLES TO FIND OPTIMAL THEME VARIATIONS.

	return oFillRect(hDC, lprc, hbr);
}

// usless, since i don't know, how to work with bitmaps rendering
int hSetDIBitsToDevice(HDC hdc, int xDest, int yDest, DWORD w, DWORD h, int xSrc, int ySrc, UINT StartScan, UINT cLines, const VOID* lpvBits, const BITMAPINFO* lpbmi, UINT ColorUse) {
	
	// TODO:
	// lpvBits here defines which color to draw pixels on screen, so if i can modify it,
	// i can probably solve the problem of drawing anything on screen such as:
	// icons, buttons, text bgs, menu bgs, etc..

	return oSetDIBitsToDevice(hdc, xDest, yDest, w, h, xSrc, ySrc, StartScan, cLines, lpvBits, lpbmi, ColorUse);
}

// useless
BOOL hBitBlt(HDC hdc, int x, int y, int cx, int cy, HDC hdcSrc, int x1, int y1, DWORD rop) {
	return oBitBlt(hdc, x, y, cx, cy, hdcSrc, x1, y1, rop);
}

BOOL hExtTextOutW(HDC hdc, int x, int y, UINT options, const RECT* lprect, LPCWSTR lpString, UINT c, const INT* lpDx) {
	//RECT rect{};

	//rect.left = lprect->left;
	//rect.top = lprect->top;
	//rect.right = lprect->right;
	//rect.bottom = lprect->bottom;

	//HDC _hdc = GetDC(current_hwnd);
	//HBRUSH _hbr = CreateSolidBrush(accent_color);

	//FillRect(_hdc, &rect, _hbr);

	//DeleteObject(_hbr);
	//DeleteDC(_hdc);

	return oExtTextOutW(hdc, x, y, options, lprect, lpString, c, lpDx);
}
BOOL hGetTextMetricsW(HDC hdc, LPTEXTMETRICW lptm) {
	return oGetTextMetricsW(hdc, lptm);
}