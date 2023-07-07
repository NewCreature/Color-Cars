#ifndef DOT_COLOR_H
#define DOT_COLOR_H

ALLEGRO_COLOR dot_darken_color(ALLEGRO_COLOR c1, float amount);
ALLEGRO_COLOR dot_transition_color(ALLEGRO_COLOR c1, ALLEGRO_COLOR c2, float amount);

#endif
