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


#ifndef __MENU_SCREEN_PKGMGR_H__
#define __MENU_SCREEN_PKGMGR_H__

#include <Evas.h>

#include "list.h"
#include "util.h"



enum package_install_status {
	UNKNOWN = 0x00,
	DOWNLOAD_BEGIN,
	DOWNLOADING,
	DOWNLOAD_END,
	INSTALL_BEGIN,
	INSTALLING,
	INSTALL_END,
	UNINSTALL_BEGIN,
	UNINSTALLING,
	UNINSTALL_END,
	UPDATE_BEGIN,
	UPDATING,
	UPDATE_END,
	MAX_STATUS,
};



struct package_info {
	enum package_install_status status;
	app_info_t ai;
	Evas_Object *item;
	Evas_Object *page;
	Eina_Bool desktop_file_found;
	int error_count;
};



extern menu_screen_error_e pkgmgr_init(Evas_Object *scroller);
extern void pkgmgr_fini(void);
extern Evas_Object *pkgmgr_find_pended_object(const char *package, int with_desktop_file, Evas_Object *scroller, Evas_Object **page);
extern menu_screen_error_e pkgmgr_uninstall(Evas_Object *obj);

#endif //__MENU_SCREEN_PKGMGR_H__

// End of a file
