#include "t3f/t3f.h"
#include "t3f/resource.h"
#include "t3f/music.h"
#include "t3f/view.h"
#include "t3f/draw.h"
#include "avc/avc.h"
#include "instance.h"
#include "intro.h"
#include "game.h"
#include "leaderboard.h"
#include "privacy.h"

static bool dot_show_touch_hand = false;
static int dot_screenshot_count = 0;

static ALLEGRO_BITMAP * dot_create_scratch_bitmap(int w, int h)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_BITMAP * bp;

	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR | ALLEGRO_NO_PRESERVE_TEXTURE);
	bp = al_create_bitmap(w, h);
	al_restore_state(&old_state);

	return bp;
}

static void dot_event_handler(ALLEGRO_EVENT * event, void * data)
{
	ALLEGRO_STATE old_state;
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	switch(event->type)
	{
		case ALLEGRO_EVENT_DISPLAY_HALT_DRAWING:
		{
			al_destroy_bitmap(app->bitmap[DOT_BITMAP_SCRATCH]);
			t3f_event_handler(event);
			break;
		}
		case ALLEGRO_EVENT_DISPLAY_RESUME_DRAWING:
		{
			t3f_event_handler(event);
			al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
			al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR | ALLEGRO_NO_PRESERVE_TEXTURE);
			app->bitmap[DOT_BITMAP_SCRATCH] = dot_create_scratch_bitmap(DOT_BITMAP_SCRATCH_WIDTH, DOT_BITMAP_SCRATCH_HEIGHT);
			al_restore_state(&old_state);
			break;
		}

		/* pass the event through to T3F for handling by default */
		default:
		{
			t3f_event_handler(event);
			break;
		}
	}
}

void app_touch_logic(void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	bool need_click = false;
	int i;

	if(!app->desktop_mode)
	{
		app->touch_id = -1;
		if(app->touch_id < 0)
		{
			for(i = 0; i < T3F_MAX_TOUCHES; i++)
			{
				if(t3f_touch[i].active)
				{
					app->touch_id = i;
					break;
				}
			}
		}
		if(app->touch_id >= 0)
		{
			app->touch_x = t3f_touch[app->touch_id].x;
			app->touch_y = t3f_touch[app->touch_id].y - 520;
		}
	}
	else
	{
		app->touch_id = -1;
		if(t3f_mouse_button[1] && app->game.state == DOT_GAME_STATE_PLAY)
		{
			app->game.state = DOT_GAME_STATE_PAUSE;
			al_show_mouse_cursor(t3f_display);
		}
		if(app->state == DOT_STATE_GAME && (app->game.state == DOT_GAME_STATE_PAUSE || app->game.state == DOT_GAME_STATE_START))
		{
			need_click = true;
		}
		if(!t3f_mouse_hidden && (!need_click || t3f_mouse_button[0]))
		{
			app->touch_id = 0;
		}
		app->touch_x = t3f_mouse_x;
		app->touch_y = t3f_mouse_y;
	}
}

static int particle_qsort_helper(const void * p1, const void * p2)
{
	DOT_PARTICLE ** sp1;
	DOT_PARTICLE ** sp2;

	sp1 = (DOT_PARTICLE **)p1;
	sp2 = (DOT_PARTICLE **)p2;

	return (*sp1)->z < (*sp2)->z;
}

