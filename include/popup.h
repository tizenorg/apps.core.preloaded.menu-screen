 /*
  * Copyright 2012  Samsung Electronics Co., Ltd
  *
  * Licensed under the Flora License, Version 1.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *     http://www.tizenopensource.org/license
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  */



#ifndef __MENU_SCREEN_POPUP_H__
#define __MENU_SCREEN_POPUP_H__

extern void popup_append_list(Evas_Object *popup);
extern void popup_remove_list(Evas_Object *popup);
extern void popup_free_list();

extern Evas_Object *popup_create(Evas_Object *parent, const char *warning);
extern void popup_destroy(Evas_Object *popup);
extern Evas_Object *popup_create_uninstall(Evas_Object *parent, Evas_Object *item);

#endif //__MENU_SCREEN_POPUP_H__

// End of a file
