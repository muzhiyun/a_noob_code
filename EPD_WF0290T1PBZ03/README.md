

### 硬件预览
- 正面

- 背面

- 屏幕背面

- 屏幕正面

    26根Pin，但与常见24pin相同，FPC排线宽度36Pin，可以剪裁为24Pin或直接用36Pin座子，

- PCB正面

- PCB背面





### 文件说明
- 2.9寸WFT0000BZ03黑白亮屏测试

    使用esp8266，淘宝造物实验室店家提供的测试demo，验证通过。驱动板使用 https://oshwhub.com/ludas/mo-shui-ping-qu-dong 。连线如下：

    |  nodeMCU  | GPIO Num | 驱动板标识|
    | --------- | -------- | -------- |
    |     D1    |  GPIO5   |   Busy   |
    |     D3    |  GPIO4   |   DC     |
    |     D4    |  GPIO2   |   RST    |
    |     D5    |  GPIO14  |   CLK    |
    |     D7    |  GPIO13  |   MOSI   |
    |     D8    |  GPIO15  |   CS     |
    |     G     |          |   GND    |
    |     3V    |          |   3.3V   |

    两个开关分别打到4Wire、0.47R。

- test_cpp

    使用MSP430G2553，参考 https://github.com/muyuchl/epaper_price_tag_mod 杂糅8266项目，基于原电路构建的项目，价签焊接出2根电源+2根调试+2根串口，利用下载器直接写入价签，已验证通过。