/*
 * MENU-SCREEN
 *
 * Copyright (c) 2009-2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef __MENU_SCREEN_INPUT_KEY_H__
#define __MENU_SCREEN_INPUT_KEY_H__

#include "util.h"

#define KEY_SELECT "XF86Home"
#define KEY_BACK "XF86Back"
#define KEY_ESC "Escape"

HAPI void key_register(void);
HAPI void key_unregister(void);
HAPI void key_grab_home(void);
HAPI void key_ungrab_home(void);

#endif //__MENU_SCREEN_INPUT_KEY_H__

// End of a file
