#ifndef _CORE_HPP
#define _CORE_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <memory>

#include "event.hpp"
#include "util.hpp"

namespace core {
	class runnable {
	public:
		virtual std::unique_ptr<sf::RenderWindow> setup() {
			return util::make_unique<sf::RenderWindow>(sf::VideoMode::getDesktopMode(), "my app", sf::Style::Default);
		}

		virtual void update(sf::RenderWindow* rw, float dt)=0;
	};

	void run(runnable& r);
}

#endif // _CORE_HPP
