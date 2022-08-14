
# MSP430G2553_2in9EPD_A7106

### 硬件预览
- 正面

- 背面

- 屏幕背面

- 屏幕正面

    FPC标识：
    FPC排线共26根Pin，但与常见24pin相同，宽度与36Pin相同，可以剪裁为24Pin宽度后接24Pin座子(翻盖下接式)，或直接用36Pin座子(抽拉上接式)，接FPC座子需要增加排线厚度。更多科普内容请参考 https://post.smzdm.com/p/a5dlqv6l

- PCB正面
    
    Pin连线：

    | MSP430G2553 |   连接到  |
    | ----------- | -------- |
    |    P2.5     | EPD_Busy |
    |    P3.1     | EPD_     |
    |    P2.1     | EPD_     |

- PCB背面





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