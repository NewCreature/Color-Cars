#include "t3f/t3f.h"
#include "t3f/draw.h"
#include "particle.h"

void dot_create_particle(DOT_PARTICLE * pp, float x, float y, float z, float vx, float vy, float vz, float gz, float size, int life, ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color)
{
	pp->x = x;
	pp->y = y;
	pp->z = z;
	pp->vx = vx;
	pp->vy = vy;
	pp->vz = vz;
	pp->gz = gz;
	pp->size = size;
	pp->bp = bp;
	pp->color = color;
	pp->life = life;
	pp->start_life = life;
	pp->active = true;
}

void dot_particle_logic(DOT_PARTICLE * pp)
{
	pp->x += pp->vx;
	pp->y += pp->vy;
	pp->z += pp->vz;
	if(pp->z >= 0.0)
	{
		pp->life = 1;
	}
	pp->vz += pp->gz;
	pp->life--;
	if(pp->life <= 0)
	{
		pp->active = false;
	}
}

void dot_particle_render(DOT_PARTICLE * pp, ALLEGRO_BITMAP * particle_bitmap)
{
	float r, g, b, a, ca;
	ALLEGRO_COLOR c;

	al_unmap_rgba_f(pp->color, &r, &g, &b, &a);
	ca = a * ((float)pp->life / (float)pp->start_life);
	c = al_map_rgba_f(r * ca, g * ca, b * ca, ca);
	t3f_draw_scaled_bitmap(particle_bitmap, c, pp->x - al_get_bitmap_width(particle_bitmap) / 2, pp->y - al_get_bitmap_height(particle_bitmap) / 2, pp->z, pp->size, pp->size, 0);
}
