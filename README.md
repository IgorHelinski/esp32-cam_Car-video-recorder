# Esp32cam Car video/timelapse camera
This is a simple software for esp32cam microprocessor to record video or timelapse. Can be used anywhere: security, car dashcam, whatever you want. Also feel free to fork this code for your own projects.

# How to use
1) You will obviously need an esp32cam and an FTDI for it. You also need an sd card formatted to FAT32
2) Either download 2 files in [src folder](/src) or clone whole repo using `git clone https://github.com/IgorHelinski/esp32-cam_Car-video-recorder`
3) In src.ino file you can change things like: delay between pictures, your esp32cam model, and all camera settings
4) Then just flash the sketch to your board and start recording!

# Turning images to video
After you are done recording you will need to convert all pictures taken to a video.
<br>
I recommend using FFMPEG wich you can download here: https://ffmpeg.org/download.html
<br>
To turn frames into video use following command in the folder of your images:
<br>
```ffmpeg -r 60 -start_number 23 -i picture%d.jpg -vcodec libx264 -crf 10 -pix_fmt yuv420p video.mp4```
<br>
`-r` framerate of a video
<br>
`-start_number` this is a start number of the picture you want to start the video for example: picture23.jpg
<br>
`-i` this is input images, %d means it will iterate through all image numbers there

 
