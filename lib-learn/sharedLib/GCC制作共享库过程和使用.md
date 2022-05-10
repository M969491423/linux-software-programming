# GCC制作共享库过程和使用
## 动态库
共享库与静态库的区别在于，共享库只是在调用模块中嵌入调用代码的在库的相对位置的地址，当执行程序时，共享库会和程序一起加载到内存中，当执行到调用共享库中代码的指令时跳转到共享库中执行，执行完毕后在跳转回来。
相比于静态库占用空间小，方便更新（共享库发生变化后，程序不需要再次编译），但相比于静态库执行效率略低。<a href="https://blog.csdn.net/weixin_43504629/article/details/97616487">Linux下共享库的创建和使用</a>

**共享库以.so结尾，Linux下需要以lib开头**

## 共享库的制作
1. 准备制作共享库的源文件, 文件目录结构如下:
```bash
# add.c 和 sub.c 都是制作动态库的源文件
# main.c 使用了动态库的可执行文件的源文件
# mymath.h 为包含库函数声明的头文件(实际的函数定义在 add.c 与 sub.c 中), 其目的是为了使 main.c 在编译时减少不必要的警告
.
├── add.c
├── main.c
├── mymath.h
└── sub.c
```

2. 源代码内容:
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

3. 对共享库源文件进行编译
即在原来编译的语句上加上 -fPIC 参数, 其作用时编译出位置无关的目标文件
```bash
gcc -c -fPIC add.c -o add.o
gcc -c -fPIC sub.c -o sub.o
```
4. 链接生成共享库(使用 -shared 参数)
```bash
gcc -shared add.o sub.o -o libmymath.so
```
当前的目录结构:
```bash
.
├── add.c
├── add.o
├── libmymath.so
├── main.c
├── mymath.h
├── sub.c
└── sub.o
```
## 共享库的使用
1. 将 mian.c 与 libmymath.so 进行编译链接
```bash
gcc main.c libmymath.so -l mymath -L ./ -o main
```
执行以上命令可以看到编译成功, 且生成了目标文件 main
2. 执行目标文件
```bash
# 直接执行 main 的话会出错
./main
# 错误信息如下
./main: error while loading shared libraries: libmymath.so: cannot open shared object file: No such file or directory
```
错误原因是其找不到共享库. 如果想让它可以**找到共享库文件**则需要**配置环境变量,** 将**共享库所在的目录**添加到名为 **LD_LIBRARY_PATH** 的环境变量中.
## 有关共享库环境变量的设置
下面提供了两种方法
1.  **临时方法**:  在终端直接设置环境变量  **LD_LIBRARY_PATH** 的值
```bash
export LD_LIBRARY_PATH=./

# 再次执行 main
./main
10 + 5 = 15
10 - 5 = 5
```
该方法只在当前终端适用, 当前终端被关闭或者用户切换到其他终端之后则会失效. 
2. 长期方法:  修改有关环境变量的配置文件 ~/.bashrc
```bash
# 使用 vim 编辑 .bashrc
vim ~/.bashrc
# 在文件末尾添加
export LD_LIBRARY_PATH=./:$LD_LIBRARY_PATH #此处使用的路径时相对路径只是为了演示, 为了能够在任何情况下都能够找到共享库, 应该使用绝对路径
# 保存并退出
# 更新环境变量
source ~/.bashrc
# 再次执行 main
./main
# 运行结果
10 + 5 = 15
10 - 5 = 5
```