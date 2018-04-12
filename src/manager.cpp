/*
 * manager.cpp:
 * implements the game manager in manager.hpp
 */

#include "manager.hpp"
#include "operation.hpp"

// specify storage of tutorial operation data within this translation unit
// to do with the nature of statics in C++
std::vector<std::pair<std::string, std::string>> default_operation::tutorial::_data;
std::size_t default_operation::tutorial::_idx;

// setup the game state to match that of *this level
void level::instantiate(std::size_t l) const {
	std::cout << "level " << l << " instantiated" << std::endl;

	// guarantee that central operations are enabled
	_lvl = l;
	posts::operations::enable_central();

	switch (type) {
		case mode::numeric:
			// set primary/moves/target/level text
			posts::text::numeric::set_primary<false>(primary, flash_mode::thrice | flash_mode::quick);
			posts::text::numeric::set_moves<false>(moves);
			posts::text::numeric::set_target<false>(target);
			posts::text::numeric::set_level<false>(_lvl);

			// allow an AC and SETTINGS button
			posts::operations::set_central(operations);
			posts::operations::set_generic_ac();
			posts::text::post();
			break;
		case mode::tutorial:
			// disable all aux buttons
			posts::operations::set_aux({});

			// set level string and pass initialization to tutorial class (operation.hpp)
			posts::text::numeric::set_level(_lvl);
			default_operation::tutorial::initialize(tutorial_text);
			break;
		default:
			std::cout << "warning: unhandled level type" << std::endl;
	}
}

namespace {

	// store a string and integral simultaneously
	class persistent_stringify {
		int _integral;
		std::string _string, _alt;
		const std::string _fmt;
		flash_mode_t _flash;

	public:
		persistent_stringify(const std::string& fmt="")
			: _integral(), _string(), _fmt(fmt), _flash(flash_mode::none) { }

		// update to an int
		void set(int to, flash_mode_t m=flash_mode::none) {
			_flash = m;
			_integral = to;
			_string = _fmt + util::as_string(to);
		}

		// update to a string
		void set(const std::string& to, flash_mode_t m=flash_mode::none) {
			_flash = m;
			_string = to;
		}

		// return data
		int get_integral() const { return _integral; }
		std::string get_string() const { return _string; }
		flash_mode_t get_mode() const { return _flash; }
	};

	// template that dispatches to the event manager only if Should is true
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

	// all data to be managed
	std::string _secondary_string; // second flash to string
	persistent_stringify _primary, // central (numeric level) text
						_moves("moves: "), // moves (numeric level) text
						_target("target: "), // target (numeric level) text
						_level("level: "); // level text (universal)

	make_operations::type _central_operations; // central operations (grey buttons)
	make_operations::type _aux_operations; // aux operations (orange buttons)
	std::string _tutorial_text; // current tutorial string

	level::mode _last_mode; // last level type e.g. level::mode::numeric
	std::size_t _lm_index = 0, // current level reached
		_lm_max_index = 0; // highest level ever reached

