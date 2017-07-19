#include<iostream>
#include"compose.hpp"

int f(int x)
{
    return x + 1;
}

int g(int x)
{
    return x * 1;
}

int h(int x)
{
    return x - 2;
}

struct f2
{
    double operator()(int x) const
    {
        return static_cast<double>(x) * 1.3;
    }
};

struct f2xy
{
    double operator()(int x, int y) const
    {
        return static_cast<double>(x) * 1.3 + y;
    }
};

struct g2
{
    int operator()(double x) const
    {
        return (static_cast<int>(x) << 1);
    }
};

int main()
{
    auto r = gt::compose(f,g,h);
    std::cout << r(3) << ' ' << sizeof(r) << '\n';

    auto r2 = gt::compose(f2{}, g2{}, f2{}, g2{});
    std::cout << r2(3) << ' ' << sizeof(r2) << '\n';

    auto r3 = gt::compose(f2xy{}, g2{}, f2{});
    std::cout << r3(3,2) << ' ' << sizeof(r2) << '\n';
}
