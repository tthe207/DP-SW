# DP-SW
使用SWD接口对stm32系列编程

swd.c 包括了
- SWD硬件IO口的初始化
- SWD时序模拟（时钟，写1字节，读一字节）
- 等

swd.h 包括了
- 硬件IO口定义
- DP AP寄存器地址

开启SWD接口基本操作顺序
- 将debug port切换为SW，默认是JTAG
- 通过DP寄存器线复位操作（大于50个时钟高电平再读ID）
- 通过AP寄存器来操作总线，实现所有寄存器的读写以及内存擦写
参考 SWD_Debug 函数

详细协议参考ARM Debug Inteface文档
