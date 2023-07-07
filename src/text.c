#include "t3f/t3f.h"
#include "t3f/font.h"
#include "instance.h"
#include "game.h"

static char * dot_entry_text = NULL;
static char dot_initial_entry_text[1024] = {0};
static int dot_entry_text_max = 0;
static int dot_entry_text_pos = 0;

void dot_shadow_text(T3F_FONT * font, ALLEGRO_COLOR color, ALLEGRO_COLOR shadow, float x, float y, float sx, float sy, int flags, const char * text)
{
	t3f_draw_text(font, shadow, x + sx, y + sy, 0, flags, text);
	t3f_draw_text(font, color, x, y, 0, flags, text);
}

void dot_show_message(void * data, const char * message)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	t3f_render(false);
	al_hold_bitmap_drawing(false);
	al_draw_filled_rectangle(0.0, 0.0, t3f_virtual_display_width, t3f_virtual_display_height, al_map_rgba_f(0.0, 0.0, 0.0, 0.75));
	dot_shadow_text(app->font[DOT_FONT_16], t3f_color_white, al_map_rgba_f(0.0, 0.0, 0.0, 0.5), t3f_virtual_display_width / 2, DOT_GAME_PLAYFIELD_HEIGHT / 2 - t3f_get_font_line_height(app->font[DOT_FONT_16]) / 2, DOT_SHADOW_OX, DOT_SHADOW_OY, ALLEGRO_ALIGN_CENTRE, message);
	al_draw_filled_rectangle(0.0, t3f_virtual_display_width, 0.0, t3f_virtual_display_height, al_map_rgba_f(0.0, 0.0, 0.0, 0.5));
	al_flip_display();
}

void dot_enter_text(char * output, int size)
{
	dot_entry_text = output;
	strcpy(dot_initial_entry_text, dot_entry_text);
	dot_entry_text_max = size;
	dot_entry_text_pos = strlen(dot_entry_text);
	t3f_clear_keys();
}

int dot_text_entry_logic(void)
{
	char k;

	k = t3f_read_key(0);

	if(k > 0)
	{
		switch(k)
		{
			/* enter */
			case 0xD:
			{
				dot_entry_text[dot_entry_text_pos] = '\0';
				t3f_key[ALLEGRO_KEY_ENTER] = 0;
				return 1;
			}

			/* escape */
			case 0x1B:
			{
				strcpy(dot_entry_text, dot_initial_entry_text);
				return -1;
			}

			/* backspace */
			#ifndef ALLEGRO_MACOSX
			case 0x8:
			#else
			case 127:
			#endif
			{
				dot_entry_text[dot_entry_text_pos] = '\0';
				if(dot_entry_text_pos > 0)
				{
					dot_entry_text_pos--;
 					dot_entry_text[dot_entry_text_pos] = '\0';
				}
				break;
			}

			/* ignore these keys */
			case '\t':
			{
				break;
			}

			/* normal key */
			default:
			{
				if(dot_entry_text_pos < dot_entry_text_max - 1)
				{
					dot_entry_text[dot_entry_text_pos] = k;
					dot_entry_text_pos++;
					dot_entry_text[dot_entry_text_pos] = '\0';
				}
				break;
			}
		}
	}
	return 0;
}
