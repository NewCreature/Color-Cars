#ifndef DOT_TEXT_H
#define DOT_TEXT_H

#include "t3f/t3f.h"
#include "t3f/font.h"

void dot_shadow_text(T3F_FONT * font, ALLEGRO_COLOR color, ALLEGRO_COLOR shadow, float x, float y, float sx, float sy, int flags, const char * text);
void dot_show_message(void * data, const char * message);

bool dot_enter_text(char * output, int size);
int dot_text_entry_logic(void);

#endif
