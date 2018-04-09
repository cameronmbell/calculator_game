#ifndef _MANAGER_HPP
#define _MANAGER_HPP

#include <memory>
#include <vector>

#include "flashing_text.hpp"
#include "event.hpp"
#include "util.hpp"

struct level;
class basic_operation;

// investigate lvalue/rvalue behavior
struct make_operations {
	typedef std::vector<std::shared_ptr<basic_operation>> type; type value;

	operator std::vector<std::shared_ptr<basic_operation>>() const noexcept { return value; }

	template <typename ...T>
	constexpr make_operations(T ...t) : value {std::make_shared<T>(std::forward<T>(t))...} {}
};

namespace posts {
	namespace text {
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

		flash_mode_t get_primary_mode() /* const */;
		flash_mode_t get_moves_mode() /* const */;
		flash_mode_t get_target_mode() /* const */;
		flash_mode_t get_level_mode() /* const */;

		template <bool Should=true>
		void post();
	};

	namespace tutorial_text {
		template <bool AutoDispatch=true>
		void set(const std::string& t);

		std::string get() /* const */;

		template <bool Should=true>
		void post();
	};

	namespace operations {
		template <bool AutoDispatch=true>
		void set_generic_ac();
		template <bool AutoDispatch=true>
		void set_generic_next();
		template <bool AutoDispatch=true>
		void set_just_ac();
		template <bool AutoDispatch=true>
		void set_just_next();
		template <bool AutoDispatch=true>
		void set_aux(const make_operations::type& t);
		template <bool AutoDispatch=true>
		void set_central(const make_operations::type& t);

		void disable_central();
		void enable_central();

		make_operations::type get_central();
		make_operations::type get_aux();

		template <bool Should=true>
		void post();
		template <bool Should=true>
		void post_redraw();
	};

	enum class display_event_mode {
		normal_text,
		tutorial_text,
		operations_central,
		operations_aux,
		operations_redraw,
		disable,
	};

	template <display_event_mode M> struct display_event {
		const make_operations::type operations;
	};

	template <> struct display_event<display_event_mode::normal_text> {
		const std::string primary, moves, target, level;
		const flash_mode_t primary_mode, moves_mode, target_mode, level_mode;
	};

	template <> struct display_event<display_event_mode::tutorial_text> {
		const std::string value, level;
	};

	template <> struct display_event<display_event_mode::disable> {
		const display_event_mode what;
		const bool to;
	};
};

struct level {
	enum class mode {
		numeric,
		tutorial
	};

	static void load();
	static void run();
	static void next(bool instantiate=true);
	static void previous(bool instantiate=true);
	static void reset();
	static level* get();
	static std::size_t get_current() /* const */;
	static std::size_t get_max() /* const */;
	static mode last_mode() /* const */;

	const mode type;
	const int primary, moves, target;
	const make_operations::type operations;
	const std::vector<std::pair<std::string, std::string>> tutorial_text;

	level(const int p, const int m, const int t, const make_operations::type& c)
	 	: type(mode::numeric), primary(p), moves(m), target(t), operations(c), _lvl(-1), _idx(0) {
	}

	level(std::initializer_list<std::pair<std::string, std::string>> l)
		: type(mode::tutorial), primary(-1), moves(-1), target(-1),
		operations{},
		tutorial_text{l}, _idx(0) {
		if (tutorial_text.size() == 0)
			throw std::logic_error("empty tutorial");
	}

	void instantiate(std::size_t l) const;

private:
	mutable int _lvl;
	std::size_t _idx;
};

#endif // !_MANAGER_HPP
