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


#ifndef __MENU_SCREEN_PAGE_SCROLLER_H__
#define __MENU_SCREEN_PAGE_SCROLLER_H__

#include <stdbool.h>
#include <Eina.h>
#include <Evas.h>
#include "list.h"

typedef enum {
	PAGE_SCROLLER_SORT_BY_DEFAULT = 0,
	PAGE_SCROLLER_SORT_BY_PACKAGE,
	PAGE_SCROLLER_SORT_BY_NAME,
	PAGE_SCROLLER_SORT_MAX,
} page_scroller_sort_type_e;

extern Evas_Object *page_scroller_create(Evas_Object *tab, Evas_Object *index, page_scroller_sort_type_e sort_type, int rotate);
extern void page_scroller_destroy(Evas_Object *scroller);
extern void page_scroller_clean(Evas_Object *scroller);

extern Evas_Object *page_scroller_get_page_at(Evas_Object *scroller, unsigned int idx);
extern unsigned int page_scroller_count_page(Evas_Object *scroller);
extern int page_scroller_get_page_no(Evas_Object* scroller, Evas_Object *page);

extern menu_screen_error_e page_scroller_push_item(Evas_Object *scroller, app_info_t *ai);
extern Evas_Object *page_scroller_find_item_by_package(Evas_Object *scroller, const char *package, int *page_no);
extern void page_scroller_trim_items(Evas_Object *scroller);
extern void page_scroller_bring_in(Evas_Object *scroller, int idx);
extern void page_scroller_show_region(Evas_Object *scroller, int idx);

#endif //__MENU_SCREEN_PAGE_SCROLLER_H__

// End of a file
