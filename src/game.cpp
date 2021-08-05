#include <stdio.h>
#include "game.hpp"

void some_game_code(Game_Memory* game_memory, Platform_Functions platform_functions) {
    char buffer[100] = {};
    sprintf(buffer, "Change me at runtime and recompile! (also counter: %.1f)", game_memory->counter/10.0f);
    platform_functions.show_message(buffer);
}

tick_fun* tick = [](Game_Memory* game_memory, Platform_Functions platform_functions) -> void {
    game_memory->counter++;
    if (game_memory->counter >= 600) {
        game_memory->counter = 0;
    }
    some_game_code(game_memory, platform_functions);
};
