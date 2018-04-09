#ifndef _BUTTON_HPP
#define _BUTTON_HPP

#include <SFML/Graphics.hpp>

#include <iostream>
#include <unordered_map>

#include "event.hpp"
#include "text.hpp"

enum class button_state {
	hover,
	click,
	normal
};

struct button_style {
	text_align_t text_alignment;
	sf::Color text_colour;
	unsigned int text_size;

	sf::Color outline_colour;
	float outline_thickness;

	std::unordered_map<button_state, sf::Color> fill_colour;

	float internal_padding;
	float fade_time;

	static const button_style default_orange;
	static const button_style default_grey;
};

class basic_button :
	public sf::Drawable,
	public event::class_listener<
		event::sf_event::MouseMoved,
		event::sf_event::MouseButtonPressed,
		event::sf_event::MouseButtonReleased> {

public:
	basic_button() : basic_button(button_style::default_grey) { }

	basic_button(const button_style& style)
		: _state(button_state::normal), _shape(), _text(), _enabled(true), _fade_progress(1.0f) {
		set_style(style);
		set_state(button_state::normal);
	};

	void set_style(button_style style) {
		_style = style;

		_shape.setOutlineColor(_style.outline_colour);
		_shape.setOutlineThickness(_style.outline_thickness);
		_shape.setPosition(0, 0);
		_shape.setSize(sf::Vector2f(50, 50));

		_text.set_alignment(_style.text_alignment);
		_text.set_colour(_style.text_colour);

		_fade_progress = 1.0f;
		_rect_target_colour = _style.fill_colour.at(button_state::normal);
	}

	void set_state(button_state state) {
		_state = state;

		if (_style.fade_time <= 0.0f) // do not fade !
			_shape.setFillColor(_style.fill_colour.at(_state));
		else {
			_rect_target_colour = _style.fill_colour.at(_state);
			_fade_progress = 0.0f;
		}
	}

	void set_font(const sf::Font& font) {
		_text.set_font(font, _style.text_size);
	}

	void set_bounds(const sf::FloatRect& bounds) {
		_shape.setPosition(bounds.left, bounds.top);
		_shape.setSize(sf::Vector2f(bounds.width - _style.internal_padding, bounds.height - _style.internal_padding));
		_text.set_bounds(bounds);
	}

	void enable() {
		_enabled = true;
	}

	void disable() {
		_enabled = false;
	}

	void set_string(const std::string& str) {
		_text.set_string(str);
	}

	virtual void on_click() {
		std::cout << "basic_button was clicked!" << std::endl;
	}

	bool mouse_intersect(const sf::RenderWindow* win) const {
		auto mp = win->mapCoordsToPixel(sf::Vector2f(sf::Mouse::getPosition(*win).x, sf::Mouse::getPosition(*win).y));

		sf::FloatRect mouse_box;
		mouse_box.width = 1;
		mouse_box.height = 1;
		mouse_box.left = mp.x;
		mouse_box.top = mp.y;

		return _shape.getGlobalBounds().intersects(mouse_box);
	}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		target.draw(_shape);
		target.draw(_text);
	}

	void fade(float dt) {
		if (_fade_progress < 1.0f) {
			_fade_progress += dt;

			sf::Color target;

			auto lerp = [](float current, float target, float time) -> float {
				return (1 - time) * current + time * target;
			};

			target.r = lerp(_shape.getFillColor().r, _rect_target_colour.r, _fade_progress);
			target.g = lerp(_shape.getFillColor().g, _rect_target_colour.g, _fade_progress);
			target.b = lerp(_shape.getFillColor().b, _rect_target_colour.b, _fade_progress);

			_shape.setFillColor(target);
		} else
			_shape.setFillColor(_rect_target_colour); // hmmm....
	}

	void simulate_click() {
		on_click();
	}

	void simulate_click_pressed() {
		set_state(button_state::click);
	}

	void simulate_click_released() {
		set_state(button_state::normal);
		on_click();
	}

	virtual void listen(const event::sf_event::MouseMoved& t) override {
		auto intersects = mouse_intersect(t.rw);

		if (intersects && _state == button_state::normal)
			set_state(button_state::hover);
		if (!intersects && _state != button_state::normal)
			set_state(button_state::normal);
	}

	virtual void listen(const event::sf_event::MouseButtonPressed& t) override {
		if (!_enabled)
			return;

		if (t.value.mouseButton.button == sf::Mouse::Button::Left) {
			auto intersects = mouse_intersect(t.rw);

			if (intersects)
				set_state(button_state::click);
		}
	}

	virtual void listen(const event::sf_event::MouseButtonReleased& t) override {
		if (!_enabled)
			return;

		if (t.value.mouseButton.button == sf::Mouse::Button::Left) {
			auto intersects = mouse_intersect(t.rw);

			if (intersects && _state == button_state::click) {
				on_click();
				set_state(button_state::hover);
			}
		}
	}

private:
	button_style _style; // ref/ptr would be smarter
	button_state _state;
	sf::RectangleShape _shape;
	aligned_text _text;
	bool _enabled;

	float _fade_progress;
	sf::Color _rect_target_colour;
};

#endif // _BUTTON_HPP
