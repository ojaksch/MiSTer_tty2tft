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

### Color definitions as used by the display driver:

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
