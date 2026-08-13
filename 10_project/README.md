在Linux系统下，用C语言实现了简单的客户端和服务器端的TCP通信。

测试服务器端可以用nc命令+IP+port
```
nc 127.0.0.1 6666
```
## 普通服务器
bind

listen

accept
## 多进程服务器
fork

waitpid
## 多线程
pthread_create

detach
