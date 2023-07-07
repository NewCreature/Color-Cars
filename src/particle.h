#ifndef DOT_PARTICLE_H
#define DOT_PARTICLE_H

typedef struct
{

	float x, y, z;
	float vx, vy, vz;
	float gz;
	float size;
	ALLEGRO_BITMAP * bp;
	ALLEGRO_COLOR color;
	int life;
	int start_life;
	bool active;

} DOT_PARTICLE;

void dot_create_particle(DOT_PARTICLE * pp, float x, float y, float z, float vx, float vy, float vz, float gz, float size, int life, ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color);
void dot_particle_logic(DOT_PARTICLE * pp);
void dot_particle_render(DOT_PARTICLE * pp, ALLEGRO_BITMAP * particle_bitmap);

#endif
