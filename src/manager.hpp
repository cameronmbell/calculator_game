/*
 * manager.hpp:
 * manage the game, that is:
 * - levels
 * - operations
 * - text
 */

#ifndef _MANAGER_HPP
#define _MANAGER_HPP

#include <memory>
#include <vector>

#include "flashing_text.hpp"
#include "event.hpp"
#include "util.hpp"

// a game level's data
// e.g. target, moves, starting num etc..
struct level;

// forward declaration of abstract base for operation.hpp
class basic_operation;

// template function factory for cleaning up the interface of a shared_ptr initializer_list
// investigate lvalue/rvalue behavior
struct make_operations {
	typedef std::vector<std::shared_ptr<basic_operation>> type; type value;

	operator std::vector<std::shared_ptr<basic_operation>>() const noexcept { return value; }

	template <typename ...T>
	constexpr make_operations(T ...t) : value {std::make_shared<T>(std::forward<T>(t))...} {}
};

// all those in the namespace involve the setting/getting of values
// which are then passed to the game renderer in main.cpp
namespace posts {

	// changing tutorial text/numeric text...
	namespace text {

		// when changing primary/moves/target/level to a number
		// this IS NOT the same as std::set(util::as_string()).. because the number is saved
		// this allows for the current number to be set and got by any class
		namespace numeric {
			template <bool AutoDispatch=true>
			void set_primary(int t, flash_mode_t mode=flash_mode::none);
			template <bool AutoDispatch=true>
			void set_moves(int t, flash_mode_t mode=flash_mode::none);
			template <bool AutoDispatch=true>
			void set_target(int t, flash_mode_t mode=flash_mode::none);
			template <bool AutoDispatch=true>
			void set_level(int t, flash_mode_t mode=flash_mode::none);

			int get_primary() /* const */;
			int get_moves() /* const */;
			int get_target() /* const */;
			int get_level() /* const */;
		};

		// change primary/moves/target/level to a string
		namespace string {
			template <bool AutoDispatch=true>
			void set_primary(const std::string& t, flash_mode_t mode=flash_mode::none);
			template <bool AutoDispatch=true>
			void set_moves(const std::string& t, flash_mode_t mode=flash_mode::none);
			template <bool AutoDispatch=true>
			void set_target(const std::string& t, flash_mode_t mode=flash_mode::none);
			template <bool AutoDispatch=true>
			void set_level(const std::string& t, flash_mode_t mode=flash_mode::none);

			std::string get_primary() /* const */;
			std::string get_moves() /* const */;
			std::string get_target() /* const */;
			std::string get_level() /* const */;
		};

		// modify the secondary state string
		// this is used to flash between
		// e.g. WIN! -> 3 -> WIN! -> 3....
		namespace secondary_string {
			template <bool AutoDispatch=true>
			void set(const std::string& t);

			std::string get() /* const */;
		}

		// retrieve the flash mode bit mask currently used (not necessarily posted)
		flash_mode_t get_primary_mode() /* const */;
		flash_mode_t get_moves_mode() /* const */;
		flash_mode_t get_target_mode() /* const */;
		flash_mode_t get_level_mode() /* const */;

		// pass all text changes to the game renderer to be drawn
		template <bool Should=true>
		void post();
	};

	// because tutorial text is internally different to numeric text (it cover two lines)
	// it generates it's own event for the renderer, hence modify tutorial text:
	namespace tutorial_text {
		template <bool AutoDispatch=true>
		void set(const std::string& t);

		std::string get() /* const */;

		// give data to the renderer
		template <bool Should=true>
		void post();
	};

	// set and get operations
	// these are later translated into the behavior and string attached to a button
	namespace operations {

		// fill the right column (aux buttons) with SETTINGS button and AC button
		template <bool AutoDispatch=true>
		void set_generic_ac();

		// fill the right column (aux buttons) with SETTINGS button and NEXT button
		template <bool AutoDispatch=true>
		void set_generic_next();

		// fill the right column (aux buttons) with AC button
		template <bool AutoDispatch=true>
		void set_just_ac();

		// fill the right column (aux buttons) with NEXT button
		template <bool AutoDispatch=true>
		void set_just_next();

		// set aux buttons, those in the orange right column
		// generally for non_numeric changed
		template <bool AutoDispatch=true>
		void set_aux(const make_operations::type& t);

		// set central button behavior
		template <bool AutoDispatch=true>
		void set_central(const make_operations::type& t);

		// ignore input on central buttons
		// used when a level is win/lost
		void disable_central();

		// re-enable input on central buttons
		void enable_central();

		// return stored central/aux operations value
		make_operations::type get_central();
		make_operations::type get_aux();

		// post data to renderer
		template <bool Should=true>
		void post();

		// request the renderer to redraw the strings on all buttons
		template <bool Should=true>
		void post_redraw();
	};

	// available event types
	enum class display_event_mode {
		normal_text, // changes to primary/moves/target/level text
		tutorial_text, // changes to the two lines of text
		operations_central, // changes to the central grey buttons
		operations_aux, // changes to the right most orange buttons
		operations_redraw, // request button string redraw
		disable, // enable/ignore input to central buttons
	};

	// base definition of a display_event
	// used for operations_central and operations_aux
	template <display_event_mode M> struct display_event {
		const make_operations::type operations;
	};

	// change primary/moves/target/level text
	template <> struct display_event<display_event_mode::normal_text> {
		const std::string primary, moves, target, level, alt;
		const flash_mode_t primary_mode, moves_mode, target_mode, level_mode;
	};

	// change the two lines of text
	template <> struct display_event<display_event_mode::tutorial_text> {
		const std::string value, level;
	};

	// enable-ignore input to central buttons
	template <> struct display_event<display_event_mode::disable> {
		const display_event_mode what;
		const bool to;
	};
};

// a level instance and also a static level management behavior
struct level {
	// a level may store either numeric or tutorial data
	// templates/inheritance not used as it breaks the non-polymorphic level manager
	enum class mode {
		numeric,
		tutorial
	};

	// load up all the level (boot strap the game)
	static void load();

	// run the current level
	static void run();

	// run the instructions level
	static void load_instructions();

	// go to the next level
	// if instantiate is false, wait till run() is called
	static void next(bool instantiate=true);

	// go to the previous level
	// if instantiate is false, wait till run() is called
	static void previous(bool instantiate=true);

	// re-run the current level
	static void reset();

	// get the current level
	static level* get();

	// get the current level index
	static std::size_t get_current() /* const */;

	// get the highest level reached index
	// used for the upper bound on level adjustment
	static std::size_t get_max() /* const */;

	// get the last mode given to a level
	static mode last_mode() /* const */;

	// payload attached to a level:

	const mode type;
	const int primary, moves, target;
	const make_operations::type operations;
	const std::vector<std::pair<std::string, std::string>> tutorial_text;

	// construct a numeric level
	level(const int p, const int m, const int t, const make_operations::type& c)
	 	: type(mode::numeric), primary(p), moves(m), target(t), operations(c), _lvl(-1), _idx(0) {
	}

	// construct a tutorial level
	level(std::initializer_list<std::pair<std::string, std::string>> l)
		: type(mode::tutorial), primary(-1), moves(-1), target(-1),
		operations{},
		tutorial_text{l}, _idx(0) {
		if (tutorial_text.size() == 0)
			throw std::logic_error("empty tutorial");
	}

	// run *this
	void instantiate(std::size_t l) const;

private:
	mutable int _lvl;
	std::size_t _idx;
};

#endif // !_MANAGER_HPP
