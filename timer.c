#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "timer.h"
#include "board.h"
#include "piece.h"
#include "input.h"
#include "graphics.h"
#include "errors.h"

#define DEBUG 0

#if DEBUG
#define TRACE(...)  do { printf(__VA_ARGS__); } while (0)
#else
#define TRACE(...)  do { } while (0)
#endif

static pthread_t        timer_thread;
static volatile bool    anchored;
static bool             suspended;
static pthread_mutex_t  suspend_mutex;
static pthread_cond_t   resume_cond;

bool timer_anchored(void)
{
	TRACE("%s()\n", __func__);
	if (anchored) {
		anchored = false;
		return true;
	}
	return false;
}

void timer_suspend(void)
{
	TRACE("%s()\n", __func__);
	pthread_mutex_lock(&suspend_mutex);
	suspended = true;
	while (suspended)
		pthread_cond_wait(&resume_cond, &suspend_mutex);
	pthread_mutex_unlock(&suspend_mutex);
}

void timer_resume(void)
{
	TRACE("%s()\n", __func__);
	pthread_mutex_lock(&suspend_mutex);
	suspended = false;
	pthread_cond_signal(&resume_cond);
	pthread_mutex_unlock(&suspend_mutex);
}

void* timer_handler(void* arg)
{
	TRACE("%s()\n", __func__);
	board_t* b = (board_t*)arg;
	
	while (1) {
		timer_suspend();
		while (!anchored) {
			anchored = piece_anchor_check(b);
			input_queue(KEY_DOWN);
			int droptime = board_getdroptime(b);
			usleep(droptime);
		}
	}
	pthread_exit(0);
}

int timer_init(board_t* b)
{
	TRACE("%s()\n", __func__);
	suspended = false;
	anchored  = false;
	
	if (pthread_mutex_init(&suspend_mutex, NULL) != 0)
		return ERR_GENERIC;
	
	if (pthread_cond_init(&resume_cond, NULL) != 0)
		return ERR_GENERIC;
	
	if (pthread_create(&timer_thread, NULL, &timer_handler, b) != 0)
		return ERR_GENERIC;
	
	return SUCCESS;
}
