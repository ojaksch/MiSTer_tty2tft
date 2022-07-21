Ready to rumble?



# Hardware

You need an [ESP32 D1 R32 and an ILI9341 (320x200) 8-bit bus display](https://github.com/ojaksch/MiSTer_tty2tft/blob/main/pictures/d1r32-ili9341.jpg). 
Other 8-bit bus displays should also work (according to librarys documentation), ~~-but are untested~~-  
- 2022-06: ILI9488 (480x320) succesfully tested by myself
- 2022-06: ILI9486 (480x320) succesfully tested by Salamantecas
- 2022-06: HX8347D (320x200) succesfully tested, many many thanks to Salamantecas!  

The ESP needs a [small modifictaion consisting of three cables](https://github.com/ojaksch/MiSTer_tty2tft/blob/main/pictures/d1r32-mod.jpg). 
([Source](https://github.com/Bodmer/TFT_eSPI#8-bit-parallel-support))  

**Notices:**
- It turned out that a 480x320 display looks great with pictures, but is somewhat too "slow" to play MJPEG videos really fluently. This is stated by the facts 
that MJPEG isn't a videostream...it's a series of single jpg's in one file that are played one after the other and are always shown at 25fps. Keep in mind that a 
480x320 display has a 2.5x resolution of a 320x200 one, so it also has to transfer 2.5 times of data - all this through very limited SD access times.
As a result, my personal recommendation is a 320x200 for pictures **and** videos - or a 480x320 for pictures (and videos if a bit stuttering is acceptable).

- Keep in mind that the ESP32 is using WiFi@2.4 GHz only (if enabled).

That's all!

---

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
for ILI9488: `wget https://raw.githubusercontent.com/ojaksch/tty2tft/main/installer.sh -O - | bash -s -- TEST-ILI9488`  
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

---

# Media

JPG pictures must be 320x240 (ILI9341, HX8347D) or 480x320 (ILI9486) pixels in size and should be saved as non-progressive with around 60 percent in compression to achieve 
a good compromise between quality and filesize (=loading time). A filesize around 20kb is fine, the 
smaller, the better, but a filesize around 35kb is ok, too. That's all.

Same goes for the MJPEG videos. Example scripts will follow.

When a core is setting it's MAME setname, tty2tft is working as follows:  
At first tty2tft is looking for a file of the same (set-) name. If one is found tty2tft looks if there's 
also a video of that name and plays the video first (if activated), then the picture as a "photo frame". 
When a file is found tty2tft also looks if there are alternative names, like "qbert_alt1" (up to _alt5) and 
dices out which to play/show.
A video is played only if a) this function is activated and b) there's also a picture of that name.

If a file can't be found tty2tft tries to shorten the filename, in hope that a file in a shorter form can be found then. 
There *are* cores/pictures for the same game but with a different name and ROM, for example *jackal*, *jackalj* and *jackalr*. 
Another example: The is a hypothetical core/game named qbertfixed without a picture for *that* name and tty2tft goes on to shorten the 
name to qbertfixe, then qbertfix, then qbertfi, then qbertf, then qbert and voila! there it is. Got it?

You can access your device by FTP using a FTP program like FileZilla (recommended!) to organise your SD's content. Username and passwort is "esp32" (without quotes).
Limit the "parallel connections" to "1" for this connection.

---

# MJPEG

## How to create a MJPEG?

Simple. In principle. Really.  
Use a linear video editor like [Avidemux](http://avidemux.sourceforge.net), get your favorite video and cut it down to the needed length.
While beeing there you could add effects etc.

Get and install the famous [ffmpeg](https://ffmpeg.org/) package.
I have found and used the following two tutorials to get an one-liner of code, which is doing all tasks in one go (fade-in at start, rescale, fade-out at end, encode).


[Add Fade In and Fade Out Effects With ffmpeg](https://dev.to/dak425/add-fade-in-and-fade-out-effects-with-ffmpeg-2bj7)

[Resizing videos with ffmpeg](https://superuser.com/questions/547296/resizing-videos-with-ffmpeg-avconv-to-fit-into-static-sized-player/1136305#1136305)

- Pillarbox or letterbox to fit  
```
ffmpeg -y -t 00:10 -i "INPUT_FILE" -an -pix_fmt yuvj420p -vf "fps=13,tpad=stop_duration=1:color=black,fade=t=out:st=9:d=1,scale=320:240:flags=lanczos:force_original_aspect_ratio=decrease,pad=320:240:-1:-1:color=black" -q:v 9 "OUTPUT_FILE.mjpeg"
```

- Same as above but without upscaling  
```
ffmpeg -y -t 00:10 -i "INPUT_FILE" -an -pix_fmt yuvj420p -vf "fps=13,tpad=stop_duration=1:color=black,fade=t=out:st=9:d=1,scale='min(320,iw)':min'(240,ih)':flags=lanczos:force_original_aspect_ratio=decrease,pad=320:240:-1:-1:color=black" -q:v 9 "OUTPUT_FILE.mjpeg"
```

Parameters used here (also see [ffmpeg's documentation page](https://ffmpeg.org/ffmpeg-all.html):

- -t *duration* (limit the duration of data read from the input file.)
- -i *INPUT_FILE* (the source video)
- -an (drop audio)
- -pix_fmt *format* (pixel format)
- -vf *filtergraph* (filtergraph specified by filtergraph and use it to filter the stream)
  - fps=*fps* (frames per second)
  - tpad=*mode* (Temporarily pad video frames - Here: *duration* of the stop delay (*1*), *color* black)
  - fade=*fade effect* (Here: *t*ype fade out, *st*art after 9 seconds, *d*uration 1 second )
  - scale*scale options* (Here: *scale* to 320x240 with *flags* using the *lanczos* rescaling algorithm, *force_original_aspect_ratio=decrease* (decreasing output video width or height if necessary to keep the original aspect ratio) and *pad*ding to WIDTHxHEIGHT2XxY and *color* black
- -q *stream_specifier* *quality* (fixed quality scale (VBR))
- OUTPUT_FILE

---

This paragraph is WiP
# The INI files

---
This paragraph is WiP
# The screensaver
This is a simple watch that is displayed and wanders around on the screen.
The time _when_ it is activated and changes it's positions is set by parameters shown below.
We are using  `cron` for that which might be misleading (for SCREENSAVER_START) the first time as
this works works a little differently you might think. The timer doesn not start exactly after
the defined "after X minutes" set, but means "start after X minutes beginning from xx:00 o'clock.
For example: "after 2 Minutes" is set, it will start every 2 minutes, i.e. 08:02, 08:04, 08:06 and so on.
Another example: "after 13 Minutes" is set, it will start every 13 minutes, i.e. 08:13, 08:26, 08:38 ...
```
SCREENSAVER="yes"       # yes activates the screensaver
SCREENSAVER_START="2"   # activate after how many minutes
SCREENSAVER_IVAL="10"   # move position after X seconds
```

---

# Commands

If you tend to extend/change/rewrite tty2tft's Daemon (the program which is driving tty2tft), you have a set of commands you can use.

| Commands                        | Meaning       |
| ------------------------------- |-------------- |
| CMDCLS                          | Black out screen |
| CMDCRED                         | Fill screen red (just for testing) |
| CMDDON                          | Turn on screen (ILI9341 only) |
| CMDDOFF                         | Turn off screen (ILI9341 only) |
| CMDDINVON                       | Invert screen on (ILI9341 only) |
| CMDDINVOFF                      | Invert screen off (ILI9341 only) |
| CMDRESET                        | Restart ESP |
| CMDSNAM                         | Show picture's name |
| CMDROT,PARAM                    | Rotate screen (0=none/Portrait, 1=90°/Landscape, 2=180°, 3=270°) |
| CMDDUPD                         | Refresh screen |
| CMDDELETE                       | Delete actual picture/video from SD |
| CMDUPLOAD,URL                   | Upload file from <URL> |
| CMDCOR,MENU                     | Shows MiSTer Menu |
| CMDCOR,CORENAME                 | Shows picture/video from SD (filename) |
| CMDSETTIME,TSTAMP               | Set time and date (timestamp) |
| CMDVIDEOPLAY,PARAM              | Play videos or not (yes/no/may) |
| CMDSAVER,PARAM,PARAM            | Screensaver after x mins. (1-59), change after y secs, (1-59) |
| CMDSAVEROPTS,PARAM,PARAM,PARAM  | Screensaver options yes/no (yes=12h, no=24h), (Text screensaver), (Graphical screensaver) |
| CMDHWINF                        | Send hardware info, BuildVersion and MAC address |
| CMDTXT,PARAM,PARAM,PARAM,PARAM,PARAM | Draw text with font type, text color, background color, x pos, y pos, text

If a picture/video can't be found on SD and WiFi isn't available, show a "...not found..." picture.
But is WiFi available load a **picture** (but not a video) from archive.org and save it on SD.
An upload with CMDUPLOAD saves the picture/video on SD.

### Color definitions used by the display driver:

| Color       | Hex    | Dec   | RGB           |
| ------------|--------|------:|-------------: |
| BLACK       | 0x0000 |     0 |   0,   0,   0 |
| NAVY        | 0x000F |    15 |   0,   0, 123 |
| DARKGREEN   | 0x03E0 |   992 |   0, 125,   0 |
| DARKCYAN    | 0x03EF |  1007 |   0, 125, 123 |
| MAROON      | 0x7800 | 30720 | 123,   0,   0 |
| PURPLE      | 0x780F | 30735 | 123,   0, 123 |
| OLIVE       | 0x7BE0 | 30735 | 123, 125,   0 |
| LIGHTGREY   | 0xC618 | 50712 | 198, 195, 198 |
| DARKGREY    | 0x7BEF | 31727 | 123, 125, 123 |
| BLUE        | 0x001F |    31 |   0,   0, 255 |
| GREEN       | 0x07E0 |  2016 |   0, 255,   0 |
| CYAN        | 0x07FF |  2047 |   0, 255, 255 |
| RED         | 0xF800 | 63488 | 255,   0,   0 |
| MAGENTA     | 0xF81F | 63519 | 255,   0, 255 |
| YELLOW      | 0xFFE0 | 65504 | 255, 255,   0 |
| WHITE       | 0xFFFF | 65535 | 255, 255, 255 |
| ORANGE      | 0xFD20 | 64800 | 255, 165,   0 |
| GREENYELLOW | 0xAFE5 | 45029 | 173, 255,  41 |
| PINK        | 0xFC18 | 64536 | 255, 130, 198 |
