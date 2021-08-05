#pragma once
#include "platform.hpp"

struct Game_Memory {
    int counter;
    // anything
};

typedef void tick_fun(Game_Memory*, Platform_Functions);

struct Game_Functions {
    tick_fun* tick;
};
