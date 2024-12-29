# CharWar
### 文字战争


### POWER BY Lamoonly LMLB

项目介绍(Project Introduction)
---
这是一个个人练习C语言的开源项目。项目的本体是一款能最多9人联机的像素抛物线游戏，其使用的引擎(或者说绘图库)是[EasyX](https://easyx.cn/)。<br>
该项目的前身是由HP Prime计算器编写的PPL程序，其放在了[HP Prime](https://github.com/LMLBLMY/CharWar/tree/fd179399fb930c36b586a86d56f957d81ff08a70/HP)中<br><br>
This is an open-source project for personal practice of the C programming language. The main project is a pixel-based parabolic game that supports up to 9 players online. The engine (or graphics library) used in this project is [EasyX](https://easyx.cn/).<br>
The predecessor of this project was a PPL program written for the HP Prime calculator, which can be found in the [HP Prime](https://github.com/LMLBLMY/CharWar/tree/fd179399fb930c36b586a86d56f957d81ff08a70/HP) repository.

源文件的使用以及声明(Use and declaration of src files)
---
* 编译器的编译环境应该为`Unicode`环境
* 该工程并未包含`easyx.h`和`graphics.h`以及其他的`lib`文件，详细请查看[EasyX](https://easyx.cn/)官网
* 有部分函数实现是由GPT完成的<br><br>

* The compiler's compilation environment should be Unicode
* The project does not include `easyx.h` and `graphics.h` and other `lib` files, please see the [EasyX](https://easyx.cn/) official website for details
* Some function implementations are done by GPT

地图的说明(Description of map)
---
地图的比例是`4:3`，有考虑到做全屏，但是那应该是之后要做的事了。`320*240`是HP Prime计算器的屏幕分辨率，与C语言版本的是兼容的(默认`768*576`)。<br>
由于这是一个像素级游戏(指判定是像素级的:D)，*地图的大小会切实影响到游戏过程*。因为没有做全局放大(全屏)的考虑(有得我改了)。<br><br>
The scale of the map is `4:3`, there is consideration of doing full screen, but that should be done later. `320*240` is the screen resolution of the HP Prime calculator, which is compatible with the C language version (the default is `768*576`). <br>
Since this is a pixel-level game(~~__AS YOU SAID HITBOX is JUST FOUR PIXELS:(__~~ *The size of the map really affects the gameplay*. Because there is no global amplification (full screen) consideration (I have to eat bugs).
