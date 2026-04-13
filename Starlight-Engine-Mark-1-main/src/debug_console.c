#include "debug_console.h"
#include "starlight.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

static DebugConsole* g_active_console = NULL;

void debug_console_init(DebugConsole* console) {
    memset(console, 0, sizeof(DebugConsole));
    console->visible = false;
    g_active_console = console;
    debug_log("[SYSTEM] Debug Console Ready. Press ~ to toggle.");
}

void debug_log(const char* fmt, ...) {
    if (!g_active_console) return;

    va_list args;
    va_start(args, fmt);
    
    char buffer[MAX_LOG_LENGTH];
    vsnprintf(buffer, MAX_LOG_LENGTH, fmt, args);
    va_end(args);

    // Buffer circular
    strncpy(g_active_console->messages[g_active_console->head], buffer, MAX_LOG_LENGTH - 1);
    g_active_console->head = (g_active_console->head + 1) % MAX_LOG_MESSAGES;
    if (g_active_console->count < MAX_LOG_MESSAGES) g_active_console->count++;

    // Print para stdout também para facilitar debug externo
    printf("%s\n", buffer);
}

void debug_console_render(SLF_App* app, DebugConsole* console) {
    if (!console->visible) return;

    float height = app->height * 0.4f;
    vec4 bg_color = {0.05f, 0.05f, 0.05f, 0.85f};
    starlight_ui_draw_rect(app, 0, 0, (float)app->width, height, bg_color);

    // Linha inferior decorativa
    vec4 border_color = {0.0f, 0.8f, 1.0f, 1.0f};
    starlight_ui_draw_rect(app, 0, height - 2, (float)app->width, 2, border_color);

    // Renderizar mensagens (de baixo para cima)
    float start_y = height - 25.0f;
    int index = (console->head - 1 + MAX_LOG_MESSAGES) % MAX_LOG_MESSAGES;
    vec4 text_color = {1.0f, 1.0f, 1.0f, 1.0f};

    // Nota: Assumimos que uma fonte padrão está carregada no app ou framework
    // Para fins de framework, usamos um placeholder ou logica simplificada
    for (int i = 0; i < console->count && start_y > 10; i++) {
        // starlight_ui_draw_text(app, app->debug_font, console->messages[index], 10, start_y, text_color);
        index = (index - 1 + MAX_LOG_MESSAGES) % MAX_LOG_MESSAGES;
        start_y -= 18.0f;
    }
    // Desenhar a linha de input
    char prompt[300];
    snprintf(prompt, sizeof(prompt), "] %s_", console->input_buffer);
    // starlight_ui_draw_text(app, app->debug_font, prompt, 10, 10, text_color);
}

void debug_console_register_cvar_float(DebugConsole* console, const char* name, float* ptr) {
    if (console->cvar_count >= 128) return;
    CVar* cv = &console->cvars[console->cvar_count++];
    strncpy(cv->name, name, 63);
    cv->type = CVAR_FLOAT;
    cv->ptr = ptr;
}

void debug_console_register_cvar_int(DebugConsole* console, const char* name, int* ptr) {
    if (console->cvar_count >= 128) return;
    CVar* cv = &console->cvars[console->cvar_count++];
    strncpy(cv->name, name, 63);
    cv->type = CVAR_INT;
    cv->ptr = ptr;
}

void debug_console_execute(DebugConsole* console, const char* command) {
    debug_log("] %s", command);
    
    // Parser super simples para CVars
    char cmd_name[64] = {0};
    char arg[64] = {0};
    int parsed = sscanf(command, "%63s %63s", cmd_name, arg);
    
    if (parsed >= 1) {
        for (int i = 0; i < console->cvar_count; i++) {
            CVar* cv = &console->cvars[i];
            if (strcmp(cv->name, cmd_name) == 0) {
                if (parsed == 1) {
                    if (cv->type == CVAR_FLOAT) debug_log("  %s = %f", cv->name, *(float*)cv->ptr);
                    else if (cv->type == CVAR_INT) debug_log("  %s = %d", cv->name, *(int*)cv->ptr);
                } else {
                    if (cv->type == CVAR_FLOAT) {
                        float v; sscanf(arg, "%f", &v); *(float*)cv->ptr = v;
                        debug_log("  %s set to %f", cv->name, v);
                    } else if (cv->type == CVAR_INT) {
                        int v; sscanf(arg, "%d", &v); *(int*)cv->ptr = v;
                        debug_log("  %s set to %d", cv->name, v);
                    }
                }
                return;
            }
        }
        debug_log("Unknown command: %s", cmd_name);
    }
}

void debug_console_on_text_input(DebugConsole* console, const char* text) {
    if (!console->visible) return;
    int len = strlen(text);
    if (console->cursor_pos + len < 255) {
        // Ignora tilde na criacao do buffer
        if (text[0] != '`' && text[0] != '~') {
            strcpy(&console->input_buffer[console->cursor_pos], text);
            console->cursor_pos += len;
        }
    }
}

void debug_console_on_key(DebugConsole* console, int keycode) {
    if (!console->visible) return;
    
    // SDLK_BACKSPACE = '\b'
    if (keycode == '\b' && console->cursor_pos > 0) {
        console->cursor_pos--;
        console->input_buffer[console->cursor_pos] = '\0';
    }
    // SDLK_RETURN = '\r'
    else if (keycode == '\r') {
        if (console->cursor_pos > 0) {
            debug_console_execute(console, console->input_buffer);
            console->input_buffer[0] = '\0';
            console->cursor_pos = 0;
        }
    }
}
