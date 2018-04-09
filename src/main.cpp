#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <thread>
#include <vector>

#include "flashing_text.hpp"
#include "operation.hpp"
#include "resource.hpp"
#include "manager.hpp"
#include "button.hpp"
#include "core.hpp"
#include "util.hpp"

class game_renderer :
	public core::runnable,
	public event::class_listener<
		event::sf_event::Resized, // window resize - stop this from happening
		event::sf_event::KeyPressed, // key press - simulate button hover
		event::sf_event::KeyReleased, // key release - simulate button click
		posts::display_event<posts::display_event_mode::normal_text>, // operation/button update - redraw
		posts::display_event<posts::display_event_mode::tutorial_text>, // operation/button update - redraw
		posts::display_event<posts::display_event_mode::operations_central>, // operation/button update - redraw
		posts::display_event<posts::display_event_mode::operations_aux>, // update text - redraw
		posts::display_event<posts::display_event_mode::operations_redraw>, // update text - redraw
		posts::display_event<posts::display_event_mode::disable>> { // operation/button update - redraw

	using opbtn=operational_button;

private:
	std::array<operational_button, 6> _cental_btns;
	std::array<operational_button, 3> _aux_btns;
	flashing_text _primary, _level, _moves, _target;
	flashing_text _tutorial_textl1, _tutorial_textl2;

	template <typename T>
	void _set_buttons(T& which, make_operations::type to) { // unnecessary data cpy
		for (auto& it : which)
			it.remove_operation();

		for (std::size_t i = 0; i < std::min(to.size(), which.size()); i++) {
			which[i].set_operation(to[i]); // should use nice alignment algorithm
		}
	}

public:

	game_renderer() { }

	virtual std::unique_ptr<sf::RenderWindow> setup() override {
		sf::ContextSettings cset;
		cset.antialiasingLevel = 4;

		// attempt to get desktop resolution bigger than 384x576 that is
		// 4x3 aspect ratio and has space for title bars (approx 128px)
		sf::VideoMode vmode(384, 576);
		for (const auto& m : sf::VideoMode::getFullscreenModes()) {
			if ((m.height - 128) % 4 == 0 && (m.height - 128) > vmode.height)
				vmode = sf::VideoMode((m.height - 128) / 4 * 3, m.height - 128);
		}

		auto win = util::make_unique<sf::RenderWindow>(vmode, "my_game", sf::Style::Titlebar | sf::Style::Close, cset);

		auto& favicon = resource_manager<sf::Image>::get("favicon");
		win->setIcon(favicon.getSize().x, favicon.getSize().y, favicon.getPixelsPtr());

		// some digusting position calculations follow
		// creating a scene graph binary file format to store this info
		// is just too much work so fuck it...
		for (std::size_t i = 0; i < _cental_btns.size(); i++) {
			_cental_btns[i].set_style(button_style::default_grey);
			_cental_btns[i].set_font(resource_manager<sf::Font>::get("roboto"));
			_cental_btns[i].set_bounds(sf::FloatRect(
					(i / 3) * (win->getSize().x / 3),
					(win->getSize().y / 4) * (1 + (i % 3)),
					win->getSize().x / 3, win->getSize().y / 4));
		}

		for (std::size_t i = 0; i < _aux_btns.size(); i++) {
			_aux_btns[i].set_style(button_style::default_orange);
			_aux_btns[i].set_font(resource_manager<sf::Font>::get("roboto"));
			_aux_btns[i].set_bounds(sf::FloatRect(
					(2 * win->getSize().x) / 3,
					(win->getSize().y / 4) * (1 + (i % 3)),
					win->getSize().x / 3, win->getSize().y / 4));
		}

		for (auto it : { &_primary, &_moves, &_level, &_target, &_tutorial_textl1, &_tutorial_textl2 }) {
			it->set_font(resource_manager<sf::Font>::get("roboto"), 18);
			it->set_bounds(sf::FloatRect(16, 16, win->getSize().x - 32, win->getSize().y / 4 - 32));
			it->set_flash_mode(flash_mode::none);
			it->set_colour(sf::Color::White);
			it->set_string("");
		}

		_primary.set_alignment(text_align::right | text_align::bottom);
		_moves.set_alignment(text_align::middle | text_align::top);
		_level.set_alignment(text_align::left | text_align::top);
		_target.set_alignment(text_align::right | text_align::top);
		_tutorial_textl1.set_alignment(text_align::right | text_align::center);
		_tutorial_textl2.set_alignment(text_align::right | text_align::bottom);

		_tutorial_textl1.set_font_size(36);
		_tutorial_textl2.set_font_size(36);
		_primary.set_font_size(108);

		level::load();

		return win;
	}

	virtual void update(sf::RenderWindow* rw, float dt) override {
		for (auto& it: _aux_btns) { it.fade(dt); rw->draw(it); }
		for (auto& it: _cental_btns) { it.fade(dt); rw->draw(it); }

		if (level::last_mode() == level::mode::numeric) {
			_moves.update(dt); rw->draw(_moves);
			_target.update(dt); rw->draw(_target);
			_level.update(dt); rw->draw(_level);
			_primary.update(dt); rw->draw(_primary);
		}

		if (level::last_mode() == level::mode::tutorial) {
			_tutorial_textl1.update(dt); rw->draw(_tutorial_textl1);
			_tutorial_textl2.update(dt); rw->draw(_tutorial_textl2);
			_level.update(dt); rw->draw(_level);
		}
	};

	virtual void listen(const posts::display_event<posts::display_event_mode::operations_central>& t) override {
		_set_buttons(_cental_btns, t.operations);
	}

	virtual void listen(const posts::display_event<posts::display_event_mode::operations_aux>& t) override {
		_set_buttons(_aux_btns, t.operations);
	}

	virtual void listen(const posts::display_event<posts::display_event_mode::operations_redraw>& t) override {
		for (auto& i : _cental_btns) i.set_string(i.get_string());
		for (auto& i : _aux_btns) i.set_string(i.get_string());
	}

	virtual void listen(const posts::display_event<posts::display_event_mode::disable>& t) override {
		// only handles enable/disable for central buttons
		for (auto& i : _cental_btns) {
			if (t.to) i.disable();
			else i.enable();
		}
	}

	virtual void listen(const posts::display_event<posts::display_event_mode::normal_text>& t) override {
		_level.set_string(t.level);
		_moves.set_string(t.moves);
		_target.set_string(t.target);
		_primary.set_string(t.primary);

		_primary.set_flash_mode(t.primary_mode);
		_level.set_flash_mode(t.level_mode);
		_moves.set_flash_mode(t.moves_mode);
		_target.set_flash_mode(t.target_mode);
	}

	virtual void listen(const posts::display_event<posts::display_event_mode::tutorial_text>& t) override {
		_level.set_string(t.level);

		auto fitting_chars = _tutorial_textl1.get_fitting_characters();
		auto words = util::split(t.value);

		std::size_t index = 0;
		std::string line = "";

		while (line.size() < fitting_chars && index < words.size())
			line.append(words[index++] + " ");

		if (index != words.size()) {
			_tutorial_textl1.set_string(util::strip(line));

			line = "";
			while (line.size() < fitting_chars && index < words.size())
				line.append(words[index++] + " ");

			if (index != words.size())
				std::cout << "warning: some words had to be truncated to fit on screen" << std::endl;

			_tutorial_textl2.set_string(util::strip(line));
		} else {
			_tutorial_textl1.set_string("");
			_tutorial_textl2.set_string(util::strip(line));
		}
	}

	virtual void listen(const event::sf_event::Resized& t) override {
		std::cout << "warning: illegal view resize not disabled by WM" << std::endl;
	}

	virtual void listen(const event::sf_event::KeyPressed& t) override {
		// trigger button 1
		if (t.value.key.code == sf::Keyboard::Space) {
			_aux_btns[0].simulate_click_pressed();
		}
	}

	virtual void listen(const event::sf_event::KeyReleased& t) override {
		// trigger button 1
		if (t.value.key.code == sf::Keyboard::Space) {
			_aux_btns[0].simulate_click_released();
		}
	}
};

int main(int argc, char* argv[]) {
	// asynchronous loading for so few resources is uncessiary
	resource_manager<sf::Image>::load("favicon", "res/favicon.jpg");
	resource_manager<sf::Font>::load("roboto", "res/Roboto-Thin.ttf");

	game_renderer g;

	core::run(g);
};
