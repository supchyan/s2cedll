#include <fstream>
#pragma once

/**
This class provides logging logic into a `.log` file.
*/
class Log {
public:
	/**
	* Clears `.log` file.
	*/
	static void clear() {
		std::fstream log(".log", std::ofstream::out | std::ofstream::trunc);
		log.close();
	}
	/**
	* Writes a message into the `.log` file.
	*/
	static void message(std::string data) {
		std::fstream log(".log", std::ios::app);
		log << data;
		log << "\n";
		log.close();
	}
};