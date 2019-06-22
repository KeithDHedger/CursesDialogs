Convenience wrappers for Curses toolkit library.

Select a file:<br>
ctkopenfile<br>
Usage: ctkopenfile [OPTION]<br>
 -w, --window-name	Window Name<br>
 -s, --start-folder	Start Folder<br>
 -v, --version		Version<br>
 -h, -?, --help		Help<br>
<br>
Example:<br>
To get the reults of the dialog into a bash varable Use:<br>
{ result=$(ctkopenfile -w MyWindow -s /etc 2>&1 >&3 3>&-); } 3>&1<br>
echo $result<br>
![Alt text](screenshots/openfile.png?raw=true "ctkopenfile")<br>


Save a file:<br>
ctksavefile<br>
Usage: ctksavefile [OPTION]<br>
 -w, --window-name	Window Name<br>
 -s, --start-folder	Start Folder<br>
 -n, --default-name	Default file name<br>
 -v, --version		Version<br>
 -h, -?, --help		Help<br>
<br>
Example:<br>
To get the reults of the dialog into a bash varable Use:<br>
{ result=$(ctksavefile -w MyWindow -s /etc -n SaveFileName 2>&1 >&3 3>&-); } 3>&1<br>
echo $result<br>
![Alt text](screenshots/savefile.png?raw=true "ctksavefile")<br>


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
Example:<br>
To get the reults of the dialog into a bash varable Use:<br>
{ result=$(ctkinput -w MyWindow -b 'Say somthing?' -t 'Input ...' -i 'Some input ...' -c  2>&1 >&3 3>&-); } 3>&1<br>
echo $result<br>
The return code from the app also reflects the button seelected like so:<br>
ctkinput -w MyWindow -b 'Say somthing?' -t 'Input ...' -i 'Some input ...' -c 2>/dev/null;echo "Button pressed $?"<br>
![Alt text](screenshots/input.png?raw=true "ctkinput")<br>


Simple query:<br>
ctkquery<br>
Usage: ctkquery [OPTION]<br>
 -w, --window-name	Window Name<br>
 -q, --query		Query text<br>
 -t, --title		Dialog title<br>
 -b, --buttons		Buttons<br>
 -v, --version		Version<br>
 -h, -?, --help		Help<br>
Example:<br>
The return code from the app reflects the button selected like so:<br>
ctkquery -w MyWindow -q 'Do What?' -t 'Question?' -b 7 2>/dev/null;echo "Button pressed $?"<br>
![Alt text](screenshots/query.png?raw=true "ctkquery")<br>



Report bugs etc to keithdhedger@gmail.com<br>

Please donate
If you have a PayPal account you can donate any amount you like by logging into your account and click the 'Send Money' tab, enter my email address and then send it
Thank you for helping to support Free software.
