/*
 *
 * ©K. D. Hedger. Thu 18 Apr 15:58:36 BST 2019 keithdhedger@gmail.com

 * This file (ctkquery.cpp) is part of CursesDialogs.

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

#define APPNAME "ctkquery"

struct option long_options[]=
	{
		{"window-name",1,0,'w'},
		{"body",1,0,'b'},
		{"title",1,0,'t'},
		{"buttons",1,0,'b'},
		{"version",0,0,'v'},
		{"help",0,0,'?'},
		{0, 0, 0, 0}
	};

void printhelp(void)
{
printf("Curses based file save dialog\n"
	"Usage: " APPNAME " [OPTION]\n"
	" -w, --window-name	Window Name\n"
	" -q, --query		Query text\n"
	" -t, --title		Dialog title\n"
	" -b, --buttons		Buttons\n"
	" -v, --version		Version\n"
	" -h, -?, --help		Help\n\n"
	"Report bugs to keithdhedger@gmail.com\n"
	"\nExample:\n"
	"The return code from the app reflects the button selected like so:\n"
	APPNAME " -w MyWindow -q 'Do What?' -t 'Question?' -b 7 2>/dev/null;echo \"Button pressed $?\"\n" 
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
	int						buttons=1;

	while(true)
		{
			option_index=0;

			c=getopt_long(argc,argv,"v?h:w:t:q:b:",long_options,&option_index);
		if(c==-1)
			break;

		switch(c)
			{
				case 'w':
				wname=optarg;
				break;
		
				case 'q':
				bodytext=optarg;
				break;
		
				case 'b':
				buttons=atoi(optarg);
				break;
		
				case 't':
				title=optarg;
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

	cu.CTK_queryDialog(mainApp,bodytext,wname,title,buttons);
	buttons=cu.intResult;
	fprintf(stderr,"%i",cu.intResult);

	SETSHOWCURS;
	delete mainApp;
	return(buttons);
}
