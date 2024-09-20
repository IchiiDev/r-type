#include "Rte/Logger.hpp"

#include "spdlog/common.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

#include <fstream>

using namespace Rte;

void Logger::init() {
    spdlog::set_pattern("\033[1;34m[%H:%M:%S %z]\033[0m \033[1;32m[thread %t]\033[0m \033[1;37m[--%L--]\033[0m: %^\"%v\"%$");
    spdlog::set_level(spdlog::level::trace);

    // Clear the log file
    std::ofstream file("logs/log.txt", std::ios::out);
    file.close();

    // Create the loggers
    spdlog::stdout_color_mt("console");
    spdlog::basic_logger_mt("file", "logs/log.txt");
}

void Logger::setLevel(LogLevel level) {
    switch (level) {
        case LogLevel::None:
            spdlog::set_level(spdlog::level::off);
            break;
        case LogLevel::Debug:
            spdlog::set_level(spdlog::level::debug);
            break;
        case LogLevel::Info:
            spdlog::set_level(spdlog::level::info);
            break;
        case LogLevel::Error:
            spdlog::set_level(spdlog::level::err);
            break;
        case LogLevel::Fatal:
            spdlog::set_level(spdlog::level::critical);
            break;
        case LogLevel::Warning:
            spdlog::set_level(spdlog::level::warn);
            break;
    }
}

void Logger::logDebug(const std::string& message) {
    spdlog::get("console")->debug(message);
    spdlog::get("file")->debug(message);
}

void Logger::logInfo(const std::string& message) {
    spdlog::get("console")->info(message);
    spdlog::get("file")->info(message);
}

void Logger::logError(const std::string& message) {
    spdlog::get("console")->error(message);
    spdlog::get("file")->error(message);
}

void Logger::logFatal(const std::string& message) {
    spdlog::get("console")->critical(message);
    spdlog::get("file")->critical(message);
}

void Logger::logWarning(const std::string& message) {
    spdlog::get("console")->warn(message);
    spdlog::get("file")->warn(message);
}
