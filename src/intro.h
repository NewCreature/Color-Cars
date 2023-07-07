#ifndef DOT_INTRO_H
#define DOT_INTRO_H

#define DOT_INTRO_STATE_LOGO        0
#define DOT_INTRO_STATE_LOGO_OUT    1
#define DOT_INTRO_STATE_CREDITS_IN  2
#define DOT_INTRO_STATE_CREDITS     3
#define DOT_INTRO_STATE_CREDITS_OUT 4
#define DOT_INTRO_STATE_LOGO_IN     5

int dot_menu_proc_play(void * data, int i, void * pp);
int dot_menu_proc_leaderboard_main_menu(void * data, int i, void * pp);
int dot_menu_proc_leaderboard_back(void * data, int i, void * pp);
int dot_menu_proc_privacy_back(void * data, int i, void * pp);

bool dot_intro_initialize(void * data);
void dot_intro_setup(void * data);
void dot_intro_logic(void * data);
void dot_intro_render_split(void * data);
void dot_intro_render(void * data);

#endif
