/*
 * text.hpp:
 * define a managed text object that supports alignment to wrap around sf::Text
 */

#ifndef _TEXT_HPP
#define _TEXT_HPP

#include <SFML/Graphics.hpp>

#include <iostream>
#include <cmath>

using text_align_t=int;

// bit values for text alignment
// given an integral value 0000 0000
// the last two bits determine vertical alignment
// the third and fourth last bit determine horizontal alignment
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

// aligned_text object is an SFML drawable to which wraps around sf::Text
class aligned_text : public sf::Drawable {
public:
	aligned_text()=default;

	// initialize an object given:
	// bounds - a rectangle enclosing the text the alignment box
	// font - the font to be used
	// al - text alignment bit value
	// c - text colour
	// font_size - text size
	aligned_text(const sf::FloatRect& bounds, const sf::Font& font, text_align_t al, sf::Color c=sf::Color::White, unsigned int font_size=24)
		: _text("", font, font_size), _bounds(bounds), _alignment(al) {
		_text.setFillColor(sf::Color::White);

		update_alignment();
	};

	// update position and origin of sf::Text object based on current alignment
	void update_alignment() {

		// retrieve the bounding box of the wrapped text
		float x, y;
		auto local = _text.getLocalBounds();

		// decode the horizontal alignment bits
		switch (_alignment & text_align::horizonal_mask) {

			// snap to left side of bounds
			case text_align::left:
				x = _bounds.left;
				break;

			// snap to right side of bounds minus local width
			case text_align::right:
				x = _bounds.left + _bounds.width - local.width;
				break;

			// snap to the average of right and left align
			case text_align::middle:
				x = _bounds.left + (_bounds.width / 2) - (local.width / 2);
				break;
			default:
				x = _bounds.left;
				break;
				throw std::logic_error("invalid horizontal text alignment: " + std::to_string(_alignment & text_align::horizonal_mask));
				break;
		}

		switch (_alignment & text_align::vertical_mask) {

			// snap to top of bounds
			case text_align::top:
				y = _bounds.top;
				break;

			// snap to bottom of bounds minus local height
			case text_align::bottom:
				y = _bounds.top + _bounds.height - local.height;
				break;

			// snap to average of top and bottom align
			case text_align::center:
				y = _bounds.top + (_bounds.height / 2) - (local.height / 2);
				break;
			default:
				y = _bounds.top;
				break;
				throw std::logic_error("invalid vertical text alignment: " + std::to_string(_alignment & text_align::vertical_mask));
				break;
		}

		// finally update origin/position based on calculated values
		_text.setOrigin(local.left, local.top);
		_text.setPosition(x, y);
	}

	// update alignment box
	void set_bounds(const sf::FloatRect& bounds) {
		_bounds = bounds;

		update_alignment();
	}

	// update font (and size) and hence recalculate alignment
	void set_font(const sf::Font& font, unsigned int font_size=24) {
		_text.setFont(font);
		_text.setCharacterSize(font_size);

		update_alignment();
	}

	// update character size and hence recalculate alignment
	void set_font_size(unsigned int font_size) {
		_text.setCharacterSize(font_size);

		update_alignment();
	}

	// update text fill colour
	void set_colour(const sf::Color& col) {
		_text.setFillColor(col);
	}

	// modify the alignment value and hence recalculate alignment
	void set_alignment(text_align_t al) {
		_alignment = al;

		update_alignment();
	}

	// modify the text string and hence recalculate alignment
	void set_string(const std::string& str) {
		_text.setString(str);

		update_alignment();
	}

	// render the text
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		target.draw(_text);

#if defined(_UNDEFINED_DEBUG_VALUE)

		// draw debug bounds
		// helps for debugging alignment code
		// not compiled in release builds
		auto bounds = _text.getGlobalBounds();
		sf::RectangleShape rect(sf::Vector2f(bounds.width, bounds.height));
		rect.setPosition(sf::Vector2f(bounds.left, bounds.top));
		rect.setOutlineThickness(1);
		rect.setFillColor(sf::Color(0, 0, 0, 0));
		rect.setOutlineColor(sf::Color::Magenta);

		target.draw(rect);

		rect.setSize(sf::Vector2f(_bounds.width, _bounds.height));
		rect.setPosition(sf::Vector2f(_bounds.left, _bounds.top));
		rect.setOutlineThickness(1);
		rect.setFillColor(sf::Color(0, 0, 0, 0));
		rect.setOutlineColor(sf::Color::Green);

		target.draw(rect);
#endif
	};

	// return current text
	std::string get_string() const {
		return _text.getString();
	}

	// calculate the number of character that would fit within the alignment bounds
	// used for tutorial level text overflow calculations
	// this approximates font height to width ratio as 1.8:1
	std::size_t get_fitting_characters() const {
		return std::floor(_bounds.width / _text.getCharacterSize() * 1.8f);
	};

protected:
	sf::Text _text;
	sf::FloatRect _bounds;
	text_align_t _alignment;
};

#endif // _TEXT_HPP
