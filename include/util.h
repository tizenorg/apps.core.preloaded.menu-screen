/**
 * Copyright (c) 2000 - 2012 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * This file is part of Menu-screen
 * Written by Jin Yoon <jinny.yoon@samsung.com>, Youngjoo Park <yjoo93.park@samsung.com>
 *
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of SAMSUNG ELECTRONICS ("Confidential Information").
 * You shall not disclose such Confidential Information and shall use it only in accordance with the terms of the license agreement you entered into with SAMSUNG ELECTRONICS.
 * SAMSUNG make no representations or warranties about the suitability of the software, either express or implied, including but not limited to the implied warranties of merchantability, fitness for a particular purpose, or non-infringement.
 * SAMSUNG shall not be liable for any damages suffered by licensee as a result of using, modifying or distributing this software or its derivatives.
 */

#ifndef __MENU_SCREEN_UTIL_H__
#define __MENU_SCREEN_UTIL_H__
#include <dlog.h>
#include <Evas.h>

#define STR_MOVE_PREV "move,prev"
#define STR_MOVE_NEXT "move,next"
#define STR_ANI_RETURN "ani,return"
#define STR_ENV_MAPBUF "BEATUX_MAPBUF"

#define ALL_APPS_TABLE "all_apps"

/* Multi-language */
#define D_(str) dgettext("sys_string", str)

/* Log */
#if !defined(_W)
#define _W(fmt, arg...) LOGW("[%s:%d] "fmt"\n", __func__, __LINE__, ##arg)
#endif

#if !defined(_D)
#define _D(fmt, arg...) LOGD("[%s:%d] "fmt"\n", __func__, __LINE__, ##arg)
#endif

#if !defined(_E)
#define _E(fmt, arg...) LOGE("[%s:%d] "fmt"\n", __func__, __LINE__, ##arg)
#endif

#if !defined(_T)
#define _T(package) LOG(LOG_DEBUG, "LAUNCH", "[%s:Menuscreen:launch:done]", package);
#endif

#ifdef APPFWK_MEASUREMENT
#define PRINT_APPFWK() do {		\
    struct timeval tv;				\
    char str_time[64] = {0,};		\
    int re;							\
    gettimeofday(&tv, NULL);		\
    sprintf(str_time, "APP_START_TIME=%u %u", (int)tv.tv_sec, (int)tv.tv_usec); \
    re = putenv(str_time);			\
} while (0)
#else
#define PRINT_APPFWK()
#endif

#if defined(TIME_CHECK)
#define PRINT_TIME() do { \
    struct timeval tv; \
    gettimeofday(&tv, NULL); \
    _D("[%s:%d] TIME=%u %u", __func__, __LINE__, (int)tv.tv_sec, (int)tv.tv_usec); \
} while (0)
#else
#define PRINT_TIME()
#endif

#define retv_if(expr, val) do { \
	if(expr) { \
		_E("(%s) -> %s() return", #expr, __FUNCTION__); \
		return (val); \
	} \
} while (0)

#define ret_if(expr) do { \
	if(expr) { \
		_E("(%s) -> %s() return", #expr, __FUNCTION__); \
		return; \
	} \
} while (0)

#define goto_if(expr, val) do { \
	if(expr) { \
		_E("(%s) -> goto", #expr); \
		goto val; \
	} \
} while (0)

#define break_if(expr) { \
	if(expr) { \
		_E("(%s) -> break", #expr); \
		break; \
	} \
}

#define continue_if(expr) { \
	if(expr) { \
		_E("(%s) -> continue", #expr); \
		continue; \
	} \
}

#if !defined(_EDJ)
#define _EDJ(a) elm_layout_edje_get(a)
#endif

typedef enum {
	MENU_SCREEN_ERROR_OK = 0,
	MENU_SCREEN_ERROR_FAIL = -1,
	MENU_SCREEN_ERROR_DB_FAILED = -2,
	MENU_SCREEN_ERROR_OUT_OF_MEMORY = -3,
	MENU_SCREEN_ERROR_INVALID_PARAMETER = -4,
	MENU_SCREEN_ERROR_NO_DATA = -5,
} menu_screen_error_e;

typedef enum {
	TRAY_TYPE_ALL_APPS = 0,
	TRAY_TYPE_RUNNING_APPS,
	TRAY_TYPE_DOWNLOADED_APPS,
	TRAY_TYPE_FAVORITE_APPS,
	TRAY_TYPE_FREQUENTLY_USED_APPS,
	TRAY_TYPE_MAX,
} tray_type_e;

enum {
	APP_STATE_PAUSE = 1,
	APP_STATE_RESUME,
};

enum {
	MENU_SCREEN_ROTATE_PORTRAIT = 0,
	MENU_SCREEN_ROTATE_LANDSCAPE,
};



extern void _evas_object_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
extern void _evas_object_event_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
extern void _evas_object_event_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
extern void _evas_object_event_show_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);


#endif /* __MENU_SCREEN_UTIL_H__ */
