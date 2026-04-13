#include "slf_events.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    SLF_EventCallback callbacks[SLF_EVENT_MAX_LISTENERS];
    int count;
} EventBus;

// Mapa Global de Eventos
static EventBus g_event_bus[SLF_EVENT_MAX_TYPES];

// Fila de Eventos Diferidos
#define MAX_DEFERRED_EVENTS 256
static SLF_Event g_deferred_queue[MAX_DEFERRED_EVENTS];
static int g_deferred_count = 0;

// Arena dinamica para o payload de eventos diferidos
#define EVENT_PAYLOAD_ARENA_SIZE (1024 * 64) // 64kb
static unsigned char g_payload_arena[EVENT_PAYLOAD_ARENA_SIZE];
static int g_payload_offset = 0;

void slf_events_init(void) {
    memset(g_event_bus, 0, sizeof(g_event_bus));
    g_deferred_count = 0;
    g_payload_offset = 0;
}

void slf_events_shutdown(void) {
    slf_events_flush(); // Dispara o que sobrou
    memset(g_event_bus, 0, sizeof(g_event_bus));
}

bool slf_subscribe(SLF_EventType type, SLF_EventCallback callback) {
    if (type < 0 || type >= SLF_EVENT_MAX_TYPES) return false;
    EventBus* bus = &g_event_bus[type];
    if (bus->count >= SLF_EVENT_MAX_LISTENERS) return false;
    
    // Evita duplicata
    for(int i=0; i < bus->count; i++) {
        if (bus->callbacks[i] == callback) return true;
    }
    
    bus->callbacks[bus->count++] = callback;
    return true;
}

bool slf_unsubscribe(SLF_EventType type, SLF_EventCallback callback) {
    if (type < 0 || type >= SLF_EVENT_MAX_TYPES) return false;
    EventBus* bus = &g_event_bus[type];
    
    for(int i=0; i < bus->count; i++) {
        if (bus->callbacks[i] == callback) {
            // Remove with swap-n-pop O(1)
            bus->callbacks[i] = bus->callbacks[bus->count - 1];
            bus->count--;
            return true;
        }
    }
    return false;
}

void slf_emit(SLF_EventType type, void* data, int data_size) {
    if (type < 0 || type >= SLF_EVENT_MAX_TYPES) return;
    EventBus* bus = &g_event_bus[type];
    
    SLF_Event ev;
    ev.type = type;
    ev.data = data;
    ev.data_size = data_size;
    
    for(int i=0; i < bus->count; i++) {
        if (bus->callbacks[i]) {
            bus->callbacks[i](&ev);
        }
    }
}

void slf_emit_deferred(SLF_EventType type, void* data, int data_size) {
    if (g_deferred_count >= MAX_DEFERRED_EVENTS) {
        printf("[SLF_EVENTS] Alerta: Fila de eventos deferred estourou!\n");
        return;
    }
    
    SLF_Event ev;
    ev.type = type;
    ev.data_size = data_size;
    
    if (data && data_size > 0) {
        if (g_payload_offset + data_size <= EVENT_PAYLOAD_ARENA_SIZE) {
            void* payload_ptr = &g_payload_arena[g_payload_offset];
            memcpy(payload_ptr, data, data_size);
            ev.data = payload_ptr;
            g_payload_offset += data_size;
        } else {
            printf("[SLF_EVENTS] Alerta: Sem memoria para payload diferido.\n");
            ev.data = NULL;
            ev.data_size = 0;
        }
    } else {
        ev.data = NULL;
    }
    
    g_deferred_queue[g_deferred_count++] = ev;
}

void slf_events_flush(void) {
    for(int i=0; i < g_deferred_count; i++) {
        SLF_Event ev = g_deferred_queue[i];
        
        EventBus* bus = &g_event_bus[ev.type];
        for(int j=0; j < bus->count; j++) {
            if (bus->callbacks[j]) {
                bus->callbacks[j](&ev);
            }
        }
    }
    
    g_deferred_count = 0;
    g_payload_offset = 0; // Reseta memory arena a cada frame
}
