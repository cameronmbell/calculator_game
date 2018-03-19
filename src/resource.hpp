#ifndef _RESOURCE_HPP
#define _RESOURCE_HPP

#include "util.hpp"

#include <unordered_map>
#include <exception>
#include <iostream>
#include <memory>

class resource_load_exception : std::runtime_error {
public:
	resource_load_exception(const std::string& path)
		: std::runtime_error("could not load file: " + path) {
	};

	virtual const char* what() const noexcept override{
		return std::runtime_error::what();
	};
};

template <typename T>
class resource_manager {
public:
	static void load(const std::string& id, const std::string& path) {
		T t;
		if (!t.loadFromFile(path))
			throw resource_load_exception(path);

		resource_manager<T>::push(id, std::move(t));
	}

	static void push(const std::string& id, T&& res) {
		auto idx = _res.find(id);
		if (idx != _res.cend()) {
			_res.erase(idx);
		}

		_res[id] = util::make_unique<T>(res);

		std::cout << "got resource: " << id << std::endl;
	}

	static T& get(const std::string& id) {
		if (_res.find(id) == _res.cend())
			throw std::logic_error("no such key");

		return *_res[id];
	}

private:
	static std::unordered_map<std::string, std::unique_ptr<T>> _res;
};

template<typename T>
std::unordered_map<std::string, std::unique_ptr<T>> resource_manager<T>::_res;


#endif // _RESOURCE_HPP
