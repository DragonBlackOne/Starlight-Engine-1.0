#ifndef SLF_EVENTS_H
#define SLF_EVENTS_H

#include <stdint.h>
#include <stdbool.h>

#define SLF_EVENT_MAX_LISTENERS 64
#define SLF_EVENT_MAX_TYPES 256

// Tipos de Eventos Internos do Framework 
typedef enum {
    SLF_EVENT_CUSTOM = 0,
    SLF_EVENT_SCENE_LOADED,
    SLF_EVENT_ENTITY_SPAWNED,
    SLF_EVENT_ENTITY_DESTROYED,
    SLF_EVENT_PLAYER_DAMAGE,
    SLF_EVENT_WINDOW_RESIZED,
    // Deixe espaco para eventos criados pelos jogos especificamente
    SLF_EVENT_USER_START = 128
} SLF_EventType;

typedef struct {
    SLF_EventType type;
    void* data; // Payload dinamico dependente do tipo do evento
    int data_size;
} SLF_Event;

typedef void (*SLF_EventCallback)(SLF_Event* event);

// Inicializa e desliga memoria do pub-sub
void slf_events_init(void);
void slf_events_shutdown(void);

// Assinatura e Desassinatura
bool slf_subscribe(SLF_EventType type, SLF_EventCallback callback);
bool slf_unsubscribe(SLF_EventType type, SLF_EventCallback callback);

// Despacho Síncrono Imediato
void slf_emit(SLF_EventType type, void* data, int data_size);

// Despacho Assíncrono (Enfileirado pro fim do frame)
void slf_emit_deferred(SLF_EventType type, void* data, int data_size);
void slf_events_flush(void);

#endif
