Commands:
- CMDCLS			Black out screen
- CMDCRED			Fill screen red (just for testing)
- CMDDON			Turn on screen
- CMDDOFF			Turn off screen
- CMDDINVON			Invert screen on
- CMDDINVOFF			Invert screen off
- CMDRESET			Restart ESP
- CMDSNAM			Show picture's name
- CMDROT,<PARAM>		Rotate screen (0=none,1=90°,2=180°,1=90°,3=270°)
- CMDDUPD			Refresh screen
- CMDDELETE			Delete actual picture/video from SD
- CMDUPLOAD,<URL>		Upload file from <URL>
- CMDCOR,<PARAM>		Load picture from SD
- CMDCOR,<CORENAME>		Show picture/video
- CMDSETTIME,<TSTAMP>		Set time and date
- CMDVIDEOPLAY,<PARAM>		Play videos or not (yes/no/may)
- CMDSAVER,<PARAM>,<PARAM>	Screensaver after x mins. (1-59), change after y secs, (1-59)
- CMDHWINF			Send hardware info, BuildVersion and MAC address

If a picture/video can't be found on SD and WiFi isn't available, show a "...not found..." picture.
But is WiFi available load a **picture** (but not a video) from archive.org and save it on SD.
An upload with CMDUPLOAD saves the picture/video on SD.