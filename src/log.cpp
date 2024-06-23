#include "log.hpp"

Log::Log() noexcept {
	HANDLE stdHandle{ GetStdHandle(STD_OUTPUT_HANDLE) };
	DWORD consoleMode{};
	GetConsoleMode(stdHandle, &consoleMode);
	consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(stdHandle, consoleMode);
}

void Log::ok(const char* text) noexcept {
	std::cout << std::format("[ {}{} | {}ok{} ] {}", Log::white, std::chrono::system_clock().now(), Log::green, Log::reset, text) << std::endl;
}

void Log::error(const char* text) noexcept {
	std::cout << std::format("[ {}{} | {}error{} ] {}", Log::white, std::chrono::system_clock().now(), Log::red, Log::reset, text) << std::endl;
}

void Log::warn(const char* text) noexcept {
	std::cout << std::format("[ {}{} | {}warn{} ] {}", Log::white, std::chrono::system_clock().now(), Log::yellow, Log::reset, text) << std::endl;
}

void Log::wait() noexcept {
	_getch();
}