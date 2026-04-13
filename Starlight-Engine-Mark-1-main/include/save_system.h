#ifndef STARLIGHT_SAVE_SYSTEM_H
#define STARLIGHT_SAVE_SYSTEM_H

#include <stdint.h>
#include <stdbool.h>

#define SAVE_MAX_GAMES  16
#define SAVE_MAX_SLOTS  32

typedef struct {
    uint32_t magic;      // 'STRG'
    uint32_t version;
    uint32_t unlocked[SAVE_MAX_GAMES]; // Bitmask for each game
    uint32_t total_wins;
    uint32_t total_hits;
    uint32_t total_score;
} SaveData;

void save_system_add_stat(const char* stat_name, int value);

void save_system_init(void);
void save_system_save(void);
void save_system_load(void);
bool save_system_is_unlocked(int game_id, int slot_index);
void save_system_unlock(int game_id, int slot_index);

#endif
