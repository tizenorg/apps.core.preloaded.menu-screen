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

#ifndef __MENU_SCREEN_MAPBUF_H__
#define __MENU_SCREEN_MAPBUF_H__

#include <Evas.h>

#include "util.h"

extern menu_screen_error_e mapbuf_enable(Evas_Object *obj, int force);
extern int mapbuf_disable(Evas_Object *obj, int force);

extern Evas_Object *mapbuf_bind(Evas_Object *box, Evas_Object *page);
extern Evas_Object *mapbuf_unbind(Evas_Object *obj);

extern Evas_Object *mapbuf_get_mapbuf(Evas_Object *obj);
extern Evas_Object *mapbuf_get_page(Evas_Object *obj);

extern int mapbuf_is_enabled(Evas_Object *obj);


#endif //__MENU_SCREEN_MAPBUF_H__

// End of a file
