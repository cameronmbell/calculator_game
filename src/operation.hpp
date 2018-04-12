/*
 * operation.hpp:
 * defines an abstract base which allows for different behavior to be attached to a button
 * and derived classes that implement this behavior
 */

#ifndef _OPERATION_HPP
#define _OPERATION_HPP

#include <stdexcept>
#include <iostream>
#include <string>
#include <memory>

#include "manager.hpp"
#include "button.hpp"

// if an operation fails, throw this
// e.g. 5/2 throws an exception due to it's resulting float
struct operation_exception : public std::runtime_error {
	operation_exception(const std::string& what_arg="op err") : std::runtime_error(what_arg) {}
	virtual const char* what() const noexcept override { return std::runtime_error::what(); }
};

// the abstract base that's polymorphically attached to buttons
class basic_operation {
public:
	virtual void call(level* l)=0;
	virtual std::string get_string() const noexcept=0;
};

// a button that calls a basic_operation
class operational_button : public basic_button {
public:
	operational_button()
		: basic_button(), _op(nullptr) {
	}

	// initialize basic_button
	operational_button(const button_style& style, std::shared_ptr<basic_operation> op)
		: basic_button(style) {
		set_operation(std::move(op));
	}

	// update internal operation
	void set_operation(std::shared_ptr<basic_operation> op) {
		_op = op;
		set_string(_op->get_string());
	}

	// deallocate internal operation
	void remove_operation() {
		_op = nullptr;
		set_string("");
	}

	// call internal operation
	// done when a mouse click is fired (from base class)
	virtual void on_click() override {
		if (_op != nullptr)
			_op->call(level::get());
	}

	// get internal operation string
	std::string get_string() {
		if (_op != nullptr)
			return _op->get_string();
		else
			return "nullptr";
	}

private:
	std::shared_ptr<basic_operation> _op;
};

// a large set of default operations that derive from basic_operation
namespace default_operation {

	// do nothing
	class nop : public basic_operation {
	public:
		virtual void call(level* l) override { }
		virtual std::string get_string() const noexcept override { return ""; }
	};

	// reset the level
	class ac : public basic_operation {
	public:
		virtual void call(level* l) override { level::reset(); }
		virtual std::string get_string() const noexcept override { return "AC"; }
	};

	// go to the next level
	class next : public basic_operation {
	public:
		virtual void call(level* l) override { level::next(); }
		virtual std::string get_string() const noexcept override { return "NEXT"; }
	};

	// display some text
	class text : public basic_operation {
	public:
		text(const std::string& s)
			: _payload(s) {
		}

		virtual void call(level* l) override {
			std::cout << "warning: click on text button ignored" << std::endl;
		}

		virtual std::string get_string() const noexcept override {
			return _payload;
		}

	private:
		std::string _payload;
	};

	// manage a tutorial
	class tutorial : public text {
	public:
		tutorial(const std::string& s) : text(s) { }

		// bootstrap this give some tutorial data
		static void initialize(const std::vector<std::pair<std::string, std::string>>& pairs) {
			if (pairs.size() == 0)
				throw std::logic_error("blank tutorial");

			_idx = 0;
			_data = pairs;
			update();
		}

		// update the tutorial: go to the next string
		static void update() {

			// generate instances of tutorial and update primary text
			// until the tutorial is finished, in which case:
			// show a next button
			if (_idx >= _data.size() - 1) {
				posts::tutorial_text::set(_data[_idx].first);
				posts::operations::set_central(make_operations());
				posts::operations::disable_central();
				posts::operations::set_just_next();
			} else {
				auto& p = _data[_idx++];
				posts::tutorial_text::set(p.first);
				posts::operations::set_central(make_operations(tutorial(p.second)));
			}
		}

		// instance must call static update routine
		virtual void call(level* l) override {
			tutorial::update();
		}

	private:

		// storage specified in manager.cpp
		static std::vector<std::pair<std::string, std::string>> _data;
		static std::size_t _idx;
	};

	// an operation that calls a lambda literal
	class untyped : public basic_operation {
	public:
		untyped(const std::string& s, std::function<void(untyped* self)> f)
			: _s(s), _f(f) {
		}

		void set_string(const std::string& to) { _s = to; posts::operations::post_redraw(); }
		virtual std::string get_string() const noexcept override { return _s; }
		virtual void call(level* l) override { _f(this); }

	private:
		std::string _s;
		std::function<void(untyped* self)> _f;
	};

	// the settings operation
	// change the display and allow for the level to be changed
	class settings : public basic_operation {
	public:
		settings()=default;

		virtual std::string get_string() const noexcept override { return "SET"; }
		virtual void call(level* l) override { settings::instantiate(); }

		// change the screen to display various lambda buttons
		static void instantiate() {
			posts::operations::enable_central();

			auto increase_level = [](untyped* self) -> void {
				if (level::get_max() > level::get_current()) { level::next(false); settings::instantiate(); }
			};

			auto decrease_level = [](untyped* self) -> void {
				if (level::get_current() > 1) { level::previous(false); settings::instantiate(); }
			};

			// set aux buttons to be + N -
			// allows for modification of current level
			posts::operations::set_aux(make_operations(
				untyped("+", increase_level),
				untyped(util::as_string(level::get_current()), [](untyped* self) -> void {}),
				untyped("-", decrease_level)
			));

			// set central buttons to < HELP EXIT
			// < restores the level
			// HELP loads the instruction level
			// EXIT quits execution
			posts::operations::set_central(make_operations(
				untyped("<", [](untyped* self) -> void { level::run(); }),
				untyped("HELP", [](untyped* self) -> void { level::load_instructions(); }),
				untyped("EXIT", [](untyped* self) -> void { std::exit(0); })
			));
		};
	};
}

