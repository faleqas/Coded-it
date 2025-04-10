#include "editor_fileio.h"


void editor_save_file(const ProgramState* state)
{
    FILE* fp;
    if (!state->current_file)
    {
        printf("No current open file.\n");
        return;
    }
    fopen_s(&fp, state->current_file, "w");

    const char* msg_format;
    if (!fp)
    {
        //it should never be NULL ??
        msg_format = "Couldn't save to file '%s'.";
    }
    else
    {
        //fucking text.text.text
        fwrite(state->text.text.text, sizeof(char), state->text.text.len, fp);
        fclose(fp);

        msg_format = "Saved to file '%s'.";
    }

    //send message
    size_t msg_size = sizeof(char) * (strlen(msg_format) + strlen(state->current_file) + 1);
    char* msg = malloc(msg_size); 

    snprintf(msg, msg_size, msg_format, state->current_file);
    
    String str = {0};
    String_set(&str, msg);
    free(msg);

    editor_push_message(state, &str);
}


void editor_open_file(ProgramState* state)
{
    FILE* fp;
    if (!state->current_file)
    {
        printf("No file selected to open.\n");
    }
    fopen_s(&fp, state->current_file, "r");

    char* msg_format = NULL;
    if (!fp)
    {
        msg_format = "Couldn't open file '%s'.";
    }
    else
    {
        String_clear(&(state->text.text));
        state->text.cursor_index = 0;
        state->camera_x = 0;
        state->camera_y = 0;
        while (!feof(fp))
        {
            char c = fgetc(fp);
            if (c <= 0) //a weird character appears at the end of every .txt file. not sure why
            {
                continue;
            }
            String_push(&(state->text.text), c);
            if (c == '\n')
            {
                continue;
            }
        }
        fclose(fp);

        msg_format = "Opened file '%s'.";
    }

    size_t msg_size = sizeof(char) * (strlen(msg_format) + strlen(state->current_file) + 1);
    char* msg = malloc(msg_size); 

    snprintf(msg, msg_size, msg_format, state->current_file);
    
    String str = {0};
    String_set(&str, msg);
    free(msg);

    editor_push_message(state, &str);
}
