#include "log.h"
#include "helper.h"
#pragma once

/**
This class manages everything related to theme data (read / write / modify). 
*/
class ThemeMgr {
public:
	/**
	Tries to write `value` pointer with a data from a `.theme` file by a `key`. 
	Returns `0` if failed, `1` otherwise.
	*/
	static bool get_value(std::string key, std::string& value) {
		std::fstream theme_file(".theme");

		std::string line;
		std::string data[255];
		int data_size = sizeof(data) / sizeof(data[0]);

		if (theme_file.is_open()) {						// check if file exists
			while (getline(theme_file, line)) {			// read each line
				Helper::split(line, data, ' ');			// split line to key / val by ' '
				if (data[0] == key) {					// check if key/value pair is expected one

					auto end = std::remove(				// remove empty items to properly parse key/val pair
						std::begin(data), 
						std::end(data), ""
					);

					value = data[1];					// write value data

					theme_file.close();					// close file

					Log::message("[I] Initialized " + key);

					return 1;							// return 1 (success)
				}
			}
			// log abscent values (error)
			Log::message("[F] Failed " + key);
			return 0;
		}
		else {											// log file doesn't exist (error)
			Log::message("[E] Cannot read \".theme\"");
		}

		theme_file.close();
		return 0; // return 0 (failed)
	}
	/**
	Sets background color for a color circle bitmap.
	*/
	static void set_color_circle_col(DWORD width, DWORD height, BYTE* lpvBits, COLORREF col) {// sizes of a color circle bitmap resource:
		// 166px - 100%; 
		// 206px - 125%; 
		// 250px - 150%; 
		// 326px - 200%.
		DWORD cc_s[4] = { 166, 206, 250, 326 };

		if (width != height) return; // return if provided rect is not belonged to a color circle

		if (width != cc_s[0] &&
			width != cc_s[1] &&
			width != cc_s[2] &&
			width != cc_s[3]
			) {
			return; // return if provided rect is not belonged to a color circle
		}

		float w = (float)width;
		float h = (float)height;

		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				int i = (y * w + x) * 4;// 32bits color index (i.e. 0x00RRGGBB -> 0 -> 4 -> 8 -> ...)
				float th = 5;			// thickness of color circle outline
				float ox = x - w / 2;	// x center
				float oy = y - h / 2;	// y center
				float r = w / 2 - th;	// radius

				bool c = ox*ox + oy*oy > r*r;

				if (c) {
					// update color of specified bit
					lpvBits[i + 0] = GetBValue(col);	// b
					lpvBits[i + 1] = GetGValue(col);	// g
					lpvBits[i + 2] = GetRValue(col);	// r
				}
			}
		}
	}
	/**
	Sets background color for a color circle bitmap [ slow, but stable alternative for `set_color_circle_col()` ].
	*/
	static void set_color_circle_col_alt(HDC hdc, DWORD width, DWORD height, COLORREF col) {
		// sizes of a color circle bitmap resource:
		// 166px - 100%; 
		// 206px - 125%; 
		// 250px - 150%; 
		// 326px - 200%.
		DWORD cc_s[4] = { 166, 206, 250, 326 };

		if (width != height) return; // return if provided rect is not belonged to a color circle

		if (width != cc_s[0] && 
			width != cc_s[1] && 
			width != cc_s[2] && 
			width != cc_s[3]
		) {
			return; // return if provided rect is not belonged to a color circle
		}
		
		float w = (float)width;
		float h = (float)height;

		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				float th = 5;			// thickness of color circle outline
				float ox = x - w / 2;	// x center
				float oy = y - h / 2;	// y center
				float r = w / 2 - th;	// radius

				bool c = ox * ox + oy * oy > r * r;

				if (c) {
					SetPixel(hdc, x, y, col);
				}
			}
		}
	}
};