#ifndef _OPERATION_HPP
#define _OPERATION_HPP

#include <stdexcept>
#include <iostream>
#include <string>
#include <memory>

#include "button.hpp"
#include "level.hpp"

// basically each operation is passed the current display
// they are then free to do what ever with it
// all the game_manager does is graphical shit
// the actual gameplay is handled by the display class

class operation_exception : public std::runtime_error {
public:
	operation_exception(const std::string& what_arg)
		: std::runtime_error(what_arg) {
	}

	virtual const char* what() const noexcept override {
		return std::runtime_error::what();
	}
};

class basic_operation {
public:
	virtual void call() { }
	virtual std::string get_string() const noexcept { return ""; }
};

class operational_button : public basic_button {
public:
	operational_button(std::shared_ptr<basic_operation> op, const button_style& style)
		: basic_button(style), _op(op) {
		set_string(_op->get_string());
	}

	void set_operation(std::shared_ptr<basic_operation> op) {
		_op = op;
		set_string(_op->get_string());
	}

	void call_operation() {
		_op->call();
	}

	virtual void on_click() override {
		_op->call();
	}

	std::string get_operation_string() {
		return _op->get_string();
	}

private:
	std::shared_ptr<basic_operation> _op;
};

template <typename T, typename... Tp>
std::shared_ptr<basic_operation> make_operation(Tp&&... args) {
	return std::shared_ptr<basic_operation>(std::forward<Tp>(args)...);
}

namespace operation {
	const auto nop=std::make_shared<basic_operation>();

	class tutorial : public basic_operation {
		using Tp=std::pair<const std::string, const std::string>;

	public:
		tutorial(std::vector<Tp> data)
			: _payload(data), _index(0) {
		}

		virtual std::string get_string() const noexcept override {
			return _payload[std::min(_payload.size()-1, _index)].first;
		}

		virtual void call() override {
			_index++; // this must come first, for reasons

			if (_index <= _payload.size()) {
				level_manager::set(_payload[_index-1].second);
			} else {
				level_manager::next();
			}
		}

	private:
		std::vector<Tp> _payload;
		std::size_t _index;
	};

	class level_switch : public basic_operation {
	public:
		level_switch(const std::string& s)
			: _text_data(s) {
		}

		virtual std::string get_string() const noexcept override {
			return _text_data;
		}

		virtual void call() override {
			level_manager::next();
		}

	private:
		std::string _text_data;
	};

	class ac : public basic_operation {
	public:
		virtual std::string get_string() const noexcept override {
			return "AC";
		}

		virtual void call() override {
			level_manager::reset();
		}
	};

	class add : public basic_operation {
	public:
		add(int payload)
			: _payload(payload) {
		}

		virtual std::string get_string() const noexcept override {
			return "+" + util::as_string(_payload);
		}

		virtual void call() override {
			level_manager::set(
				level_manager::get() + _payload);
		}

	private:
		int _payload;
	};

	class mul : public basic_operation {
	public:
		mul(int payload)
			: _payload(payload) {
		}

		virtual std::string get_string() const noexcept override {
			return "x" + util::as_string(_payload);
		}

		virtual void call() override {
			level_manager::set(
				level_manager::get() * _payload);
		}

	private:
		int _payload;
	};
};

//	class div_operation : public basic_operation {
//	public:
//		div_operation(int data) : basic_operation(data) {
//			if (_data == 0)
//				throw std::logic_error("divide by zero");
//		}
//
//		virtual std::string get_string() const override {
//			return "/" + util::as_string(_data);
//		}
//
//		virtual int call(int in) const override {
//			if (in % _data != 0)
//				throw operation_exception("float result");
//
//			return in / _data;
//		}
//	};
//
//	class cat_operation : public basic_operation {
//	public:
//		cat_operation(int data) : basic_operation(data) {}
//
//		virtual std::string get_string() const override {
//			return util::as_string(_data);
//		}
//
//		virtual int call(int in) const override {
//			int sign = (in < 0) ? -1 : 1;
//
//			in *= sign;
//
//			int _data_cpy = _data;
//			for (; _data_cpy != 0; _data_cpy /= 10) {
//				in *= 10;
//				in += _data % 10;
//			}
//
//			return in * sign;
//		}
//	};

#endif // _OPERATION_HPP
