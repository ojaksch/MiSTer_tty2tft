# How to create a MJPEG?

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
