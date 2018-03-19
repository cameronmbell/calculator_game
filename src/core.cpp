#include "core.hpp"

namespace core {
	void run(runnable& r) {
		auto win = r.setup();

		sf::Clock clk;

		bool running = true;
		while (running) {
			auto e = sf::Event();
			while (win->pollEvent(e)) {
				switch (e.type) {
					case sf::Event::MouseMoved:
						event::dispatch<event::sf_event::MouseMoved>::post(event::sf_event::MouseMoved(e, win.get()));
						break;
					case sf::Event::KeyPressed:
						event::dispatch<event::sf_event::KeyPressed>::post(event::sf_event::KeyPressed(e, win.get()));
						break;
					case sf::Event::KeyReleased:
						event::dispatch<event::sf_event::KeyReleased>::post(event::sf_event::KeyReleased(e, win.get()));
						break;
					case sf::Event::MouseButtonPressed:
						event::dispatch<event::sf_event::MouseButtonPressed>::post(event::sf_event::MouseButtonPressed(e, win.get()));
						break;
					case sf::Event::MouseButtonReleased:
						event::dispatch<event::sf_event::MouseButtonReleased>::post(event::sf_event::MouseButtonReleased(e, win.get()));
						break;
					case sf::Event::MouseWheelMoved:
						event::dispatch<event::sf_event::MouseWheelMoved>::post(event::sf_event::MouseWheelMoved(e, win.get()));
						break;
					case sf::Event::MouseWheelScrolled:
						event::dispatch<event::sf_event::MouseWheelScrolled>::post(event::sf_event::MouseWheelScrolled(e, win.get()));
						break;
					case sf::Event::MouseEntered:
						event::dispatch<event::sf_event::MouseEntered>::post(event::sf_event::MouseEntered(e, win.get()));
						break;
					case sf::Event::MouseLeft:
						event::dispatch<event::sf_event::MouseLeft>::post(event::sf_event::MouseLeft(e, win.get()));
						break;
					case sf::Event::TextEntered:
						event::dispatch<event::sf_event::TextEntered>::post(event::sf_event::TextEntered(e, win.get()));
						break;
					case sf::Event::LostFocus:
						event::dispatch<event::sf_event::LostFocus>::post(event::sf_event::LostFocus(e, win.get()));
						break;
					case sf::Event::GainedFocus:
						event::dispatch<event::sf_event::GainedFocus>::post(event::sf_event::GainedFocus(e, win.get()));
						break;
					case sf::Event::Resized:
						event::dispatch<event::sf_event::Resized>::post(event::sf_event::Resized(e, win.get()));

						win->setView(sf::View(sf::FloatRect(0.0f, 0.0f, static_cast<float>(e.size.width), static_cast<float>(e.size.height))));
						break;
					case sf::Event::JoystickButtonPressed:
						event::dispatch<event::sf_event::JoystickButtonPressed>::post(event::sf_event::JoystickButtonPressed(e, win.get()));
						break;
					case sf::Event::JoystickButtonReleased:
						event::dispatch<event::sf_event::JoystickButtonReleased>::post(event::sf_event::JoystickButtonReleased(e, win.get()));
						break;
					case sf::Event::JoystickMoved:
						event::dispatch<event::sf_event::JoystickMoved>::post(event::sf_event::JoystickMoved(e, win.get()));
						break;
					case sf::Event::JoystickConnected:
						event::dispatch<event::sf_event::JoystickConnected>::post(event::sf_event::JoystickConnected(e, win.get()));
						break;
					case sf::Event::JoystickDisconnected:
						event::dispatch<event::sf_event::JoystickDisconnected>::post(event::sf_event::JoystickDisconnected(e, win.get()));
						break;
					case sf::Event::TouchBegan:
						event::dispatch<event::sf_event::TouchBegan>::post(event::sf_event::TouchBegan(e, win.get()));
						break;
					case sf::Event::TouchMoved:
						event::dispatch<event::sf_event::TouchMoved>::post(event::sf_event::TouchMoved(e, win.get()));
						break;
					case sf::Event::TouchEnded:
						event::dispatch<event::sf_event::TouchEnded>::post(event::sf_event::TouchEnded(e, win.get()));
						break;
					case sf::Event::SensorChanged:
						event::dispatch<event::sf_event::SensorChanged>::post(event::sf_event::SensorChanged(e, win.get()));
						break;
					case sf::Event::Closed:
						running = false;
						break;
					default:
						break;
				}
			}

			win->clear(sf::Color::Black);

			r.update(win.get(), clk.restart().asSeconds());

			win->display();
		}
	}
};
