#include "behavior_tree.h"
#include <stdlib.h>
#include <stdio.h>

BTNode* bt_create_node(BTNodeType type) {
    BTNode* node = (BTNode*)malloc(sizeof(BTNode));
    node->type = type;
    node->children = NULL;
    node->child_count = 0;
    node->action = NULL;
    return node;
}

BTNode* bt_create_selector() { return bt_create_node(BT_SELECTOR); }
BTNode* bt_create_sequence() { return bt_create_node(BT_SEQUENCE); }
BTNode* bt_create_action(BTActionFn action) {
    BTNode* node = bt_create_node(BT_ACTION);
    node->action = action;
    return node;
}

void bt_add_child(BTNode* parent, BTNode* child) {
    parent->child_count++;
    parent->children = (BTNode**)realloc(parent->children, parent->child_count * sizeof(BTNode*));
    parent->children[parent->child_count - 1] = child;
}

BTStatus bt_tick(BTNode* node, void* target) {
    if (node->type == BT_ACTION) {
        return node->action(target);
    }

    if (node->type == BT_SEQUENCE) {
        for (int i = 0; i < node->child_count; i++) {
            BTStatus status = bt_tick(node->children[i], target);
            if (status != BT_SUCCESS) return status;
        }
        return BT_SUCCESS;
    }

    if (node->type == BT_SELECTOR) {
        for (int i = 0; i < node->child_count; i++) {
            BTStatus status = bt_tick(node->children[i], target);
            if (status != BT_FAILURE) return status;
        }
        return BT_FAILURE;
    }

    return BT_FAILURE;
}

void bt_destroy(BTNode* node) {
    if (!node) return;
    for (int i = 0; i < node->child_count; i++) {
        bt_destroy(node->children[i]);
    }
    if (node->children) free(node->children);
    free(node);
}
