# 为esphome添加TCL112协议——基于esphome v.2022.6.2


## What

截止目前(2022/08/13)，esphome中只有climate组件支持tcl112协议，这意味着对于采用tcl112协议的空调，esphome只能提供一个仪表盘，和遥控器区别并不大，每当需要调到一个预期的模式时，仍然需要按许多次按键，我更想要一个类似"一键27度+熟睡模式+风力低速"的自定义按键。

所以在参照了climate添加tcl112的PR和remote_base添加RC6的PR后，我添加了当前的文件，让esphome的remote_base也能支持tcl112协议。可以用在remote_receiver dump/remote_transmitter等组件中。


## How


1. 将remote_base下所有文件copy替换到esphome对应路径下，例如我使用docker运行esphome的方式，路径应该为进入docker容器后的"/esphome/esphome/components/remote_base/"，此外，如果同时有设置了tcl112的climate，后续解析协议时会被climate抢答，需要修改下/esphome/esphome/components/tcl112/tcl112.cpp文件末尾处的return true改为return false。



2.  在yaml文件中将remote_receiver的dump属性设置为tcl112，logger的level属性设置为Debug。


3. 通过串口或esphome Web观察esphome log，并按遥控器发送欲解析的红外，红外接收头收到红外信号将会解码并将结果打印在log中，类似如下格式：

    `[remote.tcl112:117]	Received TCL112: 0x23,0xCB,0x26,0x01,0x00,0x24,0x03,0x04,0x01,0x00,0x00,0x00,0x80,0xC1`

4. 拿到解码后的数据就可以创建自定义的按键了，可以再次编辑yaml文件，新增一栏switch，协议选择tcl112，并将上述解码的值复制为code字段，示例如下：
```
switch:    
  - platform: template
    name: tcl112_onekey_27
    id: tcl112_onekey_27
    turn_on_action:
      - remote_transmitter.transmit_tcl112:
          code: [0x23,0xCB,0x26,0x01,0x00,0x24,0x03,0x04,0x01,0x00,0x00,0x00,0x80,0xC1]
```

## Other
- 由于tcl112并不像NEC、RC5等，并不是一个标准协议，我不确定esphome官方是否赞成将“tcl112_protocol”相关文件合入，且目前代码中仍有问题，东拼西凑代码质量确实堪忧，属于刚刚能用级别，目前也未接触到有其他人有类似需求，目前暂时不会考虑向官方提交PR。
