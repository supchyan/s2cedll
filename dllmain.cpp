#include "pch.h"
#include <windows.h>
#include <detours.h>
#include <windef.h>
#include <winuser.h>
#include <iostream>

static COLORREF s2cePrimary = RGB(50, 50, 50);
static COLORREF s2ceText	= RGB(250, 250, 250);

static int(WINAPI* OriginalDrawTextW) (HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format) = DrawTextW;
static int(WINAPI* OriginalFillRect) (HDC hdc, const RECT* lprc, HBRUSH hbr) = FillRect;

// unused
static COLORREF(WINAPI* OriginalSetTextColor) (HDC hdc, COLORREF color) = SetTextColor;
static COLORREF(WINAPI* OriginalSetBkColor) (HDC hdc, COLORREF color) = SetBkColor;
static HBRUSH(WINAPI* OriginalGetSysColorBrush) (int nIndex) = GetSysColorBrush;
static DWORD(WINAPI* OriginalGetSysColor) (int nIndex) = GetSysColor;
static HBRUSH(WINAPI* OriginalCreateSolidBrush) (COLORREF color) = CreateSolidBrush;

int _DrawTextW(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format) {
	SetBkColor(hdc, s2cePrimary);
	SetTextColor(hdc, s2ceText);

	return OriginalDrawTextW(hdc, lpchText, cchText, lprc, format);
}
int _FillRect(HDC hdc, const RECT* lprc, HBRUSH hbr) {
	SetBkColor(hdc, s2cePrimary);
	return OriginalFillRect(hdc, lprc, CreateSolidBrush(s2cePrimary));
}
DWORD _GetSysColor(int nIndex) {
	return s2cePrimary; //OriginalGetSysColor(nIndex);
}

// unused
COLORREF _SetTextColor(HDC hdc, COLORREF color) {
	return OriginalSetTextColor(hdc, color);
}
COLORREF _SetBkColor(HDC hdc, COLORREF color) {
	return OriginalSetBkColor(hdc, color);
}
HBRUSH _GetSysColorBrush(int nIndex) {
	return OriginalGetSysColorBrush(nIndex);
}
HBRUSH _CreateSolidBrush(COLORREF color) {
	//if (color == 0x00F8F8F8) {
	//	return OriginalCreateSolidBrush(0x000000FF);
	//}
	//if (color == 0x00FFFFFF) {
	//	return OriginalCreateSolidBrush(0x000000FF);
	//}
	return OriginalCreateSolidBrush(color);
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

		DetourAttach(&(PVOID&)OriginalDrawTextW, _DrawTextW);
		DetourAttach(&(PVOID&)OriginalFillRect, _FillRect);
		DetourAttach(&(PVOID&)OriginalSetTextColor, _SetTextColor);
		DetourAttach(&(PVOID&)OriginalSetBkColor, _SetBkColor);
		DetourAttach(&(PVOID&)OriginalGetSysColorBrush, _GetSysColorBrush);
		DetourAttach(&(PVOID&)OriginalGetSysColor, _GetSysColor);
		DetourAttach(&(PVOID&)OriginalCreateSolidBrush, _CreateSolidBrush);

		DetourTransactionCommit();
	}
	else if (dwReason == DLL_PROCESS_DETACH) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetourDetach(&(PVOID&)OriginalDrawTextW, _DrawTextW);
		DetourDetach(&(PVOID&)OriginalFillRect, _FillRect);
		DetourDetach(&(PVOID&)OriginalSetTextColor, _SetTextColor);
		DetourDetach(&(PVOID&)OriginalSetBkColor, _SetBkColor);
		DetourDetach(&(PVOID&)OriginalGetSysColorBrush, _GetSysColorBrush);
		DetourDetach(&(PVOID&)OriginalGetSysColor, _GetSysColor);
		DetourDetach(&(PVOID&)OriginalCreateSolidBrush, _CreateSolidBrush);

		DetourTransactionCommit();
	}
	return TRUE;
}