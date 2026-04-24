// Este projeto é feito por IA e só o prompt é feito por um humano.
#ifndef DEBUG_CONSOLE_H
#define DEBUG_CONSOLE_H

#include <stdbool.h>
#include <cglm/cglm.h>

#define MAX_LOG_MESSAGES 128
#define MAX_LOG_LENGTH 256

typedef enum {
    CVAR_INT,
    CVAR_FLOAT,
    CVAR_STRING
} CVarType;

typedef struct {
    char name[64];
    CVarType type;
    void* ptr;
} CVar;

typedef struct {
    char messages[MAX_LOG_MESSAGES][MAX_LOG_LENGTH];
    int head;
    int count;
    bool visible;
    float animation_t;
    
    // Command Input
    char input_buffer[256];
    int cursor_pos;
    
    // Config Registry
    CVar cvars[128];
    int cvar_count;
} DebugConsole;

struct SLF_App; // Forward decl

void debug_console_init(DebugConsole* console);
void debug_log(const char* fmt, ...);
void debug_console_render(struct SLF_App* app, DebugConsole* console);

void debug_console_register_cvar_float(DebugConsole* console, const char* name, float* ptr);
void debug_console_register_cvar_int(DebugConsole* console, const char* name, int* ptr);
void debug_console_execute(DebugConsole* console, const char* command);
void debug_console_on_text_input(DebugConsole* console, const char* text);
void debug_console_on_key(DebugConsole* console, int keycode);

#endif
