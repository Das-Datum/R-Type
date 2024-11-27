#pragma once
#include "System.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>

/**
 * @class Logger
 * @brief Logs messages to a file and to a terminal.
 */
class Logger : public System
{
    public:
        /**
         * @brief Construct a new Logger object.
         */
        Logger() {};

        /**
         * @brief Destroy the Logger object.
         */
        ~Logger() {};

        /**
         * @brief Initialize the logger.
         * @param fileName The name of the log file.
         * @param level The logging level.
         * @return void
         */
        void init(const std::string& fileName, const std::string& level = "INFO") {
            _fileName = fileName;
            std::system("mkdir -p logs");
            if (level == "DEBUG") {
                _level = 0;
            } else if (level == "WARNING") {
                _level = 2;
            } else if (level == "ERROR") {
                _level = 3;
            } else {
                _level = 1;
            }
        }

        /**
         * @brief Log an info message.
         * @param message The message to log.
         * @return void
         */
        void writeFile(const std::string& message) {
            std::ofstream file("logs/" + _fileName + "_" + getDate() + ".log", std::ios::app);
            if (file.is_open()) {
                file << message << std::endl;
                file.close();
            } else {
                std::cout << "Failed to open file" << std::endl;
            }
        }

        /**
         * @brief Log an info message.
         * @param message The message to log.
         * @return void
         */
        void info(const std::string& message) {
            if (_level > 1)
                return;
            std::string msg = "[INFO]: " + getTime() + ": " + message;
            std::cout << msg << std::endl;
            writeFile(msg);
        }

        /**
         * @brief Log a debug message.
         * @param message The message to log.
         * @return void
         */
        void debug(const std::string& message) {
            if (_level > 0)
                return;
            std::string msg = "[DEBUG]: " + getTime() + ": " + message;
            std::cout << msg << std::endl;
            writeFile(msg);
        }

        /**
         * @brief Log a warning message.
         * @param message The message to log.
         * @return void
         */
        void warning(const std::string& message) {
            if (_level > 2)
                return;
            std::string msg = "[WARNING]: " + getTime() + ": " + message;
            std::cout << msg << std::endl;
            writeFile(msg);
        }

        /**
         * @brief Log an error message.
         * @param message The message to log.
         * @return void
         */
        void error(const std::string& message) {
            std::string msg = "[ERROR]: " + getTime() + ": " + message;
            std::cout << msg << std::endl;
            writeFile(msg);
        }

        /**
         * @brief Get the current time.
         * @return std::string
         */
        std::string getTime() {
            std::time_t t = std::time(nullptr);
            std::tm tm = *std::localtime(&t);
            std::stringstream ss;
            ss << std::put_time(&tm, "%H-%M-%S");
            return ss.str();
        }

        /**
         * @brief Get the current date.
         * @return std::string
         */
        std::string getDate() {
            std::time_t t = std::time(nullptr);
            std::tm tm = *std::localtime(&t);
            std::stringstream ss;
            ss << std::put_time(&tm, "%Y-%m-%d");
            return ss.str();
        }

    private:
        std::string _fileName;
        int _level;

};