void app_logic(void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	int i;
	short touch_x, touch_y;
	bool frame;
	const char * cheat_code = "1234111";
	const char * cheat_code_2 = "196626";

	while(t3f_key_pressed())
	{
		app->cheat_buffer[app->cheat_pos] = t3f_read_key(0);
		app->cheat_pos++;
		app->cheat_buffer[app->cheat_pos] = 0;
		if(!memcmp(app->cheat_buffer, cheat_code, app->cheat_pos) && strlen(app->cheat_buffer) == strlen(cheat_code))
		{
			strcpy(app->cheat_buffer, "");
			app->cheat_pos = 0;
			app->game.start_lives = 0;
		}
		else if(!memcmp(app->cheat_buffer, cheat_code_2, app->cheat_pos) && strlen(app->cheat_buffer) == strlen(cheat_code_2))
		{
			strcpy(app->cheat_buffer, "");
			app->cheat_pos = 0;
			app->game.start_level = 2000000;
		}
		else if(memcmp(app->cheat_buffer, cheat_code, app->cheat_pos) && memcmp(app->cheat_buffer, cheat_code_2, app->cheat_pos))
		{
			strcpy(app->cheat_buffer, "");
			app->cheat_pos = 0;
		}
	}
	if(app->demo_file)
	{
		/* convert touch data to integer for demo operations */
		touch_x = t3f_mouse_x;
		touch_y = t3f_mouse_y;
		t3f_touch[0].x = touch_x;
		t3f_touch[0].y = touch_y;

		/* load touch data from file if we aren't recording */
		if(!app->demo_recording)
		{
			frame = al_fgetc(app->demo_file);
			if(frame)
			{
				t3f_touch[0].active = al_fgetc(app->demo_file);
				t3f_touch[0].x = al_fread16le(app->demo_file);
				t3f_touch[0].y = al_fread16le(app->demo_file);
				t3f_mouse_x = t3f_touch[0].x;
				t3f_mouse_y = t3f_touch[0].y;
				t3f_mouse_button[0] = t3f_touch[0].active;
			}
			else
			{
				app->demo_done = true;
				t3f_exit();
			}
		}

		/* write touch data to file if we are recording */
		else
		{
			al_fputc(app->demo_file, 1);
			al_fputc(app->demo_file, t3f_touch[0].active);
			al_fwrite16le(app->demo_file, touch_x);
			al_fwrite16le(app->demo_file, touch_y);
		}
	}

	/* capture screenshot */
	if(t3f_key[ALLEGRO_KEY_PRINTSCREEN])
	{
		char buf[32];

		sprintf(buf, "screen_%d.png", dot_screenshot_count);
		al_save_bitmap(buf, al_get_backbuffer(t3f_display));
		dot_screenshot_count++;
		t3f_key[ALLEGRO_KEY_PRINTSCREEN] = 0;
	}

	app_touch_logic(data);
	switch(app->state)
	{
		case DOT_STATE_INTRO:
		{
			dot_intro_logic(data);
			break;
		}
		case DOT_STATE_LEADERBOARD:
		{
			dot_leaderboard_logic(data);
			break;
		}
		case DOT_STATE_PRIVACY:
		{
			dot_privacy_logic(data);
			break;
		}
		case DOT_STATE_GAME:
		{
			dot_game_logic(data);
			break;
		}
	}
	app->active_particles = 0;
	for(i = 0; i < DOT_MAX_PARTICLES; i++)
	{
		if(app->particle[i].active)
		{
			app->active_particle[app->active_particles] = &app->particle[i];
			app->active_particles++;
		}
		dot_particle_logic(&app->particle[i]);
	}
}

void app_render(void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	int i;

	switch(app->state)
	{
		case DOT_STATE_INTRO:
		{
			dot_intro_render(data);
			break;
		}
		case DOT_STATE_LEADERBOARD:
		{
			dot_leaderboard_render(data);
			break;
		}
		case DOT_STATE_PRIVACY:
		{
			dot_privacy_render(data);
			break;
		}
		case DOT_STATE_GAME:
		{
			dot_game_render(data);
			break;
		}
	}
	al_hold_bitmap_drawing(true);
	qsort(app->active_particle, app->active_particles, sizeof(DOT_PARTICLE *), particle_qsort_helper);
	for(i = 0; i < app->active_particles; i++)
	{
		dot_particle_render(app->active_particle[i], app->bitmap[DOT_BITMAP_PARTICLE]);
	}
	if(app->desktop_mode)
	{
		float ox = 0, oy = 0;

		if(dot_show_touch_hand && t3f_mouse_y >= t3f_virtual_display_height - DOT_GAME_PLAYFIELD_HEIGHT)
		{
			ox = 12.0;
			oy = -12.0;
			t3f_draw_rotated_bitmap(app->bitmap[DOT_BITMAP_HAND], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), 92, 24, t3f_mouse_x, t3f_mouse_y, 0, 0, 0);
			al_draw_rotated_bitmap(t3f_mouse_button[0] ? app->bitmap[DOT_BITMAP_HAND_DOWN] : app->bitmap[DOT_BITMAP_HAND], 92, 24, t3f_mouse_x + ox, t3f_mouse_y + oy, 0, 0);
		}
	}
	al_hold_bitmap_drawing(false);
}

static bool dot_load_bitmap(APP_INSTANCE * app, int bitmap, const char * fn)
{
	t3f_load_resource((void **)&app->bitmap[bitmap], t3f_bitmap_resource_handler_proc, fn, 0, 0, 0);
	if(!app->bitmap[bitmap])
	{
		printf("Failed to load image %d!\n", bitmap);
		return false;
	}
	return true;
}

