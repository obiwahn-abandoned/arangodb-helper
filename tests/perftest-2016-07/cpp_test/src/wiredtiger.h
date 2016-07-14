#pragma once

extern "C" {
    #include <wiredtiger/wiredtiger.h>
}

#include "options.h"

int run_tiger(options opts);
