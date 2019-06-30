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

struct option long_options[]=
	{
		{"window-name",1,0,'w'},
		{"dialog-title",1,0,'t'},
		{"start-folder",1,0,'s'},
		{"version",0,0,'v'},
		{"help",0,0,'?'},
		{0, 0, 0, 0}
	};

std::vector<std::string>	devStrings;
CTK_cursesChooserClass		*destChooser;
CTK_cursesTextBoxClass		*commandLine;
CTK_cursesDropClass			*compressDrop;

std::string					devLabel;
std::string					devName;
int							compressLevel=9;
int							blockSize=64;
int							dumpLevel=0;

void printhelp(void)
{
	printf("Curses frontend for dump\n"
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

void getDiskList(bool where,const char* command)
{
	FILE	*fp;
	char	*buffer=(char*)alloca(PATH_MAX);
	std::string	str;

	if(where==true)
		devStrings.clear();
//	else
//		destStrings.clear();

//	fp=popen("blkid |grep \"ext*\"|grep -i label|awk -F\"UUID\" '{print $1}'","r");
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
					if(where==true)
						devStrings.push_back(str);
//					else
//						destStrings.push_back(str);
					buffer[0]=0;
				}
			pclose(fp);
		}
}

// /sbin/dump -z9 -0 -d 64 -f "/media/SkyNet/LinuxBackups/LinuxData.190627" "/dev/sdd1"
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
	str+=" -d";
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
	fprintf(stderr,">>>command=%s<<\n",str.c_str());
}

void destSelectCB(void *inst,void *userdata)
{
	CTK_cursesChooserClass	*ch=static_cast<CTK_cursesChooserClass*>(inst);
//	std::string				str;
	setCommandLine();
//	str="dump ";
//	str+="-z9 ";
//	str+="-d64 ";
//	str+=ch->folderPath;
//	str+="/";
//	str+=devLabel;
//	commandLine->CTK_updateText(str.c_str(),false,false);
//	fprintf(stderr,"folder=%s\nname=%s\npath=%s\n",ch->folderPath.c_str(),ch->fileName.c_str(),ch->filePath.c_str());
//	fprintf(stderr,">>>command=%s<<\n",str.c_str());
}

void devSelectCB(void *inst,void *userdata)
{
	char						*buffer=(char*)alloca(256);
	CTK_cursesListBoxClass		*ls=static_cast<CTK_cursesListBoxClass*>(inst);
	long						ud=(long)ls->listItems[ls->listItemNumber]->userData;
	std::string					str;

	//fprintf(stderr,"List %i List item '%s' clicked, user data=%p.\n",(long)userdata,ls->listItems[ls->listItemNumber]->label.c_str(),ls->listItems[ls->listItemNumber]->userData);
	fprintf(stderr,">>%s<<\n",devStrings[ud].substr(0,devStrings[ud].find(":")).c_str());
	devName=devStrings[ud].substr(0,devStrings[ud].find(":")).c_str();
	str=devStrings[ud].substr(devStrings[ud].find("LABEL=\"")+7);
	fprintf(stderr,">>%s<<\n",str.substr(0,str.length()-2).c_str());
	devLabel=str.substr(0,str.length()-2);
	setCommandLine();
	
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
	CTK_cursesListBoxClass	*srcdevlist=new CTK_cursesListBoxClass();
	CTK_cursesListBoxClass	*destdirlist=new CTK_cursesListBoxClass();
	char					buffer[PATH_MAX];

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
	mainApp->colours.fancyGadgets=true;
	mainApp->colours.listBoxType=INBOX;
	mainApp->colours.textBoxType=INBOX;
	mainApp->colours.windowBackCol=BACK_WHITE;

//src
	getDiskList(true,"blkid |grep \"ext*\"|grep -i label|awk -F\"UUID\" '{print $1}'");
	srcdevlist->CTK_newListBox(3,3,40,10);
	srcdevlist->CTK_setEnterDeselects(false);
	srcdevlist->CTK_setColours(mainApp->colours);
	for(int j=0;j<devStrings.size();j++)
		{
			srcdevlist->CTK_addListItem(devStrings[j].c_str(),(void*)(long)j);
		}
	srcdevlist->CTK_setSelectCB(devSelectCB,(void*)DEVSRC);
	mainApp->CTK_addListBox(srcdevlist);

//dest
	//getDiskList(false,"mount |grep \"ext\"|awk '{print $3}'");
	destChooser=new CTK_cursesChooserClass(mainApp,3+40+3,3,40,10);
	destChooser->CTK_setShowTypes(FOLDERTYPE);
	destChooser->CTK_setShowHidden(true);
	destChooser->CTK_selectFolder(mainApp,"/media");
	mainApp->CTK_addChooserBox(destChooser);
	destChooser->CTK_setSelectCB(destSelectCB,(void*)DEVDEST);

//compress
	mainApp->CTK_addNewDropDownBox(mainApp,3+40+3+40+3,2,15,1,"Compress Level");
	compressDrop=mainApp->pages[0].dropDowns[0];
	for(int j=0;j<10;j++)
		{
			sprintf(buffer,"Compress %i ",j);
			compressDrop->CTK_addDropItem(buffer);
		}
	//compressDrop->CTK_setColours(cs);
	//mainApp->pages[0].dropDowns[0]->CTK_setSelectCB(dropboxCB,NULL);
	//mainApp->pages[0].dropDowns[0]->CTK_setItemEnabled(1,false);

//do what
	mainApp->CTK_addNewTextBox(3,3+12,80+3,1,"",true);
	commandLine=mainApp->pages[0].textBoxes[0];
	mainApp->CTK_mainEventLoop();

	SETSHOWCURS;
	delete mainApp;

	return(0);
}

