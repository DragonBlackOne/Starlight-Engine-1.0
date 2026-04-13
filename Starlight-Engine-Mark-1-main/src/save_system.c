#include "save_system.h"
#include <stdio.h>
#include <string.h>

SaveData g_save;
static const char* SAVE_FILENAME = "save_game.dat";

void save_system_init(void) {
    memset(&g_save, 0, sizeof(SaveData));
    g_save.magic = 0x47525453; // 'STRG'
    g_save.version = 1;
    save_system_load();
}

void save_system_save(void) {
    const char* tmp_name = "save_game.dat.tmp";
    FILE* f = fopen(tmp_name, "wb");
    if (f) {
        size_t written = fwrite(&g_save, sizeof(SaveData), 1, f);
        fclose(f);
        
        if (written == 1) { // 100% of the struct bytes were written
            remove(SAVE_FILENAME); // Must remove original on Windows for rename to work
            if (rename(tmp_name, SAVE_FILENAME) == 0) {
                printf("[SYSTEM] Progress saved atomically to %s\n", SAVE_FILENAME);
            } else {
                printf("[SYSTEM] CRITICAL: Failed to rename atomic save file!\n");
            }
        } else {
            printf("[SYSTEM] CRITICAL: Incomplete disk write. Save rollback triggered.\n");
            remove(tmp_name); // Cleanup corrupted tmp
        }
    } else {
        printf("[SYSTEM] CRITICAL: Failed to create atomic temporary save file!\n");
    }
}

void save_system_load(void) {
    FILE* f = fopen(SAVE_FILENAME, "rb");
    if (f) {
        SaveData temp;
        if (fread(&temp, sizeof(SaveData), 1, f) == 1) {
            if (temp.magic == 0x47525453) {
                memcpy(&g_save, &temp, sizeof(SaveData));
                printf("[SYSTEM] Progress loaded. Pong Slots: 0x%08X\n", g_save.unlocked[0]);
            }
        }
        fclose(f);
    } else {
        printf("[SYSTEM] No save file found. Starting fresh.\n");
    }
}

bool save_system_is_unlocked(int game_id, int slot_index) {
    if (game_id < 0 || game_id >= SAVE_MAX_GAMES) return false;
    return (g_save.unlocked[game_id] & (1 << slot_index)) != 0;
}

void save_system_unlock(int game_id, int slot_index) {
    if (game_id < 0 || game_id >= SAVE_MAX_GAMES) return;
    if (slot_index < 0 || slot_index >= SAVE_MAX_SLOTS) return;
    
    if (!(g_save.unlocked[game_id] & (1 << slot_index))) {
        g_save.unlocked[game_id] |= (1 << slot_index);
        printf("[SYSTEM] UNLOCKED: Game %d, Slot %d!\n", game_id, slot_index);
        save_system_save();
    }
}

void save_system_add_stat(const char* stat_name, int value) {
    if (strcmp(stat_name, "wins") == 0) g_save.total_wins += value;
    else if (strcmp(stat_name, "hits") == 0) g_save.total_hits += value;
    else if (strcmp(stat_name, "score") == 0) g_save.total_score += value;
    save_system_save();
}
