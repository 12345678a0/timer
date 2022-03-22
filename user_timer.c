#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "user_timer.h"

#define TIMER_HANDLERS_MAX    128

typedef struct _tag_timer_handler_st 
{
	timer_handle handle;
	bool is_start;
	uint8_t times;						/**< 次数，对应TIMER_TIMES_XXX */
	uint32_t interval;					/**< 间隔，单位:ms */
	uint64_t tick;						/**< Tick缓存，用于记录当前调用时间 单位:ms*/
	void (*(timeout_cb))(void *);		/**< 超时回调 */
	void * arg;						    /**< 超时回调传入数据 */
} timer_handler_st;

typedef struct _tag_timer_obj_st {
	bool is_enable;					    /**< 是否启用定时器 */
	uint64_t tick;						/**< Tick 单位:ms*/
	timer_handler_st * handlers[TIMER_HANDLERS_MAX];
} timer_obj_st;

timer_obj_st g_timer_obj = {.is_enable = false};


void timer_tick_sync(uint32_t unit)
{
	g_timer_obj.tick += unit;
	return;
}

void timer_tick_set(uint64_t tick)
{
	g_timer_obj.tick = tick;
	return;
}

uint64_t timer_tick_get(void)
{
	return g_timer_obj.tick;
}

timer_handle * timer_create(uint32_t interval, uint8_t times, timer_timeout_cb timeout_cb, void * arg)
{
	int i, index = -1;
	timer_obj_st *obj = &g_timer_obj; 

	for (i = 0; (i < TIMER_HANDLERS_MAX) && (index == -1); i++)
	{
		if (!obj->handlers[i])
		{ 
			index = i; 
		}
	}

	if (index == -1)
	{ 
		return NULL;
	}

	timer_handler_st * handler = (timer_handler_st *)malloc(sizeof(timer_handler_st));
	
	if (!handler)
	{ 
		return NULL; 
	}
	handler->handle = index;
	handler->is_start = false;
	handler->times = times;
	handler->interval = interval;
	handler->tick = obj->tick;
	handler->timeout_cb = timeout_cb;
	handler->arg = arg;

	obj->handlers[index] = handler;

	return &handler->handle;
}

void timer_destroy(timer_handle *p_handle)
{
	if (!p_handle)
	{ 
		return; 
	}

	uint32_t index = *p_handle;
	timer_obj_st *obj = &g_timer_obj; 

	free(obj->handlers[index]);
	obj->handlers[index] = NULL;
}

void timer_start(timer_handle *p_handle)
{
	if (!p_handle)
	{ 
		return; 
	}

	timer_handler_st * handler = (timer_handler_st*)p_handle;  

	uint32_t index = *p_handle;
	timer_obj_st *obj = &g_timer_obj; 

	obj->handlers[index]->is_start = true;
}

void timer_stop(timer_handle *p_handle)
{
	if (!p_handle)
	{ 
		return; 
	}

	uint32_t index = *p_handle;
	timer_obj_st *obj = &g_timer_obj; 

	obj->handlers[index]->is_start = false;
}

void timer_loop(void)
{
	int i;
	uint32_t interval;
	timer_obj_st *obj = &g_timer_obj; 
	
	if (obj->is_enable != true)
	{ 
		return; 
	}

	for (i = 0; i < TIMER_HANDLERS_MAX; i++)
	{
		if (!obj->handlers[i])
		{ 
			continue; 
		}

		interval = obj->tick - obj->handlers[i]->tick;
		
		if (obj->handlers[i]->is_start && obj->handlers[i]->times && obj->handlers[i]->interval && interval >= obj->handlers[i]->interval)
		{
			if (obj->handlers[i]->times == TIMER_TIMES_ONCE)
			{
				obj->handlers[i]->timeout_cb(obj->handlers[i]->arg);
				free(obj->handlers[i]);
				obj->handlers[i] = NULL;
			}
			else
			{
				if (obj->handlers[i]->times != TIMER_TIMES_FOREVER)
				{
					obj->handlers[i]->times--;
				}

				obj->handlers[i]->tick += obj->handlers[i]->interval;
				obj->handlers[i]->timeout_cb(obj->handlers[i]->arg);
			}
		}
	}
}

void timer_interval_change(timer_handle *p_handle, uint32_t interval)
{
	if (!p_handle)
	{ 
		return; 
	}

	timer_handler_st *handler = (timer_handler_st *)p_handle; 

	handler->interval = interval;
	handler->tick = timer_tick_get();
}

void timer_delay_ms(uint32_t ms)
{
	timer_obj_st *obj = &g_timer_obj;
	uint32_t time = 0;
	uint64_t tick = obj->tick;

	while (time < ms)
	{
		time = obj->tick - tick;
	}
}

void timer_deinit(void)
{
	timer_obj_st *obj = &g_timer_obj; 

	if (!obj->is_enable)
	{
		return;
	}

	obj->is_enable = false;

	for (uint32_t i = 0; i < TIMER_HANDLERS_MAX; i ++)
	{
		if (obj->handlers[i])
		{
			free(obj->handlers[i]);
			obj->handlers[i] = NULL;
		}
	}
}

void timer_init(void)
{
	g_timer_obj.is_enable = true;
}

