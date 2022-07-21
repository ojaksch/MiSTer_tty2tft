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
