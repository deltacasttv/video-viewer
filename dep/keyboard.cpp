/*
 * SPDX-FileCopyrightText: Copyright (c) DELTACAST.TV. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at * * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "keyboard.h"
#include <termios.h>
#include <unistd.h>

static struct termios initial_settings, new_settings, current_settings;
static int peek_character = -1;

void init_keyboard()
{
   tcgetattr(0, &initial_settings);
   new_settings = initial_settings;
   new_settings.c_lflag &= ~ICANON;
   new_settings.c_lflag &= ~ECHO;
   new_settings.c_lflag &= ~ISIG;
   new_settings.c_cc[VMIN] = 1;
   new_settings.c_cc[VTIME] = 0;
   tcsetattr(0, TCSANOW, &new_settings);
}

void close_keyboard()
{
   tcsetattr(0, TCSANOW, &initial_settings);
}

int _kbhit()
{
   char ch;
   long int nread;

   if (peek_character != -1)
      return 1;
   new_settings.c_cc[VMIN] = 0;
   tcsetattr(0, TCSANOW, &new_settings);
   nread = read(0, &ch, 1);
   new_settings.c_cc[VMIN] = 1;
   tcsetattr(0, TCSANOW, &new_settings);

   if (nread == 1)
   {
      peek_character = ch;
      return 1;
   }
   return 0;
}

int _getch()
{
   char ch;

   if (peek_character != -1)
   {
      ch = (char)peek_character;
      peek_character = -1;
      return ch;
   }
   if (read(0, &ch, 1))
      return ch;
   else
      return 0;
}

void userQueryTxtDisplayON_keyboard()
{
   tcgetattr(0, &current_settings);
   new_settings = current_settings;
   new_settings.c_lflag |= ECHO;
   tcsetattr(0, TCSANOW, &new_settings);
}

void userQueryTxtDisplayOFF_keyboard()
{
   tcgetattr(0, &current_settings);
   new_settings = current_settings;
   new_settings.c_lflag &= ~ECHO;
   tcsetattr(0, TCSANOW, &new_settings);
} 