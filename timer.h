#ifndef __TIMER_H__
#define __TIMER_H__

#include "board.h"

bool timer_anchored(void);
int timer_init(board_t*);
void timer_resume(void);
void timer_suspend(void);

#endif /* __TIMER_H__ */
