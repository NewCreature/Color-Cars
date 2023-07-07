#include "t3f/t3f.h"
#include "instance.h"
#include "text.h"
#include "color.h"
#include "intro.h"

unsigned long dot_leaderboard_obfuscate_score(unsigned long score)
{
  return score * 'd' + '2' + 'd' + 's';
}

unsigned long dot_leaderboard_unobfuscate_score(unsigned long score)
{
  return (score - 's' - 'd' - '2') / 'd';
}

void dot_leaderboard_logic(void * data)
{
    APP_INSTANCE * app = (APP_INSTANCE *)data;
    bool m = false;

    if(app->current_menu == DOT_MENU_LEADERBOARD && t3f_mouse_button[0])
    {
        m = true;
    }
    dot_bg_objects_logic(data, DOT_GAME_LEVEL_BASE_SPEED);
    if(t3f_key[ALLEGRO_KEY_ESCAPE] || t3f_key[ALLEGRO_KEY_BACK] || m)
    {
        if(app->current_menu == DOT_MENU_LEADERBOARD_2)
        {
            dot_menu_proc_leaderboard_main_menu(data, 0, NULL);
        }
        else
        {
            dot_menu_proc_leaderboard_back(data, 0, NULL);
        }
        t3f_key[ALLEGRO_KEY_ESCAPE] = 0;
        t3f_key[ALLEGRO_KEY_BACK] = 0;
        t3f_mouse_button[0] = false;
    }
    else if(app->current_menu == DOT_MENU_LEADERBOARD_2 && t3f_mouse_button[0])
    {
        dot_menu_proc_leaderboard_back(data, 0, NULL);
        app->current_menu = DOT_MENU_LEADERBOARD_2;
        t3f_mouse_button[0] = false;
    }
    app->tick++;
    if(!app->desktop_mode)
    {
        t3f_process_gui(app->menu[app->current_menu], app);
    }
}

void dot_leaderboard_render(void * data)
{
    APP_INSTANCE * app = (APP_INSTANCE *)data;
    int i;
	char buf[256] = {0};
    ALLEGRO_COLOR text_color;

    al_clear_to_color(app->level_color[0]);
    al_hold_bitmap_drawing(true);
    dot_bg_objects_render(data);
    al_draw_bitmap(app->bitmap[DOT_BITMAP_BG], 0, 0, 0);
    dot_shadow_text(app->font[DOT_FONT_32], t3f_color_white, al_map_rgba_f(0.0, 0.0, 0.0, 0.5), t3f_virtual_display_width / 2, 32, DOT_SHADOW_OX * 2, DOT_SHADOW_OY * 2, ALLEGRO_ALIGN_CENTRE, "Leaderboard");
    for(i = 0; i < app->leaderboard->entries; i++)
    {
        if(i == app->leaderboard_spot && (app->tick / 6) % 2)
        {
            text_color = al_map_rgba_f(1.0, 1.0, 0.0, 1.0);
        }
        else
        {
            text_color = t3f_color_white;
        }
        sprintf(buf, " %s", app->leaderboard->entry[i]->name);
        dot_shadow_text(app->font[DOT_FONT_16], text_color, al_map_rgba_f(0.0, 0.0, 0.0, 0.5), 4, 4 + (i + 3) * 32, DOT_SHADOW_OX, DOT_SHADOW_OY, 0, buf);
        sprintf(buf, "%lu ", dot_leaderboard_unobfuscate_score(app->leaderboard->entry[i]->score));
        dot_shadow_text(app->font[DOT_FONT_16], text_color, al_map_rgba_f(0.0, 0.0, 0.0, 0.5), t3f_virtual_display_width - 4, 4 + (i + 3) * 32, DOT_SHADOW_OX, DOT_SHADOW_OY, ALLEGRO_ALIGN_RIGHT, buf);
    }
    if(!app->desktop_mode)
    {
        t3f_render_gui(app->menu[app->current_menu]);
    }
    al_hold_bitmap_drawing(false);
	dot_intro_render_split(data);
}
