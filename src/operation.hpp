#ifndef _OPERATION_HPP
#define _OPERATION_HPP

#include <stdexcept>
#include <iostream>
#include <string>
#include <memory>

#include "manager.hpp"
#include "button.hpp"

struct operation_exception : public std::runtime_error {
	operation_exception(const std::string& what_arg="op err") : std::runtime_error(what_arg) {}
	virtual const char* what() const noexcept override { return std::runtime_error::what(); }
};

class basic_operation {
public:
	virtual void call(level* l)=0;
	virtual std::string get_string() const noexcept=0;
};

class operational_button : public basic_button {
public:
	operational_button()
		: basic_button(), _op(nullptr) {
	}

	operational_button(const button_style& style, std::shared_ptr<basic_operation> op)
		: basic_button(style) {
		set_operation(std::move(op));
	}

	void set_operation(std::shared_ptr<basic_operation> op) {
		_op = op;
		set_string(_op->get_string());
	}

	void remove_operation() {
		_op = nullptr;
		set_string("");
	}

	virtual void on_click() override {
		if (_op != nullptr)
			_op->call(level::get());
	}

	std::string get_string() {
		if (_op != nullptr)
			return _op->get_string();
		else
			return "nullptr";
	}

private:
	std::shared_ptr<basic_operation> _op;
};

namespace default_operation {
	class nop : public basic_operation {
	public:
		virtual void call(level* l) override { }
		virtual std::string get_string() const noexcept override { return ""; }
	};

	class ac : public basic_operation {
	public:
		virtual void call(level* l) override { level::reset(); }
		virtual std::string get_string() const noexcept override { return "AC"; }
	};

	class next : public basic_operation {
	public:
		virtual void call(level* l) override { level::next(); }
		virtual std::string get_string() const noexcept override { return "NEXT"; }
	};

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

	class tutorial : public text {
	public:
		tutorial(const std::string& s) : text(s) { }

		static void initialize(const std::vector<std::pair<std::string, std::string>>& pairs) {
			if (pairs.size() == 0)
				throw std::logic_error("blank tutorial");

			_idx = 0;
			_data = pairs;
			update();
		}

		static void update() {
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

		virtual void call(level* l) override {
			tutorial::update();
		}

	private:
		static std::vector<std::pair<std::string, std::string>> _data;
		static std::size_t _idx;
	};

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

	class settings : public basic_operation {
	public:
		settings()=default;

		virtual std::string get_string() const noexcept override { return "SET"; }
		virtual void call(level* l) override { settings::instantiate(); }

		static void instantiate() {
			auto increase_level = [](untyped* self) -> void {
				if (level::get_max() > level::get_current()) { level::next(false); settings::instantiate(); }
			};
			auto decrease_level = [](untyped* self) -> void {
				if (level::get_current() > 0) { level::previous(false); settings::instantiate(); }
			};

			posts::operations::set_aux(make_operations(
				untyped("+", increase_level),
				untyped(util::as_string(level::get_current()), [](untyped* self) -> void {}),
				untyped("-", decrease_level)
			));

			posts::operations::set_central(make_operations(
				untyped("<", [](untyped* self) -> void { level::run(); }),
				nop(),
				untyped("EXIT", [](untyped* self) -> void { std::exit(0); })
			));
		};
	};

	class help : public basic_operation {
	public:
		help()=default;

		virtual std::string get_string() const noexcept override { return "HELP"; }
		virtual void call(level* l) override {
			std::cout << "warning: did nothing" << std::endl;
		}
	};
}

class numeric_operation : public basic_operation {
	virtual std::string get_string() const noexcept override=0;
	virtual int perform(int on)=0;

	virtual void call(level* l) override {
		int result;

		try {
			result = this->perform(posts::text::numeric::get_primary());
		} catch (operation_exception& e) {
			posts::text::string::set_primary("ERR!", flash_mode::indefinite | flash_mode::slow);
			posts::operations::set_generic_ac();

			return;
		}

		std::cout << "performed " << posts::text::numeric::get_primary() << " " << this->get_string() << " = " << result << std::endl;

		posts::text::numeric::set_primary<false>(result, flash_mode::one_shot | flash_mode::quick);
		posts::text::numeric::set_moves<false>(posts::text::numeric::get_moves() - 1);

		if (posts::text::numeric::get_target() == posts::text::numeric::get_primary()) {
			posts::text::string::set_primary("WIN!", flash_mode::indefinite | flash_mode::slow);
			posts::operations::disable_central();
			posts::operations::set_generic_next();
		} else if (posts::text::numeric::get_moves() <= 0) {
			posts::text::string::set_primary("LOSE!", flash_mode::indefinite | flash_mode::slow);
			posts::text::string::set_moves(posts::text::string::get_moves(), flash_mode::thrice | flash_mode::quick);
			posts::operations::disable_central();
			posts::operations::set_generic_ac();
		} else posts::text::post();
	}
};

namespace default_operation {
	class add : public numeric_operation {
	public:
		explicit add(int n) : _n(n) { }
		virtual std::string get_string() const noexcept override { return "+" + util::as_string(_n); }
		virtual int perform(int on) override { return on + _n; }
	private:
		int _n;
	};

	class sub : public numeric_operation {
	public:
		explicit sub(int n) : _n(n) { }
		virtual std::string get_string() const noexcept override { return "-" + util::as_string(_n); }
		virtual int perform(int on) override { return on - _n; }
	private:
		int _n;
	};

	class mul : public numeric_operation {
	public:
		explicit mul(int n) : _n(n) { }
		virtual std::string get_string() const noexcept override { return "x" + util::as_string(_n); }
		virtual int perform(int on) override { return on * _n; }
	private:
		int _n;
	};

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

	class sign_invert : public numeric_operation {
	public:
		explicit sign_invert() { }
		virtual std::string get_string() const noexcept override { return "+/-"; }
		virtual int perform(int on) override {
			return on * -1;
		}
	};

	class sign_posative : public numeric_operation {
	public:
		explicit sign_posative() { }
		virtual std::string get_string() const noexcept override { return "|x|"; }
		virtual int perform(int on) override {
			return std::abs(on);
		}
	};

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
