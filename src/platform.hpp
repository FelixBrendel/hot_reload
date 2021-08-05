#pragma once

typedef void show_message_fun(const char*);

struct Platform_Functions {
    show_message_fun* show_message;
};
