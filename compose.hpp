// MIT License

// Copyright (c) 2017 George Tzoumas

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
    
#ifndef _GT_COMPOSE_HPP_
#define _GT_COMPOSE_HPP_

#include<tuple>
#include<type_traits>

namespace gt
{

template<bool INNERFIRST,class...F>
class Compose
{
    static constexpr size_t N= sizeof...(F);
    static_assert(N > 0, "need at least one functor");

    // <F,G,H>(x) = H(G(F(x))) or F(G(H(x)))
    // by default functor order is interpreted as inner-to-outer...
    static constexpr size_t B = (INNERFIRST) ? (N-1) : 0; // first
    static constexpr size_t L = N-B-1; // last
    static constexpr int S = (INNERFIRST) ? -1 : 1; // step
public:

    template<class... AF>
    // too-perfect forwarding protection tag...
    Compose(std::true_type, AF&&... args): _functors( std::forward<AF>(args)... )
    {}

    template<class... Args>
    auto operator()(Args&&... args) const
    {
        return eval(std::integral_constant<size_t,B>{}, std::forward<Args>(args)... );
    }

private:

    template<class... Args>
    auto eval(std::integral_constant<size_t,L>, Args&&... args) const
    {
        return std::get<L>(_functors)(std::forward<Args>(args)...);
    }

    template<size_t K,class... Args>
    auto eval(std::integral_constant<size_t,K>, Args&&... args) const
    {
        return std::get<K>(_functors)( eval(std::integral_constant<size_t,K+S>{}, std::forward<Args>(args)...) );
    }

    // by using tuples, we get empty-type optimization for free ;)
    std::tuple<F...> _functors;
};

template<class...F>
auto compose(F&&... args)
{
    // we decay the universal refs, in order to STORE the functors inside the new object,
    // either by move (if movable) or copy, and avoid storing (potentially dangling) references...

    // if the functor is neither movable nor copyable... well, we could use a std::cref I guess...

    // inner to outer composition by default
    // we could probably deduce directly a tuple via make_tuple, rather that decaying...
    return Compose<true,std::decay_t<F>...>( std::true_type{}, std::forward<F>(args)... );
}

}

#endif
