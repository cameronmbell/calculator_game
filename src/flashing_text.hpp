#ifndef _FLASHING_TEXT_HPP
#define _FLASHING_TEXT_HPP

#include "text.hpp"

using flash_mode_t=int;

namespace flash_mode {
	const flash_mode_t none=0b000;
	const flash_mode_t indefinite=0b001;
	const flash_mode_t one_shot=0b010;
	const flash_mode_t thrice=0b011;
	const flash_mode_t slow=0b000;
	const flash_mode_t quick=0b100;

	const flash_mode_t duration_mask=0b011;
	const flash_mode_t speed_mask=0b100;
};

class flashing_text : public aligned_text {
public:
	flashing_text()=default;

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

	void set_flash_mode(flash_mode_t what) {
		_flash_mode = what;
		_flash_timer = 0.0f;

		switch (_flash_mode & flash_mode::duration_mask) {
			case flash_mode::none:
				_flash_cycle = 0;
				break;
			case flash_mode::indefinite:
				_flash_cycle = -1;
				break;
			case flash_mode::one_shot:
				_flash_cycle = 1;
				_flash_visible = false;
				break;
			case flash_mode::thrice:
				_flash_cycle = 3*2;
				break;
			default:
				throw std::logic_error("invalid flash mode duration");
				break;
		};

		switch (_flash_mode & flash_mode::speed_mask) {
			case flash_mode::slow:
				_flash_speed = 1.0f;
				break;
			case flash_mode::quick:
				_flash_speed = 0.2f;
				break;
			default:
				throw std::logic_error("invalid flash mode speed");
				break;
		}
	}

	flash_mode_t get_flash_mode() const noexcept { return _flash_mode; };

	void update(float dt) {
		_flash_timer += dt;

		// no longer flash
		if (_flash_cycle == 0) {
			_flash_visible = true;
			return;
		} else if (_flash_timer > _flash_speed) {
			_flash_timer = 0.0f;
			_flash_visible = !_flash_visible;

			if (_flash_cycle > 0) // prevent an overflow
				_flash_cycle--;
		}
	}

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		if (_flash_visible)
			target.draw(this->_text);
	}

private:
	flash_mode_t _flash_mode;

	bool _flash_visible;
	int _flash_cycle;
	float _flash_timer;
	float _flash_speed;
};

#endif // !_FLASHING_TEXT_HPP
