# 寝室智能门锁 

## STC89C52RC部分

*此为BigBrotherBlueprint (BBB 老大哥计画) 的一部分*



我们的作品是应用于寝室场景的指纹解锁系统，核心功能是扫描指纹实现寝室门的开启。 

整个系统包括STC89C52RC最小系统板、ESP8266物联网WIFI模块、指纹识别模块、模拟舵机SG90、LCD显示屏、人体红外感应电子模块、LED照明模块以及电源模块。

设计发明此作品主要是为了方便平时进出寝室，同时利用数据分析，分析同学的作息习惯，给经常在寝室的同学提醒。以及在夜晚照明环境差的情况下为通过的行人自动点灯提醒方位。

实现功能：

- [x] 菜单界面

- [x] 用户管理

- [x] 用户出入数据统计(可掉电存储)
- [x] IoT 接口

- [x] 局域网STA/UDP远程开锁

- [x] 开锁提示音

- [x] 智能照明(省电)

- [x] 热插拔LCD屏幕

- [x] 开机自启动指纹

- [ ] 低功耗功能（重要）

- [ ] 自动上报开门数据

- [ ] 远程开门(互联网)

- [ ] 智能提示音（针对不同同学给予不同的提示）

- [ ] 手机蓝牙自动开锁/NFC开锁

- [ ] 更完备的统计功能（智能分析）

技术特点和优势：

 

​	利用指纹这一生物特征作为智能门锁的开门秘钥，具有人机交互友善度。使用单片机和光学指纹模块，成本廉价，易于生产组装。

​	作品适用范围广泛，其可以直接附加在原有的寝室门锁上，故方便安装和拆卸，便于升级原有门锁。采用多电池的设计，更换电池不影响工作，无需外部电源，大大增强其地域适应性。

