#pragma once

#include <iostream>

#include "../ansi.h"

struct Vec {
	float x, y;
};

struct Position {
	Vec vec;
	static std::string NAME;
};

std::string Position::NAME = "Position";

std::ostream &operator<<(std::ostream &os, Position const& m) {
	return os << ANSI::YELLOW << Position::NAME << "{" << m.vec.x << " " << m.vec.y << "}" << ANSI::RESET;
}

std::ostream &operator<<(std::ostream &os, Position const* m) {
	if (m == nullptr) {
		return os << ANSI::GRAY << Position::NAME << "{INVALID}" << ANSI::RESET;
	}
	return os << *m;
}

struct Size {
	Vec vec;
	static std::string NAME;
};

std::string Size::NAME = "Size";

std::ostream &operator<<(std::ostream &os, Size const& m) {
	return os << ANSI::YELLOW << Size::NAME << "{" << m.vec.x << " " << m.vec.y << "}" << ANSI::RESET;
}

std::ostream &operator<<(std::ostream &os, Size const* m) {
	if (m == nullptr) {
		return os << ANSI::GRAY << Size::NAME << "{INVALID}" << ANSI::RESET;
	}
	return os << *m;
}

struct Acceleration {
	Vec vec;
	static std::string NAME;
};

std::string Acceleration::NAME = "Acceleration";

std::ostream &operator<<(std::ostream &os, Acceleration const& m) {
	return os << ANSI::YELLOW << Acceleration::NAME << "{" << m.vec.x << " " << m.vec.y << "}" << ANSI::RESET;
}

std::ostream &operator<<(std::ostream &os, Acceleration const* m) {
	if (m == nullptr) {
		return os << ANSI::GRAY << Acceleration::NAME << "{INVALID}" << ANSI::RESET;
	}
	return os << *m;
}

struct Velocity {
	Vec vec;
	static std::string NAME;
};

std::string Velocity::NAME = "Velocity";

std::ostream &operator<<(std::ostream &os, Velocity const& m) {
	return os << ANSI::YELLOW << Velocity::NAME << "{" << m.vec.x << " " << m.vec.y << "}" << ANSI::RESET;
}

std::ostream &operator<<(std::ostream &os, Velocity const* m) {
	if (m == nullptr) {
		return os << ANSI::GRAY << Velocity::NAME << "{INVALID}" << ANSI::RESET;
	}
	return os << *m;
}

