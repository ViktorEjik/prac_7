#include <iostream>
#include <CLI/CLI.hpp>
#include "src/Game.hpp"


int main(int argc, char** argv)
{
//    SharedPtr<std::map<size_t, SharedPtr<Player>>> pls(new std::map<size_t, SharedPtr<Player>>);
//    SharedPtr<Player> p(new Player{2, pls});
//    (*pls)[2] = p;
//    std::cout << std::boolalpha << p->id << ' ' << (int)p->realRole() << ' ' << (p->night().has_value()) << std::endl;
//    SharedPtr<Player> d(new Doc{3, pls});
//    (*pls)[3] = d;
//    auto t = d->night();
//    std::cout  << d->id << ' ' << (int)d->realRole() << ' ' << (t.has_value()) << ' ' << t.value().second << std::endl;
//    t = d->night();
//    std::cout  << d->id << ' ' << (int)d->realRole() << ' ' << (t.has_value()) << ' ' << t.value().second << std::endl;
//    SharedPtr<Player> c(new Com{4, pls});
//    (*pls)[4] = c;
//
//    SharedPtr<std::list<size_t >> mafs(new std::list<size_t>);
//    SharedPtr<Player> maf(new Maf(5, pls, mafs));
//    mafs->push_back(5);
//    (*pls)[5] = maf;
//    t = maf->night();
//    std::cout << maf->id << ' ' << (int)maf->realRole() << ' ' << t.has_value() << ' ' << (t.has_value()? t.value().second: 0) << std::endl;
//
//    for(int i =0; i < 6; i++) {
//        t = c->night();
//        std::cout << c->id << ' ' << (int) c->realRole() << ' ' << t.has_value() << ' '
//                  << (t.has_value() ? t.value().second : 0) << std::endl;
//        if(t.has_value() && t.value().first == Do::KILL){
//            std::cout << "KILL: " << t.value().second << std::endl;
//            pls->erase(t.value().second);
//            mafs->remove(t.value().second);
//        }
//        if(t.has_value() && t.value().first == Do::CHECK){
//            std::cout << "CHECK: " << t.value().second << std::endl;
//        }
//        if(!t.has_value()) break;
//    }
//    SharedPtr<Player> man(new Man(6, pls));
//    (*pls)[6] = man;
//    t = man->night();
//    std::cout << man->id << ' ' << (int)man->realRole() << ' ' << t.has_value() << ' ' << (t.has_value()? t.value().second: 0) << std::endl;
    CLI::App gameCLI{"Mafia game"};
    int N;
    gameCLI.add_option<int>("n_players", N)->default_val(5);
    bool haveHuman = false;
    gameCLI.add_flag("--IPlay", haveHuman);

    CLI11_PARSE(gameCLI, argc, argv);

    game(N, haveHuman);

}