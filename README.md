#  TTCN-3 MQTT测试端口

## 项目概述

一种TTCN-3的MQTT系统适配器实现，用于与以MQTT作为外部接口的系统建立连接。

### 开发环境：

Eclipse TITAN

### 功能：

1.主测试程序可调用send接口发送mqtt消息，修改发布Topic，订阅MQTT消息。 

2.主测试程序可调用receive接口等待和接收消息传入。

3.通过.cfg文件动态配置默认MQTT参数。

4.启用TTCN-3实时功能，在发送接收函数出重定向时间戳参数。

### 端口文件

MQTTasp_PortType.ttcn    MQTTasp_Types.ttcn    MQTTasp_PT.hh    MQTTasp_PT.cc

## 依赖

### paho c mqtt库

见 https://github.com/imcteam/mqtt-test

### Eclipse TITAN开发环境

见 https://tower.im/teams/173781/repository_documents/7202/

## 运行示例

测试端口给出一个测试demo，主程序文件为MyExample.ttcn，
配置文件为MyExample.cfg，被测系统位于mqtt-paho-test文件夹内。

运行过程如下:

1.编译

```
cd MQTTasp
make
cd mqtt-paho-test
make
```

2.运行被测系统

```
cd mqtt-paho-test
./sub2
```

3.另开终端，运行测试程序

```
cd MQTTasp
ttcn3_start MyExample MyExample.cfg
```

4.当ttcn3程序阻塞时，在sub2程序终端输入字符串，ttcn3会接收并判定是否符合预期。
ttcn3发送hello期望接收到hi，ttcn3发送good bye期望接收bye。

## API参考

### 一.send函数
提供两种重载。

第一种为charstring类型字符串，如：

```

TestPort.send("Hello!");

```

第二种为MQTT_Data类型结构体，结构体定义如下，其中pub和sub可省：

```
type record MQTT_Data {    
	    Data       data,
	    Topic       pub optional,
          Topic       sub optional
}
```

调用示例如下：
 ```
var MQTT_Data data := {"hello2", "pub_test",omit};
MyPCO.send(data);
```

### 二.reveive函数
reveive函数仅支持字符串传入，如：
```
[] TestPort.receive("hi") ;
```

### 三.cfg文件动态配置

BrokerAddress表示默认broker地址，PubTopic和SubTopic分别
表示默认发布和订阅的topic。

配置示例：
```
[TESTPORT_PARAMETERS]
*.MyPCO.BrokerAddress := "tcp://0.0.0.0:1883"
*.MyPCO.PubTopic := "TTCN-2"
*.MyPCO.SubTopic := "TTCN-1"
```

### 四.时间戳功能

可获取发送接收时刻的时间戳，时间戳类型为float，单位为秒，精确到微妙。
用法及示例如下：

```
var float time_start := now; //获取当前时间

//捕获发送时刻时间戳
var float time_send；
MyPCO.send("Good Bye!") -> timestamp time_send;

//捕获接收时刻时间戳
var float time_receive；
[] MyPCO.receive("bye")-> timestamp time_receive；
```


