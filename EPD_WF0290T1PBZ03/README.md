
# MSP430G2553_2in9EPD_A7106

### 硬件预览
- 正面

- 背面

- PCB正面
    
    Pin连线：

    | MSP430G2553 |   连接到  |
    | ----------- | -------- |
    |    P2.3     | EPD_CLK  |
    |    P2.4     | EPD_Data |
    |    P2.5     | EPD_Busy |
    |    P2.6     | EPD_Power|
    |    P3.4     | EPD_CS   |
    |    P3.5     | EPD_DC   |
    |    P3.6     | EPD_RST  |
    |    P3.1     | EPD_BS   |


- PCB背面

- 屏幕背面

- 屏幕正面


- 关于屏幕

    FPC编号：WFT0000BZ03

    尺寸： 2.9inch

    显示颜色：黑、白

    分辨率：296 x 128

    **：2bit/Pixel**

    外形尺寸（裸屏）：79.0mm × 36.7mm × 0.34mm

    显示尺寸：66.9mm x 29.06mm

    默认刷新方向，从上向下、从左向右

    FPC共26根Pin，但与常见24pin相同，宽度与36Pin相同，可以剪裁为24Pin宽度后接24Pin座子(翻盖下接式)，或直接用36Pin座子(抽拉上接式)，接FPC座子需要增加排线厚度。更多科普内容请参考 https://post.smzdm.com/p/a5dlqv6l

    IC疑似采用uc8151d，spec见Datasheet文件夹，同方案相近型号：[GDEW029T5D](https://www.good-display.cn/product/210.html)



    



### Project文件夹说明

- 2.9寸WFT0000BZ03黑白亮屏测试

    使用esp8266，淘宝造物实验室店家提供的测试demo，验证通过。驱动板使用 https://oshwhub.com/ludas/mo-shui-ping-qu-dong 。连线如下：

    |  nodeMCU  | GPIO Num | 驱动板标识|
    | --------- | -------- | -------- |
    |     D1    |  GPIO5   |   Busy   |
    |     D3    |  GPIO0   |   DC     |
    |     D4    |  GPIO2   |   RST    |
    |     D5    |  GPIO14  |   CLK    |
    |     D7    |  GPIO13  |   MOSI   |
    |     D8    |  GPIO15  |   CS     |
    |     G     |    N/A   |   GND    |
    |     3V    |    N/A   |   3.3V   |

    两个开关分别打到4Wire、0.47R。

- test_cpp

    使用MSP430G2553，参考 https://github.com/muyuchl/epaper_price_tag_mod 杂糅8266项目，基于原电路构建的项目，价签拆开焊接出2根电源+2根调试，可利用LaunchPad直接写入价签，已验证通过。

    Debug时还可以焊出两根线做串口，LaunchPad的串口可能有问题，可以再接一个串口转USB模块。

- sender

    A7106



### 基础知识

[Allvision 全智景](http://www.jxwisevision.com)

[UltraChip 晶宏半导体](http://www.ultrachip.com/cn/home.php)

[大连佳显](https://www.good-display.cn/)

[元太科技](https://cn.eink.com/)

Adafruit


### 相关tool
[灰度测试](http://www.lagom.nl/lcd-test/zhs_gradient.php)

https://tools.clz.me/image-to-bitmap-array

https://www.e-paper-display.cn/project/iisstart.htm


### 参考链接
- https://blog.csdn.net/sandwich_iot/article/details/121136582
- https://blog.csdn.net/chenqide163/article/details/107363303/
- https://post.smzdm.com/p/a5dlqv6l/
- https://www.waveshare.net/wiki/2.9inch_e-Paper_HAT_(D)
- https://www.cnblogs.com/myfish3/p/13345004.html 
- https://github.com/waveshare/e-Paper
- https://javabin.cn/2022/epaper.html
- https://blog.csdn.net/weixin_46709820/article/details/125275539
- https://blog.csdn.net/Lanal11/article/details/106594736
- https://spotpear.cn/
- https://www.amobbs.com/thread-5655209-1-1.html