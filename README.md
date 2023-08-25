# esp32-cam_Car-video-recorder

To turn frames into video use following FFMPEG command:
ffmpeg -r 60 -start_number 23 -i picture%d.jpg -vcodec libx264 -crf 10 -pix_fmt yuv420p video.mp4

-r - framerate

 
