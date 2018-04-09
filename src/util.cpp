#include "util.hpp"

#include <iostream>
#include <cmath>

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

	// from https://en.sfml-dev.org/forums/index.php?topic=7174
	// a proposed implenentation in SFML
	sf::Vector2f get_text_size(const sf::Text& t) {
		const sf::String str = t.getString() + '\n';

		float maxLineWidth = 0.f;
		float lineWidth = 0.f;
		unsigned int lines = 0;

		for (sf::String::ConstIterator itr = str.begin(); itr != str.end(); ++itr) {
			if (*itr == '\n') {
				++lines;
				maxLineWidth = std::max(maxLineWidth, lineWidth);
				lineWidth = 0.f;
			} else {
				lineWidth += t.getFont()->getGlyph(*itr, t.getCharacterSize(), t.getStyle() & sf::Text::Bold).advance;
			}
		}

		const float lineHeight = static_cast<float>(t.getFont()->getLineSpacing(t.getCharacterSize()));
		return sf::Vector2f(maxLineWidth, lines * lineHeight);
	}

	// split a string by a certain character
	std::vector<std::string> split(const std::string& what, char by) {
		std::vector<std::string> out;

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
