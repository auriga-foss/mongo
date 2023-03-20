//
// asio.cpp
// ~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "asio/impl/src.hpp"

#ifdef __KOS__
// Note: KOS CE SDK has a sockatmark() prototype but no implementation. This is a hack to make the
// linking process successful. This hack works in both cases: with or without the --use-system-boost
// option.
// FIXME: remove when the function is implemented in KOS.
#include <stdio.h>
#include <stdlib.h>
int sockatmark(int s) {
    fprintf(stderr, "[ERROR] sockatmark() not implemented!\n");
    abort();
}
#endif