static bool dot_load_font(APP_INSTANCE * app, int font, const char * fn, int size)
{
	t3f_load_resource((void **)&app->font[font], t3f_font_resource_handler_proc, fn, size, 0, 0);
	if(!app->font[font])
	{
		printf("Failed to load font %d!\n", font);
		return false;
	}
	return true;
}

static ALLEGRO_COLOR dot_get_ball_color(ALLEGRO_BITMAP * bp)
{
	ALLEGRO_COLOR c;

	al_lock_bitmap(bp, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);
	c = al_get_pixel(bp, al_get_bitmap_width(bp) / 2, al_get_bitmap_height(bp) / 2);
	al_unlock_bitmap(bp);
	return c;
}

bool app_avc_init_proc(void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	app->demo_file = al_fopen(app->demo_filename, "rb");
	if(!app->demo_file)
	{
		printf("Failed to open demo file.\n");
		return false;
	}
	app->demo_seed = al_fread32le(app->demo_file);
	t3f_srand(&app->rng_state, app->demo_seed);
	app->demo_done = false;
	app->state = 0;
	return true;
}

bool app_avc_logic_proc(void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	app_logic(data);
	if(app->demo_done)
	{
		al_fclose(app->demo_file);
		app->demo_file = NULL;
		return false;
	}
	return true;
}

