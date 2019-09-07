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
CTK_cursesFBImageClass		*image;

const char					*folder="./";
int							midWay;
char						*resources;
int							dialogWidth;
int							listHite;
int							controlsSY;
int							buttonsSY;
std::vector<char*>			songs;
bool						playing=false;
int							currentSong=0;
bool						paused=false;

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

void controlsCB(void *inst,void *userdata)
{
	CTK_cursesFBImageClass	*bc=static_cast<CTK_cursesFBImageClass*>(inst);
	long					ud=(long)userdata;
	char					*command;

	fprintf(stderr,"control %i\n",ud);
	switch(ud)
		{
			case FRONT:
				if(playing==true)
					system("echo 'q' >/tmp/mplayerfifo");

				currentSong--;
				if(currentSong<0)
					currentSong=0;

				asprintf(&command,"mplayer -slave -input file=/tmp/mplayerfifo '%s' &>/dev/null &",songs[currentSong]);
				system(command);
				free(command);
				playing=true;
				break;

			case BACKWARD:
				break;
			case START:
				if(playing==true)
					{
						system("echo 'p' >/tmp/mplayerfifo");
						paused=!paused;
					}
				if(playing==true)
					return;
				playing=true;
				if(currentSong<songs.size())
					{
						asprintf(&command,"mplayer -slave -input file=/tmp/mplayerfifo '%s' &>/dev/null &",songs[currentSong]);
						system(command);
						free(command);
					}
				else
					{
						playing=false;
						return;
					}
				break;
			case STOP:
				if(playing==false)
					return;
				system("echo 'q' >/tmp/mplayerfifo");
				playing=false;
				break;
			case PAUSE:
				if(playing==false)
					return;
				system("echo 'p' >/tmp/mplayerfifo");
				paused=!paused;
				break;
			case FOWARD:
				break;
			case BACK:
				if(playing==true)
					system("echo 'q' >/tmp/mplayerfifo");

				currentSong++;
				if(currentSong>songs.size()-1)
					currentSong=songs.size()-1;

				asprintf(&command,"mplayer -slave -input file=/tmp/mplayerfifo '%s' &>/dev/null &",songs[currentSong]);
				system(command);
				free(command);
				playing=true;
				break;
		}
}

void buttonsCB(void *inst,void *userdata)
{
	CTK_cursesButtonClass	*bc=static_cast<CTK_cursesButtonClass*>(inst);
	long					ud=(long)userdata;

	fprintf(stderr,"Button '%s' clicked.",bc->label);
	mainApp->runEventLoop=false;
}

void playListsCB(void *inst,void *userdata)
{
	CTK_cursesChooserClass	*ch=static_cast<CTK_cursesChooserClass*>(inst);
	long					ud=(long)userdata;
	FILE					*fd=NULL;
	char					buffer[PATH_MAX];
	char					buffer2[PATH_MAX];

	fprintf(stderr,"Playlist '%s' chosen.\n",ch->filePath.c_str());
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
							fprintf(stderr,"%s/%s\n",folder,buffer);
							sprintf(buffer2,"%s/%s",folder,buffer);
							songs.push_back(strdup(buffer2));
						}
				}
			fclose(fd);
		}

for(int j=0;j<songs.size();j++)
	fprintf(stderr,"%s\n",songs[j]);
}

