#include "Logger.hpp"
#include <fstream>

void Logger::logRound(const std::string& log){
        std::ofstream log_file(log_directory / ("round_" + std::to_string(round) + ".log"), std::ios::app);
        log_file << log << std::endl;
        log_file.close();
}
void Logger::nextRound() {round++;}

void Logger::logFinal(const std::string &final_log) {
    std::ofstream final_file(log_directory / "final_results.log", std::ios::app);

    final_file << final_log << std::endl;
    final_file.close();
}