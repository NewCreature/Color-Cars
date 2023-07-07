#ifndef DOT_LEADERBOARD_H
#define DOT_LEADERBOARD_H

unsigned long dot_leaderboard_obfuscate_score(unsigned long score);
unsigned long dot_leaderboard_unobfuscate_score(unsigned long score);
void dot_leaderboard_logic(void * data);
void dot_leaderboard_render(void * data);

#endif
