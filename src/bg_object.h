#ifndef DOT_BG_OBJECT_H
#define DOT_BG_OBJECT_H

typedef struct
{

	float x, y, z;
	float vx, vy, vz;

} DOT_BG_OBJECT;

void dot_setup_bg_objects(void * data);
void dot_bg_objects_logic(void * data, float speed);
void dot_bg_objects_render(void * data);

#endif
