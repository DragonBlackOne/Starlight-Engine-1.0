// Este projeto é feito por IA e só o prompt é feito por um humano.
#ifndef BEHAVIOR_TREE_H
#define BEHAVIOR_TREE_H

#include <stdbool.h>

typedef enum {
    BT_SUCCESS,
    BT_FAILURE,
    BT_RUNNING
} BTStatus;

typedef enum {
    BT_SELECTOR,
    BT_SEQUENCE,
    BT_ACTION
} BTNodeType;

struct BTNode;
typedef BTStatus (*BTActionFn)(void* target);

typedef struct BTNode {
    BTNodeType type;
    struct BTNode** children;
    int child_count;
    BTActionFn action;
} BTNode;

BTNode* bt_create_selector();
BTNode* bt_create_sequence();
BTNode* bt_create_action(BTActionFn action);
void bt_add_child(BTNode* parent, BTNode* child);
BTStatus bt_tick(BTNode* node, void* target);
void bt_destroy(BTNode* node);

#endif
