
- The screensaver  
This is a simple watch that is displayed and wanders around on the screen.
The time _when_ it is activated and changes it's positions is set by parameters shown below.
We are using  `cron` for that which might be misleading (for SCREENSAVER_START) the first time as
this works works a little differently you might think. The timer doesn not start exactly after
the defined "after X minutes" set, but means "start after X minutes beginning from xx:00 o'clock.  
For example: "after 2 Minutes" is set, it will start every 2 minutes, i.e. 08:02, 08:04, 08:06 and so on.  
Another example: "after 13 Minutes" is set, it will start every 13 minutes, i.e. 08:13, 08:26, 08:38 ...
```
SCREENSAVER="yes"	# yes activates the screensaver
SCREENSAVER_START="2"	# activate after how many minutes
SCREENSAVER_IVAL="10"	# move position after X seconds
```
