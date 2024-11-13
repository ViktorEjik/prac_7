//
// Created by viktor on 10.11.2024.
//
#include "Generator.hpp"

void Generator::operator()(std::string path) {

    std::random_device rd;
    std::mt19937 gen(rd());
    json file;
    for(const auto&[procN, workN, min, max]: tests) {
        if ((!procN) || (!workN) || (!min) || (!max)) {
            throw std::runtime_error("One of the TestCases is not correct (all test parameters must be > 0)");
        }
        std::uniform_int_distribution<uint64_t> distrib(min, max);
        std::vector<std::tuple<uint32_t, uint64_t>> tasksT;
        for (uint32_t i = 0; i < workN; i++) {
            tasksT.emplace_back(i, distrib(gen));
        }
        file.push_back({
                               {"NProses", procN},
                               {"Works", tasksT}
                       });
    }
    std::ofstream outFile(path);
    outFile << std::setw(4) << file;
}

void write_csv(std::string filename, std::vector<std::pair<std::string, std::vector<uint32_t>>> dataset) {
    std::ofstream myFile(filename);

    for(int j = 0; j < dataset.size(); ++j)
    {
        myFile << dataset.at(j).first;
        if(j != dataset.size() - 1) myFile << ","; // No comma at end of line
    }
    myFile << "\n";

    for(int i = 0; i < dataset.at(0).second.size(); ++i)
    {
        for(int j = 0; j < dataset.size(); ++j)
        {
            myFile << dataset.at(j).second.at(i);
            if(j != dataset.size() - 1) myFile << ","; // No comma at end of line
        }
        myFile << "\n";
    }

    // Close the file
    myFile.close();
}
