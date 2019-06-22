/*
 *
 * ©K. D. Hedger. Sun 14 Apr 13:49:13 BST 2019 keithdhedger@gmail.com

 * This file (ctkopenfile.cpp) is part of CursesDialogs.

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
 
#include <getopt.h>
#include <cursesGlobals.h>

#include "config.h"

#define APPNAME "ctkopenfile"

struct option long_options[]=
	{
		{"window-name",1,0,'w'},
		{"dialog-title",1,0,'t'},
		{"start-folder",1,0,'s'},
		{"version",0,0,'v'},
		{"help",0,0,'?'},
		{0, 0, 0, 0}
	};

void printhelp(void)
{
printf("Curses based file chooser\n"
	"Usage: " APPNAME " [OPTION]\n"
	" -w, --window-name	Window Name\n"
	" -s, --start-folder	Start Folder\n"
	" -v, --version		Version\n"
	" -h, -?, --help		Help\n\n"
	"Report bugs to keithdhedger@gmail.com\n"
	"\nExample:\n"
	"To get the reults of the dialog into a bash varable Use:\n"
	"{ result=$(" APPNAME " -w MyWindow -s /etc 2>&1 >&3 3>&-); } 3>&1\n"
	"echo $result\n"
	);
}

int main(int argc, char **argv)
{
	CTK_mainAppClass		*mainApp;
	std::string				str;
	CTK_cursesUtilsClass	cu;
	char					*folder=NULL;
	int						c;
	int						option_index;
	const char				*wname=NULL;
	const char				*dname=NULL;

	while(true)
		{
			option_index=0;

			c=getopt_long(argc,argv,"v?h:w:s:",long_options,&option_index);
			if(c==-1)
				break;

			switch(c)
				{
					case 'w':
						wname=optarg;
						break;

					case 's':
						folder=optarg;
						break;

					case 'v':
						printf(APPNAME " %s\n",VERSION);
						return 0;
						break;

					case '?':
					case 'h':
						printhelp();
						return 0;
						break;

					default:
						fprintf(stderr,"?? Unknown argument ??\n");
						return(1);
					break;
				}
		}

	mainApp=new CTK_mainAppClass();
	cu.CTK_openFile(mainApp,wname,folder);
	if(cu.isValidFile==true)
		fprintf(stderr,"%s",cu.stringResult.c_str());
	else
		fprintf(stderr,"");

	SETSHOWCURS;
	delete mainApp;
	return(0);
}

