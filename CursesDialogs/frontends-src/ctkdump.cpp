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
 
#include <time.h>
#include <cursesGlobals.h>

#include "config.h"

#define APPNAME "ctkdump"

enum {DEVSRC=1,DEVDEST};
enum {BTNRUN=0,BTNPRINT,BTNQUIT,BTNABOUT};
enum {ONFINQUIT=0,ONFINHALT,ONFINRESTART,ONFINNOWT};

CTK_mainAppClass			*mainApp=new CTK_mainAppClass();
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

void getDiskList(const char* command)
{
	FILE		*fp;
	char		*buffer=(char*)alloca(PATH_MAX);
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
	mainApp->CTK_updateScreen(mainApp,NULL);
}

bool destSelectCB(void *inst,void *userdata)
{
	CTK_cursesChooserClass	*ch=static_cast<CTK_cursesChooserClass*>(inst);

	setCommandLine();
	return(true);
}

bool devSelectCB(void *inst,void *userdata)
{
	CTK_cursesListBoxClass		*ls=static_cast<CTK_cursesListBoxClass*>(inst);
	long						ud=(long)ls->listItems[ls->listItemNumber]->userData;
	std::string					str;

	devName=devStrings[ud].substr(0,devStrings[ud].find(":")).c_str();
	str=devStrings[ud].substr(devStrings[ud].find("LABEL=\"")+7);
	devLabel=str.substr(0,str.length()-2);
	setCommandLine();
	return(true);
}

bool dropboxCB(void *inst,void *userdata)
{
	CTK_cursesDropClass		*db=static_cast<CTK_cursesDropClass*>(inst);

	if(userdata==(void*)0)
		compressLevel=db->selectedItem+1;
	if(userdata==(void*)1)
		dumpLevel=db->selectedItem;
	setCommandLine();
	return(true);
}

bool inputSelectCB(void *inst,void *userdata)
{
	CTK_cursesInputClass	*inp=static_cast<CTK_cursesInputClass*>(inst);

	blockSize=atoi(blockInp->CTK_getText());
	setCommandLine();
	return(true);
}

bool buttonSelectCB(void *inst,void *userdata)
{
	CTK_cursesButtonClass	*btn=static_cast<CTK_cursesButtonClass*>(inst);

	if(userdata==(void*)BTNRUN)
		{
			system(commandLine->CTK_getText().c_str());
			switch(finishDrop->selectedItem)
				{
					case ONFINQUIT:
						mainApp->runEventLoop=false;
						break;
					case ONFINHALT:
						system("shutdown -h now");
						break;
					case ONFINRESTART:
						system("shutdown -r now");
						break;
				}
		}

	if(userdata==(void*)BTNQUIT)
		mainApp->runEventLoop=false;

	if(userdata==(void*)BTNPRINT)
		fprintf(stderr,"%s\n",commandLine->CTK_getText().c_str());

	if(userdata==(void*)BTNABOUT)
		{
			cu.CTK_aboutDialog("ctkdump","CTK Dump Frontend","Copyright 2019 K.D.Hedger","keithdhedger@gmail.com","http://keithhedger.freeddns.org","K.D.Hedger",DATADIR "/help/LICENSE");
			mainApp->CTK_clearScreen();
		}
	mainApp->CTK_updateScreen(mainApp,NULL);
	return(true);
}

int main(int argc, char **argv)
{
	std::string				str;
	char					*folder=NULL;
	CTK_cursesListBoxClass	*srcdevlist=new CTK_cursesListBoxClass(mainApp);
	CTK_cursesListBoxClass	*destdirlist=new CTK_cursesListBoxClass(mainApp);
	char					buffer[PATH_MAX];
	int						maxlen;

	mainApp->colours.fancyGadgets=true;
	mainApp->colours.listBoxType=INBOX;
	mainApp->colours.labelBoxType=NOBOX;
	mainApp->colours.textBoxType=INBOX;
	mainApp->colours.windowBackCol=BACK_WHITE;

	cu.CTK_splashScreen(mainApp,"CTK Frontend to dump\nPlease wait while I collect disk data ...\n");

//src
	getDiskList("blkid |grep \"ext*\"|grep -i label|awk -F\"UUID\" '{print $1}'");
	srcdevlist=mainApp->CTK_addNewListBox(3,3,40,10);
	srcdevlist->CTK_setSelectDeselects(false);
	for(int j=0;j<devStrings.size();j++)
		srcdevlist->CTK_addListItem(devStrings[j].c_str(),(void*)(long)j);
	srcdevlist->CTK_setSelectCB(devSelectCB,(void*)DEVSRC);

//dest
	destChooser=new CTK_cursesChooserClass(mainApp,3+40+3,3,40,10);
	destChooser->CTK_setShowTypes(FOLDERTYPE);
	destChooser->CTK_setShowHidden(true);
	destChooser->CTK_selectFolder(mainApp,"/media");
	mainApp->CTK_addChooserBox(destChooser);
	destChooser->CTK_setSelectCB(destSelectCB,(void*)DEVDEST);
//compress level
	compressDrop=mainApp->CTK_addNewDropDownBox(mainApp,3+40+3+40+3,2,22,1,"Compress Level 9");
	for(int j=1;j<10;j++)
		{
			sprintf(buffer,"Compress Level %i",j);
			compressDrop->CTK_addDropItem(buffer);
		}
	compressDrop->CTK_setSelectCB(dropboxCB,(void*)0);
	compressDrop->selectedItem=8;

//dump level
	dumpLvlDrop=mainApp->CTK_addNewDropDownBox(mainApp,3+40+3+40+3,4,22,1,"Dump Level 0");
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
	finishDrop->CTK_setSelectCB(dropboxCB,(void*)2);

	maxlen=strlen("Print Dump Command");
	std::string	labelstr=cu.CTK_padString(std::string("Run Dump Command"),maxlen);
//run
	button=mainApp->CTK_addNewButton(cu.CTK_getGadgetPosX(2,mainApp->maxCols-2,4,maxlen,0),3+12+3,1,1,labelstr.c_str());
	button->CTK_setSelectCB(buttonSelectCB,(void*)BTNRUN);
//print
	labelstr=cu.CTK_padString(std::string("Print Dump Command"),maxlen);
	button=mainApp->CTK_addNewButton(cu.CTK_getGadgetPosX(2,mainApp->maxCols-2,4,maxlen,1),3+12+3,1,1,labelstr.c_str());
	button->CTK_setSelectCB(buttonSelectCB,(void*)BTNPRINT);
//quit
	labelstr=cu.CTK_padString(std::string("Quit"),maxlen);
	button=mainApp->CTK_addNewButton(cu.CTK_getGadgetPosX(2,mainApp->maxCols-2,4,maxlen,2),3+12+3,1,1,labelstr.c_str());
	button->CTK_setSelectCB(buttonSelectCB,(void*)BTNQUIT);
//about
	labelstr=cu.CTK_padString(std::string("About"),maxlen);
	button=mainApp->CTK_addNewButton(cu.CTK_getGadgetPosX(2,mainApp->maxCols-2,4,maxlen,3),3+12+3,1,1,labelstr.c_str());
	button->CTK_setSelectCB(buttonSelectCB,(void*)BTNABOUT);

//do what
	commandLine=mainApp->CTK_addNewTextBox(3,3+12,mainApp->maxCols-4,1,"",true);
	bool sink=devSelectCB(srcdevlist,NULL);
	mainApp->CTK_setDefaultGadget(srcdevlist);
	mainApp->CTK_mainEventLoop(0,true,true);

	SETSHOWCURS;
	delete mainApp;

	return(0);
}

