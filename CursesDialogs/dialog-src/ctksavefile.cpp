/*
 *
 * ©K. D. Hedger. Sun 14 Apr 13:59:01 BST 2019 keithdhedger@gmail.com

 * This file (ctksavefile.cpp) is part of CursesDialogs.

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

#define APPNAME "ctksavefile"

struct option long_options[]=
	{
		{"start-folder",1,0,'s'},
		{"default-name",1,0,'n'},
		{"version",0,0,'v'},
		{"help",0,0,'?'},
		{0, 0, 0, 0}
	};

void printhelp(void)
{
	printf("Curses based file save dialog\n"
	"Usage: " APPNAME " [OPTION]\n"
	" -s, --start-folder	Start Folder\n"
	" -n, --default-name	Default file name\n"
	" -v, --version		Version\n"
	" -h, -?, --help		Help\n\n"
	"Report bugs to keithdhedger@gmail.com\n"
	"\nExample:\n"
	"To get the reults of the dialog into a bash varable Use:\n"
	"{ result=$(" APPNAME " -s /etc -n SaveFileName 2>&1 >&3 3>&-); } 3>&1\n"
	"echo $result\n"
	);
}

int main(int argc, char **argv)
{
	std::string				str;
	CTK_cursesUtilsClass	cu;
	char					*folder=NULL;
	int						c;
	int						option_index;
	const char				*fname="Untitled";

	while(true)
		{
			option_index=0;

			c=getopt_long(argc,argv,"v?h:w:s:n:",long_options,&option_index);
				if(c==-1)
					break;

			switch(c)
				{
					case 's':
						folder=strdup(optarg);
						break;

					case 'n':
						fname=optarg;
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


	if(folder==NULL)
		folder=get_current_dir_name();

	cu.CTK_fileChooserDialog(folder,CUSAVEFILE,NULL,fname);
	if(cu.dialogReturnData.isValidData==true)
		fprintf(stderr,"%s\n",cu.dialogReturnData.stringValue.c_str());

	SETSHOWCURS;

//	mainApp=new CTK_mainAppClass();
//	cu.CTK_openFile(mainApp,wname,folder,false,fname);
//	if(cu.isValidFile==true)
//		fprintf(stderr,"%s/%s",cu.inFolder.c_str(),cu.stringResult.c_str());
//	else
//		fprintf(stderr,"");

	free(folder);
	SETSHOWCURS;
	return(0);
}

