#include "manager.hpp"
#include "operation.hpp"

// specify storage of tutorial operation data within this translation unit
std::vector<std::pair<std::string, std::string>> default_operation::tutorial::_data;
std::size_t default_operation::tutorial::_idx;

void level::instantiate(std::size_t l) const {
	std::cout << "level " << l << " instantiated" << std::endl;

	_lvl = l;
	posts::operations::enable_central();

	switch (type) {
		case mode::numeric:
			posts::text::numeric::set_primary<false>(primary, flash_mode::thrice | flash_mode::quick);
			posts::text::numeric::set_moves<false>(moves);
			posts::text::numeric::set_target<false>(target);
			posts::text::numeric::set_level<false>(_lvl);

			posts::operations::set_central(operations);
			posts::operations::set_generic_ac();
			posts::text::post();
			break;
		case mode::tutorial:
			posts::operations::set_aux({});
			posts::text::numeric::set_level(_lvl);
			default_operation::tutorial::initialize(tutorial_text);
			break;
		default:
			std::cout << "warning: unhandled level type" << std::endl;
	}
}

namespace {
	class persistent_stringify {
		int _integral;
		std::string _string;
		const std::string _fmt;
		flash_mode_t _flash;

	public:
		persistent_stringify(const std::string& fmt="")
			: _integral(), _string(), _fmt(fmt), _flash(flash_mode::none) { }

		void set(int to, flash_mode_t m=flash_mode::none) {
			_flash = m;
			_integral = to;
			_string = _fmt + util::as_string(to);
		}

		void set(const std::string& to, flash_mode_t m=flash_mode::none) {
			_flash = m;
			_string = to;
		}

		int get_integral() const { return _integral; }
		std::string get_string() const { return _string; }
		flash_mode_t get_mode() const { return _flash; }
	};

	template <typename T, bool Should>
	struct auto_poster {
		static void post(const T& t) { }
	};

	template <typename T>
	struct auto_poster<T, true> {
		static void post(const T& t) {
			event::dispatch<T>::post(std::move(t));
		}
	};

	persistent_stringify _primary, _moves("moves: "), _target("target: "), _level("level: ");
	make_operations::type _central_operations;
	make_operations::type _aux_operations;
	std::string _tutorial_text;

	level::mode _last_mode;
	std::size_t _lm_index = 0, _lm_max_index = 0;
	std::vector<std::shared_ptr<level>> _lm_lvls ({
		std::make_shared<level>(level{
			{ "hey there!", "hi" },
			{ "can you help me?", "sure" },
			{ "the engineers who designed me was very lazy", "next" },
			{ "the only way I can calculate is with your help", "okay" },
			{ "I'll give you a number and a target", "okay" },
			{ "you need to press buttons to get to the target", "okay" },
			{ "oh here's a problem now...", "" }
		}),

		// starting number, moves allowed, target number, ....
		std::make_shared<level>(0, 3, 3, make_operations(default_operation::add(1))),

		std::make_shared<level>(level{
			{ "you're already a master", "thanks" },
			{ "see if you can do these...", "" }
		}),

		std::make_shared<level>(2, 2, 5, make_operations(default_operation::add(1), default_operation::add(2))),
		std::make_shared<level>(5, 3, 21, make_operations(default_operation::add(7), default_operation::add(2))),
		std::make_shared<level>(10, 5, 2, make_operations(default_operation::sub(7), default_operation::add(2))),
		std::make_shared<level>(-3, 3, -4, make_operations(default_operation::sub(2), default_operation::add(3))),
		std::make_shared<level>(0, 3, 8, make_operations(default_operation::mul(3), default_operation::add(2))),
		std::make_shared<level>(3, 4, -16, make_operations(default_operation::mul(-2), default_operation::add(2))),
		std::make_shared<level>(4, 3, 126, make_operations(default_operation::mul(6), default_operation::sub(3))),
		std::make_shared<level>(7, 4, 56, make_operations(default_operation::mul(-4), default_operation::add(7))),
		std::make_shared<level>(100, 3, 5, make_operations(default_operation::divi(5), default_operation::add(5))),
		std::make_shared<level>(52, 3, 12, make_operations(default_operation::divi(2), default_operation::sub(2))),

		std::make_shared<level>(level{
			{ "you seem to know your arithmetic pretty well", "thanks" },
			{ "but there's more to a calculator than just that", "like?" },
			{ "A new button has been added... good luck", "" }
		}),

		std::make_shared<level>(2, 3, 2112, make_operations(default_operation::cat(1), default_operation::cat(2))),
		std::make_shared<level>(0, 4, 42, make_operations(default_operation::cat(1), default_operation::mul(2))),
		std::make_shared<level>(25, 4, 111, make_operations(default_operation::cat(5), default_operation::divi(5))),
		std::make_shared<level>(3, 5, 8, make_operations(default_operation::cat(2), default_operation::divi(-4), default_operation::sub(5))),

		std::make_shared<level>(level{
			{ "you can add numbers, but also delete them", "" }
		}),

		std::make_shared<level>(123, 3, 2, make_operations(default_operation::del(), default_operation::mul(2))),

		std::make_shared<level>(level{
			{ "thanks to you all the customers questions have been solved", "yay" },
			{ "and the engineers had time to fix me", "phew" },
			{ "well, this is goodbye", "bye" },
		}),
	});
};

