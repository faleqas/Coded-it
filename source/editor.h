#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL.h>
#include <SDL_ttf.h>

#include "string.h"
#include "button.h"
#include "queue.h"
#include "stack.h"
#include "text_action.h"

enum
{
    EDITOR_STATE_EDIT,
    EDITOR_STATE_COMMAND,
    EDITOR_STATE_COMMAND_INPUT,
    EDITOR_STATE_COUNT //not an actual state. used for counting
};


typedef struct
{
    String text;
    int cursor_index;
    int cursor_line;
    int cursor_col;

    int x;
    int y;
} InputBuffer;


enum
{
    DRAW_AREA_BOTTOM_BORDER = 1 << 0,
    DRAW_AREA_TOP_BORDER = 1 << 1,
    DRAW_AREA_LEFT_BORDER = 1 << 2,
    DRAW_AREA_RIGHT_BORDER = 1 << 3 
};


typedef struct
{
    int x;
    int y;
    int w;
    int h;
    int border_thickness;

    int flags;
} DrawArea;


typedef struct _ProgramState
{
    SDL_Window* window;
    SDL_Surface* window_surface;
    TTF_Font* font;
    unsigned int font_size;
    TTF_Font* static_font; //the font for text not affected by zooming. like the status bar

    SDL_Color bg_color; //background color
    SDL_Color cursor_color;

    bool running;

    int window_w;
    int window_h;

    //NOTE(omar): this is the offset at which we render the
    //            file's text.
    //            used for scrolling.
    int camera_x;
    int camera_y;

    DrawArea editor_area;
    DrawArea file_explorer_area;

    int state;

    const char* current_file;

    Stack undo_tree;
    Stack redo_tree;

    Queue messages;
    String* message;
    uint32_t message_change_tic;
    
    InputBuffer text;
    InputBuffer command_input;
    Button* clicked_button; //the button clicked during the last
                            //EDITOR_STATE_COMMAND if any

//    String command_input_result; //the text the user sent/typed to us from
                                 //EDITOR_STATE_COMMAND_INPUT to be used by the
                                 //clicked button from EDITOR_STATE_COMMAND

    //TODO(omar): make char_w and char_h variables for the static_font too
    //instead of having to call TTF_SizeText
    int char_w;
    int char_h;
    
    bool draw_cursor; //used for a blinking cursor
    int last_cursor_blink_tic;

    Button buttons[10];

    int selection_start_index; //if selecting text. this is the index of the character we began selecting from
    String clipboard; //copied text if any

    //Themes/syntax highlighting/whatever stuff

    //every token type (TOKEN_NONE, TOKEN_KEYWORD) will work as an index into this colors array
    //to get the color of keywords in the current theme, state->token_colors[TOKEN_KEYWORD];
    SDL_Color* token_colors;

    //File explorer
    TTF_Font* file_explorer_font;
    Button* file_buttons;
    int file_count;
} ProgramState;


void editor_init(ProgramState* state);
void editor_destroy(ProgramState* state);

void editor_loop(ProgramState* state);

void editor_update(ProgramState* state);
void editor_draw(ProgramState* state);

void editor_render_draw_area(ProgramState* state, const DrawArea* area);

void editor_add_file_to_explorer(ProgramState* state, const char* filename);

bool editor_check_button_mouse_click(ProgramState* state, Button* buttons, int button_count);


//TODO(omar): find a better name than this shit
void editor_do_timed_events(ProgramState* state, bool* should_update);

//TODO(omar): move this function from editor.h/.c into another more suitable file

//bg_color (br, bg, bb) is the color of the background of the editor. set by the theme.
//this is used to color the box that is rendered around SDL ttf text when using RenderText_Shaded
void draw_text(TTF_Font* font, SDL_Surface* dst_surface, const char* text,
                int x, int y, int r, int g, int b,
                int br, int bg, int bb);


void editor_set_cursor(ProgramState* state, int index);

void editor_set_state(ProgramState* state, int new_state);

void editor_set_filename(ProgramState* state, const char* new_filename);

void editor_resize_and_reposition(ProgramState* state); //called after
                                                              //changing font size

//gets position in X and Y of the current input buffer's cursor
//returns false if there is no current input buffer.
bool editor_get_cursor_pos(ProgramState* state, int* out_x, int* out_y,
                           int char_h);

void editor_push_message(ProgramState* state, const String* msg);

void editor_push_text_action(ProgramState* state, TextAction* new_action);

void editor_select_first_enabled_button(ProgramState* state);

//also frees the action and any malloced variables it has
void editor_undo_text_action(ProgramState* state, const TextAction* action);
void editor_redo_text_action(ProgramState* state, const TextAction* action);
