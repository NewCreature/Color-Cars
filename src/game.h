#ifndef DOT_GAME_H
#define DOT_GAME_H

typedef struct
{

	float x, y, z, a, s, vs, fs;
	float vx, vy;
	float r;
	int type;
	int timer;
	int target_tick;
	bool active;

} DOT_BALL;

typedef struct
{

	DOT_BALL ball;
	bool lost_touch;
	bool want_shield;
	float touch_offset_x, touch_offset_y;

} DOT_PLAYER;

typedef struct
{

	float x, y, r;
	bool active;

} DOT_SHIELD;

typedef struct
{

	int x, y;

} DOT_PARTICLE_LIST_ITEM;

#define DOT_MAX_PARTICLE_LIST_ITEMS 256

/* store a list of particle positions to generate particles from */
typedef struct
{

	DOT_PARTICLE_LIST_ITEM item[DOT_MAX_PARTICLE_LIST_ITEMS];
	int items;

} DOT_PARTICLE_LIST;

#define DOT_GAME_MAX_LEVELS    10
#define DOT_GAME_COMBO_TIME   120
#define DOT_GAME_BASE_POINTS 1000
#define DOT_GAME_COMBO_POINTS 250
#define DOT_GAME_TARGET_TICKS  30

#define DOT_GAME_MAX_BALLS  4096
#define DOT_GAME_STATE_START   0
#define DOT_GAME_STATE_PLAY    1
#define DOT_GAME_STATE_PAUSE   2
#define DOT_GAME_STATE_DONE    3

#define DOT_GAME_EMO_STATE_NORMAL 0
#define DOT_GAME_EMO_STATE_BLINK  1
#define DOT_GAME_EMO_STATE_WOAH   2
#define DOT_GAME_EMO_STATE_DEAD   3

#define DOT_GAME_PLAYFIELD_WIDTH  540
#define DOT_GAME_PLAYFIELD_HEIGHT 440

#define DOT_GAME_LEVEL_BASE_BALLS       16
#define DOT_GAME_LEVEL_BALLS_INC         4
#define DOT_GAME_LEVEL_BASE_BLACK_BALLS  2
#define DOT_GAME_LEVEL_BLACK_BALLS_INC   1
#define DOT_GAME_LEVEL_BASE_SPEED      1.0
#define DOT_GAME_LEVEL_TOP_SPEED       2.0

#define DOT_GAME_SHIELD_MAX_SIZE        96
#define DOT_GAME_BALL_SIZE            16.0

#define DOT_GAME_GRAB_SPOT_SIZE 48

#define DOT_GAME_TOUCH_START_X (DOT_GAME_SHIELD_MAX_SIZE + DOT_GAME_BALL_SIZE * 2)
#define DOT_GAME_TOUCH_START_Y (DOT_GAME_SHIELD_MAX_SIZE + DOT_GAME_BALL_SIZE * 2)
#define DOT_GAME_TOUCH_END_X (DOT_GAME_PLAYFIELD_WIDTH - DOT_GAME_SHIELD_MAX_SIZE - DOT_GAME_BALL_SIZE * 2)
#define DOT_GAME_TOUCH_END_Y (DOT_GAME_PLAYFIELD_HEIGHT - DOT_GAME_SHIELD_MAX_SIZE - DOT_GAME_BALL_SIZE * 2)

typedef struct
{

	int state;
	int state_tick;
	float speed, speed_inc;
	ALLEGRO_COLOR bg_color;
	ALLEGRO_COLOR old_bg_color;
	float bg_color_fade;
	int tick;

	/* player data */
	int lives;
	int level;
	int ascore, score, high_score;
	int combo;
	bool level_start;
	int emo_state;
	int emo_tick;

	DOT_BALL ball[DOT_GAME_MAX_BALLS];
	DOT_PLAYER player;
	DOT_SHIELD shield;

	/* cheats */
	bool cheats_enabled;
	int start_level;
	float speed_multiplier;
	int start_lives;

} DOT_GAME;

void dot_game_initialize(void * data, bool demo_seed);
void dot_game_emo_logic(void * data);
void dot_game_logic(void * data);
void dot_game_render_hud(void * data);
void dot_game_render(void * data);

#endif
