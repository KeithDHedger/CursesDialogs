/*
 *
 * ©K. D. Hedger. Mon 24 Jun 14:33:12 BST 2019 keithdhedger@gmail.com

 * This file (ctkdump.cpp) is part of CursesDialogs.

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
#include <time.h>
#include <cursesGlobals.h>

#include "config.h"

#define APPNAME "ctkdump"
enum {DEVSRC=1,DEVDEST};
enum {BTNRUN=0,BTNPRINT,BTNQUIT,BTNABOUT};
enum {ONFINQUIT=0,ONFINHALT,ONFINRESTART,ONFINNOWT};

struct option long_options[]=
	{
		{"window-name",1,0,'w'},
		{"dialog-title",1,0,'t'},
		{"start-folder",1,0,'s'},
		{"version",0,0,'v'},
		{"help",0,0,'?'},
		{0, 0, 0, 0}
	};

CTK_mainAppClass		*mainApp;
std::vector<std::string>	devStrings;
CTK_cursesChooserClass		*destChooser;
CTK_cursesTextBoxClass		*commandLine;
CTK_cursesDropClass			*compressDrop;
CTK_cursesDropClass			*dumpLvlDrop;
CTK_cursesDropClass			*finishDrop;
CTK_cursesInputClass		*blockInp;
CTK_cursesButtonClass		*button;
CTK_cursesUtilsClass		cu;

std::string					devLabel;
std::string					devName;
int							compressLevel=9;
int							blockSize=64;
int							dumpLevel=0;

//void printhelp(void)
//{
//	printf("Curses frontend for dump\n"
//	"Usage: " APPNAME " [OPTION]\n"
//	" -w, --window-name	Window Name\n"
//	" -s, --start-folder	Start Folder\n"
//	" -v, --version		Version\n"
//	" -h, -?, --help		Help\n\n"
//	"Report bugs to keithdhedger@gmail.com\n"
//	"\nExample:\n"
//	"To get the reults of the dialog into a bash varable Use:\n"
//	"{ result=$(" APPNAME " -w MyWindow -s /etc 2>&1 >&3 3>&-); } 3>&1\n"
//	"echo $result\n"
//	);
//}

void getDiskList(const char* command)
{
	FILE	*fp;
	char	*buffer=(char*)alloca(PATH_MAX);
	std::string	str;

	devStrings.clear();

	fp=popen(command,"r");
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
					str=buffer;
					devStrings.push_back(str);
					buffer[0]=0;
				}
			pclose(fp);
		}
}

void setCommandLine(void)
{
	std::string	str;
	struct tm	tstruct;
	time_t		now=time(0);
	char		buf[80];
	tstruct=*localtime(&now);

	str="/sbin/dump";
	str+=" -z";
	str+=std::to_string(compressLevel);
	str+=" -";
	str+=std::to_string(dumpLevel);
	str+=" -b";
	str+=std::to_string(blockSize);
	str+=" -f \"";
	str+=destChooser->folderPath;
	str+="/";
	str+=devLabel;
    strftime(buf,sizeof(buf),".%y%m%d\"",&tstruct);
    str+=buf;
	str+=" ";
	str+=devName;
	commandLine->CTK_updateText(str.c_str(),false,false);
}

void destSelectCB(void *inst,void *userdata)
{
	CTK_cursesChooserClass	*ch=static_cast<CTK_cursesChooserClass*>(inst);

	setCommandLine();
}

void devSelectCB(void *inst,void *userdata)
{
	CTK_cursesListBoxClass		*ls=static_cast<CTK_cursesListBoxClass*>(inst);
	long						ud=(long)ls->listItems[ls->listItemNumber]->userData;
	std::string					str;

	devName=devStrings[ud].substr(0,devStrings[ud].find(":")).c_str();
	str=devStrings[ud].substr(devStrings[ud].find("LABEL=\"")+7);
	devLabel=str.substr(0,str.length()-2);
	setCommandLine();
}

void dropboxCB(void *inst,void *userdata)
{
	CTK_cursesDropClass		*db=static_cast<CTK_cursesDropClass*>(inst);
	if(userdata==(void*)0)
		compressLevel=db->selectedItem;
	if(userdata==(void*)1)
		dumpLevel=db->selectedItem;

	setCommandLine();
}

void inputSelectCB(void *inst,void *userdata)
{
	CTK_cursesInputClass	*inp=static_cast<CTK_cursesInputClass*>(inst);
	blockSize=atoi(blockInp->CTK_getText());
	setCommandLine();
}

void buttonSelectCB(void *inst,void *userdata)
{
	CTK_cursesButtonClass	*btn=static_cast<CTK_cursesButtonClass*>(inst);

	if(userdata==(void*)BTNRUN)
		{
			fprintf(stderr,">>>command=%s<<\n",commandLine->CTK_getText().c_str());
			switch(finishDrop->selectedItem)
				{
					case ONFINQUIT:
						mainApp->runEventLoop=false;
						break;
					case ONFINHALT:
						fprintf(stderr,"shutdown\n");
						break;
					case ONFINRESTART:
						fprintf(stderr,"restart\n");
						break;
					default:
						fprintf(stderr,"nothing\n");
				}
		}
	if(userdata==(void*)BTNQUIT)
		mainApp->runEventLoop=false;
	if(userdata==(void*)BTNPRINT)
		{
			fprintf(stderr,"%s\n",commandLine->CTK_getText().c_str());
			mainApp->runEventLoop=false;
		}

	if(userdata==(void*)BTNABOUT)
		{
			cu.CTK_aboutDialog(mainApp,"ctkdump","CTK Dump Frontend","Copyright 2019 K.D.Hedger","keithdhedger@gmail.com","http://keithhedger.freeddns.org","K.D.Hedger",DATADIR "/help/LICENSE");
			
		}

}

int main(int argc, char **argv)
{
	std::string				str;
//	CTK_cursesUtilsClass	cu;
	char					*folder=NULL;
//	int						c;
//	int						option_index;
//	const char				*wname=NULL;
//	const char				*dname=NULL;
	CTK_cursesListBoxClass	*srcdevlist=new CTK_cursesListBoxClass();
	CTK_cursesListBoxClass	*destdirlist=new CTK_cursesListBoxClass();
	char					buffer[PATH_MAX];

//	while(true)
//		{
//			option_index=0;
//
//			c=getopt_long(argc,argv,"v?h:w:s:",long_options,&option_index);
//			if(c==-1)
//				break;
//
//			switch(c)
//				{
//					case 'w':
//						wname=optarg;
//						break;
//
//					case 's':
//						folder=optarg;
//						break;
//
//					case 'v':
//						printf(APPNAME " %s\n",VERSION);
//						return 0;
//						break;
//
//					case '?':
//					case 'h':
//						printhelp();
//						return 0;
//						break;
//
//					default:
//						fprintf(stderr,"?? Unknown argument ??\n");
//						return(1);
//					break;
//				}
//		}

	mainApp=new CTK_mainAppClass();
	mainApp->colours.fancyGadgets=true;
	mainApp->colours.listBoxType=INBOX;
	mainApp->colours.labelBoxType=NOBOX;
	mainApp->colours.textBoxType=INBOX;
	mainApp->colours.windowBackCol=BACK_WHITE;

	cu.CTK_splashScreen(mainApp,"CTK Frontend to dump\nPlease wait while I collect disk data ...\n");

//src
	getDiskList("blkid |grep \"ext*\"|grep -i label|awk -F\"UUID\" '{print $1}'");
	srcdevlist=mainApp->CTK_addNewListBox(3,3,40,10);
	srcdevlist->CTK_setEnterDeselects(false);
	for(int j=0;j<devStrings.size();j++)
		srcdevlist->CTK_addListItem(devStrings[j].c_str(),(void*)(long)j);
	srcdevlist->CTK_setSelectCB(devSelectCB,(void*)DEVSRC);
	//srcdevlist->listItemNumber=0;

//dest
	destChooser=new CTK_cursesChooserClass(mainApp,3+40+3,3,40,10);
	destChooser->CTK_setShowTypes(FOLDERTYPE);
	destChooser->CTK_setShowHidden(true);
	destChooser->CTK_selectFolder(mainApp,"/media");
	mainApp->CTK_addChooserBox(destChooser);
	destChooser->CTK_setSelectCB(destSelectCB,(void*)DEVDEST);

//compress level
	compressDrop=mainApp->CTK_addNewDropDownBox(mainApp,3+40+3+40+3,2,22,1,"Compress Level");
	for(int j=0;j<10;j++)
		{
			sprintf(buffer,"Compress Level %i",j);
			compressDrop->CTK_addDropItem(buffer);
		}
	compressDrop->CTK_setSelectCB(dropboxCB,(void*)0);

//dump level
	dumpLvlDrop=mainApp->CTK_addNewDropDownBox(mainApp,3+40+3+40+3,4,22,1,"Dump Level");
	for(int j=0;j<10;j++)
		{
			sprintf(buffer,"Dump Level %i",j);
			dumpLvlDrop->CTK_addDropItem(buffer);
		}
	dumpLvlDrop->CTK_setSelectCB(dropboxCB,(void*)1);

//block size
	mainApp->CTK_addNewLabel(3+40+3+40+3,6,strlen("Block Size:"),1,"Block Size:");
	blockInp=mainApp->CTK_addNewInput(3+40+3+40+3+strlen("Block Size:")+2,6,4,1,"64");
	blockInp->CTK_setSelectCB(inputSelectCB,NULL);

//what to do on finish
	finishDrop=mainApp->CTK_addNewDropDownBox(mainApp,3+40+3+40+3,8,22,1,"On Finish Do Nothing");
	finishDrop->CTK_addDropItem("On Finish Quit");
	finishDrop->CTK_addDropItem("On Finish Do Shutdown");
	finishDrop->CTK_addDropItem("On Finish Do Restart");
	finishDrop->CTK_addDropItem("On Finish Do Nothing");
	finishDrop->selectedItem=3;

//run
	button=mainApp->CTK_addNewButton(cu.CTK_getGadgetPosX(2,mainApp->maxCols-2,4,strlen("Run Dump Command"),0),3+12+3,1,1,"Run Dump Command");
	button->CTK_setSelectCB(buttonSelectCB,(void*)BTNRUN);
//print
	button=mainApp->CTK_addNewButton(cu.CTK_getGadgetPosX(2,mainApp->maxCols-2,4,strlen("Print Dump Command"),1),3+12+3,1,1,"Print Dump Command");
	button->CTK_setSelectCB(buttonSelectCB,(void*)BTNPRINT);
//quit
	button=mainApp->CTK_addNewButton(cu.CTK_getGadgetPosX(2,mainApp->maxCols-2,4,strlen("Quit"),2),3+12+3,1,1,"Quit");
	button->CTK_setSelectCB(buttonSelectCB,(void*)BTNQUIT);
//about
	button=mainApp->CTK_addNewButton(cu.CTK_getGadgetPosX(2,mainApp->maxCols-2,4,strlen("About"),3),3+12+3,1,1,"About");
	button->CTK_setSelectCB(buttonSelectCB,(void*)BTNABOUT);

//do what
	mainApp->CTK_addNewTextBox(3,3+12,mainApp->maxCols-4,1,"",true);
	commandLine=mainApp->pages[0].textBoxes[0];
	devSelectCB(srcdevlist,NULL);
	mainApp->CTK_setDefaultGadget(LIST,0);
	mainApp->CTK_mainEventLoop();

	SETSHOWCURS;
	delete mainApp;

	return(0);
}

