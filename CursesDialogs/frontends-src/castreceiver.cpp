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
	asprintf(&fifoName,"/tmp/mplayerfifo%i",getpid());
	asprintf(&outName,"/tmp/mplayerout%i",getpid());
	sprintf(commandString,"mkfifo '%s'",fifoName);
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

	return(0);
}