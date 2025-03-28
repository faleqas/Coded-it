#pragma once

#include "editor.h"


void editor_handle_events(ProgramState* state);
void editor_handle_events_keydown(ProgramState* state, SDL_Event e);
void editor_handle_events_keydown_command(ProgramState* state, SDL_Event e);
void editor_handle_events_keydown_textual(ProgramState* state, SDL_Event e);
