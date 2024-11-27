#include <gtest/gtest.h>
#include <MyMath/Factory.hpp>
#include <MyMath/Functions.hpp>
#include <iostream>
#include <cmath>

using namespace functions;

TEST(Function_test, Factory) {
    FunctionFactory F;

    auto A1 = F.Create("ident", {});
    Identical A2;
    ASSERT_EQ(typeid(*A1), typeid(A2));

    auto B1 = F.Create("power", {1, 2, 3});
    Power B2(5);
    ASSERT_EQ(typeid(*B1), typeid(B2));

    auto C1 = F.Create("exp");
    Exponential C2(10);
    ASSERT_EQ(typeid(*C1), typeid(C2));

    auto D1 = F.Create("const", {1});
    Constant D2;
    ASSERT_EQ(typeid(*D1), typeid(D2));

    auto E1 = F.Create("polynomial");
    Polynomial E2({1, 2, 3});
    ASSERT_EQ(typeid(*E1), typeid(E2));
}

TEST(Function_test, FunctionSemanticsORString) {
    FunctionFactory F;

    auto A1 = F.Create("polynomial", {0, 2});
    ASSERT_EQ(A1->ToString(), "2.000000*x");
    auto A2 = F.Create("ident");
    ASSERT_EQ(A2->ToString(), "x");

    auto B1 = F.Create("polynomial", {0, 0, 1, 0, 1});
    ASSERT_EQ((*B1).ToString(), "1.000000*x^2 + 1.000000*x^4");

    auto C1 = F.Create("polynomial", {10});
    auto C2 = F.Create("const", 10);
    ASSERT_EQ((*C1).ToString(), (*C2).ToString());

    auto D1 = F.Create("exp", {10});
    auto D2 = F.Create("exp", 10);
    ASSERT_EQ((*D1).ToString(), (*D2).ToString());
}

TEST(Function_test, Arithmetic) {
    FunctionFactory F;

    auto A1 = F.Create("polynomial", {0, 2, 1});
    auto A2 = F.Create("polynomial", {0, 1, 2});
    auto A3 = *A1 + *A2;
    ASSERT_EQ(A3(1), 6);
    ASSERT_EQ(A3(2), 18);
    ASSERT_EQ((*A1 - *A2)(1), 0);
    ASSERT_EQ((*A1 - *A2)(2), -2);
    ASSERT_EQ((*A1 * *A2)(1), 9);
    ASSERT_EQ((*A1 * *A2)(2), 80);

    auto A4 = F.Create("exp", 0);
    ASSERT_EQ((A3 / *A4)(1), 6);
    ASSERT_EQ((A3 / *A4)(2), 18);

    auto A5 = A3 / *A4;
    auto A6 = F.Create("const", 18);
    ASSERT_EQ((A5 / *A6)(2), 1);

    auto A7 = F.Create("power", 2);
    ASSERT_EQ((*A7 + *A6)(2), 22);
    ASSERT_EQ((*A7 - *A6)(2), -14);
    ASSERT_EQ((*A7 * *A6)(2), 4 * 18);
    ASSERT_EQ((*A7 / *A6)(2), 4.0 / 18.0);
}

TEST(Function_test, Derive) {
    FunctionFactory F;

    /* From example: */

    auto f = F.Create("power", 2);
    auto g = F.Create("polynomial", {7, 0, 3, 15});
    auto p = *f + *g;
    ASSERT_EQ(p.Derive(1), 53);
    ASSERT_THROW(*f + "abc", std::logic_error);
    ASSERT_EQ(f->Derive(3), 6);

    /* Other */

    auto k = F.Create("exp", 1);
    ASSERT_EQ((*k).Derive(2), ((*k))(2));
    ASSERT_EQ((*k).Derive(20), ((*k))(20));

    auto power = F.Create("power", 2);
    ASSERT_EQ((*k * *power).Derive(1), 8.1548454853771357);
    ASSERT_EQ((*k / *power).Derive(1), -2.7182818284590451);
}

TEST(Function_test, RootSearch) {
    FunctionFactory F;

    auto f = F.Create("ident", 1);
    ASSERT_EQ(FindRoot((*f), 2), 0);
    auto g = F.Create("polynomial", {4, -1});
    ASSERT_EQ(std::round(FindRoot((*g), 1000, -10)), 4);
    auto p = F.Create("polynomial", {1, 0, -1});
    ASSERT_EQ(std::round(FindRoot((*p), 20, -2)), -1);
    ASSERT_EQ(std::round(FindRoot((*p), 20, 2)), 1);
    auto h = F.Create("polynomial", {8, 0, 0, -1});
    ASSERT_EQ(std::round(FindRoot((*h), 20, 2)), 2);

}

TEST(Function_test, Error) {
    FunctionFactory F;

    auto A1 = F.Create("polynomial", {0, 2});
    ASSERT_THROW(*A1 + "abc", std::logic_error);
    ASSERT_THROW(*A1 + A1, std::logic_error);

    auto A2 = F.Create("ident");
    auto A3 = *A1 + *A2;
    ASSERT_THROW(A3 + "abc", std::logic_error);
    ASSERT_THROW(A3 + A1, std::logic_error);

    ASSERT_THROW(F.Create("power", std::vector<double>()), std::invalid_argument);
    ASSERT_THROW(F.Create("polynomial", std::vector<double>()), std::invalid_argument);
}