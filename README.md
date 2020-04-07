# 词法分析程序

## 一. 实验目的

1. 设计实现c语言的词法分析程序

2. 掌握并熟悉自动机的原理和方法
3. 了解词法分析程序的过程及结果

## 二. 实验内容

设计c语言词法分析程序，完成以下功能

1. 能识别c语言的关键字、定义的标识符、无符号数、常量、标点符号以及运算符
2. 能过滤注释
3. 可以检测一些错误的转义字符、字符串引号残缺以及错误字符

## 三. 实验环境

* Win 10
* MSVC 2019（Visual Studio 2019）
* 支持 `ANSI C99` 词法

## 四. 实验过程

通过从字符流读取的方式来模拟从缓冲区读取字符的过程。

命名空间 `ALEI` 中初始化了关键字集合，标点符号集合以及合法转义字符集合。初始化了枚举类 `Tokens` 

其中四个枚举类型为 `identifier`，`keyword`，`constant`，`punctuator`。

从 `main` 函数开始，将输入文件和输出文件以文件流传入 `ALEI::start()` 函数，开始循环。

* 对于空白符

当从文件流中读到空白字符时，进入处理空白字符的自动机，对应直接跳过，读取下一个字符

* 对于标识符

当读取字符为字母、数字或下划线中的一个，则可进入处理标识符和关键字的自动机---

```
while (true)
{
	超前读取字符ch_p
	if (ch_p不是字母、数字或下划线中的一个)
		break;
}
if (读取到底字符串是关键字)
	保存到输出结果中
else
	保存标识符到输出结果中
```

* 对于无符号数

当读取的字符为数字时，进入读取无符号数的自动机---

```
设置当前状态为1
while (true)
{
超前读取字符ch_p
状态1：
	if (ch_p是数字)
		继续
	else if (ch_p是E或e)
		跳转至状态4
	else if (ch_p是.)
		跳转至状态2
状态2：
	if (ch_p是数字)
		跳转至状态3
	else
		退出循环
状态3：
	if (ch_p是数字)
		继续
	else if (ch_p是E或e)
		跳转至状态4
	else
		退出循环
状态4：
	if (ch_p是数字)
		跳转至状态6
	else if (ch_p是+或-)
		跳转至状态5
	else
		退出循环
状态5:
	if (ch_p是数字)
		跳转至状态6
	else
		退出循环
状态6：
	if (ch_p是数字)
		继续
	else
		退出循环
}
将读取到的字符串保存为常量到输出结果中
```

* 对于标点符号

当读取的字符判断为标点符号时，进入读取标点符号及运算符的流程---

```
if (读取到的字符是/)
	超前读取字符ch_p
	if (ch_p是/)
		循环读取完整行字符，都为注释内容
	if (ch_p是*)
		循环读取至读到*/或EOF时，期间都为注释内容
else
	超前读取字符ch_p
	if (读取到..)
		检测到错误标点..，保存至错误结果
	else
		while (true)
		{
			if (已读取的字符串加上当前读取的字符组合不是标点符号时)
				break;
			超前读取字符
		}
		将读取到的字符串保存为标点符号到输出结果中
```

* 对于字符串常量

当读取到 `'` 或 `"` 时，进入读取字符串常量的流程

```
while (true)
{
	读取字符ch
	if (ch为EOF或'\n')
		检测到错误，缺少匹配的引号，将错误信息保存，退出循环
	if (ch成功匹配字符串开始的引号)
		将字符串保存为常量到输出结果中
	else if (ch为转义字符'\')
	{
		if (转义字符后是换行符)
			继续
		else
			if (转义的是常规字符)
				继续
			else
				检测到错误，将无效的转义字符信息保存
	}
}
```

注：

以上判断是否是关键字、标点符号或转义字符的查询，均为哈希表实现，查询效率为O(1)。

## 五. 实验结果

词法分析程序没考虑宏定义以及文件头，所以预编译后的内容就是去掉这些剩下的，并保留注释作为测试输入

### 输入：test.c

