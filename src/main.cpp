#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <thread>
#include <vector>

#include "level.hpp"
#include "operation.hpp"
#include "resource.hpp"
#include "button.hpp"
#include "core.hpp"
#include "util.hpp"
#include "text.hpp"

class game_renderer :
	public core::runnable,
	public event::class_listener<
		event::sf_event::Resized,
		event::sf_event::KeyPressed,
		event::sf_event::KeyReleased,
		level_manager::display_event,
		level_manager::button_event> {
private:
	std::array<operational_button, 9> _btns;
	aligned_text _display, _level, _moves, _target;

public:
	game_renderer()
		: _btns ({{
			operational_button(operation::nop, button_style::default_grey),
			operational_button(operation::nop, button_style::default_grey),
			operational_button(operation::nop, button_style::default_orange),
			operational_button(operation::nop, button_style::default_grey),
			operational_button(operation::nop, button_style::default_grey),
			operational_button(operation::nop, button_style::default_orange),
			operational_button(operation::nop, button_style::default_grey),
			operational_button(operation::nop, button_style::default_grey),
			operational_button(operation::nop, button_style::default_orange)
		}}) {
	}

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

		for (int i = 0; i < 9; i++) {
			_btns[i].set_font(resource_manager<sf::Font>::get("roboto"));
			_btns[i].set_bounds(sf::FloatRect(
					(i % 3) * (win->getSize().x / 3),
					(win->getSize().y / 4) * (1 + (i / 3)),
					win->getSize().x / 3, win->getSize().y / 4));
		}

		for (auto it : { &_display, &_moves, &_level, &_target }) {
			it->set_font(resource_manager<sf::Font>::get("roboto"), 18);
			it->set_bounds(sf::FloatRect(16, 16, win->getSize().x - 32, win->getSize().y / 4 - 32));
			it->set_colour(sf::Color::White);
		}

		_display.set_alignment(text_align::right | text_align::bottom);
		_moves.set_alignment(text_align::middle | text_align::top);
		_level.set_alignment(text_align::left | text_align::top);
		_target.set_alignment(text_align::right | text_align::top);

		_display.set_font_size(108);

		// to bootstrap the game, force the level_manager to dispatch it's events
		level_manager::make_post();

		return win;
	}

	virtual void update(sf::RenderWindow* rw, float dt) override {
		for (auto& it: _btns) {
			it.fade(dt);
			rw->draw(it);
		}

		rw->draw(_moves);
		rw->draw(_target);
		rw->draw(_level);
		rw->draw(_display);
	};

	virtual void listen(const level_manager::button_event& t) override {
		std::cout << "level_manager::button_event" << std::endl;

		for (std::size_t i = 0; i < std::min(t.ops.size(), _btns.size()); i++) {
			std::cout << "button " << i << " has str " << t.ops[i]->get_string() << std::endl;
			_btns[i].set_operation(t.ops[i]); // should use nice alignment algorithm
		}
	}

	virtual void listen(const level_manager::display_event& t) override {
		_moves.set_string("moves: " + t.moves);
		_target.set_string("target: " + t.target);
		_level.set_string("level: " + t.level);
		_display.set_string(t.number);
	}

	virtual void listen(const event::sf_event::Resized& t) override {
		std::cout << "warning: view resize" << std::endl;
	}

	virtual void listen(const event::sf_event::KeyPressed& t) override {
		// as an example trigger button 1
		if (t.value.key.code == sf::Keyboard::Space) {
			_btns[0].simulate_click_pressed();
		}
	}

	virtual void listen(const event::sf_event::KeyReleased& t) override {
		// as an example trigger button 1
		if (t.value.key.code == sf::Keyboard::Space) {
			_btns[0].simulate_click_released();
		}
	}
};

int main(int argc, char* argv[]) {
	resource_manager<sf::Image>::load("favicon", "res/favicon.jpg");
	resource_manager<sf::Font>::load("roboto", "res/Roboto-Thin.ttf");

	game_renderer g;

	core::run(g);
};
