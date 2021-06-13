### 说明

此实例需要放到via下去执行；测试步骤：

- 启动via服务器；
- 启动server.py服务，创建网络拓扑，并调用via的注册函数将服务的rpc接口注册到via上（绑定nodeid和taskid）；
- 启动client.py向via地址发送数据，验证数据是否发送成功；