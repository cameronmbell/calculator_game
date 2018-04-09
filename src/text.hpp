#ifndef _TEXT_HPP
#define _TEXT_HPP

#include <SFML/Graphics.hpp>

#include <iostream>
#include <cmath>

using text_align_t=int;

namespace text_align {
	const text_align_t top=0;
	const text_align_t center=1;
	const text_align_t bottom=2;
	const text_align_t vertical_mask=3;
	const text_align_t left=0;
	const text_align_t middle=4;
	const text_align_t right=8;
	const text_align_t horizonal_mask=12;
};

class aligned_text : public sf::Drawable {
public:
	aligned_text()=default;

	aligned_text(const sf::FloatRect& bounds, const sf::Font& font, text_align_t al, sf::Color c=sf::Color::White, unsigned int font_size=24)
		: _text("", font, font_size), _bounds(bounds), _alignment(al) {
		_text.setFillColor(sf::Color::White);

		update_alignment();
	};

	void update_alignment() {
		float x, y;
		auto local = _text.getLocalBounds();

		switch (_alignment & text_align::horizonal_mask) {
			case text_align::left:
				x = _bounds.left;
				break;
			case text_align::right:
				x = _bounds.left + _bounds.width - local.width;
				break;
			case text_align::middle:
				x = _bounds.left + (_bounds.width / 2) - (local.width / 2);
				break;
			default:
				x = _bounds.left; // @TODO FIXXX
				break;
				throw std::logic_error("invalid horizontal text alignment: " + std::to_string(_alignment & text_align::horizonal_mask));
				break;
		}

		switch (_alignment & text_align::vertical_mask) {
			case text_align::top:
				y = _bounds.top;
				break;
			case text_align::bottom:
				y = _bounds.top + _bounds.height - local.height;
				break;
			case text_align::center:
				y = _bounds.top + (_bounds.height / 2) - (local.height / 2);
				break;
			default:
				y = _bounds.top; // @TODO FIXXX
				break;
				throw std::logic_error("invalid vertical text alignment: " + std::to_string(_alignment & text_align::vertical_mask));
				break;
		}

		_text.setOrigin(local.left, local.top);
		_text.setPosition(x, y);
	}


	void set_bounds(const sf::FloatRect& bounds) {
		_bounds = bounds;

		update_alignment();
	}

	void set_font(const sf::Font& font, unsigned int font_size=24) {
		_text.setFont(font);
		_text.setCharacterSize(font_size);

		update_alignment();
	}

	void set_font_size(unsigned int font_size) {
		_text.setCharacterSize(font_size);

		update_alignment();
	}

	void set_colour(const sf::Color& col) {
		_text.setFillColor(col);
	}

	void set_alignment(text_align_t al) {
		_alignment = al;

		update_alignment();
	}

	void set_string(const std::string& str) {
		_text.setString(str);

		update_alignment();
	}

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		target.draw(_text);

//		auto bounds = _text.getGlobalBounds();
//		sf::RectangleShape rect(sf::Vector2f(bounds.width, bounds.height));
//		rect.setPosition(sf::Vector2f(bounds.left, bounds.top));
//		rect.setOutlineThickness(1);
//		rect.setFillColor(sf::Color(0, 0, 0, 0));
//		rect.setOutlineColor(sf::Color::Magenta);
//
//		target.draw(rect);
//
//		rect.setSize(sf::Vector2f(_bounds.width, _bounds.height));
//		rect.setPosition(sf::Vector2f(_bounds.left, _bounds.top));
//		rect.setOutlineThickness(1);
//		rect.setFillColor(sf::Color(0, 0, 0, 0));
//		rect.setOutlineColor(sf::Color::Green);
//
//		target.draw(rect);
	};

	std::string get_string() const {
		return _text.getString();
	}

	std::size_t get_fitting_characters() const {
		return std::floor(_bounds.width / _text.getCharacterSize() * 1.8f /* approx ratio of font height to width */);
	};


protected:
	sf::Text _text;
	sf::FloatRect _bounds;
	text_align_t _alignment;
};

#endif // _TEXT_HPP
