/******************************************************
/*
 *
 * ©K. D. Hedger. Fri  6 Sep 20:14:16 BST 2019 keithdhedger@gmail.com

 * This file (musicplayer.h) is part of CursesDialogs.

 * CursesDialogs is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * CursesDialogs is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with CursesDialogs.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _MUSICPLAYER_
#define _MUSICPLAYER_

#include <cursesGlobals.h>
#include <getopt.h>
#include "config.h"

#define APPNAME "ctkmusicplayer"

struct option long_options[]=
	{
		{"version",0,0,'v'},
		{"help",0,0,'?'},
		{0, 0, 0, 0}
	};

enum {FRONT=1,BACKWARD,START,STOP,PAUSE,FOWARD,BACK};

#endif
