/*
 * util.cpp:
 * a set of utility functions, implementation of util.hpp
 */

#include "util.hpp"

#include <iostream>
#include <cmath>

// define all whitespace characters (taken from the python 3.6 standard)
// to be used for striping strings
namespace {
	static const char* whitespace = " \t\n\r\f\v";
}

namespace util {

	// int -> string of certain width, for text fields
	std::string as_string(int i, std::size_t width) {
		std::string str = std::to_string(i);

		if (width && str.length() <= width) {
			return str.substr(width);
		}

		return str;
	}

	// string -> int
	int from_string(std::string str) {
		return std::atoi(str.c_str());
	}

	// split a string by a certain character
	std::vector<std::string> split(const std::string& what, char by) {
		std::vector<std::string> out;

		// loop through successive str.find on the tokenize character till end
		// append these substrings to the output
		for (std::size_t i = 0, n = 0; n != std::string::npos;) {
			n = what.find(by, i+1);
			out.push_back(what.substr(i, n-i));
			i = n+1;
		}

		return out;
	}

	// remove trailing whitespace from string
	std::string rstrip(std::string s) {
		return s.erase(s.find_last_not_of(whitespace)+1);
	}

	// remove leading whitespace from string
	std::string lstrip(std::string s) {
		return s.erase(0, s.find_first_not_of(whitespace));
	}

	// remove leading/trailing whitespace from string
	std::string strip(std::string s) {
		return lstrip(rstrip(s));
	}
};
