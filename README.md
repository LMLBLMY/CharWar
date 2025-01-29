# CharWar
### 文字战争


### POWER BY Lamoonly LMLB

项目介绍(Project Introduction)
---
这是一个个人练习C语言的开源项目。项目的本体是一款能最多9人联机的像素抛物线游戏，其使用的引擎(或者说绘图库)是[EasyX](https://easyx.cn/)。<br>
该项目的前身是由HP Prime计算器编写的PPL程序，其放在了[HP Prime](https://github.com/LMLBLMY/CharWar/tree/fd179399fb930c36b586a86d56f957d81ff08a70/HP)中<br>
目前还没有英文版本<br><br>
This is an open-source project for personal practice of the C programming language. The main project is a pixel-based parabolic game that supports up to 9 players online. The engine (or graphics library) used in this project is [EasyX](https://easyx.cn/).<br>
The predecessor of this project was a PPL program written for the HP Prime calculator, which can be found in the [HP Prime](https://github.com/LMLBLMY/CharWar/tree/fd179399fb930c36b586a86d56f957d81ff08a70/HP) repository.<br>
There is no English version yet<br>

源文件的使用以及声明(Use and declaration of src files)
---
* 编译器的编译环境应该为`Unicode`环境
* 该工程并未包含`easyx.h`和`graphics.h`以及其他的`lib`文件，详细请查看[EasyX](https://easyx.cn/)官网
* 有部分函数实现是由GPT完成的<br><br>

* The compiler's compilation environment should be Unicode
* The project does not include `easyx.h` and `graphics.h` and other `lib` files, please see the [EasyX](https://easyx.cn/) official website for details
* Some function implementations are done by GPT

>[EasyX](https://easyx.cn/)

地图的说明(Description of map)
---
地图的比例是`4:3`，有考虑到做全屏，但是那应该是之后要做的事了。`320*240`是HP Prime计算器的屏幕分辨率，与C语言版本的是兼容的(默认`768*576`)。<br>
由于这是一个像素级游戏(指判定是像素级的:D)，*地图的大小会切实影响到游戏过程*。因为没有做全局放大(全屏)的考虑(有得我改了)。<br><br>
The scale of the map is `4:3`, there is consideration of doing full screen, but that should be done later. `320*240` is the screen resolution of the HP Prime calculator, which is compatible with the C language version (the default is `768*576`). <br>
Since this is a pixel-level game(~~__AS YOU SAID HITBOX is JUST FOUR PIXELS:(__~~ *The size of the map really affects the gameplay*. Because there is no global amplification (full screen) consideration (I have to eat bugs).

存储文件的说明(Description of save files)
---
目前(241226)的存储文件还只是分开存储的(分成存档地图，~~回放存档~~ 有回放这种东西？，存档文本)
* 关于存档文本-->  游戏数据是由`22*24`的二维数组组成的，其中第一行与第一列是空数据。(详细见Save Files)<br><br>

Currently (241226) save files are only stored separately (into archive maps, ~~replay archive~~ IS there such a thing? , archive text)
* About save text --> The game data is made up of a `22*24` two-dimensional array, where the first row and column are empty. (See Save Files for details)
```c
#define 数据 PlayersData

PlayersData[1][1] = x1;
PlayersData[1][2] = y1;
数据[2][1] = x2;
数据[2][2] = y2;
```
~~the "数据" is same as PlayersData~~


### 关于游戏及游玩(About the Game & Play)
希望你能认真看完这篇帮助，若是看了还是解决不了困难可以联系作者LMLB<br>
这个游戏花了我很长时间，所以感谢你能游玩本游戏，感谢支持，祝你玩得开心。<br>
如果你想要帮助我改进程序或者进行代码交流，请联系我q：2606575063<br>
欢迎反馈bug<br><br>

I hope you can read this help carefully, if you still can't solve the difficulties you can contact the author LMLB!<br>
This game took me a long time, so thank you for playing this game, thanks for the support and have fun.<br>
If you want to help me improve the programm or do a code exchange, please contact me discord: lamoonly  (Stochastic Online:D)<br>
Welcome to feedback bugs<br>


游戏内 UI::::::
![UI说明](https://github.com/LMLBLMY/CharWar/blob/master/UI%20description(UI%20%E8%AF%B4%E6%98%8E)/UI%E8%AF%B4%E6%98%8E(C%E7%89%88).png)

![UI description](https://github.com/LMLBLMY/CharWar/blob/master/UI%20description(UI%20%E8%AF%B4%E6%98%8E)/UI%20description(Ver%20C).png)

默认按键(Default Key)
---

①移动：AD键分别为人物向左.向右移动(这个行为将消耗本回合燃油)<br>
  →燃油：分为回合燃油(左上角绿色柱子）和总燃油(力度条下面的字).<br>
   回合燃油最大为10，下次轮到自己时从总燃油补充到回合燃油<br>
  →在空中可以~~滑翔~~掉落(80°角向下)<br>
  →掉下悬崖后将会扣25%的血并随机在空中复活<br>
②攻击角度与攻击方向：`W` `S`键分别为增大与减小发射角度（<ins>最大为80</ins>，<ins>最小为-60</ins>，超过自动换方向，也能按  `Q`键  换方向）
③力度：`+`(加号)键  与  `-`(减号)键  分别为力度的加减，<ins>最低为5</ins>，<ins>最大23</ins>
④发射：按下  `Enter`(回车)键  发射.请注意风向与等级，调整好角度与力度
⑤技能：点击  `当前技能` 或者 按下 `F`键 查看技能帮助.  数字键盘对应相应技能(`0`至`9`)，额外技能需先按  `*`(乘号)键  ，`Esc`(取消)键 或 `Backspace`(退格)键  取消选用技能或转换
