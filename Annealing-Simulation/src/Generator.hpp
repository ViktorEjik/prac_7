//
// Created by viktor on 10.11.2024.
//

#ifndef ANNEALING_SIMULATION_GENERATOR_HPP
#define ANNEALING_SIMULATION_GENERATOR_HPP
#include <random>
#include <utility>
#include <string>
#include <fstream>
#include "../thirdparty/json/single_include/nlohmann/json.hpp"

using json = nlohmann::json;

struct TestCase{
    size_t procN;
    size_t workN;
    uint64_t min;
    uint64_t max;
};

class Generator {
public:
    Generator(): tests(){};
    explicit Generator(std::vector<TestCase>& tests): tests(tests){};
    Generator(std::initializer_list<TestCase>& initList): tests(initList){};

    void addTestCase(TestCase test){
        tests.push_back(test);
    }

    TestCase& operator[] (size_t index){
        return tests[index];
    }

    void operator()(std::string path);
private:
    std::vector<TestCase> tests;
};

void write_csv(std::string filename, std::vector<std::pair<std::string, std::vector<uint32_t >>> dataset);


#endif //ANNEALING_SIMULATION_GENERATOR_HPP
