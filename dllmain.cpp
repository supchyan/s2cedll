#include "pch.h"
#include <windows.h>
#include <detours.h>
#include <windef.h>
#include <winuser.h>
#include <iostream>
#include <string>
#include <fstream>

static COLORREF s2cePrimary = RGB(30,  30,  30 );
static COLORREF s2ceText	= RGB(100, 100, 200);

static int(WINAPI* oDrawTextW) (HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format) = DrawTextW;

HBRUSH(WINAPI* oCreateSolidBrush) (COLORREF color) = CreateSolidBrush;

static BOOL(WINAPI* oBitBlt) (
	HDC   hdc,
	int   x,
	int   y,
	int   cx,
	int   cy,
	HDC   hdcSrc,
	int   x1,
	int   y1,
	DWORD rop
) = BitBlt;

static HBITMAP(WINAPI* oCreateDIBitmap) (
	HDC                    hdc,
	const BITMAPINFOHEADER* pbmih,
	DWORD                  flInit,
	const VOID* pjBits,
	const BITMAPINFO* pbmi,
	UINT                   iUsage
) = CreateDIBitmap;

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
HBITMAP CreateFilledBitmap(int width, int height, COLORREF c) {
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

// This one is cool for non-bitmap depended redrawings
HBRUSH hCreateSolidBrush(COLORREF color) {
	if (color == 0x00D0D0D0 || color == 0x00F8F8F8) {
		color = s2cePrimary;
	}

	return oCreateSolidBrush(color);
}

int hDrawTextW(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format) {
	SetTextColor(hdc, s2ceText);
	return oDrawTextW(hdc, lpchText, cchText, lprc, format);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  dwReason, LPVOID lpReserved)
{
	if (DetourIsHelperProcess()) {
		return TRUE;
	}

	if (dwReason == DLL_PROCESS_ATTACH) {
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		
		DetourAttach(&(PVOID&)oCreateSolidBrush, hCreateSolidBrush);
		DetourAttach(&(PVOID&)oDrawTextW, hDrawTextW);

		DetourTransactionCommit();
	}
	else if (dwReason == DLL_PROCESS_DETACH) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetourDetach(&(PVOID&)oCreateSolidBrush, hCreateSolidBrush);
		DetourDetach(&(PVOID&)oDrawTextW, hDrawTextW);

		DetourTransactionCommit();
	}

	return TRUE;
}