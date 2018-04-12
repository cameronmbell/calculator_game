/*
 * resource.hpp:
 * load and get resources from a file
 */

#ifndef _RESOURCE_HPP
#define _RESOURCE_HPP

#include "util.hpp"

#include <unordered_map>
#include <exception>
#include <iostream>
#include <memory>

// in the case where a file does not exist, this object is thrown
class resource_load_exception : std::runtime_error {
public:
	resource_load_exception(const std::string& path)
		: std::runtime_error("could not load file: " + path) {
	};

	virtual const char* what() const noexcept override{
		return std::runtime_error::what();
	};
};

// given a (conforming SFML) type, load files into memory
// e.g. resource_manager<sf::SoundBuffer>
template <typename T>
class resource_manager {
public:
	// load a resource from a file path and attach an id to it
	// decltype(T) must contain a member function loadFromFile
	static void load(const std::string& id, const std::string& path) {
		T t;

		if (!t.loadFromFile(path))
			throw resource_load_exception(path);

		resource_manager<T>::push(id, std::move(t));
	}

	// load a resource literal and attach an id to it
	static void push(const std::string& id, T&& res) {

		// if a resource already exists at that id erase it
		auto idx = _res.find(id);
		if (idx != _res.cend()) {
			_res.erase(idx);
		}

		_res[id] = util::make_unique<T>(res);

		std::cout << "got resource: " << id << std::endl;
	}

	// return the resource attached to a certain id
	// if non exists throw an std::logic_error
	static T& get(const std::string& id) {
		if (_res.find(id) == _res.cend())
			throw std::logic_error("no such key");

		return *_res[id];
	}

private:

	// the data store
	static std::unordered_map<std::string, std::unique_ptr<T>> _res;
};

// define resource declaration
template<typename T>
std::unordered_map<std::string, std::unique_ptr<T>> resource_manager<T>::_res;


#endif // _RESOURCE_HPP
