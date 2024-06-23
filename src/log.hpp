#pragma once

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <format>
#include <chrono>
#include <Windows.h>
#include <conio.h>

class Log {
	const char* reset = "[0m";

	const char* white = "[97m";

	const char* red = "[31m";
	const char* green = "[32m";
	const char* yellow = "[33m";
public:
	Log() noexcept;
	void ok(const char* text) noexcept;
	void error(const char* text) noexcept;
	void warn(const char* text) noexcept;
	void wait() noexcept;
};