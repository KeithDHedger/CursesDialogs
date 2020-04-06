/******************************************************
/*
 *
 * ©K. D. Hedger. Fri  6 Sep 20:14:24 BST 2019 keithdhedger@gmail.com

 * This file (musicplayer.cpp) is part of CursesDialogs.

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

#include "musicplayer.h"

CTK_mainAppClass			*mainApp=new CTK_mainAppClass();
LFSTK_findClass				*files=new LFSTK_findClass();;
CTK_cursesChooserClass		*playLists;
CTK_cursesListBoxClass		*songList;
CTK_cursesFBImageClass		*image;
CTK_cursesFBImageClass		*albumArt;
CTK_cursesTextBoxClass		*nowPlaying;

const char					*folder="./";
int							midWay;
int							chooserWidth;
int							songsWidth;
char						*resources;
int							dialogWidth;
int							chooserHite;
int							songsHite;
int							controlsSY;
int							artSY;
int							artHite;
std::vector<char*>			songs;
bool						playing=false;
bool						paused=false;
bool						doQuit=false;
char						*oldfile=NULL;
bool						updated=false;
char						*outName=NULL;
char						*fifoName=NULL;
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

//make clean && make CPPFLAGS='-DDATADIR=\""'$(pwd)'/CursesDialogs/resources"\"' -j4 && CursesDialogs/frontends/ctkmusicplayer  /media/Music/Flacs/Playlists 2>/tmp/debug.log 

char* oneLiner(const char* fmt,...)
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
			return(strdup(buffer));
		}
	return(NULL);
}

void sendToPipe(const char *command)
{
	char	buffer[PATH_MAX];

	sprintf(buffer,"echo -e \"%s\" >\"%s\" &",command,fifoName);
	system(buffer);
}

void getMeta(void)
{
	char	*filename;
	char	*album;
	char	*title;
	char	*artist;
	char	*all;
	char	*command;
	char	*jpeg;

	if((playing==false) || (paused==true))
		return;

	sprintf(commandString,"get_property path\\nget_meta_album\\nget_meta_title\\nget_meta_artist");
	sendToPipe(commandString);
	filename=oneLiner("tail -n4 '%s' |sed -n '1p'|awk -F= '{print $2}'",outName);

	if(strcmp(filename,"(null)")==0)
		{
			for(int j=0;j<songs.size();j++)
				free(songs[j]);
			songs.clear();
			songList->CTK_clearList();
			albumArt->CTK_newFBImage(chooserWidth+6,artSY,artHite*2,artHite,"",false);
			nowPlaying->CTK_updateText("");
			mainApp->CTK_setDefaultGadget(playLists->lb);
			mainApp->CTK_clearScreen();
			mainApp->CTK_updateScreen(mainApp,(void*)1);
			playing=false;
			paused=false;
			return;
		}

	if(oldfile!=NULL)
		{
			if(strcmp(filename,oldfile)==0)
				{
					while(updated==false)
						{
							album=oneLiner("tail -n4 '%s' |sed -n '2p'|awk -F= '{print $2}'|sed -n 's/^.\\(.*\\).$/\\1/p'",outName);
							title=oneLiner("tail -n4 '%s' |sed -n '3p'|awk -F= '{print $2}'|sed -n 's/^.\\(.*\\).$/\\1/p'",outName);
							artist=oneLiner("tail -n4 '%s' |sed -n '4p'|awk -F= '{print $2}'|sed -n 's/^.\\(.*\\).$/\\1/p'",outName);
							asprintf(&all,"Album:  %s\nArtist: %s\nSong:   %s\n",album,artist,title);

							nowPlaying->CTK_updateText(all,false,false);
							updated=true;
							free(title);
							free(album);
							free(artist);
							free(all);
							sprintf(commandString,":>'%s'",outName);
							system(commandString);

							sprintf(commandString,"%s",filename);
							asprintf(&jpeg,"%s/folder.jpg",dirname(commandString));
							albumArt->CTK_newFBImage(chooserWidth+6,artSY,artHite*2,artHite,jpeg,false);
							mainApp->CTK_updateScreen(mainApp,NULL);
							free(jpeg);
						}

					free(filename);
					sprintf(commandString,":>'%s'",outName);
					system(commandString);
					return;
				}
		}
	oldfile=strdup(filename);
	updated=false;
}

bool selectSongCB(void *inst,void *userdata)
{
	char *command;
	CTK_cursesListBoxClass	*sl=static_cast<CTK_cursesListBoxClass*>(inst);

	if(sl->listItems.size()==0)
		return(true);

	sprintf(commandString,"p\\npausing_keep_force loadlist \\\"%s\\\"",playLists->filePath.c_str());
	sendToPipe(commandString);

	if((long)sl->listItems[sl->listItemNumber]->userData!=0)
		{
			sprintf(commandString,"pausing_keep_force pt_step %i\\np",(long)sl->listItems[sl->listItemNumber]->userData);
			sendToPipe(commandString);
		}
	else
		sendToPipe("p");

	playing=true;
	paused=false;
	return(true);
}

bool controlsCB(void *inst,void *userdata)
{
	CTK_cursesFBImageClass	*bc=static_cast<CTK_cursesFBImageClass*>(inst);
	long					ud=(long)userdata;
	char					*command;

	switch(ud)
		{
			case START:
				if(playing==false)
					return(true);
				sprintf(commandString,"loadlist \\\"%s\\\"",playLists->filePath.c_str());
				sendToPipe(commandString);
				playing=true;
				paused=false;
				break;

			case PREVIOUS:
				if(playing==false)
					return(true);
				sendToPipe("pt_step -1");
				paused=false;
				break;

			case PLAY:
				if(playing==true)
					{
						sendToPipe("p");
						paused=!paused;
					}
				if(playing==true)
					return(true);
				break;

			case STOP:
				if(playing==false)
					return(true);
				for(int j=0;j<songs.size();j++)
					free(songs[j]);
				songs.clear();
				songList->CTK_clearList();
				sendToPipe("stop");
				albumArt->CTK_newFBImage(chooserWidth+6,artSY,artHite*2,artHite,"",false);
				nowPlaying->CTK_updateText("");
				mainApp->CTK_setDefaultGadget(playLists->lb);
				mainApp->CTK_clearScreen();
				mainApp->CTK_updateScreen(mainApp,(void*)1);
				playing=false;
				paused=false;
				break;

			case PAUSE:
				if(playing==false)
					return(true);
				sendToPipe("p");
				paused=!paused;
				break;

			case END:
				if(playing==false)
					return(true);
				sprintf(commandString,"p\\npausing_keep_force loadlist \\\"%s\\\"",playLists->filePath.c_str());
				sendToPipe(commandString);
				sprintf(commandString,"pausing_keep_force pt_step %i\\np",songs.size()-1);
				sendToPipe(commandString);
				playing=true;
				paused=false;
				break;

			case NEXT:
				if(playing==false)
					return(true);
				sendToPipe("pt_step 1");
				paused=false;
				break;

			case QUIT:
				mainApp->runEventLoop=false;
				doQuit=true;
				break;
		}
	return(true);
}

bool playListsCB(void *inst,void *userdata)
{
	CTK_cursesChooserClass	*ch=static_cast<CTK_cursesChooserClass*>(inst);
	long					ud=(long)userdata;
	FILE					*fd=NULL;
	char					buffer[PATH_MAX];
	char					buffer2[PATH_MAX];

	if((ch->files->data[ch->lb->listItemNumber].fileType==FOLDERTYPE) || (ch->files->data[ch->lb->listItemNumber].fileType==FILELINKTYPE))
		return(true);
	folder=ch->folderPath.c_str();
	for(int j=0;j<songs.size();j++)
		free(songs[j]);
	songs.clear();
	songList->CTK_clearList();

	fd=fopen(ch->filePath.c_str(),"r");
	if(fd!=NULL)
		{
			while(feof(fd)==0)
				{
					buffer[0]=0;
					fgets(buffer,2048,fd);
					if(strlen(buffer)>0)
						{
							buffer[strlen(buffer)-1]=0;
							sprintf(buffer2,"%s/%s",folder,buffer);
							songs.push_back(strdup(buffer2));
						}
				}
			fclose(fd);
		}
	for(int j=0;j<songs.size();j++)
		{
			char	*ptr=strrchr(songs[j],'/');
			songList->CTK_addListItem(++ptr,(void*)j);
		}

	sprintf(commandString,"loadlist '%s'",ch->filePath.c_str());
	sendToPipe(commandString);
	playing=true;
	return(true);
}

int main(int argc, char **argv)
{
	int						c;
	int						option_index;
	CTK_cursesButtonClass	*button;
	char					imagepath[PATH_MAX];

	midWay=mainApp->maxCols/2;
	dialogWidth=mainApp->maxCols-4;
	chooserWidth=((mainApp->maxCols/8)*5)-2;
	chooserHite=mainApp->maxRows-16;
	controlsSY=mainApp->maxRows-5;
	songsWidth=mainApp->maxCols-chooserWidth-7;
	songsHite=(chooserHite+4)/2;
	artSY=songsHite+4;
	artHite=songsHite;
	asprintf(&resources,"%s/MusicPlayer",DATADIR);
//	asprintf(&resources,"%s","/media/LinuxData/Development64/Projects/CursesDialogs/CursesDialogs/resources/");
//	fprintf(stderr,"resources=%s\n",resources);
	asprintf(&fifoName,"/tmp/mplayerfifo%i",getpid());
	asprintf(&outName,"/tmp/mplayerout%i",getpid());
	sprintf(commandString,"mkfifo '%s'",fifoName);
	system(commandString);

//start mplayer
	sprintf(commandString,"mplayer -quiet -slave -input file='%s' -idle >'%s' 2>/dev/null &",fifoName,outName);
	system(commandString);

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
		folder=argv[optind];

	mainApp->colours.fancyGadgets=true;
	mainApp->colours.boxType=NOBOX;
	mainApp->colours.textBoxType=INBOX;
	mainApp->colours.windowBackCol=BACK_WHITE;
	mainApp->colours.backCol=BACK_WHITE;
	mainApp->colours.foreCol=FORE_BLACK;

	playLists=new CTK_cursesChooserClass(mainApp,3,2,chooserWidth,chooserHite);
	playLists->CTK_setShowFileTypes(".m3u;");
	playLists->CTK_setShowTypes(ANYTYPE);
	playLists->CTK_setShowHidden(false);
	playLists->CTK_selectFolder(mainApp,folder);
	playLists->CTK_setSelectCB(playListsCB,NULL);
	mainApp->CTK_addChooserBox(playLists);

	songList=mainApp->CTK_addNewListBox(chooserWidth+6,2,songsWidth,songsHite);
	songList->CTK_setSelectCB(selectSongCB,NULL);

	nowPlaying=mainApp->CTK_addNewTextBox(3,chooserHite+4,chooserWidth,3,"");
	nowPlaying->CTK_setSelectable(false);

	sprintf(imagepath,"%s/MusicPlayer/start.png",resources);
	image=mainApp->CTK_addNewFBImage(mainApp->utils->CTK_getGadgetPosX(midWay-(dialogWidth/2),dialogWidth,CONTROLCNT,4,0),controlsSY,4,4,imagepath);
	image->CTK_setSelectCB(controlsCB,(void*)START);
	sprintf(imagepath,"%s/MusicPlayer/prev.png",resources);
	image=mainApp->CTK_addNewFBImage(mainApp->utils->CTK_getGadgetPosX(midWay-(dialogWidth/2),dialogWidth,CONTROLCNT,4,1),controlsSY,4,4,imagepath);
	image->CTK_setSelectCB(controlsCB,(void*)PREVIOUS);
	sprintf(imagepath,"%s/MusicPlayer/play.png",resources);
	image=mainApp->CTK_addNewFBImage(mainApp->utils->CTK_getGadgetPosX(midWay-(dialogWidth/2),dialogWidth,CONTROLCNT,4,2),controlsSY,4,4,imagepath);
	image->CTK_setSelectCB(controlsCB,(void*)PLAY);
	sprintf(imagepath,"%s/MusicPlayer/stop.png",resources);
	image=mainApp->CTK_addNewFBImage(mainApp->utils->CTK_getGadgetPosX(midWay-(dialogWidth/2),dialogWidth,CONTROLCNT,4,3),controlsSY,4,4,imagepath);
	image->CTK_setSelectCB(controlsCB,(void*)STOP);
	sprintf(imagepath,"%s/MusicPlayer/pause.png",resources);
	image=mainApp->CTK_addNewFBImage(mainApp->utils->CTK_getGadgetPosX(midWay-(dialogWidth/2),dialogWidth,CONTROLCNT,4,4),controlsSY,4,4,imagepath);
	image->CTK_setSelectCB(controlsCB,(void*)PAUSE);
	sprintf(imagepath,"%s/MusicPlayer/next.png",resources);
	image=mainApp->CTK_addNewFBImage(mainApp->utils->CTK_getGadgetPosX(midWay-(dialogWidth/2),dialogWidth,CONTROLCNT,4,5),controlsSY,4,4,imagepath);
	image->CTK_setSelectCB(controlsCB,(void*)NEXT);
	sprintf(imagepath,"%s/MusicPlayer/end.png",resources);
	image=mainApp->CTK_addNewFBImage(mainApp->utils->CTK_getGadgetPosX(midWay-(dialogWidth/2),dialogWidth,CONTROLCNT,4,6),controlsSY,4,4,imagepath);
	image->CTK_setSelectCB(controlsCB,(void*)END);
	sprintf(imagepath,"%s/MusicPlayer/quit.png",resources);
	image=mainApp->CTK_addNewFBImage(mainApp->utils->CTK_getGadgetPosX(midWay-(dialogWidth/2),dialogWidth,CONTROLCNT,4,7),controlsSY,4,4,imagepath);
	image->CTK_setSelectCB(controlsCB,(void*)QUIT);

	albumArt=mainApp->CTK_addNewFBImage(chooserWidth+6,artSY,artHite,artHite,NULL,false);
	albumArt->CTK_setSelectable(false);

	mainApp->CTK_setDefaultGadget(playLists->lb);
	mainApp->CTK_clearScreen();
	mainApp->CTK_updateScreen(mainApp,NULL);

	do
		{
			mainApp->CTK_mainEventLoop(-250,false);
			getMeta();
		}
	while (doQuit==false);

	sendToPipe("q");
	SETSHOWCURS;
	delete mainApp;
	free(resources);
	sprintf(commandString,"rm '%s' '%s'",fifoName,outName);
	system(commandString);
	free(fifoName);
	free(outName);
	for(int j=0;j<songs.size();j++)
		free(songs[j]);

	return(0);
}