bool app_load_data(APP_INSTANCE * app)
{
	int i;

	/* load images */
	if(!dot_load_bitmap(app, DOT_BITMAP_BALL_RED, "data/graphics/ball_red.png"))
	{
		return false;
	}
	if(!dot_load_bitmap(app, DOT_BITMAP_BALL_GREEN, "data/graphics/ball_green.png"))
	{
		return false;
	}
	if(!dot_load_bitmap(app, DOT_BITMAP_BALL_BLUE, "data/graphics/ball_blue.png"))
	{
		return false;
	}
	if(!dot_load_bitmap(app, DOT_BITMAP_BALL_PURPLE, "data/graphics/ball_purple.png"))
	{
		return false;
	}
	if(!dot_load_bitmap(app, DOT_BITMAP_BALL_YELLOW, "data/graphics/ball_yellow.png"))
	{
		return false;
	}
	if(!dot_load_bitmap(app, DOT_BITMAP_BALL_ORANGE, "data/graphics/ball_orange.png"))
	{
		return false;
	}
	if(!dot_load_bitmap(app, DOT_BITMAP_BALL_BLACK, "data/graphics/ball_black.png"))
	{
		return false;
	}
	if(!dot_load_bitmap(app, DOT_BITMAP_BALL_EYES, "data/graphics/ball_eyes.png"))
	{
		return false;
	}
	if(!dot_load_bitmap(app, DOT_BITMAP_COMBO, "data/graphics/combo.png"))
	{
		return false;
	}
	if(!dot_load_bitmap(app, DOT_BITMAP_PARTICLE, "data/graphics/particle.png"))
	{
		return false;
	}
	if(!dot_load_bitmap(app, DOT_BITMAP_EMO_NORMAL, "data/graphics/emo_normal.png"))
	{
		return false;
	}
	if(!dot_load_bitmap(app, DOT_BITMAP_EMO_BLINK, "data/graphics/emo_blink.png"))
	{
		return false;
	}
	if(!dot_load_bitmap(app, DOT_BITMAP_EMO_WOAH, "data/graphics/emo_woah.png"))
	{
		return false;
	}
	if(!dot_load_bitmap(app, DOT_BITMAP_EMO_DEAD, "data/graphics/emo_dead.png"))
	{
		return false;
	}
	if(!dot_load_bitmap(app, DOT_BITMAP_TARGET, "data/graphics/target.png"))
	{
		return false;
	}
	if(!dot_load_bitmap(app, DOT_BITMAP_BG, "data/graphics/bg.png"))
	{
		return false;
	}
	if(!dot_load_bitmap(app, DOT_BITMAP_LOGO, "data/graphics/logo.png"))
	{
		return false;
	}
	if(!dot_load_bitmap(app, DOT_BITMAP_HUD_CHARACTER, "data/graphics/hud_character.png"))
	{
		return false;
	}
	app->bitmap[DOT_BITMAP_SCRATCH] = dot_create_scratch_bitmap(DOT_BITMAP_SCRATCH_WIDTH, DOT_BITMAP_SCRATCH_HEIGHT);
	if(!app->bitmap[DOT_BITMAP_SCRATCH])
	{
		printf("Failed to create effects scratch bitmap!\n");
		return false;
	}

	/* load sounds */
	app->sample[DOT_SAMPLE_START] = al_load_sample("data/sounds/sfx0005.wav");
	if(!app->sample[DOT_SAMPLE_START])
	{
		printf("Failed to load sound %d!\n", DOT_SAMPLE_START);
		return false;
	}
	app->sample[DOT_SAMPLE_GRAB] = al_load_sample("data/sounds/sfx0000.wav");
	if(!app->sample[DOT_SAMPLE_GRAB])
	{
		printf("Failed to load sound %d!\n", DOT_SAMPLE_GRAB);
		return false;
	}
	app->sample[DOT_SAMPLE_LOSE] = al_load_sample("data/sounds/sfx0001.wav");
	if(!app->sample[DOT_SAMPLE_LOSE])
	{
		printf("Failed to load sound %d!\n", DOT_SAMPLE_LOSE);
		return false;
	}
	app->sample[DOT_SAMPLE_GO] = al_load_sample("data/sounds/sfx0004.wav");
	if(!app->sample[DOT_SAMPLE_GO])
	{
		printf("Failed to load sound %d!\n", DOT_SAMPLE_GO);
		return false;
	}
	app->sample[DOT_SAMPLE_SCORE] = al_load_sample("data/sounds/score_up.wav");
	if(!app->sample[DOT_SAMPLE_SCORE])
	{
		printf("Failed to load sound %d!\n", DOT_SAMPLE_SCORE);
		return false;
	}

	/* load fonts */
	if(!dot_load_font(app, DOT_FONT_14, "data/fonts/kongtext.ttf", 14))
	{
		printf("Failed to load font %d!\n", DOT_FONT_8);
		return false;
	}
	if(!dot_load_font(app, DOT_FONT_8, "data/fonts/kongtext.ttf", 8))
	{
		printf("Failed to load font %d!\n", DOT_FONT_8);
		return false;
	}
	if(!dot_load_font(app, DOT_FONT_16, "data/fonts/kongtext.ttf", 16))
	{
		printf("Failed to load font %d!\n", DOT_FONT_16);
		return false;
	}
	if(!dot_load_font(app, DOT_FONT_32, "data/fonts/kongtext.ttf", 32))
	{
		printf("Failed to load font %d!\n", DOT_FONT_32);
		return false;
	}

	/* build atlas */
	app->atlas = t3f_create_atlas(1024, 1024);
	if(app->atlas)
	{
		for(i = 0; i < DOT_BITMAP_ATLAS_BOUNDARY; i++)
		{
			if(app->bitmap[i])
			{
				t3f_add_bitmap_to_atlas(app->atlas, &app->bitmap[i], T3F_ATLAS_SPRITE);
			}
		}
	}

	return true;
}

void app_read_config(APP_INSTANCE * app)
{
	const char * val;

	/* see if we need to run setup */
	app->first_run = true;
	val = al_get_config_value(t3f_config, "Game Data", "Setup Done");
	if(val)
	{
		if(!strcasecmp(val, "true"))
		{
			app->first_run = false;
		}
	}

	/* load high score */
	val = al_get_config_value(t3f_config, "Game Data", "High Score");
	if(val)
	{
		app->game.high_score = atoi(val);
	}
	else
	{
		app->game.high_score = 0;
	}

	/* load user name */
	val = al_get_config_value(t3f_config, "Game Data", "User Name");
	if(val)
	{
		strcpy(app->user_name, val);
	}
	else
	{
		strcpy(app->user_name, "Anonymous");
	}

	app->upload_scores = false;
	val = al_get_config_value(t3f_config, "Game Data", "Upload Scores");
	if(val)
	{
		if(!strcasecmp(val, "true"))
		{
			app->upload_scores = true;
		}
	}

	app->music_enabled = false;
	val = al_get_config_value(t3f_config, "Game Data", "Music Enabled");
	if(val)
	{
		if(!strcasecmp(val, "true"))
		{
			app->music_enabled = true;
		}
	}

	app->game.cheats_enabled = false;
	app->game.speed_multiplier = 1.0;
	val = al_get_config_value(t3f_config, "Game Data", "speed_multiplier");
	if(val)
	{
		app->game.speed_multiplier = atof(val);
		app->game.cheats_enabled = true;
	}

	app->game.start_level = 0;
	val = al_get_config_value(t3f_config, "Game Data", "start_level");
	if(val)
	{
		app->game.start_level = atoi(val);
		if(app->game.start_level != 0)
		{
			app->game.cheats_enabled = true;
		}
	}

	app->game.start_lives = 3;
	val = al_get_config_value(t3f_config, "Game Data", "start_lives");
	if(val)
	{
		app->game.start_lives = atoi(val);
		if(app->game.start_lives != 3)
		{
			app->game.cheats_enabled = true;
		}
	}
}