```c
int main(int argc, char *argv[])
{
    int a = 1, 23, 1243, 0, 00, 0123, 000321, 3e2, 4E+2;
    float b = 1.23, 23.44, 2.0908, 2., 2.0, 0.1230;
    double c = 234.4, 1e-3, 01e-2, 11e-12;
    cosnt char *str = "hello world";
    char *str2 = "string str \"string\" str";
    char *str3 = "aaaa \a\n\c\e\f\t bbbb好";
    // 注释
    // //2131dfaaef3;'\/.\t//
    /*
    注释
    */
    char ch = 'hi';
    char ch2 = 'hello hi\
    hello';
    ,++=/ -% [[()]()] {}{}({[]}) 
    <= <=>= == ... &*!<< <<=
    \\
    ..
    'aefaf
    \
    return 0;
}
```

### 输出：test.c.txt

```c
< Keyword, int >
< Identifier, main >
< Punctuator, ( >
< Keyword, int >
< Identifier, argc >
< Punctuator, , >
< Keyword, char >
< Punctuator, * >
< Identifier, argv >
< Punctuator, [ >
< Punctuator, ] >
< Punctuator, ) >
< Punctuator, { >
< Keyword, int >
< Identifier, a >
< Punctuator, = >
< constant, 1 >
< Punctuator, , >
< constant, 23 >
< Punctuator, , >
< constant, 1243 >
< Punctuator, , >
< constant, 0 >
< Punctuator, , >
< constant, 00 >
< Punctuator, , >
< constant, 0123 >
< Punctuator, , >
< constant, 000321 >
< Punctuator, , >
< constant, 3e2 >
< Punctuator, , >
< constant, 4E+2 >
< Punctuator, ; >
< Keyword, float >
< Identifier, b >
< Punctuator, = >
< constant, 1.23 >
< Punctuator, , >
< constant, 23.44 >
< Punctuator, , >
< constant, 2.0908 >
< Punctuator, , >
< constant, 2. >
< Punctuator, , >
< constant, 2.0 >
< Punctuator, , >
< constant, 0.1230 >
< Punctuator, ; >
< Keyword, double >
< Identifier, c >
< Punctuator, = >
< constant, 234.4 >
< Punctuator, , >
< constant, 1e-3 >
< Punctuator, , >
< constant, 01e-2 >
< Punctuator, , >
< constant, 11e-12 >
< Punctuator, ; >
< Identifier, cosnt >
< Keyword, char >
< Punctuator, * >
< Identifier, str >
< Punctuator, = >
< constant, "hello world" >
< Punctuator, ; >
< Keyword, char >
< Punctuator, * >
< Identifier, str2 >
< Punctuator, = >
< constant, "string str \"string\" str" >
< Punctuator, ; >
< Keyword, char >
< Punctuator, * >
< Identifier, str3 >
< Punctuator, = >
< constant, "aaaa \a\n\f\t bbbb好" >
< Punctuator, ; >
< Keyword, char >
< Identifier, ch >
< Punctuator, = >
< constant, 'hi' >
< Punctuator, ; >
< Keyword, char >
< Identifier, ch2 >
< Punctuator, = >
< constant, 'hello hi    hello' >
< Punctuator, ; >
< Punctuator, , >
< Punctuator, ++ >
< Punctuator, = >
< Punctuator, / >
< Punctuator, - >
< Punctuator, % >
< Punctuator, [ >
< Punctuator, [ >
< Punctuator, ( >
< Punctuator, ) >
< Punctuator, ] >
< Punctuator, ( >
< Punctuator, ) >
< Punctuator, ] >
< Punctuator, { >
< Punctuator, } >
< Punctuator, { >
< Punctuator, } >
< Punctuator, ( >
< Punctuator, { >
< Punctuator, [ >
< Punctuator, ] >
< Punctuator, } >
< Punctuator, ) >
< Punctuator, <= >
< Punctuator, <= >
< Punctuator, >= >
< Punctuator, == >
< Punctuator, ... >
< Punctuator, & >
< Punctuator, * >
< Punctuator, ! >
< Punctuator, << >
< Punctuator, <<= >
< Keyword, return >
< constant, 0 >
< Punctuator, ; >
< Punctuator, } >
-----------------------
-----------------------
< Invalid escape: \c >
< Invalid escape: \e >
< Error char: \ >
< Invalid punctuator: .. >
< No end bracket: 'aefaf >

```

涉及到的所有测试情况都包含在以上文件内。



