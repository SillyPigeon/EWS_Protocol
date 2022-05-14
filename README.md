# EWS 模块说明
## 版本说明
1.0.1 经过初步审核并修正格式(该版本已于3663_NOS平台导入测试)

## 功能说明
EWS模块为一个C语言为基础，对接底层SI解析与UI控制的固定标准定义的逻辑代码。其中逻辑控制为最新印尼标准指定，并已通过认证测试可直接模仿使用。

1、模块定义ews.c为主控制逻辑和spec定义解析函数，相关调用功能组件定义在ews_func.c中，需要按平台实现对接SI和相关系统的接口。

2、ews.c已调试完成，无需变更。

3、ews_func.c为移植平台时应该编写的代码。

4、若有问题，优先查询ews_func.c中接口为何没实现注释描述的功能。

## 代码目录架构

| 目录     |       说明 |
| ------------ |  ---- |
| source | 包含模块核心逻辑ews.c和组件接口实现的demo: ews_func_00.c (ews_func.c) |
| object_demo | 已经完成移植的demo平台代码，可直接使用。 |
| doc      | 说明文档和spec |
| MODULE_FRAME | 模块基本框架图示 |

## 移植(安装)说明

1、阅读相关doc文件夹中的spec了解EWS

2、按照doc中XXX_GUIDE文档按步骤移植到需要使用的系统

3、阅读doc中EWS功能与系统冲突检查，并执行相关修改

4、按doc中的T2检查当前TV系统是否满足

## 接口使用说明

1、源码可见，按注释了解原理后，调用即可

2、EWS_TASK 当作个进程/任务/线程调用启动

3、EWS_UserInput_Handler置于按键解析处

