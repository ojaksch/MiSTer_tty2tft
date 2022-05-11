Ready to rumble?

- If not already done get the archive from [here](https://drive.google.com/drive/folders/1KByTczqLIjfHOb9TDX-hi4kdtbvZnYc9?usp=sharing) and extract the content to your SD card. Insert SD card into your display.

- Open a ssh-shell to your MiSTer or open a console at MiSTer and logon.

- Type in the following line;  
`wget https://raw.githubusercontent.com/ojaksch/tty2tft/main/installer.sh -O - | bash -s -- IDENTIFY`  
This gives you the MAC address of your ESP device. Tell me that MAC address. Keep the session/console open.

- I'll create a "repository" especially for you depending on your MAC and will give you an OK then.

- After my OK type in  
`wget https://raw.githubusercontent.com/ojaksch/tty2tft/main/installer.sh -O - | bash`  
which will download and flash the ESP firmware. You should see a welcome screen after the reboot of the ESP.

- Type in  
`wget https://raw.githubusercontent.com/ojaksch/tty2tft/main/update_tty2tft.sh -O - | bash`  
which downloads and setup the software (init and daemon).


Voila, tty2tft is alive :)
