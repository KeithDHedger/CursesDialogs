Convenience wrappers for Curses toolkit library.

Select a file:<br>
ctkopenfile<br>
Usage: ctkopenfile [OPTION]<br>
 -w, --window-name	Window Name<br>
 -s, --start-folder	Start Folder<br>
 -v, --version		Version<br>
 -h, -?, --help		Help<br>
<br>
Report bugs to keithdhedger@gmail.com<br>
<br>
Example:<br>
To get the reults of the dialog into a bash varable Use:<br>
{ result=$(ctkopenfile -w MyWindow -s /etc 2>&1 >&3 3>&-); } 3>&1<br>
echo $result<br>
![Alt text](screenshots/openfile.png?raw=true "ctkopenfile")


Save a file:<br>
ctksavefile<br>
Usage: ctksavefile [OPTION]<br>
 -w, --window-name	Window Name<br>
 -s, --start-folder	Start Folder<br>
 -n, --default-name	Default file name<br>
 -v, --version		Version<br>
 -h, -?, --help		Help<br>
<br>
Report bugs to keithdhedger@gmail.com<br>
<br>
Example:<br>
To get the reults of the dialog into a bash varable Use:<br>
{ result=$(ctksavefile -w MyWindow -s /etc -n SaveFileName 2>&1 >&3 3>&-); } 3>&1<br>
echo $result<br>
![Alt text](screenshots/savefile.png?raw=true "ctksavefile")


Get input:<br>
ctkinput<br>
Usage: ctkinput [OPTION]<br>
 -w, --window-name	Window Name<br>
 -b, --body		Body text<br>
 -t, --title		Dialog title<br>
 -i, --input		Default input<br>
 -c, --cancel		Show cancel<br>
 -v, --version		Version<br>
 -h, -?, --help		Help<br>
<br>
Report bugs to keithdhedger@gmail.com<br>
<br>
Example:<br>
To get the reults of the dialog into a bash varable Use:<br>
{ result=$(ctkinput -w MyWindow -b 'Say somthing?' -t 'Input ...' -i 'Some input ...' -c  2>&1 >&3 3>&-); } 3>&1<br>
echo $result<br>
The return code from the app also reflects the button seelected like so:<br>
ctkinput -w MyWindow -b 'Say somthing?' -t 'Input ...' -i 'Some input ...' -c 2>/dev/null;echo "Button pressed $?"<br>
![Alt text](screenshots/input.png?raw=true "ctkinput")


Simple query:<br>
ctkquery<br>
Usage: ctkquery [OPTION]<br>
 -w, --window-name	Window Name<br>
 -q, --query		Query text<br>
 -t, --title		Dialog title<br>
 -b, --buttons		Buttons<br>
 -v, --version		Version<br>
 -h, -?, --help		Help<br>
<br>
Report bugs to keithdhedger@gmail.com<br>

Example:<br>
The return code from the app reflects the button selected like so:<br>
ctkquery -w MyWindow -q 'Do What?' -t 'Question?' -b 7 2>/dev/null;echo "Button pressed $?"<br>
![Alt text](screenshots/query.png?raw=true "ctkquery")
