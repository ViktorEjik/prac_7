#include <iostream>
#include <MyMath/Factory.hpp>
#include <cmath>

int main(){
    functions::FunctionFactory F;
    auto constant = F.Create("const", 0);
    std::cout << functions::FindRoot(*constant, 5) << std::endl;

    auto liner = *F.Create("ident") + *F.Create("const", 1);
    std::cout << liner.ToString() << " root: " << std::round(functions::FindRoot(liner, 100)) << std::endl;

    auto quadraticPolynomial = F.Create("polynomial", {-1, 0, 1});
    std::cout << quadraticPolynomial->ToString() << "\n\t"
              << "x_1: " << std::round(functions::FindRoot(*quadraticPolynomial, 100, -2)) << "\n\t"
              << "x_2: " << std::round(functions::FindRoot(*quadraticPolynomial, 100, 2)) << "\n";

    auto quadraticPower = *F.Create("power", 2) - *F.Create("const", 1);
    std::cout << quadraticPower.ToString() << "\n\t"
              << "x_1: " << std::round(functions::FindRoot(quadraticPower, 100, -2)) << "\n\t"
              << "x_2: " << std::round(functions::FindRoot(quadraticPower, 100, 2)) << "\n";

    auto exp = *F.Create("exp", 1) - *F.Create("const", 2);
    std::cout << exp.ToString() << " root: " << functions::FindRoot(exp, 10000) << std::endl;

    auto func = *F.Create("polynomial", {5, 1}) * *F.Create("polynomial", {-2, 1});
    std::cout << func.ToString() << '\n';
    std::cout << func(3) << '\n';
    std::cout << func.Derive(5) << '\n';
    std::cout << F.Create("polynomial", {-10, 3, 1})->ToString() << '\n';

    std::cout << "root1: " << functions::FindRoot(func, 100, 4) << '\n'
              << "root2: " << functions::FindRoot(func, 100, -8) << '\n';

    return 0;
}
