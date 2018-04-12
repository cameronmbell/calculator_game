/*
 * core.hpp:
 * create a 'runnable' interface representative of a class that is attached to an SFML window
 */

#ifndef _CORE_HPP
#define _CORE_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "util.hpp"

namespace core {
	class runnable {
	public:

		// override-able window generator, acts as program initializer
		virtual std::unique_ptr<sf::RenderWindow> setup() {
			return util::make_unique<sf::RenderWindow>(sf::VideoMode::getDesktopMode(), "my app", sf::Style::Default);
		}

		// pure abstract update, draw routine to be called per update
		// dt is a calculated delta time i.e. the ms/1000 passed since last update
		virtual void update(sf::RenderWindow* rw, float dt)=0;
	};

	// enter a main loop with calls to a runnable
	void run(runnable& r);
}

#endif // _CORE_HPP
