/*
 *
 * ©K. D. Hedger. Sun 15 Sep 13:14:31 BST 2019 keithdhedger@gmail.com

 * This file (castreceiver.cpp) is part of CursesDialogs.

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

#include "castreceiver.h"

CTK_mainAppClass			*mainApp=new CTK_mainAppClass();
CTK_cursesListBoxClass		*urlList;
CTK_cursesLabelClass		*infoLabel;
CTK_cursesLabelClass		*currentDLFolder;
CTK_cursesLabelClass		*currentURL;
CTK_cursesButtonClass		*selectDLFolder;
CTK_cursesButtonClass		*waitForCast;
CTK_cursesButtonClass		*showURL;
CTK_cursesButtonClass		*downloadURL;
CTK_cursesButtonClass		*quit;

const char					*downloadFolder="~/Downloads";
const char					*recentName=".config/ctkcastreceiver.recent";

int							midWay;
int							dialogWidth;
int							dialogHite;
int							urlListHite;
int							urlListWidth;
int							dialogSX=2;
int							dialogSY=2;
//int							chooserWidth;
//int							songsWidth;
//char						*resources;
//int							chooserHite;
//int							controlsSY;
//int							artSY;
//int							artHite;
//std::vector<char*>			songs;
//bool						playing=false;
//bool						paused=false;
//bool						doQuit=false;
//char						*oldfile=NULL;
//bool						updated=false;
char						commandString[PATH_MAX];


void printhelp(void)
{
	printf("Curses based file save dialog\n"
	"Usage: " APPNAME " [OPTION] /path/to/folder/with/playlists\n"
	" -v, --version	Version\n"
	" -h, -?, --help	Help\n\n"
	"Report bugs to keithdhedger@gmail.com\n"
	);
}

char* oneLiner(bool usereturn,const char* fmt,...)
{
	FILE	*fp;
	va_list	ap;
	char	*buffer,*subbuffer;

	buffer=(char*)alloca(PATH_MAX);
	subbuffer=(char*)alloca(PATH_MAX);

	buffer[0]=0;
	subbuffer[0]=0;
	va_start(ap, fmt);
	while (*fmt)
		{
			subbuffer[0]=0;
			if(fmt[0]=='%')
				{
					fmt++;
					switch(*fmt)
						{
							case 's':
								sprintf(subbuffer,"%s",va_arg(ap,char*));
								break;
							case 'i':
								sprintf(subbuffer,"%i",va_arg(ap,int));
								break;
							case '%':
								sprintf(subbuffer,"%%");
								break;
							default:
								sprintf(subbuffer,"%c",fmt[0]);
								break;
						}
				}
			else
				sprintf(subbuffer,"%c",fmt[0]);
			strcat(buffer,subbuffer);
			fmt++;
		}
	va_end(ap);

	fp=popen(buffer,"r");
	if(fp!=NULL)
		{
			buffer[0]=0;
			fgets(buffer,PATH_MAX,fp);
			if(strlen(buffer)>0)
				{
					if(buffer[strlen(buffer)-1] =='\n')
						buffer[strlen(buffer)-1]=0;
				}
			pclose(fp);
			if(usereturn==true)
				return(strdup(buffer));
		}
	return(NULL);
}

void getRecents(void)//TODO//add to library
{
	FILE	*fp;
	char	buffer[PATH_MAX];

	urlList->CTK_clearList();
	sprintf(commandString,"%s/%s",getenv("HOME"),recentName);
	fp=fopen(commandString,"r");
	if(fp!=NULL)
		{
			buffer[0]=0;
			while(fgets(buffer,PATH_MAX,fp))
				{
					if(strlen(buffer)>0)
						{
							if(buffer[strlen(buffer)-1] =='\n')
								buffer[strlen(buffer)-1]=0;
						}
					urlList->CTK_addListItem(buffer);
				}
			fclose(fp);
		}
}

void selectURLCB(void *inst,void *userdata)
{
	std::string				label;

	CTK_cursesListBoxClass	*sl=static_cast<CTK_cursesListBoxClass*>(inst);

	if(sl->listItems.size()==0)
		return;

	label="Download Folder: ";
	label+=downloadFolder;
	label+="\nCurrent URL    : ";
	label+=sl->listItems[sl->listItemNumber]->label;
	infoLabel->CTK_updateText(label.c_str());
}

void buttonsCB(void *inst,void *userdata)
{
	CTK_cursesButtonClass	*bc=static_cast<CTK_cursesButtonClass*>(inst);
	long					ud=(long)userdata;

	fprintf(stderr,"buutonno=%i\n",ud);
	switch(ud)
		{
			case QUIT:
				mainApp->runEventLoop=false;
				MOVETO(1,mainApp->maxRows);
				SETSHOWCURS;
				delete mainApp;
				exit(0);
				break;
		}
}

int main(int argc, char **argv)
{
	int						c;
	int						option_index;
	CTK_cursesButtonClass	*button;
	char					imagepath[PATH_MAX];
	std::string				label;

	midWay=mainApp->maxCols/2;
	dialogWidth=mainApp->maxCols-2;
	dialogHite=mainApp->maxRows-4;
//	chooserWidth=((mainApp->maxCols/8)*5)-2;
//	chooserHite=mainApp->maxRows-16;
//	controlsSY=mainApp->maxRows-5;
//	songsWidth=mainApp->maxCols-chooserWidth-7;
//	songsHite=(chooserHite+4)/2;
//	artSY=songsHite+4;
//	artHite=songsHite;

	urlListWidth=dialogWidth;
	urlListHite=dialogHite/2;

	while(true)
		{
			option_index=0;

			c=getopt_long(argc,argv,"v?h",long_options,&option_index);
				if(c==-1)
					break;

			switch(c)
				{
					case 'v':
						printf(APPNAME " %s\n",VERSION);
						delete mainApp;
						printf("\n");
						return 0;
						break;

					case '?':
					case 'h':
						printhelp();						
						SETSHOWCURS;
						delete mainApp;
						printf("\n");
						return 0;
						break;

					default:
						fprintf(stderr,"?? Unknown argument ??\n");
						return(1);
						break;
				}
		}

	if(optind<argc)
		downloadFolder=argv[optind];

	oneLiner(false,"mkdir -p '%s' &>/dev/null",downloadFolder);
	oneLiner(false,"touch '%s' &>/dev/null",recentName);
	mainApp->colours.fancyGadgets=true;
	mainApp->colours.windowBackCol=BACK_WHITE;
	mainApp->colours.backCol=BACK_WHITE;
	mainApp->colours.foreCol=FORE_BLACK;

	urlList=mainApp->CTK_addNewListBox(dialogSX,dialogSY,urlListWidth,urlListHite);
	urlList->CTK_setSelectCB(selectURLCB,NULL);
	urlList->CTK_setSelectDeselects(false);
	getRecents();
/*
CTK_cursesLabelClass		*currentDLFolder;
CTK_cursesLabelClass		*currentURL;
CTK_cursesButtonClass		*selectDLFolder;
CTK_cursesButtonClass		*waitForCast;
CTK_cursesButtonClass		*showURL;
CTK_cursesButtonClass		*downloadURL;
CTK_cursesButtonClass		*quit;

*/

	label="Download Folder: ";
	label+=downloadFolder;
	label+="\nCurrent URL    :";
	infoLabel=mainApp->CTK_addNewLabel(2,dialogSY+urlListHite+2,urlListWidth,2,label.c_str());
	
	selectDLFolder=mainApp->CTK_addNewButton(mainApp->utils->CTK_getGadgetPosX(2,urlListWidth,CONTROLCNT,16,0),dialogSY+urlListHite+2+2+2,16,1,"  DL Folder  ");
	selectDLFolder->CTK_setSelectCB(buttonsCB,(void*)SELECTDLFOLDER);
	waitForCast=mainApp->CTK_addNewButton(mainApp->utils->CTK_getGadgetPosX(2,urlListWidth,CONTROLCNT,16,1),dialogSY+urlListHite+2+2+2,16,1,"Wait For Cast");
	waitForCast->CTK_setSelectCB(buttonsCB,(void*)WAITFORCAST);
	showURL=mainApp->CTK_addNewButton(mainApp->utils->CTK_getGadgetPosX(2,urlListWidth,CONTROLCNT,16,2),dialogSY+urlListHite+2+2+2,16,1,"  Play  URL  ");
	showURL->CTK_setSelectCB(buttonsCB,(void*)PLAYURL);
	downloadURL=mainApp->CTK_addNewButton(mainApp->utils->CTK_getGadgetPosX(2,urlListWidth,CONTROLCNT,16,3),dialogSY+urlListHite+2+2+2,16,1,"Download  URL");
	downloadURL->CTK_setSelectCB(buttonsCB,(void*)DLURL);
	quit=mainApp->CTK_addNewButton(mainApp->utils->CTK_getGadgetPosX(2,urlListWidth,CONTROLCNT,16,4),dialogSY+urlListHite+2+2+2,16,1,"     Quit    ");
	quit->CTK_setSelectCB(buttonsCB,(void*)QUIT);

	mainApp->CTK_setDefaultGadget(urlList);
	mainApp->CTK_clearScreen();
	mainApp->CTK_updateScreen(mainApp,NULL);
	while(mainApp->CTK_mainEventLoop(-500,false)!='q');

	MOVETO(1,mainApp->maxRows);
	SETSHOWCURS;
	delete mainApp;

	return(0);
}
