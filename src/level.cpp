#include "level.hpp"
#include "operation.hpp"

level::level(int num, int targ, int mov, std::initializer_list<std::shared_ptr<basic_operation>> li)
	: number(num), target(targ), moves(mov), ops(li) {
}

level::level(std::initializer_list<std::pair<const std::string, const std::string>> l)
	: number(-1), target(-1), moves(-1),
	ops({std::make_shared<basic_operation>(operation::tutorial(l))}) {
}

namespace {
	int _number=0, _target=0, _moves=0, _level=0;
	std::vector<std::shared_ptr<basic_operation>> _ops({});

	std::size_t _level_index=0;
	std::vector<level> _levels = {
		level(2, 12, 3, { std::make_shared<operation::add>(2), std::make_shared<operation::mul>(2)} ),
		level({
			{ "hello", "hey!" },
			{ "welcome!", "let's play" }
		})
	};
};

namespace level_manager {
	void __post_display_event() {
		std::cout << "posted display event at level " << _level << std::endl;

		event::dispatch<display_event>::post({
			util::as_string(_number),
			util::as_string(_target),
			util::as_string(_moves),
			util::as_string(_level)
		});
	}

	void __post_button_event() {
		std::cout << "posted button event at level " << _level << std::endl;

		for (auto& i : _ops) {
			std::cout << i->get_string() << std::endl;
		}

		event::dispatch<button_event>::post({ _ops });
	}

	void make_post() {
		_number = _levels[_level_index].number;
		_target = _levels[_level_index].target;
		_moves = _levels[_level_index].moves;
		_level = _level_index + 1;

		auto& ref = _levels[_level_index].ops;
		_ops.assign(ref.cbegin(), ref.cend());

		__post_button_event();
		__post_display_event();
	}

	void reset() {
		_number = _levels[_level_index].number;
		_moves = _levels[_level_index].moves;

		__post_display_event();
	}

	void next() {
		_level_index++; // check for win

		_number = _levels[_level_index].number;
		_target = _levels[_level_index].target;
		_moves = _levels[_level_index].moves;
		_level = _level_index + 1;
		_ops = _levels[_level_index].ops;

		__post_button_event();
		__post_display_event();
	}

	void set(int to) {
		_number = to;

		if (_number == _target)
			next();
		else if (_moves == 0)
			reset();
		else
			__post_display_event();
	}

	// just for tutorial levels
	void set(const std::string& to) {
		event::dispatch<display_event>::post({
			to,
			util::as_string(_target),
			util::as_string(_moves),
			util::as_string(_level)
		});

		event::dispatch<button_event>::post({ _ops });
	}

	int get() {
		return _number;
	}
};
