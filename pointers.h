/// <summary>
/// This file contains pointers of hookable functions.
/// </summary>

int(WINAPI* oDrawTextW) (HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format) = DrawTextW;
HFONT(WINAPI* oCreateFontIndirectW) (const LOGFONTW* lplf) = CreateFontIndirectW;
HBRUSH(WINAPI* oCreateSolidBrush) (COLORREF color) = CreateSolidBrush;
HWND(WINAPI* oCreateWindowExW) (DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) = CreateWindowExW;
BOOL(WINAPI* oSetWindowTextW) (HWND hWnd, LPCWSTR lpString) = SetWindowTextW;
HBITMAP(WINAPI* oCreateDIBitmap) (HDC hdc, const BITMAPINFOHEADER* pbmih, DWORD flInit, const VOID* pjBits, const BITMAPINFO* pbmi, UINT iUsage) = CreateDIBitmap;
HBITMAP(WINAPI* oCreateBitmap) (int nWidth,int nHeight,UINT nPlanes,UINT nBitCount,const VOID* lpBits) = CreateBitmap;
ATOM(WINAPI* oRegisterClassExW) (const WNDCLASSEXW* unnamedParam1) = RegisterClassExW;
BOOL(WINAPI* oAppendMenuW) (HMENU hMenu, UINT uFlags, UINT_PTR uIDNewItem, LPCWSTR lpNewItem) = AppendMenuW;
BOOL(WINAPI* oSetMenuInfo) (HMENU unnamedParam1, LPCMENUINFO unnamedParam2) = SetMenuInfo;
BOOL(WINAPI* oSetMenuItemInfoW) (HMENU hmenu, UINT item, BOOL fByPositon, LPCMENUITEMINFOW lpmii) = SetMenuItemInfoW;
BOOL(WINAPI* oInsertMenuItemW) (HMENU hmenu, UINT item, BOOL fByPositon,LPCMENUITEMINFOW lpmi) = InsertMenuItemW;
COLORREF(WINAPI* oSetBkColor) (HDC hdc, COLORREF color) = SetBkColor;
int(WINAPI* oSetWindowRgn) (HWND hWnd, HRGN hRgn, BOOL bRedraw) = SetWindowRgn;
HDC(WINAPI* oBeginPaint) (HWND hWnd, LPPAINTSTRUCT lpPaint) = BeginPaint;
BOOL(WINAPI* oEndPaint) (HWND hWnd, const PAINTSTRUCT* lpPaint) = EndPaint;
int(WINAPI* oFillRect) (HDC hDC,const RECT* lprc, HBRUSH hbr) = FillRect;
int(WINAPI* oSetDIBitsToDevice) (HDC hdc, int xDest, int yDest, DWORD w, DWORD h, int xSrc, int ySrc, UINT StartScan, UINT cLines, const VOID* lpvBits, const BITMAPINFO* lpbmi, UINT ColorUse) = SetDIBitsToDevice;
BOOL(WINAPI* oBitBlt) (HDC hdc, int x, int y, int cx, int cy, HDC hdcSrc, int x1, int y1, DWORD rop) = BitBlt;
BOOL(WINAPI* oExtTextOutW) (HDC hdc, int x, int y, UINT options, const RECT* lprect, LPCWSTR lpString, UINT c, const INT* lpDx) = ExtTextOutW;
BOOL(WINAPI* oGetTextMetricsW) (HDC hdc, LPTEXTMETRICW lptm) = GetTextMetricsW;
HRGN(WINAPI* oCreateRectRgn) (int x1,int y1,int x2,int y2) = CreateRectRgn;