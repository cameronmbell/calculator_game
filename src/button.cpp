/*
 * button.cpp:
 * define storage for button styles
 */

#include "button.hpp"

const button_style button_style::default_orange = {
	text_align::center | text_align::middle, sf::Color::White, // text properties
	sf::Color::Black, 1.0f, // outline colour and outline thickness
	{
		{ button_state::normal, sf::Color(248, 123, 15) },
		{ button_state::hover, sf::Color(240, 115, 10) },
		{ button_state::click, sf::Color(230, 110, 5) },
	}, // rect state and associated rect colour
	0.0f, // no internal padding
	1.0f // 1 second fade time
};

const button_style button_style::default_grey = {
	text_align::center | text_align::middle, sf::Color::Black, // text properties
	sf::Color::Black, 1.0f, // outline colour and outline thickness
	{
		{ button_state::normal, sf::Color(210, 210, 210) },
		{ button_state::hover, sf::Color(200, 200, 200) },
		{ button_state::click, sf::Color(190, 190, 190) },
	}, // rect state and associated rect colour
	0.0f, // no internal padding
	1.0f // 1 second fade time
};
