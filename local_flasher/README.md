This is the menu driven new flashing tool. It should be self-explanatory.
The script requires an already installed (not necessarily running or working) 
setup of tty2tft and perhaps a tty2tft-user.ini, eg. to set TTYDEV if necessary.

- If you want to flash a local file, it's name has to be MiSTer_tty2tft.bin 
(case sensitive!) and that file must already exist in the same directory of this script.
- After an online update ("latest stable") you could keep all files, also for different 
types of displays, for faster flashing: Just copy the needed firmware to 
MiSTer_tty2tft.bin and give it a go.
- You can run this script with a filename as parameter. Menu will be skipped then and
the given file will be flashed w/o further interaction.

Best would be to have this script in a subfolder of /media/fat/tty2tft/ to keep other 
directories clean. Then you could write a simple script for /media/fat/Scripts/ which 
runs /path/to/tty2tft_flasher.sh which ensures that you can tart it from MiSTers menu.