namespace posts {
	namespace text {
		namespace numeric {
			template <bool AutoDispatch>
			void set_primary(int t, flash_mode_t mode) { _primary.set(t, mode); posts::text::post<AutoDispatch>(); }
			template <bool AutoDispatch>
			void set_moves(int t, flash_mode_t mode) { _moves.set(t, mode); posts::text::post<AutoDispatch>(); }
			template <bool AutoDispatch>
			void set_target(int t, flash_mode_t mode) { _target.set(t, mode); posts::text::post<AutoDispatch>(); }
			template <bool AutoDispatch>
			void set_level(int t, flash_mode_t mode) { _level.set(t, mode); posts::text::post<AutoDispatch>(); }

			int get_primary() { return _primary.get_integral(); }
			int get_moves() { return _moves.get_integral(); }
			int get_target() { return _target.get_integral(); }
			int get_level() { return _level.get_integral(); }
		};

		namespace string {
			template <bool AutoDispatch>
			void set_primary(const std::string& t, flash_mode_t mode) { _primary.set(t, mode); posts::text::post<AutoDispatch>(); }
			template <bool AutoDispatch>
			void set_moves(const std::string& t, flash_mode_t mode) { _moves.set(t, mode); posts::text::post<AutoDispatch>(); }
			template <bool AutoDispatch>
			void set_target(const std::string& t, flash_mode_t mode) { _target.set(t, mode); posts::text::post<AutoDispatch>(); }
			template <bool AutoDispatch>
			void set_level(const std::string& t, flash_mode_t mode) { _level.set(t, mode); posts::text::post<AutoDispatch>(); }

			std::string get_primary() { return _primary.get_string(); }
			std::string get_moves() { return _moves.get_string(); }
			std::string get_target() { return _target.get_string(); }
			std::string get_level() { return _level.get_string(); }
		};

		flash_mode_t get_primary_mode() { return _primary.get_mode(); }
		flash_mode_t get_moves_mode() { return _moves.get_mode(); }
		flash_mode_t get_target_mode() { return _target.get_mode(); }
		flash_mode_t get_level_mode() { return _level.get_mode(); }

		template <bool Should>
		void post() {
			auto_poster<display_event<display_event_mode::normal_text>, Should>::post({
				posts::text::string::get_primary(),
				posts::text::string::get_moves(),
				posts::text::string::get_target(),
				posts::text::string::get_level(),
				posts::text::get_primary_mode(),
				posts::text::get_moves_mode(),
				posts::text::get_target_mode(),
				posts::text::get_level_mode(),
			});
		}
	};

	namespace tutorial_text {
		template <bool AutoDispatch>
		void set(const std::string& t) { _tutorial_text = t; posts::tutorial_text::post<AutoDispatch>(); }

		std::string get() { return _tutorial_text; }

		template <bool Should>
		void post() {
			auto_poster<display_event<display_event_mode::tutorial_text>, Should>::post({
				posts::tutorial_text::get(),
				posts::text::string::get_level()
			});
		}
	};

	namespace operations {
		template <bool AutoDispatch>
		void set_generic_ac() {
			posts::operations::set_aux<AutoDispatch>(make_operations(
					default_operation::ac(),
					default_operation::nop(),
					default_operation::settings()));
		}
		template <bool AutoDispatch>
		void set_just_ac() {
			posts::operations::set_aux<AutoDispatch>(make_operations(
					default_operation::ac()));
		}
		template <bool AutoDispatch>
		void set_generic_next() {
			posts::operations::set_aux<AutoDispatch>(make_operations(
					default_operation::next(),
					default_operation::nop(),
					default_operation::settings()));
		}
		template <bool AutoDispatch>
		void set_just_next() {
			posts::operations::set_aux<AutoDispatch>(make_operations(
					default_operation::next()));
		}

		template <bool AutoDispatch>
		void set_aux(const make_operations::type& t) {
			_aux_operations = t;
			posts::operations::post<AutoDispatch>();
		}

		template <bool AutoDispatch>
		void set_central(const make_operations::type& t) {
			_central_operations = t;
			posts::operations::post<AutoDispatch>();
		}

		make_operations::type get_central() { return _central_operations; }
		make_operations::type get_aux() { return _aux_operations; }

		void disable_central() { event::dispatch<display_event<display_event_mode::disable>>::post({ display_event_mode::operations_central, true }); }
		void enable_central() { event::dispatch<display_event<display_event_mode::disable>>::post({ display_event_mode::operations_central, false }); }

		template <bool Should>
		void post() { // always generated two events which is NOT optimal
			auto_poster<display_event<display_event_mode::operations_aux>, Should>::post({
				posts::operations::get_aux()
			});

			auto_poster<display_event<display_event_mode::operations_central>, Should>::post({
				posts::operations::get_central()
			});
		}

		template <bool Should>
		void post_redraw() {
			auto_poster<display_event<display_event_mode::operations_redraw>, Should>::post({});
		}

	};
};

void level::run() {
	_last_mode = _lm_lvls[_lm_index]->type;
	_lm_lvls[_lm_index]->instantiate(_lm_index);
}

void level::load() {
	_lm_index = 0;
	_lm_max_index = _lm_lvls.size() - 1; // for debugging
	level::run();
}

void level::next(bool instantiate) {
	_lm_index++;

	if (_lm_index > _lm_max_index)
		_lm_max_index = _lm_index;

	// @TODO temporary
	if (_lm_index >= _lm_lvls.size())
		std::exit(0);

	if (instantiate)
		level::run();
}

void level::previous(bool instantiate) {
	if (_lm_index > 0)
		_lm_index--;

	if (instantiate)
		level::run();
}

void level::reset() {
	level::run();
}

level* level::get() {
	return _lm_lvls[_lm_index].get();
}

std::size_t level::get_current() { return _lm_index; }
std::size_t level::get_max() { return _lm_max_index; }
level::mode level::last_mode() { return _last_mode; }
