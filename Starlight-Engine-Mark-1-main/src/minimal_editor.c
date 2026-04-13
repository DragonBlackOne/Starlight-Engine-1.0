#define SDL_MAIN_HANDLED
#include "starlight.h"
#include <stdio.h>

void Minimal_OnStart(SLF_App* app) {
    printf("[MINIMAL] OnStart called!\n"); fflush(stdout);
}

int main(int argc, char* argv[]) {
    printf("[MINIMAL] main started!\n"); fflush(stdout);
    SLF_Config config = {0};
    config.on_start = Minimal_OnStart;
    return starlight_framework_init("Minimal Starlight", 800, 600, &config);
}
