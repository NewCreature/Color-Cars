#ifndef DOT_CREDITS_H
#define DOT_CREDITS_H

#define DOT_CREDIT_MAX_NAMES  8
#define DOT_CREDIT_MAX_SIZE  32
#define DOT_MAX_CREDITS      32

#define DOT_CREDITS_STATE_WAIT 0
#define DOT_CREDITS_STATE_IN   1
#define DOT_CREDITS_STATE_SHOW 2
#define DOT_CREDITS_STATE_OUT  3

typedef struct
{

    char header[DOT_CREDIT_MAX_SIZE];
    char name[DOT_CREDIT_MAX_NAMES][DOT_CREDIT_MAX_SIZE];
    int names;

} DOT_CREDIT;

typedef struct
{

    DOT_CREDIT credit[DOT_MAX_CREDITS];
    int credits;
    int current_credit;
    int state;
    int tick;
    float ox;
    bool done;

} DOT_CREDITS;

void dot_setup_credits(DOT_CREDITS * cp);
void dot_credits_logic(DOT_CREDITS * cp);
void dot_credits_render(void * data, float ox);

#endif
