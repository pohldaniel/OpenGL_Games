#pragma once

#include "NkContext.h"

struct node {
    int ID;
    char name[32];
    struct nk_rect bounds;
    float value;
    struct nk_color color;
    int input_count;
    int output_count;
    struct node* next;
    struct node* prev;
};

struct node_link {
    int input_id;
    int input_slot;
    int output_id;
    int output_slot;
    struct nk_vec2 in;
    struct nk_vec2 out;
};

struct node_linking {
    int active;
    struct node* node;
    int input_id;
    int input_slot;
};

struct node_editor {
    int initialized;
    struct node node_buf[32];
    struct node_link links[64];
    struct node* begin;
    struct node* end;
    int node_count;
    int link_count;
    struct nk_rect bounds;
    struct node* selected;
    int show_grid;
    struct nk_vec2 scrolling;
    struct node_linking linking;
};
static struct node_editor nodeEditor;

extern "C" {
    void node_editor_push(struct node_editor* editor, struct node* node);
    void node_editor_pop(struct node_editor* editor, struct node* node);
    struct node* node_editor_find(struct node_editor* editor, int ID);
    void node_editor_add(struct node_editor* editor, const char* name, struct nk_rect bounds, struct nk_color col, int in_count, int out_count);
    void node_editor_link(struct node_editor* editor, int in_id, int in_slot, int out_id, int out_slot);
    void node_editor_init(struct node_editor* editor);
    int node_editor();
}