int main(int argc, char **argv)
{
	int						c;
	int						option_index;
	CTK_cursesButtonClass	*button;
	char					imagepath[PATH_MAX];

	midWay=mainApp->maxCols/2;
	dialogWidth=mainApp->maxCols-4;
	listHite=mainApp->maxRows-8;
	controlsSY=mainApp->maxRows-5;
	buttonsSY=mainApp->maxRows-1;

	asprintf(&resources,"%s/MusicPlayer",DATADIR);
	//fprintf(stderr,"resources=%s\n",resources);
	system("mkfifo /tmp/mplayerfifo");

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
	mainApp->colours.listBoxType=NOBOX;
	mainApp->colours.boxType=NOBOX;
	mainApp->colours.windowBackCol=BACK_BLACK;
	mainApp->colours.backCol=BACK_BLACK;
	mainApp->colours.foreCol=FORE_WHITE;

	playLists=new CTK_cursesChooserClass(mainApp,2,2,mainApp->maxCols-2,listHite);
	playLists->CTK_setShowFileTypes(".m3u;");
	playLists->CTK_setShowTypes(ANYTYPE);
	playLists->CTK_setShowHidden(false);
	playLists->CTK_selectFolder(mainApp,folder);
	playLists->CTK_setSelectCB(playListsCB,NULL);
	mainApp->CTK_addChooserBox(playLists);
//CTK_getGadgetPosX((selectapp->maxCols/2)-(dialogwidth/2),dialogwidth,maxbtns,11,btnnum++)

	button=mainApp->CTK_addNewButton(mainApp->utils->CTK_getGadgetPosX(midWay-(dialogWidth/2),dialogWidth,1,18,0),buttonsSY,12,1,"   Quit   ");
	button->CTK_setSelectCB(buttonsCB,NULL);

	sprintf(imagepath,"%s/MusicPlayer/front.png",DATADIR);
	image=mainApp->CTK_addNewFBImage(mainApp->utils->CTK_getGadgetPosX(midWay-(dialogWidth/2),dialogWidth,7,4,0),controlsSY,4,4,imagepath,false);
	image->CTK_setSelectCB(controlsCB,(void*)FRONT);
	sprintf(imagepath,"%s/MusicPlayer/backward.png",DATADIR);
	image=mainApp->CTK_addNewFBImage(mainApp->utils->CTK_getGadgetPosX(midWay-(dialogWidth/2),dialogWidth,7,4,1),controlsSY,4,4,imagepath,false);
	image->CTK_setSelectCB(controlsCB,(void*)BACKWARD);
	sprintf(imagepath,"%s/MusicPlayer/start.png",DATADIR);
	image=mainApp->CTK_addNewFBImage(mainApp->utils->CTK_getGadgetPosX(midWay-(dialogWidth/2),dialogWidth,7,4,2),controlsSY,4,4,imagepath,false);
	image->CTK_setSelectCB(controlsCB,(void*)START);
	sprintf(imagepath,"%s/MusicPlayer/stop.png",DATADIR);
	image=mainApp->CTK_addNewFBImage(mainApp->utils->CTK_getGadgetPosX(midWay-(dialogWidth/2),dialogWidth,7,4,3),controlsSY,4,4,imagepath,false);
	image->CTK_setSelectCB(controlsCB,(void*)STOP);
	sprintf(imagepath,"%s/MusicPlayer/pause.png",DATADIR);
	image=mainApp->CTK_addNewFBImage(mainApp->utils->CTK_getGadgetPosX(midWay-(dialogWidth/2),dialogWidth,7,4,4),controlsSY,4,4,imagepath,false);
	image->CTK_setSelectCB(controlsCB,(void*)PAUSE);
	sprintf(imagepath,"%s/MusicPlayer/forward.png",DATADIR);
	image=mainApp->CTK_addNewFBImage(mainApp->utils->CTK_getGadgetPosX(midWay-(dialogWidth/2),dialogWidth,7,4,5),controlsSY,4,4,imagepath,false);
	image->CTK_setSelectCB(controlsCB,(void*)FOWARD);
	sprintf(imagepath,"%s/MusicPlayer/back.png",DATADIR);
	image=mainApp->CTK_addNewFBImage(mainApp->utils->CTK_getGadgetPosX(midWay-(dialogWidth/2),dialogWidth,7,4,6),controlsSY,4,4,imagepath,false);
	image->CTK_setSelectCB(controlsCB,(void*)BACK);

	//chdir(folder);
	mainApp->CTK_mainEventLoop();

	SETSHOWCURS;
	delete mainApp;
	free(resources);
	system("rm /tmp/mplayerfifo");
	for(int j=0;j<songs.size();j++)
		free(songs[j]);

	return(0);
}