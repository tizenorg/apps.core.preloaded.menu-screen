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

#ifndef __APP_TRAY_PAGE_CONTROL_H__
#define __APP_TRAY_PAGE_CONTROL_H__

#include <Evas.h>

extern void index_bring_in(Evas_Object *index, int idx);

extern Evas_Object *index_create(Evas_Object *layout, unsigned int count);
extern Evas_Object *index_update(Evas_Object *layout, Evas_Object *index, unsigned int count);
extern void index_destroy(Evas_Object *index);

extern void index_show(Evas_Object *index);
extern void index_hide(Evas_Object *index);

#endif //__APP_TRAY_PAGE_CONTROL_H__

// End of the file
