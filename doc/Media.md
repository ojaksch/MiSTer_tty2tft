JPG pictures must be 320x240 pixels in size and should be saved with around 60 percent in compression to achieve 
a good compromise between quality and filesize (=loading time). A filesize around 20kb is fine, the 
smaller, the better, but a filesize around 35kb is ok, too. That's all.


When a core is setting it's MAME setname, tty2tft is working as follows:  
At first tty2tft is looking for a file of the same (set-) name. If one is found tty2tft looks if there's 
also a video of that name (if activated) and plays the video first, then the picture as a "photo frame". 
When a file is found tty2tft also looks if there are alternative names, like "qbert_alt1" (up to _alt5) and 
dices out which to play/show.
A video is played only if a) this function is activated and b) there's also a picture of that name.

If a file can't be found tty2tft tries to shorten the filename, in hope that a file in a shorter form can be found then. 
There *are* cores/pictures for the same game but with a different name and ROM, for example *jackal*, *jackalj* and *jackalr*. 
Another example: The is a hypothetical core/game named qbertfixed without a picture for *that* name and tty2tft goes on to shorten the 
name to qbertfixe, then qbertfix, then qbertfi, then qbertf, then qbert and voila! there it is. Got it?
