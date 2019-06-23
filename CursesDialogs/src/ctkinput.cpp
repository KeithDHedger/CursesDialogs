/*
 *
 * ©K. D. Hedger. Sun 14 Apr 13:49:13 BST 2019 keithdhedger@gmail.com

 * This file (ctkinput.cpp) is part of CursesDialogs.

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

#define APPNAME "ctkinput"

struct option long_options[]=
	{
		{"window-name",1,0,'w'},
		{"body",1,0,'b'},
		{"title",1,0,'t'},
		{"input",1,0,'i'},
		{"cancel",0,0,'c'},
		{"version",0,0,'v'},
		{"help",0,0,'?'},
		{0, 0, 0, 0}
	};

void printhelp(void)
{
	printf("Curses based file save dialog\n"
	"Usage: " APPNAME " [OPTION]\n"
	" -w, --window-name	Window Name\n"
	" -b, --body		Body text\n"
	" -t, --title		Dialog title\n"
	" -i, --input		Default input\n"
	" -c, --cancel		Show cancel\n"
	" -v, --version		Version\n"
	" -h, -?, --help		Help\n\n"
	"Report bugs to keithdhedger@gmail.com\n"
	"\nExample:\n"
	"To get the reults of the dialog into a bash varable Use:\n"
	"{ result=$(" APPNAME " -w MyWindow -b 'Say somthing?' -t 'Input ...' -i 'Some input ...' -c  2>&1 >&3 3>&-); } 3>&1\n"
	"echo $result\n"
	"The return code from the app also reflects the button seelected like so:\n"
	APPNAME " -w MyWindow -b 'Say somthing?' -t 'Input ...' -i 'Some input ...' -c 2>/dev/null;echo \"Button pressed $?\"\n" 
	);
}

int main(int argc, char **argv)
{
	CTK_mainAppClass		*mainApp;
	std::string				str;
	CTK_cursesUtilsClass	cu;
	int						c;
	int						option_index;
	const char				*wname=NULL;
	const char				*title=NULL;
	const char				*bodytext="What? ...";
	const char				*inputtext="...";
	int						buttons=1;
	bool					cancel=false;

	while(true)
		{
			option_index=0;

			c=getopt_long(argc,argv,"cv?h:w:t:b:i:",long_options,&option_index);
			if(c==-1)
				break;

			switch(c)
				{
					case 'w':
						wname=optarg;
						break;

					case 'b':
						bodytext=optarg;
						break;

					case 'c':
						cancel=true;
						break;

					case 't':
						title=optarg;
						break;

					case 'i':
						inputtext=optarg;
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

	mainApp=new CTK_mainAppClass;

	if(cu.CTK_entryDialog(mainApp,bodytext,inputtext,wname,title,cancel))
		fprintf(stderr,"%s",cu.stringResult.c_str());

	buttons=cu.intResult;

	SETSHOWCURS;
	delete mainApp;
	return(buttons);
}
