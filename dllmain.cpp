#include "pch.h"
#include <windows.h>
#include <detours.h>
#include <windef.h>
#include <winuser.h>
#include <iostream>
#include <string>
#include <fstream>

// --- Theme definitions [ TODO: remove later and read them from a config.json file ] ---
static COLORREF s2cePrimary	= RGB(30,  30,  30);
static COLORREF s2ceText	= RGB(100, 100, 200);
static LPCWSTR  s2ceFont	= L"ProggyCleanTT"; // font family name
static long s2ceFontSizeOffset = 5; // example: 14px + s2ceFontSizeOffset = 19px, something like that

// --- Functions' pointers ---

static int(WINAPI* oDrawTextW) (HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format) = DrawTextW;

static HFONT(WINAPI* oCreateFontIndirectW) (const LOGFONTW* lplf) = CreateFontIndirectW;

static HBRUSH(WINAPI* oCreateSolidBrush) (COLORREF color) = CreateSolidBrush;

static HWND(WINAPI* oCreateWindowExW) (DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) = CreateWindowExW;

static BOOL(WINAPI* oSetWindowTextW) (HWND hWnd, LPCWSTR lpString) = SetWindowTextW;

static BOOL(WINAPI* oBitBlt) (HDC hdc, int x, int y, int cx, int cy, HDC hdcSrc, int x1, int y1, DWORD rop) = BitBlt;

static HBITMAP(WINAPI* oCreateDIBitmap) (HDC hdc, const BITMAPINFOHEADER* pbmih, DWORD flInit, const VOID* pjBits, const BITMAPINFO* pbmi,UINT iUsage) = CreateDIBitmap;

// --- Extensions ---

/**
Clears `.log` file.
*/
static void cls_log() {
	std::fstream log(".log", std::ofstream::out | std::ofstream::trunc);
	log.close();
}
/**
Logs some data into a `.log` file.
*/
static void log(std::string data) {
	std::fstream log(".log", std::ios::app);
	log << data;
	log << "\n";
	log.close();
}

/**
Converts `LPCWSTR` type to a `string` type.
*/
static std::string LPCWSTRtoSTR(LPCWSTR lpcwszStr)
{
	int strLength = WideCharToMultiByte(CP_UTF8, 0, lpcwszStr, -1, nullptr, 0, nullptr, nullptr);
	std::string str(strLength, 0);
	WideCharToMultiByte(CP_UTF8, 0, lpcwszStr, -1, &str[0], strLength, nullptr, nullptr);
	return str;
}

/**
Creates filled bitmap rectangle.
*/
static HBITMAP CreateFilledBitmap(int width, int height, COLORREF c) {
	HBITMAP hBitmap = CreateBitmap(width, height, 1, 64, NULL);
	HDC		hdc		= CreateCompatibleDC(NULL);

	SelectObject(hdc, hBitmap);
	
	HBRUSH hBrush	= CreateSolidBrush(c);
	RECT   rect		= { 0, 0, width, height };
	
	FillRect(hdc, &rect, hBrush);

	DeleteObject(hBrush);
	DeleteDC(hdc);

	return hBitmap;
}

// --- Experiments ---

//BOOL hBitBlt(
//	HDC   hdc,
//	int   x,
//	int   y,
//	int   cx,
//	int   cy,
//	HDC   hdcSrc,
//	int   x1,
//	int   y1,
//	DWORD rop
//) {
	//DeleteDC(hdcSrc);
	//HBITMAP hbm = CreateFilledBitmap(cx, cy, RGB(0, 0, 255));
	//HDC hdcMem = CreateCompatibleDC(hdc);
	//SelectObject(hdcMem, hbm);
	//return oBitBlt(hdc, 20, 20, 20, 20, hdcSrc, 20, 20, rop);
