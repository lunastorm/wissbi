//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <vector>

// Index const vector out of bounds.

#if _LIBCPP_DEBUG2 >= 1

#define _LIBCPP_ASSERT(x, m) ((x) ? (void)0 : std::exit(0))

#include <vector>
#include <cassert>
#include <iterator>
#include <exception>
#include <cstdlib>

int main()
{
    typedef int T;
    typedef std::vector<T> C;
    const C c(1);
    assert(c[0] == 0);
    assert(c[1] == 0);
    assert(false);
}

#else

int main()
{
}

#endif
