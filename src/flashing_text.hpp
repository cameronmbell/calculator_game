/*
 * flashing_text.hpp:
 * derived class of aligned_text that can flash
 * used in the game to signal some value has changed
 */

#ifndef _FLASHING_TEXT_HPP
#define _FLASHING_TEXT_HPP

#include "util.hpp"
#include "text.hpp"

using flash_mode_t=int;

// the mode of a flash, to be ORed together
// e.g. flash_mode::thrice | flash_mode::quick means three quick flashes
namespace flash_mode {
	const flash_mode_t none=0b000; // do not flash
	const flash_mode_t indefinite=0b001; // flash for ever
	const flash_mode_t one_shot=0b010; // flash once
	const flash_mode_t thrice=0b011; // flash three times
	const flash_mode_t slow=0b000; // flash slowly (~1Hz)
	const flash_mode_t quick=0b100; // flash quickly (~5Hz)

	const flash_mode_t duration_mask=0b011;
	const flash_mode_t speed_mask=0b100;
};

// a derived aligned_text class that can flash
class flashing_text : public aligned_text {
public:
	flashing_text()=default;

	// forward parameters to base class with additional flash_mode param
	flashing_text(
			const sf::FloatRect& bounds,
			const sf::Font& font,
			text_align_t al,
			flash_mode_t mode=flash_mode::none,
			sf::Color c=sf::Color::White,
			unsigned int font_size=24)
		: aligned_text(bounds, font, al, c, font_size) {
			set_flash_mode(mode);
		}

	// set the current flash mode
	// determines the state change initiated by the flash_mode_t mask
	void set_flash_mode(flash_mode_t what) {
		_flash_mode = what;
		_flash_timer = 0.0f;

		// AND the flash mode with the duration mask (0000 0011)
		// meaning the last two bits of 'what' determine the flash mode
		switch (_flash_mode & flash_mode::duration_mask) {
			case flash_mode::none:
				_flash_cycle = 0; // cycle stays at zero
				break;
			case flash_mode::indefinite:
				_flash_cycle = -1; // cycle continues forever
				break;
			case flash_mode::one_shot:
				_flash_cycle = 1; // cycles once then reaches zero
				_flash_visible = false;
				break;
			case flash_mode::thrice: // cycles three times then reaches zero
				_flash_cycle = 3*2;
				break;
			default:
				throw std::logic_error("invalid flash mode duration");
				break;
		};

		// AND the flash mode with the speed mask (0000 0100)
		// meaning the third last bit of 'what' determine the flash mode
		switch (_flash_mode & flash_mode::speed_mask) {
			case flash_mode::slow:
				_flash_speed = 1.0f; // ~1Hz
				break;
			case flash_mode::quick:
				_flash_speed = 0.2f; // ~5Hz
				break;
			default:
				throw std::logic_error("invalid flash mode speed");
				break;
		}
	}

	// set the string and additionally it's alternate
	void set_alt_string(const std::string to) {
		_alt_text = util::make_unique<std::string>(to);
	}

	// dealloc the current alt_string
	void remove_alt_string() {
		_alt_text = nullptr;
	}

	// get the current flash mode state enumeration
	flash_mode_t get_flash_mode() const noexcept { return _flash_mode; };

	// calculate whether the text should be visible or not based on the current time
	// and it's flash_mode as set by set_flash_mode()
	void update(float dt) {

		// set flash visible depends on on weather an alt_string is specified
		// need to determine the primary string before changing over to the alt string
		if (_alt_text != nullptr && get_string() != *_alt_text)
			_primary_text = get_string();

		// store every second in a clock
		_flash_timer += dt;

		// once reached no longer flash
		if (_flash_cycle == 0) {
			_set_visible(true);
			return;
		} else if (_flash_timer > _flash_speed) {

			// keep flashing till zero is reached
			// and invert visibility
			_flash_timer = 0.0f;
			_set_visible(!_flash_visible);

			// prevent an overflow for an indefinite flash mode
			if (_flash_cycle > 0)
				_flash_cycle--;
		}
	}

	// render the text
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {

		// when it comes down to it, only visible dependent on the state boolean
		// given an alt_string, disabling drawing in undesired
		 if (_flash_visible || _alt_text != nullptr)
			target.draw(this->_text);
	}

private:

	// private member to change visibility, which depends on whether an alt_string has been set
	// if it has change the primary string to that, otherwise disable primary
	void _set_visible(bool to) {
		if (_alt_text != nullptr)
			set_string((to)? _primary_text : *_alt_text);

		_flash_visible = to;
	}

	std::unique_ptr<std::string> _alt_text;
	std::string _primary_text;

	flash_mode_t _flash_mode;

	bool _flash_visible;
	int _flash_cycle;
	float _flash_timer;
	float _flash_speed;
};

#endif // !_FLASHING_TEXT_HPP
