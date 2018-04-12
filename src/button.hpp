/*
 * button.hpp:
 * defines a generic renderable button with state fading and style objects
 */

#ifndef _BUTTON_HPP
#define _BUTTON_HPP

#include <SFML/Graphics.hpp>

#include <unordered_map>
#include <iostream>

#include "event.hpp"
#include "text.hpp"

// state of a button used to determines it's colour
enum class button_state {
	hover,
	click,
	normal
};

// a button style object outlining generic reusable properties
// (most) properties can be modified after instantiation
struct button_style {
	text_align_t text_alignment;
	sf::Color text_colour;

	sf::Color outline_colour;
	float outline_thickness;

	std::unordered_map<button_state, sf::Color> fill_colour;

	float internal_padding;
	float fade_time;

	static const button_style default_orange;
	static const button_style default_grey;
};

// the button class
// which listens to SFML mouse events for state changes
class basic_button :
	public sf::Drawable,
	public event::class_listener<
		event::sf_event::MouseMoved,
		event::sf_event::MouseButtonPressed,
		event::sf_event::MouseButtonReleased> {

public:
	// default to grey style if no parameters are passed
	// this is necessary because else an array of buttons cannot be implicitly initialized
	basic_button() : basic_button(button_style::default_grey) { }

	// construct state as normal from style object
	basic_button(const button_style& style)
		: _state(button_state::normal), _shape(), _text(), _enabled(true), _fade_progress(1.0f) {
		set_style(style);
		set_state(button_state::normal);
	};

	// change attributes of member text and rect
	void set_style(button_style style) {
		_style = style;

		_shape.setOutlineColor(_style.outline_colour);
		_shape.setOutlineThickness(_style.outline_thickness);
		_shape.setPosition(0, 0);
		_shape.setSize(sf::Vector2f(50, 50));

		_text.set_alignment(_style.text_alignment);
		_text.set_colour(_style.text_colour);

		// reset any fades
		_fade_progress = 1.0f;
		_rect_target_colour = _style.fill_colour.at(button_state::normal);
	}

	// set button state and modify fill colour
	// also needs to fade into the updated fill
	void set_state(button_state state) {
		_state = state;

		if (_style.fade_time <= 0.0f) // do not fade
			_shape.setFillColor(_style.fill_colour.at(_state));
		else {
			_rect_target_colour = _style.fill_colour.at(_state);
			_fade_progress = 0.0f;
		}
	}

	// change font (and size)
	void set_font(const sf::Font& font, unsigned int size=24) {
		_text.set_font(font, size);
	}

	// modify text object bounds i.e. how big it is and where it is
	void set_bounds(const sf::FloatRect& bounds) {
		_shape.setPosition(bounds.left, bounds.top);
		_shape.setSize(sf::Vector2f(bounds.width - _style.internal_padding, bounds.height - _style.internal_padding));
		_text.set_bounds(bounds);
	}

	// do not ignore click events
	void enable() {
		_enabled = true;
	}

	// ignore click events
	void disable() {
		_enabled = false;
	}

	// set button string
	// this will recalculate alignment
	void set_string(const std::string& str) {
		_text.set_string(str);
	}

	// virtual to be overloaded on derived members, called on click
	virtual void on_click() {
		std::cout << "basic_button was clicked!" << std::endl;
	}

	// determine if the mouse is intersecting with this button object
	// needs the RenderWindow* object to translate mouse coords to window coords
	bool mouse_intersect(const sf::RenderWindow* win) const {

		// transform mouse space to window space
		auto mp = win->mapCoordsToPixel(sf::Vector2f(sf::Mouse::getPosition(*win).x, sf::Mouse::getPosition(*win).y));

		sf::FloatRect mouse_box;
		mouse_box.width = 1;
		mouse_box.height = 1;
		mouse_box.left = mp.x;
		mouse_box.top = mp.y;

		return _shape.getGlobalBounds().intersects(mouse_box);
	}

	// render this object's components
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		target.draw(_shape);
		target.draw(_text);
	}

	// fade the button's fill by a delta time i.e. linearly interpolate between active states
	void fade(float dt) {
		if (_fade_progress < 1.0f) {
			_fade_progress += dt;

			sf::Color target;

			// given a value N and a target T find the linear interval between them by time
			// where time is between 0.0f and 1.0f
			// could easily implement quadratic fading, but it doesn't look as nice
			auto lerp = [](float current, float target, float time) -> float {
				return (1 - time) * current + time * target;
			};

			target.r = lerp(_shape.getFillColor().r, _rect_target_colour.r, _fade_progress);
			target.g = lerp(_shape.getFillColor().g, _rect_target_colour.g, _fade_progress);
			target.b = lerp(_shape.getFillColor().b, _rect_target_colour.b, _fade_progress);

			_shape.setFillColor(target);
		} else {

			// the target colour has been reached so lerping is no longer necessary
			_shape.setFillColor(_rect_target_colour);
		}
	}

	// fake a click
	// does not modify rendering of the button
	// instead use simulate_click_pressed() + simulate_click_released()
	void simulate_click() {
		on_click();
	}

	// fake a hover
	void simulate_click_pressed() {
		set_state(button_state::click);
	}

	// fake a click
	void simulate_click_released() {
		set_state(button_state::normal);
		on_click();
	}

	// listen to mouse move event from the event dispatch
	virtual void listen(const event::sf_event::MouseMoved& t) override {
		auto intersects = mouse_intersect(t.rw);

		// when intersecting --> hover
		// otherwise --> normal
		if (intersects && _state == button_state::normal)
			set_state(button_state::hover);
		if (!intersects && _state != button_state::normal)
			set_state(button_state::normal);
	}

	// listen to mouse pressed event (the instant when the mouse is pressed down) from the event dispatch
	virtual void listen(const event::sf_event::MouseButtonPressed& t) override {
		if (!_enabled)
			return;

		if (t.value.mouseButton.button == sf::Mouse::Button::Left) {
			auto intersects = mouse_intersect(t.rw);

			// update state but don't call on_click yet
			// that will be called when mouse button is released
			if (intersects)
				set_state(button_state::click);
		}
	}

	// listen to mouse pressed event (the instant when the mouse is released) from the event dispatch
	virtual void listen(const event::sf_event::MouseButtonReleased& t) override {
		if (!_enabled)
			return;

		if (t.value.mouseButton.button == sf::Mouse::Button::Left) {
			auto intersects = mouse_intersect(t.rw);

			// only trigger click if the mouse is still intersecting, meaning:
			// if a click is pressed down on a button but then the mouse is moved away
			// while the button is still down, do not trigger a click
			// this means mis-clicks can be cancelled
			if (intersects && _state == button_state::click) {
				on_click();
				set_state(button_state::hover);
			}
		}
	}

private:
	// consider using a style ref/ptr to prevent extra copy
	button_style _style;
	button_state _state;
	sf::RectangleShape _shape;
	aligned_text _text;
	bool _enabled;

	float _fade_progress;
	sf::Color _rect_target_colour;
};

#endif // _BUTTON_HPP
