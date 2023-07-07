#include "t3f/t3f.h"
#include "instance.h"
#include "intro.h"
#include "text.h"

static const char * privacy_text[] =
{
	"",
	"",
	"",
	"Information We Collect:",
	"",
	" We collect only enough",
	" information to process your",
	" request. With regards to this",
	" software, that entails storing",
	" any scores you submit to our",
	" leaderboards (includes your",
	" profile name and score",
	" information).",
	"",
	"",
	"What We Do with Your",
	"Information:",
	"",
	" Your leaderboard data is",
	" accessible to everyone, so it",
	" is advisable to not put",
	" personal information you don't",
	" want to share in your screen",
	" name.",
	NULL
};

void dot_privacy_logic(void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	bool m = false;

	dot_bg_objects_logic(data, DOT_GAME_LEVEL_BASE_SPEED);
	if(app->desktop_mode && t3f_mouse_button[0])
	{
		m = true;
	}
	if(t3f_key[ALLEGRO_KEY_ESCAPE] || t3f_key[ALLEGRO_KEY_BACK] || m)
	{
		dot_menu_proc_privacy_back(data, 0, NULL);
		t3f_key[ALLEGRO_KEY_ESCAPE] = 0;
		t3f_key[ALLEGRO_KEY_BACK] = 0;
		t3f_mouse_button[0] = false;
	}
	app->tick++;
	if(!app->desktop_mode)
	{
		t3f_process_gui(app->menu[app->current_menu], app);
	}
}

void dot_privacy_render(void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	int i;

	al_clear_to_color(app->level_color[0]);
	al_hold_bitmap_drawing(true);
	dot_bg_objects_render(data);
	al_draw_bitmap(app->bitmap[DOT_BITMAP_BG], 0, 0, 0);
	for(i = 0; i < 128; i++)
	{
		if(privacy_text[i])
		{
			dot_shadow_text(app->font[DOT_FONT_16], t3f_color_white, al_map_rgba_f(0.0, 0.0, 0.0, 0.5), 8, i * 16, DOT_SHADOW_OX, DOT_SHADOW_OY, 0, privacy_text[i]);
		}
		else
		{
			break;
		}
	}
	if(!app->desktop_mode)
	{
		t3f_render_gui(app->menu[app->current_menu]);
	}
	al_hold_bitmap_drawing(false);
	dot_intro_render_split(data);
}
