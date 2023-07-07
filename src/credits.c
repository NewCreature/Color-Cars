#include "t3f/t3f.h"
#include "instance.h"
#include "credits.h"
#include "color.h"
#include "text.h"

static bool dot_add_credit(DOT_CREDITS * cp, const char * header)
{
    if(cp->credits < DOT_MAX_CREDITS)
    {
        strcpy(cp->credit[cp->credits].header, header);
        cp->credits++;
        return true;
    }
    return false;
}

static bool dot_add_credit_name(DOT_CREDITS * cp, const char * name)
{
    if(cp->credit[cp->credits - 1].names < DOT_CREDIT_MAX_NAMES)
    {
        strcpy(cp->credit[cp->credits - 1].name[cp->credit[cp->credits - 1].names], name);
        cp->credit[cp->credits - 1].names++;
        return true;
    }
    return false;
}

void dot_setup_credits(DOT_CREDITS * cp)
{
    memset(cp, 0, sizeof(DOT_CREDITS));

    dot_add_credit(cp, "Production");
    dot_add_credit_name(cp, "Todd Cope");

    dot_add_credit(cp, "Design");
    dot_add_credit_name(cp, "Todd Cope");

    dot_add_credit(cp, "Programming");
    dot_add_credit_name(cp, "Todd Cope");

    dot_add_credit(cp, "Graphics");
    dot_add_credit_name(cp, "Travis Cope");
    dot_add_credit_name(cp, "Todd Cope");

    dot_add_credit(cp, "Sound");
    dot_add_credit_name(cp, "Travis Cope");
    dot_add_credit_name(cp, "Todd Cope");

    dot_add_credit(cp, "Music");
    dot_add_credit_name(cp, "Todd Cope");

    dot_add_credit(cp, "Testing");
    dot_add_credit_name(cp, "Todd Cope");
    dot_add_credit_name(cp, "Travis Cope");
}

void dot_credits_logic(DOT_CREDITS * cp)
{
    if(cp->current_credit < cp->credits)
    {
        switch(cp->state)
        {
            case DOT_CREDITS_STATE_IN:
            {
                cp->ox += 8.0;
                if(cp->ox >= 0.0)
                {
                    cp->ox = 0.0;
                    cp->tick = 0;
                    cp->state = DOT_CREDITS_STATE_SHOW;
                }
                break;
            }
            case DOT_CREDITS_STATE_SHOW:
            {
                cp->tick++;
                if(cp->tick >= (cp->credit[cp->current_credit].names + 1) * 120)
                {
                    cp->state = DOT_CREDITS_STATE_OUT;
                }
                break;
            }
            case DOT_CREDITS_STATE_OUT:
            {
                cp->ox -= 8.0;
                if(cp->ox < -t3f_virtual_display_width)
                {
                    cp->current_credit++;
                    cp->tick = 0;
                    cp->state = DOT_CREDITS_STATE_IN;
                }
                break;
            }
        }
    }
}

void dot_credits_render(void * data, float ox)
{
    APP_INSTANCE * app = (APP_INSTANCE *)data;
    int i;
    float pos_y;
    float pos_ox;

	al_hold_bitmap_drawing(true);
	dot_bg_objects_render(data);
	for(i = 0; i < 540 / 32; i++)
	{
		al_draw_bitmap(app->bitmap[DOT_BITMAP_BALL_RED + i % 6], i * 32 + 18 + ox, sin((float)(i * 4 + app->tick) / 10.0) * 32 + 32 + 12, 0);
		al_draw_bitmap(app->bitmap[DOT_BITMAP_BALL_RED + (i + 3) % 6], i * 32 + 18 + ox, cos((float)(i * 4 + app->tick) / 10.0) * 32 + DOT_GAME_PLAYFIELD_HEIGHT - 32 - 32 - 12 - 1, 0);
    }
    al_hold_bitmap_drawing(false);

    if(app->credits.state != DOT_CREDITS_STATE_WAIT)
    {
        if(app->credits.current_credit < app->credits.credits)
        {
            pos_y = DOT_GAME_PLAYFIELD_HEIGHT / 2.0 - ((app->credits.credit[app->credits.current_credit].names + 1) * t3f_get_font_line_height(app->font[DOT_FONT_32])) / 2.0;
            dot_shadow_text(app->font[DOT_FONT_32], al_map_rgba_f(1.0, 1.0, 0.0, 1.0), al_map_rgba_f(0.0, 0.0, 0.0, 0.5), t3f_virtual_display_width / 2 + app->credits.ox + ox, pos_y, DOT_SHADOW_OX * 2, DOT_SHADOW_OY * 2, ALLEGRO_ALIGN_CENTRE, app->credits.credit[app->credits.current_credit].header);
            for(i = 0; i < app->credits.credit[app->credits.current_credit].names; i++)
            {
                pos_y += t3f_get_font_line_height(app->font[DOT_FONT_32]);
                pos_ox = i % 2 ? app->credits.ox : -app->credits.ox;
                dot_shadow_text(app->font[DOT_FONT_32], t3f_color_white, al_map_rgba_f(0.0, 0.0, 0.0, 0.5), t3f_virtual_display_width / 2 + pos_ox + ox, pos_y, DOT_SHADOW_OX * 2, DOT_SHADOW_OY * 2, ALLEGRO_ALIGN_CENTRE, app->credits.credit[app->credits.current_credit].name[i]);
            }
        }
    }
}
