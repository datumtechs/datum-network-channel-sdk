## via测试

go-via测试

## 编译

进入测试目录`channel-sdk/test/c++/via_test`，然后执行；

### 创建编译目录

```bash
mkdir build && cd build
```

### 编译选项说明

包括异步和同步两种类型；

```bash
cmake -DASYNC_SERVER=1 .. 
```

> 说明：
>
> - ASYNC_SERVER=1表示使用异步服务器；不指定时（默认）表示使用同步服务器；
> - USE_SSL=ON表示使用ssl证书验证；
> - USE_GMSSL=ON表示使用gmssl证书验证;


### 执行编译

```bash
make
```

> 编译成功后在build目录下生成so文件，包括io库`libextio.so`和client，server可执行文件；



## 测试步骤

### 测试前准备

- 启动go-via

  参考：https://github.com/bglmmz/rosettanet-via

- 复制测试脚本

  ```bash
  cp ../script/*.sh .
  ```

- 复制节点配置文件

  ```bash
  cp ../config.json .
  ```

  


### 开始测试

- 启动服务器

  ```bash
  ./server
  ```

- 启动客户端

  ```bash
  ./client
  ```


