/*
 *
 * ©K. D. Hedger. Sun 15 Sep 13:14:24 BST 2019 keithdhedger@gmail.com

 * This file (castreceiver.h) is part of CursesDialogs.

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

#ifndef _CASTRECEIVER_
#define _CASTRECEIVER_

#include <libgen.h>
#include <cursesGlobals.h>
#include <stdarg.h>
#include <getopt.h>
#include "config.h"

#define APPNAME "ctkcastreceiver"
#define CONTROLCNT 5

enum {NOSELECT=0,SELECTDLFOLDER,WAITFORCAST,PLAYURL,DLURL,QUIT};

struct option long_options[]=
	{
		{"version",0,0,'v'},
		{"help",0,0,'?'},
		{0, 0, 0, 0}
	};

#endif
