/*
 * util.cpp:
 * a set of utility functions, not provided by the C++11 STL
 */

#ifndef _UTIL_HPP
#define _UTIL_HPP

#include <SFML/Graphics.hpp>

#include <vector>
#include <utility>
#include <memory>
#include <string>

namespace util {

	// unique_ptr function factory (part of the C++14 standard, but not C++11)
	// acts the same as make_shared
	template <typename T, typename Q=T, typename... Tp>
	std::unique_ptr<T> make_unique(Tp&&... args) {
		return std::unique_ptr<T>(new Q(std::forward<Tp>(args)...));
	}

	// int -> string of certain width, for text fields
	std::string as_string(int i, std::size_t width=0);

	// string -> int
	int from_string(std::string str);

	// split a string by a certain character
	std::vector<std::string> split(const std::string& what, char by=' ');

	// remove trailing whitespace from string
	std::string rstrip(std::string s);

	// remove leading whitespace from string
	std::string lstrip(std::string s);

	// remove leading/trailing whitespace from string
	std::string strip(std::string s);
};

#endif // _UTIL_HPP
