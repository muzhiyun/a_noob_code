One day, my client complained that they shot a video on the Mi 11 using the built-in camera function of WeChat, but the hardware decoder displayed the wrong color when playing the video. When using software codecs and other platforms, the colors are normal. When testing with PC, other players show normal colors except potplayer showing the same wrong colors.

I get a sample file. The matrix coefficients displayed as YCgCo in mediainfo caught my attention, I started to wonder if the video was declaring the wrong parameters, I noticed that after fixing the player to BT.709, the displayed colors returned to normal. When I turned off YCgCo in the potplayer tab, the color also returned to normal, my colleague assisted to find the data of two similar pixels from the data stream of this and another normal file, and passed YUV2RGB, YCgCo2RGB According to the calculation, he confirmed that this video should be in the YUV format of BT.709, not the declared YCgCo. Thanks for his work, but other than that, I would like to test this conjecture in a simpler and more intuitive way, such as correcting YCgCo to BT.709 by modifying the metadata of this video. As we all know, H.264 uses sps and pps frames in the header to announce basic information, and uses NAL to store data. However, when I face the data after opening the video file with hex editor, how should I confirm which byte stores the matrix? coefficient?

I searched for a lot of software on Google that claims to modify exif, HDR metadata, MP4 info, and frustratingly, until now, I still haven't found a software that can modify the matrix_coefficients in AVC.

Finally, I started downloading and started analyzing the source code of mediainfo, thanks to the great open source project, I chose MediaInfo_CLI_GNU as the test object. mediainfo relies on mediainfoLib and Zenlib, and it is easy to trace that Zenlib provides a series of Get1, Get2 and other interfaces. mediainfoLib uses these interfaces to obtain file information, and uses BS to place a part of the file content. The method of obtaining various parameters looks interesting. The acquisition of each information corresponds to a section of BS+BS_offest. I finally made a patch file, this patch Helps to print out the hex value of matrix_coefficients in the file, which allows me to modify YCgCo to BT.709 by only modifying 1Byte metadata. After modification, the video successfully plays the correct color.

When I described these experiments to my customer and told him that the wrong colors were because of the wrong sample files, he was amazed, "Why? If the sample files are wrong, why software decoding and players other than potplayer on PC display the correct color?"

In fact, I also wanted to know how other multimedia players do this, but at the time I couldn't explain it so I shrugged: "Maybe other platforms and software don't support YCgCo and so ignore the wrong information?", then, the customer Asked me for a real YCgCo video file, he wanted to see if that was the case.

So, I started to search on the Internet, God, what a strange SDR format, there are a lot of introductions about this format, but there is no YCgCo test file, until I searched for this issue: https:/ /bugs.chromium.org/p/chromium/issues/detail?id=1085434 , after downloading the demo.mp4.zip, I found that this file is the same as the sample file I got: also declares YCgCo, also appears Wrong color, I started to peruse this issue, when I read "purple sand", "this is the video in the original YCbCr (BT.709 or BT.601 matrix_coefficients), it was just marked as YCgCo matrix_coefficients", I realized that chromium and I had the same problem. The demo.mp4 used in this issue is also a BT.709 video file that falsely declares YCgCo.

Then I found the source of the demo.mp4.zip in the issue at https://github.com/mpv-player/mpv/issues/4340, it seems that mpv also encountered the same problem, and got the same conclusion . When someone in the mpv discussion asked "do we have any other samples of YCgCo", I saw mc4man answer "You could probably use ffmpeg to create various test files, possibly with parameters like -x264-params colormatrix=YCgCo" ,good idea!

After continuous testing, I found that using the following command can convert a BT.709 media file to YCgCo:

`ffmpeg -i BT709.mp4 -vf colorspace=bt709:ycgco -c:a copy -color_primaries bt709 -color_trc bt709 -colorspace ycgco YCgCo.mp4`

The converted file parameters still show YCgCo, but this is a real YCgCo, far more than just modifying the metadata. It can also be played correctly when the 'YCgCo optimization' option of potplayer is not turned off. Great ffmpeg and mediainfo. I sent the generated file to the client and told him that chromium and mpv had the same problem and he accepted the fact.

If you're also someone looking for YCgCo sample files, congratulations, this repo houses the only real YCgCo sample files currently available on Google.




some Tool：
https://res18h39.netlify.app/color
