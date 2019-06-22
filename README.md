Convenience wrappers for Curses toolkit library.

To get the reults of the dialogs into a bash varable Use e.g:
{ result=$(CursesDialogs/app/ctkopenfile 2>&1 >&3 3>&-); } 3>&1
echo $result

Select a file:
ctkopenfile
Usage: ctkopenfile [OPTION]
 -w, --window-name	Window Name
 -s, --start-folder	Start Folder
 -v, --version		Version
 -h, -?, --help		Help

Report bugs to keithdhedger@gmail.com

Example:
To get the reults of the dialog into a bash varable Use:
{ result=$(ctkopenfile -w MyWindow -s /etc 2>&1 >&3 3>&-); } 3>&1
echo $result

Save a file:
ctksavefile
Usage: ctksavefile [OPTION]
 -w, --window-name	Window Name
 -s, --start-folder	Start Folder
 -n, --default-name	Default file name
 -v, --version		Version
 -h, -?, --help		Help

Report bugs to keithdhedger@gmail.com

Example:
To get the reults of the dialog into a bash varable Use:
{ result=$(ctksavefile -w MyWindow -s /etc -n SaveFileName 2>&1 >&3 3>&-); } 3>&1
echo $result

Get input:
ctkinput
Usage: ctkinput [OPTION]
 -w, --window-name	Window Name
 -b, --body		Body text
 -t, --title		Dialog title
 -i, --input		Default input
 -c, --cancel		Show cancel
 -v, --version		Version
 -h, -?, --help		Help

Report bugs to keithdhedger@gmail.com

Example:
To get the reults of the dialog into a bash varable Use:
{ result=$(ctkinput -w MyWindow -b 'Say somthing?' -t 'Input ...' -i 'Some input ...' -c  2>&1 >&3 3>&-); } 3>&1
echo $result
The return code from the app also reflects the button seelected like so:
ctkinput -w MyWindow -b 'Say somthing?' -t 'Input ...' -i 'Some input ...' -c 2>/dev/null;echo "Button pressed $?"

Simple query:
ctkquery
Usage: ctkquery [OPTION]
 -w, --window-name	Window Name
 -q, --query		Query text
 -t, --title		Dialog title
 -b, --buttons		Buttons
 -v, --version		Version
 -h, -?, --help		Help

Report bugs to keithdhedger@gmail.com

Example:
The return code from the app reflects the button selected like so:
ctkquery -w MyWindow -q 'Do What?' -t 'Question?' -b 7 2>/dev/null;echo "Button pressed $?"

