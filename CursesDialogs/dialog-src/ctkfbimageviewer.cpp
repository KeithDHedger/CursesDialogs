/*
 *
 * ©K. D. Hedger. Fri 23 Aug 14:44:42 BST 2019 keithdhedger@gmail.com

 * This file (ctkfbimageviewer.cpp) is part of CursesDialogs.

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

//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>
//#define _USEFRAMBUFFER_
#include <getopt.h>
#include <cursesGlobals.h>

#include "config.h"

#define APPNAME "ctkfbimageviewer"

struct option long_options[]=
	{
		{"pause",1,0,'p'},
		{"loop",1,0,'l'},
		{"version",0,0,'v'},
		{"help",0,0,'?'},
		{0, 0, 0, 0}
	};

void printhelp(void)
{
	printf("Curses based file save dialog\n"
	"Usage: " APPNAME " [OPTION] /path/to/folder/with/images\n"
	" -p, --pause	Delay before next picture\n"
	" -l, --loop	Loop this many times ( -1 = forever )\n"
	" -v, --version		Version\n"
	" -h, -?, --help		Help\n\n"
	"Report bugs to keithdhedger@gmail.com\n"
	"\nExample:\n"
	"To get the reults of the dialog into a bash varable Use:\n"
	"{ result=$(" APPNAME " -w MyWindow -s /etc -n SaveFileName 2>&1 >&3 3>&-); } 3>&1\n"
	"echo $result\n"
	);
}

int main(int argc, char **argv)
{
	CTK_mainAppClass		*mainApp=new CTK_mainAppClass();
	coloursStruct			cs;
	CTK_cursesLabelClass	*lab;
	const char				*label="Simple Framebuffer Image Example, Press Any Key ...";
	int						labellen=strlen(label);
	struct fbData			*fbinf=mainApp->CTK_getFBData();
	CTK_cursesUtilsClass	cu;
	int						c;
	int						option_index;
	const char				*folder="./";
	int						delay=1;
	int						loop=0;

	while(true)
		{
			option_index=0;

			c=getopt_long(argc,argv,"v?h:p:d:",long_options,&option_index);
				if(c==-1)
					break;

			switch(c)
				{
					case 'p':
						delay=atoi(optarg);
						break;

					case 'l':
						loop=atoi(optarg);
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

	if(argc>1)
		folder=argv[1];

	if(fbinf->usingIM==false)
		{
			fprintf(stderr,"Library not compiled with framebuffer support, can't display image :( ...\n");
			return(1);
		}

	cs.windowBackCol=BACK_WHITE;
	mainApp->CTK_setColours(cs);

	LFSTK_findClass			*files=new LFSTK_findClass();;
	files->LFSTK_setFindType(FILETYPE);
	files->LFSTK_setFileTypes(".png;.tiff;.jpg;");

	files->LFSTK_setFollowLinks(true);
	files->LFSTK_setFullPath(true);
	files->LFSTK_setIncludeHidden(true);
	files->LFSTK_setIgnoreNavLinks(true);
	//files->LFSTK_setDepth(10,20);//TODO//
	files->LFSTK_findFiles(folder);
	files->LFSTK_setSort(false);
	files->LFSTK_sortByTypeAndName();

	lab=mainApp->CTK_addNewLabel((mainApp->maxCols/2)-(labellen/2),1,labellen,1,label);
	lab->CTK_setJustify(CENTRE);

	CTK_cursesFBImageClass	*img=mainApp->CTK_addNewFBImage(2,2,mainApp->maxCols-2,mainApp->maxRows-2,files->data[0].path.c_str());
	img->sx=(mainApp->maxCols/2)-(img->wid/2/fbinf->charWidth)+1;
	mainApp->CTK_mainEventLoop(1);
//	sleep(delay);
	for(int j=1;j<files->data.size();j++)
		{
			//fprintf(stderr,"%s\n",files->data[j].path.c_str());
			img->CTK_newFBImage(2,2,mainApp->maxCols-2,mainApp->maxRows-2,files->data[j].path.c_str());
			img->sx=(mainApp->maxCols/2)-(img->wid/2/fbinf->charWidth)+1;
			mainApp->CTK_mainEventLoop(1);
		//	sleep(delay);
		}

	SETSHOWCURS;

	delete mainApp;
	printf("\n");
	return 0;
}

