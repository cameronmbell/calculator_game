#ifndef _LEVEL_HPP
#define _LEVEL_HPP

#include <stdexcept>
#include <vector>
#include <string>

#include "button.hpp"
#include "event.hpp"
#include "util.hpp"

class basic_operation;

namespace level_manager {
	struct display_event {
		std::string number, target, moves, level;
	};

	struct button_event {
		std::vector<std::shared_ptr<basic_operation>> ops;
	};

	void reset();
	void next();

	void set(const std::string& to);
	void set(int to);
	int get();

	void make_post();
};

struct level {
public:
	const int number, target, moves;
	const std::vector<std::shared_ptr<basic_operation>> ops;

//	template <typename ...Tp>
//	level(int num, int targ, int mov, Tp&&... parm)
//		: number(num), target(targ), moves(mov),
//		ops{std::make_shared<basic_operation>(parm)...} {
//	}

	level(int num, int targ, int mov, std::initializer_list<std::shared_ptr<basic_operation>> li);

	level(std::initializer_list<std::pair<const std::string, const std::string>> l);
};

#endif // _LEVEL_HPP
