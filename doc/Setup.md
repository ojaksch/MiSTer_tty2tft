Ready to rumble?



# Hardware

You need an [ESP32 D1 R32 and an ILI9341 8-bit bus display](https://github.com/ojaksch/MiSTer_tty2tft/blob/main/pictures/d1r32-ili9341.jpg). 
Other 8-bit bus displays should also work (according to librarys documentation), ~~-but are untested~~-  
- 2022-06: ILI9486 succesfully tested by Salamantecas
- 2022-06: HX8347D succesfully tested, many many thanks to Salamantecas!  

The ESP needs a [small modifictaion consisting of three cables](https://github.com/ojaksch/MiSTer_tty2tft/blob/main/pictures/d1r32-mod.jpg). 
([Source](https://github.com/Bodmer/TFT_eSPI#8-bit-parallel-support))  

Keep in mind that the ESP32 is using WiFi@2.4 GHz only.

That's all!



# Software

Follow the instructions below. Don't continue with the next step if something won't run, is showing errors or doing something unexpected/weired.
Call for help first.

- Copy the **content** of *SD_card_content* to a reliable FAT32 formatted SD card <32GB. While beeing there you can fill up the subdirectorys with your own content.
The file **wifi.txt** is a three-liner text file containing your WiFi credentials and an URL for online picture access. Only needed if you want to use WiFi, otherwise leave it as it is.
When editing, use an editor capable of Linux linebreaks, like notepad++ (Windows).

- Open a ssh-shell to your MiSTer or open a console at MiSTer and logon.

- We will do a quick first test to see that everythins is working / to ensure ESP32 and display are working together. Type in the following line:
for ILI9341: `wget https://raw.githubusercontent.com/ojaksch/tty2tft/main/installer.sh -O - | bash -s -- TEST-ILI9341`  
for ILI9486: `wget https://raw.githubusercontent.com/ojaksch/tty2tft/main/installer.sh -O - | bash -s -- TEST-ILI9486`  
This will flash a simple and looping graphics demo.   
If you see a message while flashing like
> WARNING: Failed to communicate with the flash chip...

then the ESP doesn't reset quick enough. Just rerun the line from above. 
If flash happened successfuly and demo works, just continue - otherwise stop here, check your hardware/cabling and call for help then.

- Type in the following line:  
`wget https://raw.githubusercontent.com/ojaksch/tty2tft/main/installer.sh -O - | bash -s -- IDENTIFY`  
This gives you the MAC address of your ESP device. Tell me that MAC address and your used display type (see **Hardware** above). Keep the session/console open.

- I'll create a "repository" especially for you depending on your MAC and will give you an OK then. This means STOP HERE AND WAIT. Take a break. Get a coffee.

- **After my OK** type in  
`wget https://raw.githubusercontent.com/ojaksch/tty2tft/main/installer.sh -O - | bash -s -- FORCE`  
which will download and flash the ESP firmware. You should see a [welcome screen](https://github.com/ojaksch/MiSTer_tty2tft/blob/main/pictures/000-arcade.jpg) after the reboot of the ESP.

- Type in  
`wget https://raw.githubusercontent.com/ojaksch/tty2tft/main/update_tty2tft.sh -O - | bash`  
which downloads and setup the software (init and daemon).


Voila, tty2tft is alive :)
