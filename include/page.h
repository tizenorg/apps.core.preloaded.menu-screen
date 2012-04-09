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

#ifndef __MENU_SCREEN_PAGE_H__
#define __MENU_SCREEN_PAGE_H__

#include <Evas.h>
#include "util.h"
#include "list.h"

extern void page_mark_dirty(Evas_Object *page);
extern void page_clean_dirty(Evas_Object *page);
extern void page_unmark_dirty(Evas_Object *page);
extern int page_is_dirty(Evas_Object *page);

extern Evas_Object *page_get_item_at(Evas_Object *menu, unsigned int idx);

extern menu_screen_error_e page_unpack_item(Evas_Object *page, Evas_Object *item);
extern Evas_Object *page_unpack_item_at(Evas_Object *page, int idx);

extern void page_pack_item(Evas_Object *menu, int idx, Evas_Object *item);
extern void page_set_item(Evas_Object *page, int idx, Evas_Object *item);

extern unsigned int page_count_item(Evas_Object *page);

extern Evas_Object *page_create(Evas_Object *scroller, int idx, int rotate);
extern void page_destroy(Evas_Object *scroller, Evas_Object *page);

extern int page_find_empty_near(Evas_Object *menu_edje, int pivot);
extern int page_find_first_empty(Evas_Object *page, int pivot);
extern void page_trim_items(Evas_Object *page);

#endif //__MENU_SCREEN_PAGE_H__
// End of a file

