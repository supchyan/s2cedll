#include "pch.h"
#include "log.h"
#include "pointers.h"
#include "hooks.h"
#include <detours.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  dwReason, LPVOID lpReserved)
{
	if (DetourIsHelperProcess()) {
		return TRUE;
	}

	if (dwReason == DLL_PROCESS_ATTACH) {
		Log::clear(); // clear old log data upon dll started a job
		Log::message("[W] Injected s2ce.dll"); // log "welcome" message

		init_theme(); // init theme values

		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		
		DetourAttach(&(PVOID&)oCreateSolidBrush, hCreateSolidBrush);
		DetourAttach(&(PVOID&)oDrawTextW, hDrawTextW);
		DetourAttach(&(PVOID&)oCreateFontIndirectW, hCreateFontIndirectW);
		DetourAttach(&(PVOID&)oCreateWindowExW, hCreateWindowExW);
		DetourAttach(&(PVOID&)oSetWindowTextW, hSetWindowTextW);
		DetourAttach(&(PVOID&)oRegisterClassExW, hRegisterClassExW);
		DetourAttach(&(PVOID&)oAppendMenuW, hAppendMenuW);
		DetourAttach(&(PVOID&)oSetMenuInfo, hSetMenuInfo);
		DetourAttach(&(PVOID&)oSetMenuItemInfoW, hSetMenuItemInfoW);
		DetourAttach(&(PVOID&)oInsertMenuItemW, hInsertMenuItemW);
		DetourAttach(&(PVOID&)oSetBkColor, hSetBkColor);
		DetourAttach(&(PVOID&)oCreateDIBitmap, hCreateDIBitmap);
		DetourAttach(&(PVOID&)oSetWindowRgn, hSetWindowRgn);
		DetourAttach(&(PVOID&)oBeginPaint, hBeginPaint);
		DetourAttach(&(PVOID&)oEndPaint, hEndPaint);
		DetourAttach(&(PVOID&)oFillRect, hFillRect);
		DetourAttach(&(PVOID&)oSetDIBitsToDevice, hSetDIBitsToDevice);
		DetourAttach(&(PVOID&)oBitBlt, hBitBlt);

		DetourTransactionCommit();
	}
	else if (dwReason == DLL_PROCESS_DETACH) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetourDetach(&(PVOID&)oCreateSolidBrush, hCreateSolidBrush);
		DetourDetach(&(PVOID&)oDrawTextW, hDrawTextW);
		DetourDetach(&(PVOID&)oCreateFontIndirectW, hCreateFontIndirectW);
		DetourDetach(&(PVOID&)oCreateWindowExW, hCreateWindowExW);
		DetourDetach(&(PVOID&)oSetWindowTextW, hSetWindowTextW);
		DetourDetach(&(PVOID&)oRegisterClassExW, hRegisterClassExW);
		DetourDetach(&(PVOID&)oAppendMenuW, hAppendMenuW);
		DetourDetach(&(PVOID&)oSetMenuInfo, hSetMenuInfo);
		DetourDetach(&(PVOID&)oSetMenuItemInfoW, hSetMenuItemInfoW);
		DetourDetach(&(PVOID&)oInsertMenuItemW, hInsertMenuItemW);
		DetourDetach(&(PVOID&)oSetBkColor, hSetBkColor);
		DetourDetach(&(PVOID&)oCreateDIBitmap, hCreateDIBitmap);
		DetourDetach(&(PVOID&)oSetWindowRgn, hSetWindowRgn);
		DetourDetach(&(PVOID&)oBeginPaint, hBeginPaint);
		DetourDetach(&(PVOID&)oEndPaint, hEndPaint);
		DetourDetach(&(PVOID&)oFillRect, hFillRect);
		DetourDetach(&(PVOID&)oSetDIBitsToDevice, hSetDIBitsToDevice);
		DetourDetach(&(PVOID&)oBitBlt, hBitBlt);

		DetourTransactionCommit();

		Log::message("[W] Exited with code 0"); // log "exit" message
	}

	return TRUE;
}