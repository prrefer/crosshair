#include "input.hpp"

bool input::toggled(int keyCode) {
	return GetAsyncKeyState(keyCode) & 1;
}