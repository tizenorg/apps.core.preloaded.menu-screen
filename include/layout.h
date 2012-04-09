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


#ifndef __MENU_SCREEN_LAYOUT_H__
#define __MENU_SCREEN_LAYOUT_H__

#include <Evas.h>
#include "util.h"

extern Evas_Object *layout_create(Evas_Object *win, const char *file, const char *group, int rotate);
extern void layout_destroy(Evas_Object *layout);

extern void layout_enable_block(Evas_Object *layout);
extern void layout_disable_block(Evas_Object *layout);

extern Evas_Object* layout_load_edj(Evas_Object *parent, const char *edjname, const char *grpname);
extern void layout_unload_edj(Evas_Object *layout);

#endif //__MENU_SCREEN_LAYOUT_H__

// End of a file
