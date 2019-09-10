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
int							buttonsSY;
int							artSY;
int							artHite;
std::vector<char*>			songs;
bool						playing=false;
bool						paused=false;
bool						doQuit=false
;

void printhelp(void)
{
	printf("Curses based file save dialog\n"
	"Usage: " APPNAME " [OPTION] /path/to/folder/with/playlistd\n"
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

char	*oldfile=NULL;
bool	updated=false;

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

	system("echo -e \"get_meta_album\nget_meta_title\nget_meta_artist\nget_property path\" >/tmp/mplayerfifo");
	filename=oneLiner("tail -n4 /tmp/mplayerout |sed -n '4p'|awk -F= '{print $2}'");

	if(oldfile!=NULL)
		{
			if(strcmp(filename,oldfile)==0)
				{
					if(updated==false)
						{
							album=oneLiner("tail -n4 /tmp/mplayerout |sed -n '1p'|awk -F= '{print $2}'|sed -n 's/^.\\(.*\\).$/\\1/p'");
							title=oneLiner("tail -n4 /tmp/mplayerout |sed -n '2p'|awk -F= '{print $2}'|sed -n 's/^.\\(.*\\).$/\\1/p'");
							artist=oneLiner("tail -n4 /tmp/mplayerout |sed -n '3p'|awk -F= '{print $2}'|sed -n 's/^.\\(.*\\).$/\\1/p'");
							asprintf(&all,"Album:  %s\nArtist: %s\nSong:   %s\n",album,artist,title);

							nowPlaying->CTK_updateText(all,false,false);
							updated=true;
							free(title);
							free(album);
							free(artist);
							free(all);
							system(":>/tmp/mplayerout");

							asprintf(&command,"%s",filename);
							asprintf(&jpeg,"%s/folder.jpg",dirname(command));
							albumArt->CTK_newFBImage(chooserWidth+6,artSY,artHite*2,artHite,jpeg,false);
							free(command);
							free(jpeg);
						}

					free(filename);
					system(":>/tmp/mplayerout");
						return;
				}
		}
	oldfile=strdup(filename);
	updated=false;
}

void selectSongCB(void *inst,void *userdata)
{
	char *command;
	CTK_cursesListBoxClass	*sl=static_cast<CTK_cursesListBoxClass*>(inst);

	if(sl->listItems.size()==0)
		return;

				asprintf(&command,"echo -e \"p\npausing_keep_force loadlist '%s'\" >/tmp/mplayerfifo",playLists->filePath.c_str());
				system(command);
				free(command);
				if((long)sl->listItems[sl->listItemNumber]->userData!=0)
					{
						asprintf(&command,"echo -e \"pausing_keep_force pt_step %i\np\" >/tmp/mplayerfifo",(long)sl->listItems[sl->listItemNumber]->userData);
						system(command);
						free(command);
					}
				else
					system("echo \"p\" >/tmp/mplayerfifo");
				playing=true;
				paused=false;
}

void controlsCB(void *inst,void *userdata)
{
	CTK_cursesFBImageClass	*bc=static_cast<CTK_cursesFBImageClass*>(inst);
	long					ud=(long)userdata;
	char					*command;

	switch(ud)
		{
			case START:
				asprintf(&command,"echo -e \"loadlist '%s'\" >/tmp/mplayerfifo",playLists->filePath.c_str());
				system(command);
				free(command);
				playing=true;
				paused=false;
				break;

			case PREVIOUS:
				system("echo -e \"pt_step -1\" >/tmp/mplayerfifo");
				break;

			case PLAY:
				if(playing==true)
					{
						system("echo 'p' >/tmp/mplayerfifo");
						paused=!paused;
					}
				if(playing==true)
					return;
				break;

			case STOP:
				if(playing==false)
					return;
				system("echo 'q' >/tmp/mplayerfifo");
				playing=false;
				paused=false;
				break;

			case PAUSE:
				if(playing==false)
					return;
				system("echo 'p' >/tmp/mplayerfifo");
				paused=!paused;
				break;

			case END:
				asprintf(&command,"echo -e \"p\npausing_keep_force loadlist '%s'\" >/tmp/mplayerfifo",playLists->filePath.c_str());
				system(command);
				free(command);
				asprintf(&command,"echo -e \"pausing_keep_force pt_step %i\np\" >/tmp/mplayerfifo",songs.size()-1);
				system(command);
				free(command);
				playing=true;
				paused=false;
				break;

			case NEXT:
				system("echo -e \"pt_step 1\" >/tmp/mplayerfifo");
				break;

			case QUIT:
				mainApp->runEventLoop=false;
				doQuit=true;
				break;
		}
}

void playListsCB(void *inst,void *userdata)
{
	CTK_cursesChooserClass	*ch=static_cast<CTK_cursesChooserClass*>(inst);
	long					ud=(long)userdata;
	FILE					*fd=NULL;
	char					buffer[PATH_MAX];
	char					buffer2[PATH_MAX];

	if((ch->files->data[ch->lb->listItemNumber].fileType==FOLDERTYPE) || (ch->files->data[ch->lb->listItemNumber].fileType==FILELINKTYPE))
		return;
	folder=ch->folderPath.c_str();
	for(int j=0;j<songs.size();j++)
		free(songs[j]);
	songs.clear();
	songList->CTK_clearList();

	//fprintf(stderr,"Playlist '%s' chosen. file=%s type files=%i\n",ch->filePath.c_str(),ch->fileName.c_str(),ch->files->data[ch->lb->listItemNumber].fileType);
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

	sprintf(buffer,"echo -e \"loadlist '%s'\" >/tmp/mplayerfifo",ch->filePath.c_str());
	playing=true;
	system(buffer);
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
	buttonsSY=mainApp->maxRows-1;
	songsWidth=mainApp->maxCols-chooserWidth-7;
	songsHite=(chooserHite+4)/2;
	artSY=songsHite+4;
	artHite=songsHite;
	asprintf(&resources,"%s/MusicPlayer",DATADIR);
	fprintf(stderr,"resources=%s\n",resources);
	system("rm /tmp/mplayerfifo");
	system("mkfifo /tmp/mplayerfifo");

//start mplayer
	system("mplayer -quiet -slave -input file=/tmp/mplayerfifo -idle >/tmp/mplayerout 2>/dev/null &");

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
	//mainApp->colours.listBoxType=NOBOX;
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
//	button=mainApp->CTK_addNewButton(mainApp->utils->CTK_getGadgetPosX(midWay-(dialogWidth/2),dialogWidth,1,18,0),buttonsSY,12,1,"   Quit   ");
//	button->CTK_setSelectCB(buttonsCB,NULL);
//
	sprintf(imagepath,"%s/MusicPlayer/start.png",DATADIR);
	image=mainApp->CTK_addNewFBImage(mainApp->utils->CTK_getGadgetPosX(midWay-(dialogWidth/2),dialogWidth,CONTROLCNT,4,0),controlsSY,4,4,imagepath);
	image->CTK_setSelectCB(controlsCB,(void*)START);
	sprintf(imagepath,"%s/MusicPlayer/prev.png",DATADIR);
	image=mainApp->CTK_addNewFBImage(mainApp->utils->CTK_getGadgetPosX(midWay-(dialogWidth/2),dialogWidth,CONTROLCNT,4,1),controlsSY,4,4,imagepath);
	image->CTK_setSelectCB(controlsCB,(void*)PREVIOUS);
	sprintf(imagepath,"%s/MusicPlayer/play.png",DATADIR);
	image=mainApp->CTK_addNewFBImage(mainApp->utils->CTK_getGadgetPosX(midWay-(dialogWidth/2),dialogWidth,CONTROLCNT,4,2),controlsSY,4,4,imagepath);
	image->CTK_setSelectCB(controlsCB,(void*)PLAY);
	sprintf(imagepath,"%s/MusicPlayer/stop.png",DATADIR);
	image=mainApp->CTK_addNewFBImage(mainApp->utils->CTK_getGadgetPosX(midWay-(dialogWidth/2),dialogWidth,CONTROLCNT,4,3),controlsSY,4,4,imagepath);
	image->CTK_setSelectCB(controlsCB,(void*)STOP);
	sprintf(imagepath,"%s/MusicPlayer/pause.png",DATADIR);
	image=mainApp->CTK_addNewFBImage(mainApp->utils->CTK_getGadgetPosX(midWay-(dialogWidth/2),dialogWidth,CONTROLCNT,4,4),controlsSY,4,4,imagepath);
	image->CTK_setSelectCB(controlsCB,(void*)PAUSE);
	sprintf(imagepath,"%s/MusicPlayer/next.png",DATADIR);
	image=mainApp->CTK_addNewFBImage(mainApp->utils->CTK_getGadgetPosX(midWay-(dialogWidth/2),dialogWidth,CONTROLCNT,4,5),controlsSY,4,4,imagepath);
	image->CTK_setSelectCB(controlsCB,(void*)NEXT);
	sprintf(imagepath,"%s/MusicPlayer/end.png",DATADIR);
	image=mainApp->CTK_addNewFBImage(mainApp->utils->CTK_getGadgetPosX(midWay-(dialogWidth/2),dialogWidth,CONTROLCNT,4,6),controlsSY,4,4,imagepath);
	image->CTK_setSelectCB(controlsCB,(void*)END);
	sprintf(imagepath,"%s/MusicPlayer/quit.png",DATADIR);
	image=mainApp->CTK_addNewFBImage(mainApp->utils->CTK_getGadgetPosX(midWay-(dialogWidth/2),dialogWidth,CONTROLCNT,4,7),controlsSY,4,4,imagepath);
	image->CTK_setSelectCB(controlsCB,(void*)QUIT);

	albumArt=mainApp->CTK_addNewFBImage(chooserWidth+6,artSY,artHite,artHite,NULL,false);
	albumArt->CTK_setSelectable(false);

	mainApp->CTK_clearScreen();
	do
		{
			mainApp->CTK_mainEventLoop(-500,false);
			getMeta();
		}
	while (doQuit==false);
	system("echo 'q' >/tmp/mplayerfifo &");

	SETSHOWCURS;
	delete mainApp;
	free(resources);
	system("rm /tmp/mplayerfifo");
	for(int j=0;j<songs.size();j++)
		free(songs[j]);

	return(0);
}