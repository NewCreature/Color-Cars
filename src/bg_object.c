#include "t3f/t3f.h"
#include "t3f/draw.h"
#include "t3f/rng.h"
#include "instance.h"
#include "defines.h"

static T3F_RNG_STATE dot_bg_rng_state;

void dot_setup_bg_objects(void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	int i;
	float s, a;

	t3f_srand(&dot_bg_rng_state, time(0));

	/* initialize background objects */
	memset(app->bg_object, 0, sizeof(DOT_BG_OBJECT) * DOT_MAX_BG_OBJECTS);
	a = ALLEGRO_PI / 8.0; // angle slightly toward the player
	for(i = 0; i < DOT_MAX_BG_OBJECTS; i++)
	{
		s = t3f_drand(&dot_bg_rng_state) * 0.5 + 0.25;
		s = 0.25;
		app->bg_object[i].x = t3f_drandom(&dot_bg_rng_state, DOT_GAME_PLAYFIELD_WIDTH + DOT_GAME_BALL_SIZE) - DOT_GAME_BALL_SIZE;
		app->bg_object[i].y = t3f_drandom(&dot_bg_rng_state, DOT_GAME_PLAYFIELD_HEIGHT + DOT_GAME_BALL_SIZE) - DOT_GAME_BALL_SIZE;
		app->bg_object[i].z = -t3f_drandom(&dot_bg_rng_state, 320.0);
		app->bg_object[i].vx = -cos(a) * s;
		app->bg_object[i].vy = 0.0;
		app->bg_object[i].vz = -sin(a) * s;
	}
}

void dot_bg_objects_logic(void * data, float speed)
{
    APP_INSTANCE * app = (APP_INSTANCE *)data;
    int i;

    for(i = 0; i < DOT_MAX_BG_OBJECTS; i++)
	{
		app->bg_object[i].x += app->bg_object[i].vx * speed;
		if(app->bg_object[i].x < -DOT_GAME_BALL_SIZE)
		{
			app->bg_object[i].x = DOT_GAME_PLAYFIELD_WIDTH;
			app->bg_object[i].z = -t3f_drandom(&dot_bg_rng_state, 320.0);
		}
		app->bg_object[i].z += app->bg_object[i].vz * speed;
	}
}

static void dot_create_bg_object_effect(void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;
	float sx = 512.0 / (float)t3f_virtual_display_width;
	int i;
	bool held = al_is_bitmap_drawing_held();

	if(held)
	{
		al_hold_bitmap_drawing(false);
	}
	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_TRANSFORM);
	al_set_target_bitmap(app->bitmap[DOT_BITMAP_SCRATCH]);
	al_identity_transform(&identity);
	al_use_transform(&identity);
	al_set_clipping_rectangle(0, 0, 512, 512);
    al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
    al_set_clipping_rectangle(0, 0, 512, DOT_GAME_PLAYFIELD_HEIGHT);
    al_hold_bitmap_drawing(true);
	for(i = 0; i < DOT_MAX_BG_OBJECTS; i++)
	{
		t3f_draw_scaled_bitmap(app->bitmap[DOT_BITMAP_BALL_RED], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), app->bg_object[i].x * sx, app->bg_object[i].y, app->bg_object[i].z, DOT_GAME_BALL_SIZE * 2.0 * sx, DOT_GAME_BALL_SIZE * 2.0, 0);
	}
	al_hold_bitmap_drawing(false);
	t3f_set_clipping_rectangle(0, 0, 0, 0);
	al_restore_state(&old_state);
	al_hold_bitmap_drawing(held);
}

void dot_bg_objects_render(void * data)
{
    APP_INSTANCE * app = (APP_INSTANCE *)data;

    dot_create_bg_object_effect(data);
	t3f_draw_scaled_bitmap(app->bitmap[DOT_BITMAP_SCRATCH], al_map_rgba_f(0.0, 0.0, 0.0, 0.05), 0, 0, 0.0, DOT_GAME_PLAYFIELD_WIDTH, al_get_bitmap_height(app->bitmap[DOT_BITMAP_SCRATCH]), 0);
}
