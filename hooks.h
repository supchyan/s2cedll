#include "thememgr.h"
#include "bmrect.h"

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


HWND current_hwnd { 0 };

HBITMAP old_bm{ 0 };
HDC old_bmhdc{ 0 };
BMRECT old_bmrect{ 0 };
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
	wndclass.hbrBackground = CreateSolidBrush(main_color);
	
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
	//for (int row = 0; row < pbmih->biHeight; row++) {
	//	for (int col = 0; col < pbmih->biWidth; col++) {
	//		int index = (row * pbmih->biWidth + col) * 4;

	//		((BYTE*)pjBits)[index + 0] = GetBValue(main_color); // b
	//		((BYTE*)pjBits)[index + 1] = GetGValue(main_color); // g
	//		((BYTE*)pjBits)[index + 2] = GetRValue(main_color); // r
	//	}
	//}
	//iUsage = DIB_RGB_COLORS;

	//BYTE* _pjBits{ 0 };

	//_pjBits = (BYTE*)pjBits;

	//for (int row = 0; row < pbmih->biHeight; row++) {
	//	for (int col = 0; col < pbmih->biWidth; col++) {
	//		int index = (row * pbmih->biWidth + col) * 4;
	//		//Log::message(Helper::colorref_to_hex(RGB(((BYTE*)pjBits)[index + 2], ((BYTE*)pjBits)[index + 1], ((BYTE*)pjBits)[index + 0])));
	//		_pjBits[index + 0] = GetBValue(main_color); // b
	//		_pjBits[index + 1] = GetGValue(main_color); // g
	//		_pjBits[index + 2] = GetRValue(main_color); // r
	//	}
	//}

	HBITMAP callback = oCreateDIBitmap(hdc, pbmih, flInit, pjBits, pbmi, iUsage);

	old_bm = callback;

	return callback;
}
HBITMAP hCreateBitmap(int nWidth, int nHeight, UINT nPlanes, UINT nBitCount, const VOID* lpBits) {
	//for (int row = 0; row < nHeight; row++) {
	//	for (int col = 0; col < nWidth; col++) {
	//		int index = (row * nWidth + col) * 4;
	//		//Log::message(Helper::colorref_to_hex(RGB(((BYTE*)lpBits)[index + 2], ((BYTE*)lpBits)[index + 1], ((BYTE*)lpBits)[index + 0])));
	//		((BYTE*)lpBits)[index + 0] = 0; // b
	//		((BYTE*)lpBits)[index + 1] = 0; // g
	//		((BYTE*)lpBits)[index + 2] = 0; // r
	//	}
	//}
	//HBITMAP hbm = oCreateBitmap(nWidth, nHeight, nPlanes, nBitCount, lpBits);
	return oCreateBitmap(nWidth, nHeight, nPlanes, nBitCount, lpBits);
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
	current_hwnd = hWnd;

	//HBRUSH _hbr = CreateSolidBrush(main_color);
	//FillRect(current_hdc, &current_rect, _hbr);
	//DeleteObject(_hbr);

	return oBeginPaint(hWnd, lpPaint);
}

// TODO:
// an interesting one.
// can be used to overdraw already drawn elements
// with a content specified.
// probably use to fix "menu_items / text" background rendering problem.
BOOL hEndPaint(HWND hWnd, const PAINTSTRUCT* lpPaint) {
	//BitBlt(old_bmhdc, old_bmrect.x, old_bmrect.y, old_bmrect.w, .5 * old_bmrect.h, old_bmhdc, old_bmrect.x, old_bmrect.y, PATINVERT);
	//if (old_bmhdc != (HDC)0) { // check the existance of bitmap data

	//	

	//	//POINT point[3] = { old_bmrect.y, old_bmrect.h, old_bmrect.x };
	//	//PlgBlt(old_bmhdc, point, old_bmhdc, old_bmrect.x, old_bmrect.y, old_bmrect.w, old_bmrect.h, NULL, 0, 0);

	//	//Log::message(std::to_string((int)old_bmhdc));

	//	old_bmhdc = (HDC)0; // flush data to prevent useless re-drawings
	//}
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
// usless, hooks cursor outline
int hSetDIBits(HDC hdc, HBITMAP hbm, UINT start, UINT cLines, const VOID* lpBits, const BITMAPINFO* lpbmi, UINT ColorUse) {
	return oSetDIBits(hdc, hbm, start, cLines, lpBits, lpbmi, ColorUse);
}
/**
Hooks all bitmap related element, including icons, color circle (bg + ramp), buttons, text and etc.
P.S. Some elements may be repainted after call, so it should probably be handled as well.
*/
int hSetDIBitsToDevice(HDC hdc, int xDest, int yDest, DWORD w, DWORD h, int xSrc, int ySrc, UINT StartScan, UINT cLines, const VOID* lpvBits, const BITMAPINFO* lpbmi, UINT ColorUse) {
	// TODO
	// replace colors of menu items / icons.
	// icons color may be changed just by inverting their bits color with BitBlt().
	// Menu items way more complicated since they have text drawn above,
	// so we cant just "invert" or overpaint the background...
	// for menus it's better to use ExtTextOutW() function like:
	//	change bitmap bits before text is painted over it, 
	//	then call ExtTextOutW() to render a text over changed bits 
	//	and that's it for the most of items.

	int callback = oSetDIBitsToDevice(hdc, xDest, yDest, w, h, xSrc, ySrc, StartScan, cLines, lpvBits, lpbmi, ColorUse);
	
	// replace color circle bg color
	// this method checks if provided data is color circle belonged, 
	// so it's ok here
	ThemeMgr::set_color_circle_col_alt(hdc, w, h, main_color);

	return callback;
}

/** 
Hooks GDI area after cursor (but BitBlt itself can be useful to emulate theme colors via multiple raster-operation codes (like filters for SVG fill in CSS))
*/
BOOL hBitBlt(HDC hdc, int x, int y, int cx, int cy, HDC hdcSrc, int x1, int y1, DWORD rop) {
	return oBitBlt(hdc, x, y, cx, cy, hdcSrc, x1, y1, rop);
}
// useless, hooks text drawing logic
BOOL hExtTextOutW(HDC hdc, int x, int y, UINT options, const RECT* lprect, LPCWSTR lpString, UINT c, const INT* lpDx) {
	return oExtTextOutW(hdc, x, y, options, lprect, lpString, c, lpDx);
}
// useless
BOOL hGetTextMetricsW(HDC hdc, LPTEXTMETRICW lptm) {
	return oGetTextMetricsW(hdc, lptm);
}
// useless
HRGN hCreateRectRgn(int x1, int y1, int x2, int y2) {
	return oCreateRectRgn(x1, y1, x2, y2);
}
/**
Hooks process exit
*/
void hExitProcess(UINT uExitCode) {
	Log::message("[W] Exited with code " + std::to_string(uExitCode));
	return oExitProcess(uExitCode);
}