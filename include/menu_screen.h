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

#ifndef __MENU_SCREEN_H__
#define __MENU_SCREEN_H__

#include <Ecore_Evas.h>
#include <Evas.h>

extern int menu_screen_get_root_width(void);
extern int menu_screen_get_root_height(void);
extern double menu_screen_get_yscale(void);
extern Evas *menu_screen_get_evas(void);
extern Evas_Object *menu_screen_get_win(void);

#endif //__MENU_SCREEN_H__

// End of a file
