/*
 * event.hpp
 * an interface for posting and listening to events across classes
 * the event system is single threaded but could easily be pareleised
 * a naieve culmination of the ideas in many existing event libraries, namely:
 * events - https://github.com/LB--/events
 * Events - https://github.com/Submanifold/Events
 * events - https://github.com/MCGallaspy/events
 */

#ifndef _EVENT_HPP
#define _EVENT_HPP

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <unordered_map>
#include <iostream>
#include <functional>
#include <vector>

namespace event
{
	// a pure abstract class that listens to events of type T
	// these are passed to void listen(const T&);
	template <typename ...T>
	class class_listener;

	// a lambda/std::bind/function ptr to listens to an event
	template <typename T>
	using lambda_listener = std::function<void(const T& t)>;

	// a way of separating the union of sf::Events into separate types
	// provides a way of listening to sf events
	// unfortunatly this creates a messy interface
	template <sf::Event::EventType E>
	struct sf_typed_event {
		sf_typed_event(const sf::Event& e, sf::RenderWindow* rw_arg)
			: type(e.type), value(e), rw(rw_arg) {
		}

		const sf::Event::EventType type;
		const sf::Event value;
		sf::RenderWindow* rw;
	};

	// (static) class for posting events and
	// connecting a listener to a type dispatch
	template <typename T>
	class dispatch;

	// a connection between dispatch and listener
	// a connection can break itself, meaning ignore future event dispatches
	template <typename T>
	class connection {
	public:
		connection(std::size_t id)
			: _id(id) {
		}

		constexpr std::size_t get_id() const noexcept {
			return _id;
		}

		void disconnect() {
			dispatch<T>::disconnect(*this);
		}

		virtual ~connection()=default;

	private:
		std::size_t _id;
	};

	template <typename T>
	class dispatch {
	public:
		dispatch()=delete;

		// push a lambda_listener onto the list of listeners, so that it
		// recieves events from any post on dispatch of T type
		// also return the id of this connection, so connections can break
		static constexpr connection<T> connect(lambda_listener<T> thing) {
			_listeners[_id] = thing;

			auto ret_val = connection<T>(_id);

			_id++;

			return ret_val;
		}
		// connect for class_listener, really the same as for a lambda_listener
		// except have to get an std::mem_fun to be called from within a lambda
		// need to consider both the performance of this and potential for dangling pointer
		static connection<T> connect(class_listener<T>* thing) {
			return connect([thing](const T& t) -> void {
				std::mem_fun(&class_listener<T>::listen)(thing, t);
			});
		}

		// disconnect an event_pair, that is: a listener from this dispatch
		static void disconnect(const connection<T>& con) {
			for (auto it = _listeners.begin(); it != _listeners.end();) {
				if (it->first == con.get_id())
					it = _listeners.erase(it);
				else
					it++;
			}

		}

		// send out data to all connected listeners
		static void post(const T& t) {
			for (const auto& x : _listeners) {
				x.second(t);
			}
		}

	private:
		static std::size_t _id;
		static std::unordered_map<std::size_t, lambda_listener<T>> _listeners;
	};

	// little trick that allows separate data for dispatch of each type
	template <typename T> std::size_t dispatch<T>::_id;
	template <typename T> std::unordered_map<std::size_t, lambda_listener<T>> dispatch<T>::_listeners;

	// create overloads of on_event() for each type passed to class_listener
	// idea from MCGallaspy's events (recursive template inheritance for type overloading)
	template <>
	class class_listener<> { };

	template <typename A, typename ...B>
	class class_listener<A, B...> : public class_listener<A>, public class_listener<B...> { };

	template <typename T>
	class class_listener<T> {
	public:

		// this will actually establish a connection for each template param
		// but all these connections have the same id, which doesn't really matter
		explicit class_listener()
			: _connection(dispatch<T>::connect(this)) {
		};

		virtual ~class_listener() {
			_connection.disconnect();
		};

		virtual void listen(const T& t)=0;

	private:
		connection<T> _connection;
	};

	// to create a nicer interface around sf_typed_event
	namespace sf_event {
		using Closed=sf_typed_event<sf::Event::EventType::Closed>;
		using Resized=sf_typed_event<sf::Event::EventType::Resized>;
        using LostFocus=sf_typed_event<sf::Event::EventType::LostFocus>;
        using GainedFocus=sf_typed_event<sf::Event::EventType::GainedFocus>;
        using TextEntered=sf_typed_event<sf::Event::EventType::TextEntered>;
        using KeyPressed=sf_typed_event<sf::Event::EventType::KeyPressed>;
        using KeyReleased=sf_typed_event<sf::Event::EventType::KeyReleased>;
        using MouseWheelMoved=sf_typed_event<sf::Event::EventType::MouseWheelMoved>;
        using MouseWheelScrolled=sf_typed_event<sf::Event::EventType::MouseWheelScrolled>;
        using MouseButtonPressed=sf_typed_event<sf::Event::EventType::MouseButtonPressed>;
        using MouseButtonReleased=sf_typed_event<sf::Event::EventType::MouseButtonReleased>;
        using MouseMoved=sf_typed_event<sf::Event::EventType::MouseMoved>;
        using MouseEntered=sf_typed_event<sf::Event::EventType::MouseEntered>;
        using MouseLeft=sf_typed_event<sf::Event::EventType::MouseLeft>;
        using JoystickButtonPressed=sf_typed_event<sf::Event::EventType::JoystickButtonPressed>;
        using JoystickButtonReleased=sf_typed_event<sf::Event::EventType::JoystickButtonReleased>;
        using JoystickMoved=sf_typed_event<sf::Event::EventType::JoystickMoved>;
        using JoystickConnected=sf_typed_event<sf::Event::EventType::JoystickConnected>;
        using JoystickDisconnected=sf_typed_event<sf::Event::EventType::JoystickDisconnected>;
        using TouchBegan=sf_typed_event<sf::Event::EventType::TouchBegan>;
        using TouchMoved=sf_typed_event<sf::Event::EventType::TouchMoved>;
        using TouchEnded=sf_typed_event<sf::Event::EventType::TouchEnded>;
        using SensorChanged=sf_typed_event<sf::Event::EventType::SensorChanged>;
	};
};

#endif // _EVENTS_HPP
