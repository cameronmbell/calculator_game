// helper functions not provided by the C++11 STL

#ifndef _UTIL_HPP
#define _UTIL_HPP

#include <SFML/Graphics.hpp>

#include <utility>
#include <memory>
#include <string>

namespace util {

	// unique_ptr function factory (part of the C++14 standard)
	template <typename T, typename... Tp>
	std::unique_ptr<T> make_unique(Tp&&... args) {
		return std::unique_ptr<T>(new T(std::forward<Tp>(args)...));
	}

	// int -> string of certain width, for text fields
	std::string as_string(int i, std::size_t width=0);

	// string -> int
	int from_string(std::string str);

	sf::Vector2f get_text_size(const sf::Text& t);
};

#endif // _UTIL_HPP
