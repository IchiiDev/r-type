#pragma once

#include <string>

namespace Rte {

    enum class LogLevel {
        None,
        Debug,
        Info,
        Warning,
        Error,
        Fatal
    };

    class Logger {
        public:
            static void init();

            /**
             * @brief Set the level of the logger.
             * None: No log will be printed.
             *
             * @param level The level of the logger (Debug, Info, Warning, Error, Fatal).
             */
            static void setLevel(LogLevel level);

            /**
             * @brief Log a message with the Debug level.
             *
             * @param message The message to log.
             */
            static void logDebug(const std::string& message);
            static void logInfo(const std::string& message);
            static void logWarning(const std::string& message);
            static void logError(const std::string& message);
            static void logFatal(const std::string& message);
    };

}   // namespace Rte