	// definition of all levels:
	std::vector<std::shared_ptr<level>> _lm_lvls ({
		std::make_shared<level>(level{
			{ "below is a grid of buttons, click 'next'", "next" },
			{ "once you enter the game those will look different", "next" },
			{ "for example it might say +1 which means add one", "next" },
			{ "there will be four pieces of text, these are..", "next" },
			{ "'level' is how far through the game you are", "next" },
			{ "'moves' is how many moves you can do before you lose", "next" },
			{ "'target' is what number you need to try get to", "next" },
			{ "the main text shows what you're currently on", "next" },
			{ "hopefully that helps!", "got it" }
		}),

		std::make_shared<level>(level{
			{ "hey there!", "hi" },
			{ "can you help me?", "sure" },
			{ "the engineers who designed me was very lazy", "next" },
			{ "the only way I can calculate is with your help", "okay" },
			{ "I'll give you a number and a target", "okay" },
			{ "you need to press buttons to get to the target", "okay" },
			{ "oh here's a problem now...", "" }
		}),

		std::make_shared<level>(0, 3, 3, make_operations(default_operation::add(1))),

		std::make_shared<level>(level{
			{ "you're already a master", "thanks" },
			{ "see if you can do these...", "" }
		}),

		// starting number, moves allowed, target number, ....
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

// implementation of posts
namespace posts {
	namespace text {
		namespace numeric {
			// set and dispatch numerics (wraps around calls to persistent_stringify)
			template <bool AutoDispatch>
			void set_primary(int t, flash_mode_t mode) { _primary.set(t, mode); posts::text::post<AutoDispatch>(); }
			template <bool AutoDispatch>
			void set_moves(int t, flash_mode_t mode) { _moves.set(t, mode); posts::text::post<AutoDispatch>(); }
			template <bool AutoDispatch>
			void set_target(int t, flash_mode_t mode) { _target.set(t, mode); posts::text::post<AutoDispatch>(); }
			template <bool AutoDispatch>
			void set_level(int t, flash_mode_t mode) { _level.set(t, mode); posts::text::post<AutoDispatch>(); }

			// get int data
			int get_primary() { return _primary.get_integral(); }
			int get_moves() { return _moves.get_integral(); }
			int get_target() { return _target.get_integral(); }
			int get_level() { return _level.get_integral(); }
		};

		namespace string {
			// set and dispatch strings (wraps around calls to persistent_stringify)
			template <bool AutoDispatch>
			void set_primary(const std::string& t, flash_mode_t mode) { _primary.set(t, mode); posts::text::post<AutoDispatch>(); }
			template <bool AutoDispatch>
			void set_moves(const std::string& t, flash_mode_t mode) { _moves.set(t, mode); posts::text::post<AutoDispatch>(); }
			template <bool AutoDispatch>
			void set_target(const std::string& t, flash_mode_t mode) { _target.set(t, mode); posts::text::post<AutoDispatch>(); }
			template <bool AutoDispatch>
			void set_level(const std::string& t, flash_mode_t mode) { _level.set(t, mode); posts::text::post<AutoDispatch>(); }

			// get string data
			std::string get_primary() { return _primary.get_string(); }
			std::string get_moves() { return _moves.get_string(); }
			std::string get_target() { return _target.get_string(); }
			std::string get_level() { return _level.get_string(); }
		};

		namespace secondary_string {
			// modify the secondary string for flashing to
			template <bool AutoDispatch>
			void set(const std::string& t) { _secondary_string = t; posts::text::post<AutoDispatch>(); }

			// return string data
			std::string get() { return _secondary_string; }
		}

		// return current mode value from persistent_stringify objects
		flash_mode_t get_primary_mode() { return _primary.get_mode(); }
		flash_mode_t get_moves_mode() { return _moves.get_mode(); }
		flash_mode_t get_target_mode() { return _target.get_mode(); }
		flash_mode_t get_level_mode() { return _level.get_mode(); }

		// post all modified string data
		template <bool Should>
		void post() {

			// form a display_event object and pass that to the auto_poster
			// which calls the event manager
			// which is received by the game renderer
			auto_poster<display_event<display_event_mode::normal_text>, Should>::post({
				posts::text::string::get_primary(),
				posts::text::string::get_moves(),
				posts::text::string::get_target(),
				posts::text::string::get_level(),
				posts::text::secondary_string::get(),
				posts::text::get_primary_mode(),
				posts::text::get_moves_mode(),
				posts::text::get_target_mode(),
				posts::text::get_level_mode(),
			});

			// the secondary string must be reset between posts
			// so that all string changes don't need to
			if (Should)
				posts::text::secondary_string::set<false>("");
		}
	};

	// modify tutorial text data and post that to the game renderer
	namespace tutorial_text {

		// set current tutorial text and post that to the renderer
		template <bool AutoDispatch>
		void set(const std::string& t) { _tutorial_text = t; posts::tutorial_text::post<AutoDispatch>(); }

		// get tutorial text data
		std::string get() { return _tutorial_text; }

		// post a display_event object to the game renderer
		template <bool Should>
		void post() {
			auto_poster<display_event<display_event_mode::tutorial_text>, Should>::post({
				posts::tutorial_text::get(),
				posts::text::string::get_level()
			});
		}
	};

	// set/get/post operations (buttons)
	namespace operations {
		// fill the right column (aux buttons) with SETTINGS button and AC button
		template <bool AutoDispatch>
		void set_generic_ac() {
			posts::operations::set_aux<AutoDispatch>(make_operations(
					default_operation::ac(),
					default_operation::nop(),
					default_operation::settings()));
		}

		// fill the right column (aux buttons) with SETTINGS button and NEXT button
		template <bool AutoDispatch>
		void set_generic_next() {
			posts::operations::set_aux<AutoDispatch>(make_operations(
					default_operation::next(),
					default_operation::nop(),
					default_operation::settings()));
		}

		// fill the right column (aux buttons) with AC button
		template <bool AutoDispatch>
		void set_just_ac() {
			posts::operations::set_aux<AutoDispatch>(make_operations(
					default_operation::ac()));
		}

		// fill the right column (aux buttons) with NEXT button
		template <bool AutoDispatch>
		void set_just_next() {
			posts::operations::set_aux<AutoDispatch>(make_operations(
					default_operation::next()));
		}

		// set aux buttons, those in the orange right column
		template <bool AutoDispatch>
		void set_aux(const make_operations::type& t) {
			_aux_operations = t;
			posts::operations::post<AutoDispatch>();
		}

		// set central button behavior
		template <bool AutoDispatch>
		void set_central(const make_operations::type& t) {
			_central_operations = t;
			posts::operations::post<AutoDispatch>();
		}

		// return stored central/aux operations value
		make_operations::type get_central() { return _central_operations; }
		make_operations::type get_aux() { return _aux_operations; }

		// ignore input on central buttons
		// done so by dispatching a separate event to the renderer
		// hence the renderer will call disable on managed buttons
		void disable_central() { event::dispatch<display_event<display_event_mode::disable>>::post({ display_event_mode::operations_central, true }); }
		void enable_central() { event::dispatch<display_event<display_event_mode::disable>>::post({ display_event_mode::operations_central, false }); }


		// pass operation data to the game renderer
		// unfortunately this always generated two events which is NOT optimal
		// one for central_operations and one for aux_operations
		template <bool Should>
		void post() {
			auto_poster<display_event<display_event_mode::operations_aux>, Should>::post({
				posts::operations::get_aux()
			});

			auto_poster<display_event<display_event_mode::operations_central>, Should>::post({
				posts::operations::get_central()
			});
		}

		// dispatch a button string redraw event
		template <bool Should>
		void post_redraw() {
			auto_poster<display_event<display_event_mode::operations_redraw>, Should>::post({});
		}

	};
};

// instantiate the current level (given by _lm_index)
void level::run() {
	_last_mode = _lm_lvls[_lm_index]->type;
	_lm_lvls[_lm_index]->instantiate(_lm_index);
}

// reset the game by starting at level 1
// level 0 represents the instructions screen
void level::load() {
	_lm_index = _lm_max_index = 1;
	level::run();
}

// go to the next level
void level::next(bool instantiate) {
	_lm_index++;

	// update the max index if so required
	if (_lm_index > _lm_max_index)
		_lm_max_index = _lm_index;

	// when the final level is finished
	// quit
	if (_lm_index >= _lm_lvls.size())
		std::exit(0);

	if (instantiate)
		level::run();
}

// go to the previous level
void level::previous(bool instantiate) {
	// ensure no underflows
	if (_lm_index > 0)
		_lm_index--;

	if (instantiate)
		level::run();
}

// run the instructions level
void level::load_instructions() {
	_lm_index = 0;

	level::run();

	// reset to the last (highest) level
	// somewhat problematic, but far simpler than an alt solution
	_lm_index = _lm_max_index;
}

// re-run the current level
void level::reset() {
	level::run();
}

// get the current level
level* level::get() {
	return _lm_lvls[_lm_index].get();
}

// return level data
std::size_t level::get_current() { return _lm_index; }
std::size_t level::get_max() { return _lm_max_index; }
level::mode level::last_mode() { return _last_mode; }
