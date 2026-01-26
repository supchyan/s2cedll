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
};