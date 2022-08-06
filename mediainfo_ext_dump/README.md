某天，我的客户抱怨他们在小米11手机上，使用微信内置的摄像功能拍摄了一段视频，然而硬件解码器播放这段视频时却出现了错误的颜色。当使用软件解码器和其他平台，颜色正常。使用PC测试时，除了potplayer显示了同样错误的颜色之外，其他播放器也显示了正常的颜色。

我拿到一份示例文件。mediainfo中矩阵系数显示为YCgCo引起了我的注意，我开始怀疑这段视频是否声明了错误的参数，我注意到将播放器固定为BT.709后，显示的颜色恢复了正常。当我关闭potplayer选项卡中的YCgCo时，颜色也恢复了正常，我的同事协助从这份和另一份正常文件的数据流中找到了两个相似的像素点的数据，并通过YUV2RGB、YCgCo2RGB的演算，他确认：这段视频应当是BT.709的YUV格式，而并非宣告的YCgCo。感谢他的工作，但除此之外，我想喜欢用更简单直观的方式验证这一猜想，比如通过修改这段视频的metadata改正YCgCo为BT.709。众所周知，H.264使用头部的sps和pps帧来宣告基本信息，使用NAL存储数据，然而，当我面对hex editer打开视频后的数据，我应该如何确认哪一个字节存储的是矩阵系数？

我在Google上寻找了许多宣称可以修改exif、HDR metadata、MP4 info的软件，令人沮丧的是，直到现在，我仍然没有找到一款软件，能实现修改AVC中的matrix_coefficients。

最后，我开始下载并开始分析mediainfo的源代码，感谢伟大的开源项目，我选择了MediaInfo_CLI_GNU为测试对象。mediainfo依赖mediainfoLib和Zenlib，轻松地追踪到Zenlib提供了一系列Get1、Get2等接口。mediainfoLib使用这些接口来获取文件信息，使用BS放置文件内容的一部分，其中获取各个参数的方法看起来很有趣，每个信息的获取都与一段BS+BS_offest对应，我最终做了patch文件，这个patch帮助打印出matrix_coefficients在文件中的hex值，这让我可以通过只修改1Byte metadate，就可以将YCgCo修改为BT.709，修改之后该视频成功播放出了正确的颜色。

当我把这个实验描述给我的顾客，告知他错误的颜色是因为示例文件错误时，他感到很诧异，"为什么？如果是示例文件错误，为什么软件解码和PC上除potplayer之外的播放器都显示正确的颜色？"

事实上，我也想了解其他多媒体播放器是如何做到这点，但那时我无法解释只好耸耸肩："可能其他的平台和软件不支持YCgCo所以忽略了错误的信息？"，接着，客户向我索要一段真正的YCgCo文件，他想看确认事实是否真的如此。

所以，我开始接着在网上搜索，老天，这是一个多么奇怪的SDR格式，这里有一些关于此格式的介绍，却没有任何一份YCgCo的测试文件，直到我搜索到了这个issue：https://bugs.chromium.org/p/chromium/issues/detail?id=1085434 ，在下载了demo.mp4.zip后，我发现这份文件与我得到的示例文件相同：同样宣告了YCgCo，同样出现了错误的颜色，我开始完整的阅读这个issue，当我读到"紫色的沙子"、"这是最初 YCbCr（BT.709 或 BT.601 matrix_coefficients）中的视频，它只是被标记为 YCgCo matrix_coefficients"时，我意识到，chromium和我遇到了同一个问题。这个issue中使用的demo.mp4同样是一份错误宣告了YCgCo的BT.709视频文件。

接着我在 https://github.com/mpv-player/mpv/issues/4340 找到了issue中这份demo.mp4.zip的源头 ，看起来mpv也遇到了同样的问题，并且得到了相同的结论。当mpv的讨论中有人问到"我们是否还有其他的YCgCo样本时"，我看到mc4man回答"您可能可以使用 ffmpeg 创建各种测试文件，可能使用类似-x264-params colormatrix=YCgCo 的参数",好主意！

经过不断的测试后，我发现使用下面的指令可以将一段BT.709的媒体文件转为YCgCo:

`ffmpeg -i  BT709.mp4 -vf  colorspace=bt709:ycgco   -c:a copy -color_primaries bt709 -color_trc bt709 -colorspace ycgco YCgCo.mp4`

转换后的文件参数依然显示YCgCo，但这是一份真正的YCgCo，远远不止仅修改metadata，当不关闭potplayer的'YCgCo优化'选项，它也能正确播放。伟大的ffmpeg和mediainfo。我将生成的文件发送给客户，同时告诉他chromium和mpv也遇到了这样的问题，他接受了这个事实。

如果你也是一位在寻找YCgCo样本文件的人，那么祝贺你，这个repo存放着目前Google上唯一的真正的YCgCo示例文件。




some Tool：
https://res18h39.netlify.app/color
