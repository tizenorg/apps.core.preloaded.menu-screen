/*
 * MENU-SCREEN
 *
 * Copyright (c) 2009-2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Contact: Jin Yoon <jinny.yoon@samsung.com>
 *          Junkyu Han <junkyu.han@samsung.com>

 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __MENU_SCREEN_PKGMGR_H__
#define __MENU_SCREEN_PKGMGR_H__


HAPI menu_screen_error_e pkgmgr_item_list_append_item(const char *pkg_id, const char *app_id, Evas_Object *item);
HAPI menu_screen_error_e pkgmgr_item_list_remove_item(const char *pkg_id, const char *app_id, Evas_Object *item);

HAPI void pkgmgr_item_list_affect_pkgid(const char *pkg_id, Eina_Bool (*_affected_cb)(const char *, Evas_Object *, void *), void *data);
HAPI void pkgmgr_item_list_affect_appid(const char *app_id, Eina_Bool (*_affected_cb)(const char *, Evas_Object *, void *), void *data);

HAPI inline menu_screen_error_e pkgmgr_uninstall(Evas_Object *item);

HAPI menu_screen_error_e pkgmgr_reserve_list_push_request(const char *package, const char *key, const char *val);
HAPI menu_screen_error_e pkgmgr_reserve_list_pop_request(void);

HAPI menu_screen_error_e pkgmgr_init(void);
HAPI void pkgmgr_fini(void);

#endif //__MENU_SCREEN_PKGMGR_H__

// End of a file
