#include <stdio.h>
#include <windows.h>
#include "platform.hpp"
#include "game.hpp"

struct Game_Lib {
    Game_Functions game_functions;
    HMODULE  dll_handle;
    FILETIME last_changed;
};


show_message_fun* show_message = [](const char* message) {
    int printed = printf("\r%s", message);
    printf("%*s", 100-printed, "");
    fflush(stdout);
};

FILETIME get_last_write_time(const char* path) {
    FILETIME last_write_time = {};

    WIN32_FILE_ATTRIBUTE_DATA attrib_data;
    if(GetFileAttributesExA(path, GetFileExInfoStandard, &attrib_data))
        last_write_time = attrib_data.ftLastWriteTime;

    return last_write_time;
}

Game_Lib load_game_lib() {
    Game_Lib game_lib = {};
    game_lib.last_changed = get_last_write_time(game_dll_name);

    // NOTE(Felix): We cannot load the dll directly, because our exe will lock
    //   it then and the compiler won't be able to overwirte it. So we copy it,
    //   load it, and when time comes to reload, we unload the dll and overwrite
    //   the copy with a new copy and load that one again.
    CopyFileA(game_dll_name, "game_copy.dll", false);

    game_lib.dll_handle = LoadLibraryA("game_copy.dll");
    if (game_lib.dll_handle != NULL) {
        game_lib.game_functions.tick = *(tick_fun**)GetProcAddress(game_lib.dll_handle, "tick");
    }

    return game_lib;
}

void unload_game_lib(Game_Lib* game_lib) {
    FreeLibrary(game_lib->dll_handle);
    game_lib->dll_handle = 0;
    game_lib->game_functions.tick = nullptr;
}

bool should_reload_game_lib(Game_Lib* game_lib) {
    FILETIME last_write_time = get_last_write_time(game_dll_name);
    return (CompareFileTime(&last_write_time, &game_lib->last_changed) != 0);
}

int main() {
    Game_Lib game_lib;
    Game_Memory game_memory {};
    Platform_Functions platform_functions {
        .show_message = *show_message,
    };

    game_lib = load_game_lib();

    // NOTE(Felix): Variables so we only reload the dll when it is done being
    //   written to, not while it is written; See note below
    bool reload_asap = false;
    FILETIME last_observed_write;

    // Game loop
    while (true) {
        game_lib.game_functions.tick(&game_memory, platform_functions);
        Sleep(100);

        // NOTE(Felix): When a change is detected, it waits until the frame when
        //   the file time is not chaning anymore, to allow the file to be
        //   written completely before we attempt to load it, otherwise we might
        //   end up reloading it while it is being written by the compiler. So
        //   when we detect that it is chaning we set =reload_asap= to true and
        //   check every frame if it stopped being written to and only then
        //   actually reload it.
        if (!reload_asap) {
            if (should_reload_game_lib(&game_lib)) {
                // the file changed suddenly!
                reload_asap = true;
                last_observed_write = get_last_write_time(game_dll_name);
            }
        } else {
            FILETIME dll_now_time = get_last_write_time(game_dll_name);
            if (CompareFileTime(&last_observed_write, &dll_now_time) != 0) {
                // the file is still being written to
                last_observed_write = dll_now_time;
            } else {
                // the file stopped being written to: we reload it
                show_message("Hot reload!");
                printf("\n");
                unload_game_lib(&game_lib);
                game_lib = load_game_lib();
                reload_asap = false;
            }
        }
    }
}
