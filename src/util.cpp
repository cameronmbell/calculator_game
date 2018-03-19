#include "util.hpp"

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
};
