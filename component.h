#pragma once

#include <iostream>

#include "ansi.h"

struct Vec3 {
	float x, y;
};

struct Position {
	Vec3 pos;
	static std::string NAME;
};

std::ostream &operator<<(std::ostream &os, Position const& m) {
	return os << ANSI::YELLOW << Position::NAME << "{" << m.pos.x << " " << m.pos.y << "}" << ANSI::RESET;
}

std::ostream &operator<<(std::ostream &os, Position const* m) {
	if (m == nullptr) {
		return os << ANSI::GRAY << Position::NAME << "{INVALID}" << ANSI::RESET;
	}
	return os << *m;
}

std::string Position::NAME = "Position";



struct Velocity {
	Vec3 pos;
	static std::string NAME;
};

std::ostream &operator<<(std::ostream &os, Velocity const& m) {
	return os << ANSI::YELLOW << Velocity::NAME << "{" << m.pos.x << " " << m.pos.y << "}" << ANSI::RESET;
}

std::ostream &operator<<(std::ostream &os, Velocity const* m) {
	if (m == nullptr) {
		return os << ANSI::GRAY << Velocity::NAME << "{INVALID}" << ANSI::RESET;
	}
	return os << *m;
}

std::string Velocity::NAME = "Velocity";