//}
//HBITMAP hCreateDIBitmap(
//	HDC                    hdc,
//	const BITMAPINFOHEADER* pbmih,
//	DWORD                  flInit,
//	const VOID* pjBits,
//	const BITMAPINFO* pbmi,
//	UINT                   iUsage
//) {
//	BITMAPINFO bmi = BITMAPINFO();
//
//	bmi.bmiHeader = pbmi->bmiHeader;
//	BITMAPINFO* _pbmi = &bmi;
//
//	for (int i = 0; i < 256; i++) {
//		_pbmi->bmiColors[i].rgbRed = (BYTE)i;
//		_pbmi->bmiColors[i].rgbGreen = (BYTE)i;
//		_pbmi->bmiColors[i].rgbBlue = (BYTE)i;
//		_pbmi->bmiColors[i].rgbReserved = (BYTE)0;
//	}
//
//
//	return oCreateDIBitmap(hdc, pbmih, flInit, NULL, _pbmi, iUsage);
//}


// --- Functions' overrides ---

/**
Hooks color of all non-bitmap elements.
*/
HBRUSH hCreateSolidBrush(COLORREF color) {
	// update main colors of application
	if (color == 0x00D0D0D0 || color == 0x00F8F8F8) {
		color = s2cePrimary;
	}

	// keep other colors unchanged
	return oCreateSolidBrush(color);
}

/** 
Hooks text color in application.

TODO:
	Add a check of which text color is selected by default 
	to prevent injector from re-coloring the whole text elements at once.
*/ 
int hDrawTextW(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format) {
	// update text color before rendering it
	SetTextColor(hdc, s2ceText);
	return oDrawTextW(hdc, lpchText, cchText, lprc, format);
}

/**
Hooks font family and size.
*/
HFONT hCreateFontIndirectW(const LOGFONTW* lplf) {
	// replace only the GUI font
	if (LPCWSTRtoSTR(lplf->lfFaceName).find("Tahoma") != std::string::npos) {
		return CreateFontW(
			lplf->lfHeight - s2ceFontSizeOffset, // decreasing the height value => font goes bigger (idk why its like that)
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
			s2ceFont
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
	if (LPCWSTRtoSTR(lpWindowName).find("PaintTool SAI") != std::string::npos) {
		lpWindowName = L"PaintTool SAI 2 (Colored Edition)";
	}

	return oCreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}
BOOL hSetWindowTextW(HWND hWnd, LPCWSTR lpString) {

	// replace a window title containing software name with a custom one
	if (LPCWSTRtoSTR(lpString).find("PaintTool SAI") != std::string::npos) {
		lpString = L"PaintTool SAI 2 (Colored Edition)";
	}

	return oSetWindowTextW(hWnd, lpString);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  dwReason, LPVOID lpReserved)
{
	if (DetourIsHelperProcess()) {
		return TRUE;
	}

	if (dwReason == DLL_PROCESS_ATTACH) {
		cls_log(); // clear old log data
		log("[INFO] s2ce.dll has been injected."); // log "welcome" message

		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		
		//DetourAttach(&(PVOID&)oCreateSolidBrush, hCreateSolidBrush);
		//DetourAttach(&(PVOID&)oDrawTextW, hDrawTextW);
		DetourAttach(&(PVOID&)oCreateFontIndirectW, hCreateFontIndirectW);
		DetourAttach(&(PVOID&)oCreateWindowExW, hCreateWindowExW);
		DetourAttach(&(PVOID&)oSetWindowTextW, hSetWindowTextW);

		DetourTransactionCommit();
	}
	else if (dwReason == DLL_PROCESS_DETACH) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		//DetourDetach(&(PVOID&)oCreateSolidBrush, hCreateSolidBrush);
		//DetourDetach(&(PVOID&)oDrawTextW, hDrawTextW);
		DetourDetach(&(PVOID&)oCreateFontIndirectW, hCreateFontIndirectW);
		DetourDetach(&(PVOID&)oCreateWindowExW, hCreateWindowExW);
		DetourDetach(&(PVOID&)oSetWindowTextW, hSetWindowTextW);

		DetourTransactionCommit();
	}

	return TRUE;
}