bool app_process_arguments(APP_INSTANCE * app, int argc, char * argv[])
{
	int i;

	/* see if we want to record a demo */
	for(i = 1; i < argc; i++)
	{
		if(!strcmp(argv[i], "--record_demo"))
		{
			if(argc <= i + 1)
			{
				printf("Missing argument.\n");
				return false;
			}
			else
			{
				if(!dot_load_bitmap(app, DOT_BITMAP_HAND, "data/graphics/hand.png"))
				{
					return false;
				}
				if(!dot_load_bitmap(app, DOT_BITMAP_HAND_DOWN, "data/graphics/hand_down.png"))
				{
					return false;
				}
				app->demo_file = al_fopen(argv[i + 1], "wb");
				if(!app->demo_file)
				{
					printf("Failed to open demo file.\n");
					return false;
				}
				dot_show_touch_hand = true;
				app->demo_recording = true;
				app->demo_seed = time(0);
				al_fwrite32le(app->demo_file, app->demo_seed);
			}
		}
		if(!strcmp(argv[i], "--play_demo"))
		{
			if(argc <= i + 1)
			{
				printf("Missing argument.\n");
				return false;
			}
			else
			{
				app->demo_filename = argv[i + 1];
				dot_show_touch_hand = true;
			}
			app_avc_init_proc(app);
		}
		if(!strcmp(argv[i], "--capture_demo"))
		{
			if(argc <= i + 1)
			{
				printf("Missing argument.\n");
				return false;
			}
			else
			{
				if(!dot_load_bitmap(app, DOT_BITMAP_HAND, "data/graphics/hand.png"))
				{
					return false;
				}
				if(!dot_load_bitmap(app, DOT_BITMAP_HAND_DOWN, "data/graphics/hand_down.png"))
				{
					return false;
				}
				app->demo_filename = argv[i + 1];
				dot_show_touch_hand = true;
			}
			if(!avc_start_capture(t3f_display, "myvideo.mp4", app_avc_init_proc, app_avc_logic_proc, app_render, 60, 0, app))
			{
				printf("Capture failed!\n");
				return false;
			}
		}
	}
	return true;
}

void app_check_mobile_argument(APP_INSTANCE * app, int argc, char * argv[])
{
	int i;

	for(i = 1; i < argc; i++)
	{
		if(!strcmp(argv[i], "--mobile"))
		{
			app->desktop_mode = false;
			break;
		}
	}
}

static void set_optimal_display_size(APP_INSTANCE * app)
{
	ALLEGRO_MONITOR_INFO info;
	int width, height;
	int current_width = DOT_DISPLAY_WIDTH;
	int current_height = DOT_DISPLAY_HEIGHT;

	al_get_monitor_info(0, &info);
	width = info.x2 - info.x1 - 64;
	height = info.y2 - info.y1 - 64;
	while(current_width <= width && current_height <= height)
	{
		current_width += DOT_DISPLAY_WIDTH / 4;
		current_height += DOT_DISPLAY_HEIGHT / 4;
	}
	current_width -= DOT_DISPLAY_WIDTH / 4;
	current_height -= DOT_DISPLAY_HEIGHT / 4;
	if(current_width != al_get_display_width(t3f_display) || current_height != al_get_display_height(t3f_display))
	{
		t3f_set_gfx_mode(current_width, current_height, t3f_flags);
		al_set_window_position(t3f_display, (info.x2 - info.x1) / 2 - current_width / 2, (info.y2 - info.y1) / 2 - current_height / 2);
	}
}

