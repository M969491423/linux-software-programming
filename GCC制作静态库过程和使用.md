# GCC制作静态库过程和使用
# 静态库
静态库是指在我们的应用中，有一些公共代码是需要反复使用，就把这些代码编译为“库”文件；在链接步骤中，连接器将从库文件取得所需的代码，复制到生成的可执行文件中的这种库。<a href="https://baike.baidu.com/item/%E9%9D%99%E6%80%81%E5%BA%93/8955694">百度文库</a>

Linux下的静态库为lib*.a格式的二进制文件, **注意 lib 和 .a 不能改变, 能改变只有中间的内容**, 对应到Windows下的 .lib 格式文件.

静态库在使用的时候会在**链接**阶段直接复制到目标文件中. 其优点是速度更快, 静态库特点是可执行文件中包含了库代码的一份完整拷贝, 缺点是编译后的文件体积较大, 且更新静态库之后需要再次进行编译链接, 不方便调试.

## GCC 制作静态库的流程
### 使用gcc对库源文件进行编译
**本示例中的目录结构并不符合标准, 真正开发时应按照制定好的目录规范存放各类文件**
示例目录结构:
```bash
├── add.c
├── main.c
├── mymath.h
└── sub.c
```
1. 源文件: 
add.c
```c
int add(int a,int b) 
{
	return a+b;
}
```

sub.c
```c
int sub(int a,int b)
{
	return a-b;
}
```

mymath.h
此文件主要是静态库函数的声明, 方便编码, 减少编译时的警告 (可以不存在). 
```c
#ifndef _MYMATH_H_ //判断是否声明了 _MYMATH_H_,若没有声明则执行以下三行, 作用是防止头文件被多次包含, 可省略
#define _MYMATH_H_
int add(int, int);
int sub(int, int);
#endif
```

main.c
```c
#include <stdio.h>
#include "mymath.h"
int main()
{
    int a = 10;
    int b = 5;
    printf("%d + %d = %d\n", a, b, add(a, b));
    printf("%d - %d = %d\n", a, b, sub(a, b));
    return 0;
}
```

2. 编译
```bash
gcc -c add.c -o add.o
#将add.c进行编译(不执行链接) 目标文件名为add.o
gcc -c sub.c -o sub.o
```

### 制作静态库
静态库的制作命令一般是固定格式: ar rcs lib库名.a 编译文件(可以是多个, 也可以用*.o  指定当前目录下的所有 .o 文件)
```bash
ar rcs libmymath.a add.o sub.o
```
执行完以上步骤后的文件目录结构;
```bash
# .o 文件为源码编译后的文件, 用作共享库的制作
# .a 为静态库的目标文件
.
├── add.c
├── add.o
├── libmymath.a*
├── main.c
├── mymath.h
├── sub.c
└── sub.o
```
## 静态库的使用
使用gcc编译同时加上静态库 源文件,并指明链接.
```bash
# 将源代码和静态库文件同时进行编译链接
# -l 参数指定链接的库
# -L 参数指定库文件的位置
# 如果库文件与源文件在同一目录下以上两个参数则可以省略
gcc main.c libmymath.a -l mymath -L ./ -o main
# 执行可执行程序
./main
```
```bash
#执行结果:
10 + 5 = 15
10 - 5 = 5
```

## 扩展
以上有提到 mymath.h 文件可有可无, 都可以使最后编译 main.c 与 库文件时编译成功. 但是还是建议保存该文件. 一方面可以方便编辑器的代码提示, 一方面可以减少编译时的警告. 以下是没有使用 mymath.h 头文件时编译的情况. 
首先删除 main.c 中的 
```c
include "mymath.h"


// 源文件代码为:
#include <stdio.h>

int main()
{
    int a = 10;
    int b = 5;
    printf("%d + %d = %d\n", a, b, add(a, b));
    printf("%d - %d = %d\n", a, b, sub(a, b));
    return 0;
}
```
```bash
gcc main.c libmymath.a -lmymath -L./ -o main -Wall # -Wall参数 用于显示gcc运行时所有的信息 包括警告

#编译输出

main.c: In function ‘main’:
main.c:7:36: warning: implicit declaration of function ‘add’ [-Wimplicit-function-declaration]
    7 |     printf("%d + %d = %d\n", a, b, add(a, b));
      |                                    ^~~
main.c:8:36: warning: implicit declaration of function ‘sub’ [-Wimplicit-function-declaration]
    8 |     printf("%d - %d = %d\n", a, b, sub(a, b));

# 可执行文件 main 的执行
./main
#执行结果
10 + 5 = 15
10 - 5 = 5
```