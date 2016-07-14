#pragma once

extern "C" {
    #include <wiredtiger-build/wiredtiger.h>
}

#include "options.h"

int run_tiger(options opts);
