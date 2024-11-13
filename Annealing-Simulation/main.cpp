#include <iostream>

#include <CLI/CLI.hpp>

#include "src/Generator.hpp"
#include "src/Random.hpp"
#include "src/Schedule.hpp"
#include "src/Annealing.hpp"
#include "src/Temperature.hpp"


int main(int argc, char* argv[]) {
    CLI::App generate{"ClI to Annealing Simulation"};
    auto test = generate.add_subcommand("test", "Work with json files");

    std::string testPath;
    test->add_option("--testPath", testPath)->required();

    std::string syncPath;
    test->add_option("--syncPath", syncPath)->default_val("");

    std::string parallelPath;
    test->add_option("--parallelPath", parallelPath)->default_val("");

    test->callback([&](){
        if(!syncPath.empty()){
            std::ifstream f(testPath);
            json tests = json::parse(f);
            std::vector<std::pair<std::string, std::vector<uint32_t>>> table = {
                    {"num", {}},
                    {"procN", {}},
                    {"taskN", {}},
                    {"criteria", {}},
                    {"timeWork", {}}
            };
            size_t num = 1;
            Boltzmann bol;
            Temperature t(&bol, 150);
//            for(auto test: tests){
                auto test = tests[7];
                uint32_t procN = test["NProses"];
                uint32_t taskN = test["Works"].size();
                auto t_start = std::chrono::high_resolution_clock::now();
                auto res = Parallel_Simulation(Schedule(test, false), t, 100, 20, 1);
                auto t_end = std::chrono::high_resolution_clock::now();
                auto t_duration = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start).count();
                table[0].second.push_back(num);
                table[1].second.push_back(procN);
                table[2].second.push_back(taskN);
                table[3].second.push_back(res.get_criterion());
                table[4].second.push_back(t_duration);
                std::cout << num << ' ' << res.get_criterion() << ' ' << t_duration << " ok\n";
                num++;
//            }
            write_csv(syncPath, table);
        }
        if(!parallelPath.empty()){
            std::ifstream f(testPath);
            json tests = json::parse(f);
            std::vector<std::pair<std::string, std::vector<uint32_t>>> table = {
                    {"num", {}},
                    {"procN", {}},
                    {"taskN", {}},
                    {"criteria", {}},
                    {"timeWork", {}}
            };
            size_t num = 1;
            Boltzmann bol;
            Temperature t(&bol, 150);
//            for(auto test: tests){
                auto test = tests[7];
                uint32_t procN = test["NProses"];
                uint32_t taskN = test["Works"].size();
                auto t_start = std::chrono::high_resolution_clock::now();
                auto res = Parallel_Simulation(Schedule(test, false), t, 100, 5, 8);
                auto t_end = std::chrono::high_resolution_clock::now();
                auto t_duration = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start).count();
                table[0].second.push_back(num);
                table[1].second.push_back(procN);
                table[2].second.push_back(taskN);
                table[3].second.push_back(res.get_criterion());
                table[4].second.push_back(t_duration);
                std::cout << num << ' ' << res.get_criterion() << ' ' << t_duration << " ok\n";
                num++;
//            }
            write_csv(parallelPath, table);
        }
    });

    CLI11_PARSE(generate, argc, argv);
    return 0;
}
