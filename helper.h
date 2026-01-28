#include <sstream>
#pragma once

/**
This class contains variety of methods to perform different helping things, such as checks / convertions / etc.
*/
class Helper {
public:
	/**
	Returns `true` whenever `str` includes `seq` (For `LPCWSTR` use `lpcwstr_includes()` instead).
	*/
	static bool includes(std::string str, std::string seq) {
		return str.find(seq) != std::string::npos;
	}
	/**
	Splits string to array of strings by `delimiter`.
	*/
	static void split(std::string& in, std::string out[], char delimiter)
	{
		int8_t index = 0;

		std::istringstream stream(in);
		std::string token;

		while (getline(stream, token, delimiter)) {
			out[index++] = token;
		}
	}
	/**
	Returns `true` whenever `lpStr` includes `seq`.
	*/
	static bool lpcwstr_includes(LPCWSTR lpStr, std::string seq) {
		return lpcwstr_to_str(lpStr).find(seq) != std::string::npos;
	}
	/**
	Converts `LPCWSTR` type to a `string` type.
	*/
	static std::string lpcwstr_to_str(LPCWSTR lpcwszStr)
	{
		int strLength = WideCharToMultiByte(CP_UTF8, 0, lpcwszStr, -1, nullptr, 0, nullptr, nullptr);
		std::string str(strLength, 0);
		WideCharToMultiByte(CP_UTF8, 0, lpcwszStr, -1, &str[0], strLength, nullptr, nullptr);
		return str;
	}
	/**
	Converts hex color to a COLORREF. Example: "#ffffff" -> RGB(255,255,255)
	*/
	static COLORREF hex_to_colorref(std::string hex) {
		// # 12 34 56
		// 0 12 34 56

		long r = strtol(hex.substr(1, 2).c_str(), NULL, 16);
		long g = strtol(hex.substr(3, 2).c_str(), NULL, 16);
		long b = strtol(hex.substr(5, 2).c_str(), NULL, 16);
		
		return RGB(r, g, b);
	}
	/**
	Converts `string` to an `int`.
	*/
	static int str_to_int(std::string str) {
		return strtol(str.c_str(), NULL, 10);
	}
	/**
	Converts `string` to an `float`.
	*/
	static float str_to_float(std::string str) {
		return std::stof(str);
	}
	/**
	Converts `string` to a `wstring` (Use to convert `string` to a `lpcwstr` via `.c_str()` in the end).
	Example: LPCWSTR lpcwstr = str_to_wstr("example").c_str();
	*/
	static std::wstring str_to_wstr(std::string str) {
		return std::wstring(str.begin(), str.end());
	}
	/**
	Converts `uint` value to a `hex` string.
	Example: 255 -> ff.
	*/
	static std::string uint_to_hex(UINT uint) {
		std::stringstream ss;
		ss << std::hex << uint;
		return ss.str();
	}
	/**
	Converts `COLORREF` to `hex` string in order of 0x + ABGR (Alpha Blue Green Red).
	*/
	static std::string colorref_to_hex(COLORREF colorref) {
		RGBTRIPLE rgb = { sizeof(RGBTRIPLE) };

		rgb.rgbtRed		= GetRValue(colorref);
		rgb.rgbtGreen	= GetGValue(colorref);
		rgb.rgbtBlue	= GetBValue(colorref);

		return "0x00" + uint_to_hex(rgb.rgbtBlue) + uint_to_hex(rgb.rgbtGreen) + uint_to_hex(rgb.rgbtRed);
	}
	/**
	Converts `HBRUSH` to `COLORREF`.
	*/
	static COLORREF hbr_to_colorref(HBRUSH hbr) {
		LOGBRUSH lbr = { 0 };

		if (GetObjectW(hbr, sizeof(lbr), &lbr))
		{
			return lbr.lbColor;
		}
	}
	static void ReplaceColorInBitmap(HBITMAP hBitmap, COLORREF oldColor, COLORREF newColor) {
		BITMAP bmp;
		GetObjectW(hBitmap, sizeof(bmp), &bmp);

		// Create a DIB section to access pixel data
		BITMAPINFOHEADER bmih = { 0 };
		bmih.biSize = sizeof(BITMAPINFOHEADER);
		bmih.biWidth = bmp.bmWidth;
		bmih.biHeight = -bmp.bmHeight;  // Negative for top-down DIB
		bmih.biPlanes = 1;
		bmih.biBitCount = 32; // Use 32-bit for RGBA
		bmih.biCompression = BI_RGB;

		// Allocate memory for pixel data
		const int pixelCount = bmp.bmWidth * bmp.bmHeight;
		BYTE* pixels = new BYTE[bmp.bmWidthBytes * bmp.bmHeight];

		// Get the pixel data
		HDC hdc = GetDC(nullptr);
		GetDIBits(hdc, hBitmap, 0, bmp.bmHeight, pixels, (BITMAPINFO*)&bmih, DIB_RGB_COLORS);
		ReleaseDC(nullptr, hdc);

		// Define the target colors for quicker comparison
		BYTE oldR = GetRValue(oldColor);
		BYTE oldG = GetGValue(oldColor);
		BYTE oldB = GetBValue(oldColor);

		BYTE newR = GetRValue(newColor);
		BYTE newG = GetGValue(newColor);
		BYTE newB = GetBValue(newColor);

		// Replace oldColor with newColor
		for (int y = 0; y < bmp.bmHeight; y++) {
			for (int x = 0; x < bmp.bmWidth; x++) {
				int index = (y * bmp.bmWidthBytes) + (x * 4);
				// Perform a direct memory comparison
				if (pixels[index] == oldB && pixels[index + 1] == oldG && pixels[index + 2] == oldR) {
					// Set new color (goes BGR)
					pixels[index] = newB;
					pixels[index + 1] = newG;
					pixels[index + 2] = newR;
				}
			}
		}

		// Update the bitmap with the modified pixel data
		hdc = GetDC(nullptr);
		SetDIBits(hdc, hBitmap, 0, bmp.bmHeight, pixels, (BITMAPINFO*)&bmih, DIB_RGB_COLORS);
		ReleaseDC(nullptr, hdc);

		// Cleanup
		delete[] pixels;
	}
};