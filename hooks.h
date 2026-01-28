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


HWND current_hwnd { 0 };
HDC current_hdc { 0 };
RECT current_rect { 0 };
RECT menu_lprect { 0 }; // try to get a menu item rectangle for this. like.. hook something like CreateRect() for this.
HBITMAP cur_hbitmap{ 0 };
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
	HBITMAP hbm = oCreateDIBitmap(hdc, pbmih, flInit, pjBits, pbmi, iUsage);
	return hbm;
}
HBITMAP hCreateBitmap(int nWidth, int nHeight, UINT nPlanes, UINT nBitCount, const VOID* lpBits) {
	HBITMAP hbm = oCreateBitmap(nWidth, nHeight, nPlanes, nBitCount, lpBits);
	return hbm;
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
	current_hdc = lpPaint->hdc;
	current_rect = lpPaint->rcPaint;

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
	// COLORS IN BITS RECEIVED, BUT REPLACEMENT IS... JUNKY.

	if (w >= 164 && w == h) { // hook only color circle, since its bitmap is big and has a square shape (i.e. unique)
		for (int row = 0; row < h; row++) {
			for (int col = 0; col < w; col++) {

				//for x32 bitmaps:
				int index = (row * w + col) * 4;
				
				// LAST MESSAGE BEFORE BED !!!!!!!!
				// maybe try to handle other methods where lpvBits exist
				// я тут про те методы, которые создают картинку, а не применяют ее
				// какой-нибудь типа createBitmap()
				// что всасывает биты как раз
				// проверять в таком методе наличие f8f8f8 битов и менять как тут ниже.
				// возможно это устранит гличи, которые в этом хуке появляются.
				// всё-таки этот метод затрагивает тексты и иконы, чего может и не быть в других методах битмапов.
				// 
				// список методов для работы с битмапами есть на сайте майкрософта. Bitmap Functions раздел называется.
				
				// override color circle with a custom fill
				((BYTE*)lpvBits)[index + 0] = GetBValue(main_color); // b
				((BYTE*)lpvBits)[index + 1] = GetGValue(main_color); // g
				((BYTE*)lpvBits)[index + 2] = GetRValue(main_color); // r
			}
		}
	}

	int callback = oSetDIBitsToDevice(hdc, xDest, yDest, w, h, xSrc, ySrc, StartScan, cLines, lpvBits, lpbmi, ColorUse);

	return callback;
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

	//DeleteDC(_hdc);
	//DeleteObject(_hbr);

	return oExtTextOutW(hdc, x, y, options, lprect, lpString, c, lpDx);
}
BOOL hGetTextMetricsW(HDC hdc, LPTEXTMETRICW lptm) {
	return oGetTextMetricsW(hdc, lptm);
}
HRGN hCreateRectRgn(int x1, int y1, int x2, int y2) {
	return oCreateRectRgn(x1, y1, x2, y2);
}