// definition of an operation that operates on a number
// used to simplify the definition of all numerics
class numeric_operation : public basic_operation {
	virtual std::string get_string() const noexcept override=0;
	virtual int perform(int on)=0;

	// on click make call to perform and modify game based on resulting WIN/ERR/LOSE state
	virtual void call(level* l) override {
		int result;

		// attempt to perform the operation on the current primary
		// if it fails set the ERR! string and place an AC button
		try {
			result = this->perform(posts::text::numeric::get_primary());
		} catch (operation_exception& e) {
			posts::text::string::set_primary("ERR!", flash_mode::indefinite | flash_mode::slow);
			posts::operations::disable_central();
			posts::operations::set_generic_ac();

			return;
		}

		std::cout << "performed " << posts::text::numeric::get_primary() << " " << this->get_string() << " = " << result << std::endl;

		// flash an update to the primary int
		// and decrease the moves by 1
		posts::text::numeric::set_primary<false>(result, flash_mode::one_shot | flash_mode::quick);
		posts::text::numeric::set_moves<false>(posts::text::numeric::get_moves() - 1);

		// if the target number has been reached:
		// set WIN text and place a NEXT button
		if (posts::text::numeric::get_target() == posts::text::numeric::get_primary()) {
			posts::text::string::set_primary<false>("WIN!", flash_mode::indefinite | flash_mode::slow);
			posts::text::secondary_string::set<false>(util::as_string(posts::text::numeric::get_primary()));
			posts::text::post();

			posts::operations::disable_central();
			posts::operations::set_generic_next();
		}

		// if the moves dips below zero
		// set LOSE text and place a AC button
		else if (posts::text::numeric::get_moves() <= 0) {
			posts::text::string::set_moves<false>(posts::text::string::get_moves(), flash_mode::thrice | flash_mode::quick);
			posts::text::string::set_primary<false>("LOSE!", flash_mode::indefinite | flash_mode::slow);
			posts::text::secondary_string::set<false>(util::as_string(posts::text::numeric::get_primary()));
			posts::text::post();

			posts::operations::disable_central();
			posts::operations::set_generic_ac();
		}

		// simply post previous update to primary/moves
		else posts::text::post();
	}
};

namespace default_operation {

	// add N
	class add : public numeric_operation {
	public:
		explicit add(int n) : _n(n) { }
		virtual std::string get_string() const noexcept override { return "+" + util::as_string(_n); }
		virtual int perform(int on) override { return on + _n; }
	private:
		int _n;
	};

	// subtract N
	class sub : public numeric_operation {
	public:
		explicit sub(int n) : _n(n) { }
		virtual std::string get_string() const noexcept override { return "-" + util::as_string(_n); }
		virtual int perform(int on) override { return on - _n; }
	private:
		int _n;
	};

	// multiply by N
	class mul : public numeric_operation {
	public:
		explicit mul(int n) : _n(n) { }
		virtual std::string get_string() const noexcept override { return "x" + util::as_string(_n); }
		virtual int perform(int on) override { return on * _n; }
	private:
		int _n;
	};

	// divide by N and throw an exception if that produced a float
	class divi : public numeric_operation {
	public:
		explicit divi(int n) : _n(n) { }
		virtual std::string get_string() const noexcept override { return "/" + util::as_string(_n); }
		virtual int perform(int on) override {
			if (on % _n != 0)
				throw operation_exception("floating point");

			return on / _n;
		}

	private:
		int _n;
	};

	// mod N
	class mod : public numeric_operation {
	public:
		explicit mod(int n) : _n(n) { }
		virtual std::string get_string() const noexcept override { return "%" + util::as_string(_n); }
		virtual int perform(int on) override {
			return on % _n;
		}

	private:
		int _n;
	};

	// append N to the end
	// e.g:
	// 0 ... 2 -> 2
	// 42 ... 0 -> 420
	// 1234 ... 5 -> 12345
	class cat : public numeric_operation {
	public:
		explicit cat(int n) : _n(n) { }
		virtual std::string get_string() const noexcept override { return ".." + util::as_string(_n); }
		virtual int perform(int on) override {
			int sign = (on < 0) ? -1 : 1;

			on *= sign;

			int _data_cpy = _n;
			for (; _data_cpy != 0; _data_cpy /= 10) {
				on *= 10;
				on += _n % 10;
			}

			return on * sign;
		}

	private:
		int _n;
	};

	// remove the last digit from the end
	// semantically the same as x // 10
	class del : public numeric_operation {
	public :
		explicit del() { }
		virtual std::string get_string() const noexcept override { return "<<"; }
		virtual int perform(int on) override {
			return on / 10;
		}

	private:
		int _n;
	};

	// x * -1. multiply by -1
	class sign_invert : public numeric_operation {
	public:
		explicit sign_invert() { }
		virtual std::string get_string() const noexcept override { return "+/-"; }
		virtual int perform(int on) override {
			return on * -1;
		}
	};

	// |x|. take the absolute value of x
	class sign_posative : public numeric_operation {
	public:
		explicit sign_posative() { }
		virtual std::string get_string() const noexcept override { return "|x|"; }
		virtual int perform(int on) override {
			return std::abs(on);
		}
	};

	// x^n. raise x to the nth power
	class power : public numeric_operation {
	public :
		explicit power(int n) : _n(n) { }
		virtual std::string get_string() const noexcept override { return "x^" + util::as_string(_n); }
		virtual int perform(int on) override {
			return std::pow(on, _n);
		}

	private:
		int _n;
	};
};

#endif // !_OPERATION_HPP
