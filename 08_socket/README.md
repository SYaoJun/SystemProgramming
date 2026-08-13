# socket套接字
1. TCP
2. UDP
3. UDS (Unix domain socket)
## 编译
```sh
gcc client.c
```
## 区别
```sh
# TCP
socket(AF_INET, SOCK_STREAM, 0);
# UDP
socket(AF_INET, SOCK_DGRAM, 0);
# UDS
socket(AF_LOCAL, SOCK_STREAM, 0);
```
## UDS 本地套接字
通过本地的一个文件进行通信
## 知识点
1. 临时端口
2. 端口复用
3. htons
4. socket
5. listen
6. accept
7. getsockname