/* initialize our app, load graphics, etc. */
bool app_initialize(APP_INSTANCE * app, int argc, char * argv[])
{
	int i;

	/* detect game type */
	app->desktop_mode = false;
	#ifndef ALLEGRO_ANDROID
		app->desktop_mode = true;
	#endif
	app_check_mobile_argument(app, argc, argv);

	/* initialize T3F */
	if(!t3f_initialize(T3F_APP_TITLE, DOT_DISPLAY_WIDTH, DOT_DISPLAY_HEIGHT, 60.0, app_logic, app_render, T3F_DEFAULT, app))
	{
		printf("Error initializing T3F\n");
		return false;
	}
	t3f_set_event_handler(dot_event_handler);
	set_optimal_display_size(app);
	t3net_setup(NULL, al_path_cstr(t3f_temp_path, '/'));
	if(!app_load_data(app))
	{
		printf("Failed to load data!\n");
		return false;
	}

	app_read_config(app);

	if(!dot_intro_initialize(app))
	{
		printf("Failed to create menu!\n");
		return false;
	}

	/* change view focus so 3D effects look right */
	t3f_adjust_view(t3f_current_view, t3f_current_view->offset_x, t3f_current_view->offset_y, t3f_current_view->width, t3f_current_view->height, DOT_GAME_PLAYFIELD_WIDTH / 2, DOT_GAME_PLAYFIELD_HEIGHT / 2, t3f_current_view->flags);

	/* create color tables */
	for(i = 0; i <= DOT_BITMAP_BALL_BLACK; i++)
	{
		app->dot_color[i] = dot_get_ball_color(app->bitmap[i]);
	}
	app->level_color[0] = al_map_rgb(47, 151, 218);
	app->level_color[1] = al_map_rgb(48, 196, 218);
	app->level_color[2] = al_map_rgb(48, 218, 192);
	app->level_color[3] = al_map_rgb(48, 218, 134);
	app->level_color[4] = al_map_rgb(147, 186, 33);
	app->level_color[5] = al_map_rgb(186, 178, 33);
	app->level_color[6] = al_map_rgb(218, 175, 48);
	app->level_color[7] = al_map_rgb(218, 123, 48);
	app->level_color[8] = al_map_rgb(218, 48, 48);
	app->level_color[9] = al_map_rgb(204, 48, 218);

	dot_setup_bg_objects(app);
	dot_setup_credits(&app->credits);

	dot_intro_setup(app);
	if(app->music_enabled)
	{
		t3f_play_music(DOT_MUSIC_TITLE);
	}
	if(!app->desktop_mode)
	{
		app->menu_showing = true;
	}
	if(app->first_run)
	{
		app->current_menu = DOT_MENU_UPLOAD_SCORES;
	}
	app->state = DOT_STATE_INTRO;

	if(!app_process_arguments(app, argc, argv))
	{
		printf("Failed to process command line arguments!\n");
		return false;
	}
	return true;
}

void app_exit(APP_INSTANCE * app)
{
	int i;

	if(app->demo_file)
	{
		al_fputc(app->demo_file, 0);
		al_fclose(app->demo_file);
	}
	for(i = 0; i < DOT_MAX_BITMAPS; i++)
	{
		if(app->bitmap[i])
		{
			if(!t3f_destroy_resource(app->bitmap[i]))
			{
				al_destroy_bitmap(app->bitmap[i]);
			}
		}
	}
	for(i = 0; i < DOT_MAX_SAMPLES; i++)
	{
		if(app->sample[i])
		{
			al_destroy_sample(app->sample[i]);
		}
	}
	for(i = 0; i < DOT_MAX_FONTS; i++)
	{
		if(app->font[i])
		{
			if(!t3f_destroy_resource(app->font[i]))
			{
				t3f_destroy_font(app->font[i]);
			}
		}
	}
	if(app->atlas)
	{
		t3f_destroy_atlas(app->atlas);
	}
	for(i = 0; i < DOT_MAX_MENUS; i++)
	{
		if(app->menu[i])
		{
			t3f_destroy_gui(app->menu[i]);
		}
	}
	if(app->leaderboard)
	{
		t3net_destroy_leaderboard(app->leaderboard);
	}
}

int main(int argc, char * argv[])
{
	APP_INSTANCE app;

	memset(&app, 0, sizeof(APP_INSTANCE));
	if(app_initialize(&app, argc, argv))
	{
		t3f_run();
	}
	else
	{
		printf("Error: could not initialize game!\n");
	}
	app_exit(&app);
	t3f_finish();
	return 0;
}
