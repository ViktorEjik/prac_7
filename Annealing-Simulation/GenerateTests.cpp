//
// Created by viktor on 11.11.2024.
//

#include <CLI/CLI.hpp>

#include "src/Generator.hpp"


int main(int argc, char *argv[]){
    CLI::App generate{"Generate tests for Annealing Simulation"};

    std::string path;
    generate.add_option("file-name", path)->required();

    std::tuple<uint32_t, uint32_t, uint32_t> minMaxStepCPU;
    generate.add_option("--mMsCPU", minMaxStepCPU)->required();

    std::tuple<uint32_t, uint32_t, uint32_t> minMaxStepWorks;
    generate.add_option("--mMsWorks", minMaxStepWorks)->required();

    std::pair<uint32_t, uint32_t> minMaxTime;
    generate.add_option("--mMsTime", minMaxTime)->required();

    generate.callback([&](){
        Generator g;
        auto [minCPU, maxCPU, stepCPU] = minMaxStepCPU;
        auto [minWorks, maxWorks, stepWorks] = minMaxStepWorks;
        for(auto i = minCPU; i <=maxCPU; i+=stepCPU){
            for(auto j = minWorks; j <= maxWorks; j+=stepWorks){
                g.addTestCase({i, j, minMaxTime.first, minMaxTime.second});
            }
        }
        g(path);
    });

    CLI11_PARSE(generate, argc, argv);
}