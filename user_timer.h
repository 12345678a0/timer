#ifndef _USER_TIMER_H_
#define _USER_TIMER_H_ 

#define TIMER_TIMES_ONCE					0x01
#define TIMER_TIMES_FOREVER					0xFF

typedef uint32_t timer_handle;
typedef void (*timer_timeout_cb)(void *arg);


/**
 * @brief timer初始化
 */
void timer_init(void);

/**
 * @brief timer反初始化(退出)
 */
void timer_deinit(void);

/**
 * @brief 同步timer tick
 * @brief unit同步的单位
 */
void timer_tick_sync(uint32_t unit);

/**
 * @brief 设置timer tick
 *
 * @param tick
 */
void timer_tick_set(uint64_t tick);

/**
 * @brief 获取timer tick
 *
 * @return tick
 */
uint64_t timer_tick_get(void);

/**
 * @brief timer循环处理
 *
 * @note 所有timer都在这个函数中处理，需要在主循环中调用
 * @note 调用间隔不能小于设置的timer单位，即'timer_tick_sync函数'入参设置的值
 */
void timer_loop(void);

/**
 * @brief 创建定时器
 *
 * @param interval 单位:ms
 * @param times 次数，对应TIMER_TIMES_XXX，如果是多次，则直接用数字表示，如times为2，则表示调用两次
 * @param timeoutcb[in] 超时回调，表示超过间隔则进行回调
 * @param arg[in] 回调传入参数
 *
 * @return 'timer_handle'指针
 * @return NULL表示失败
 */
timer_handle* timer_create(uint32_t interval, uint8_t times, timer_timeout_cb timeout_cb, void * arg);


/**
 * @brief 开始定时器
 *
 * @param timer [in] 'timer_handle'指针
 * @param interval 间隔
 */

void timer_start(timer_handle *p_handle);

/**
 * @brief 停止定时器
 *
 * @param timer [in] 'timer_handle'指针
 * @param interval 间隔
 */
void timer_stop(timer_handle *p_handle);

/**
 * @brief 销毁定时器
 *
 * @param timer [in] 'timer_handle'指针
 */
void timer_destroy(timer_handle *p_handle);

/**
 * @brief 设置timer间隔
 *
 * @param timer [in] 'timer_handle'指针
 * @param interval 间隔
 */
void timer_interval_change(timer_handle *p_handle, uint32_t interval);

/**
 * @brief 毫秒延时
 *
 * @param ms 毫秒
 */
void timer_delay_ms(uint32_t ms);

#endif /* _USER_TIMER_H_ */
