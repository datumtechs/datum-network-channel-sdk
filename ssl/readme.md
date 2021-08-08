## Channel-sdk支持SSL

​		OpenSSL是一套开放源代码的安全套接字层密码学基础库，囊括主要的密码算法、常用的密钥和证书封装管理功能及SSL/TLS协议，并提供丰富的API，以供应用程序开发、测试或其它目的使用。它广泛集成在各种类型的操作系统中，作为基础组件之一，深受广大IT爱好者的喜爱。即使某些操作系统没有将其集成为组件，通过源代码下载，也可以十分轻松地构建OpenSSL的开发及应用环境。

​		尽管OpenSSL的功能十分强大且丰富，然而对于中国商用密码体系的算法及相关应用来说，它距离我们还是十分遥远的。因为OpenSSL仅仅包含国际通用的密码算法、认证体系及相关协议，却没有将中国商用密码体系中的公开算法SM2、SM3、SM4及祖冲之流密码算法纳入其中，也不支持**双证书体系（签名证书和加密证书）**的应用及相关协议。这对于推广及研究中国商用密码体系的密码爱好者来说，是十分无奈的事情。国内也有不少密码界同仁尝试着将OpenSSL国密化，但大多都局限于公司内部交流使用。针对这种现状，江南天安经过长时间的研究分析，于2017年上半年推出天安版国密OpenSSL，并将其命名为TaSSL，解决了中国商用密码体系无法构建基于OpenSSL应用的实际问题。

​		为了信道安全，Channel-sdk支持openssl和gmssl(TaSSL)两种；

### 支持OpenSSL

- OpenSSL工具简介
  - openssl：多用途的命令行工具，可以实现：秘钥证书管理、对称加密和非对称加密。
  -  libcrypto：加密算法库；
  - libssl：加密模块应用库，实现了ssl及tls；

- 服务器证书配置文件：server_cert.conf

  ```toml
  [ req ]
  distinguished_name     = req_distinguished_name
  prompt                 = no
   
  [ req_distinguished_name ]
  O = DEMO
  CN = localhost
  
  [req_ext]
  subjectAltName = @alt_names
  
  [alt_names]
  DNS.1 = test.com
  DNS.2 = *.test.com
  DNS.3 = www.test.com
  IP.1 = 192.168.21.126
  IP.2 = 127.0.0.1
  IP.3 = 0.0.0.0
  ```

  >说明：
  >
  >客户端支持通过域名或ip+port的方式访问服务器，所以生成服务器的证书之前，需配置允许外部访问的域名和ip，配置方式如上所示；

  

- 生成ssl证书相关信息

  执行脚本：

  ```bash
  ./generate.sh
  ```

  > 执行此命令可生成证书相关的文件：根证书的私钥/根证书，服务器私钥/服务器证书，客户端私钥/客户端证书；

  

- 编译命令

  channel-sdk默认不使用ssl，如需使用openssl(类型为1)或gmssl(类型为2)，需使用`--ssl-type`编译参数：

  ```bash
  ./build.sh compile --ssl-type=1
  ```

  > 编译完成之后，进行重新安装；



- 节点配置文件

  ```json
  {
  	"TASK_ID": "task-1",
  	"ROOT_CERT": "ssl_key/ca.pem",
  	"NODE_INFO": [
  		{
  			"NODE_ID": "p0",
  			"ADDRESS": "127.0.0.1:10001",
  			"VIA": "VIA1",
  			"SERVER_KEY": "ssl_key/server.key",
  			"SERVER_CERT": "ssl_key/server.pem",
  			"CLIENT_KEY": "ssl_key/client.key",
  			"CLIENT_CERT": "ssl_key/client.pem"
  		},
  		{
  			"NODE_ID": "p1",
  			"ADDRESS": "127.0.0.1:20001",
  			"VIA": "VIA2",
  			"SERVER_KEY": "ssl_key/server.key",
  			"SERVER_CERT": "ssl_key/server.pem",
  			"CLIENT_KEY": "ssl_key/client.key",
  			"CLIENT_CERT": "ssl_key/client.pem"
  		},
  		{
  			"NODE_ID": "p2",
  			"ADDRESS": "127.0.0.1:30001",
  			"VIA": "VIA3",
  			"SERVER_KEY": "ssl_key/server.key",
  			"SERVER_CERT": "ssl_key/server.pem",
  			"CLIENT_KEY": "ssl_key/client.key",
  			"CLIENT_CERT": "ssl_key/client.pem"
  		}
  	],
  	"VIA_INFO": {
  		"VIA1": "127.0.0.1:10001",
  		"VIA2": "127.0.0.1:20001",
  		"VIA3": "127.0.0.1:30001"
  	},
  	"DATA_NODES": [
  		"p0",
  		"p1",
  		"p2"
  	],
  	"COMPUTATION_NODES": {
  		"p0": "P0",
  		"p1": "P1",
  		"p2": "P2"
  	},
  	"RESULT_NODES": [
  		"p0",
  		"p1",
  		"p2"
  	]
  }
  ```

  > 说明：
  >
  > - ROOT_CERT：根证书路径；
  >   - 用于服务器验证客户端的证书是否由根证书颁发，服务器启动时加载；
  >   - 由于服务器需要接收多个客户端的连接，多个客户端的证书可能不一样，所以服务器使用根证书作为客户端的验证证书；
  > - SERVER_KEY：服务器私钥文件路径；
  >   - 验证服务器证书是否有效，服务器启动时加载；
  > - SERVER_CERT：服务器证书文件路径；
  >   - 用于服务器验证客户端的连接是否有效（ip/域名等），服务器启动时加载；
  >   - 用于客户端验证服务器的响应是否合法，连接服务器时作为客户端的根证书加载；
  >   - 由于根证书相同，所以可能存在多个不同的服务器证书，如果使用根证书作为客户端的验证证书，则用根证书颁发的服务器证书都可以被验证通过，但使用服务器证书验证，则可以保证当次连接的服务器返回的证书才可以被验证通过；
  > - CLIENT_KEY：客户端私钥文件路径；
  >   - 验证客户端证书是否有效，连接服务器时加载；
  > - CLIENT_CERT：客户端证书文件路径；
  >   - 用于连接服务器时，将此证书提供给服务器，验证此证书是否由根证书颁发，连接服务器时时加载；
  >
  > 
  >
  > 总结：
  >
  > - 客户端证书必须由根证书颁发；
  > - 服务器证书可不由根证书颁发（脚本中是由根证书颁发）；
  
  

### 支持GMSSL(TASSL)

- 参考地址

  ```bash
  https://github.com/jntass/TASSL-1.1.1b
  https://www.cnblogs.com/musea/articles/10891515.html
  https://mp.weixin.qq.com/mp/appmsgalbum?__biz=MzI3NTQyMzEzNQ==&action=getalbum&album_id=1338591541056815107&subscene=159&subscene=158&scenenote=https%3A%2F%2Fmp.weixin.qq.com%2Fs%3F__biz%3DMzI3NTQyMzEzNQ%3D%3D%26mid%3D2247485921%26idx%3D1%26sn%3Dd511b445fe44c6e1851bc3e1788b87a1%26chksm%3Deb044311dc73ca072a6ee8c45bb4d8866c0bcf4b842f1184c34b6286ef584d82917818f5c21d%26scene%3D158%23rd#wechat_redirect
  ```

- 下载gmssl

  ```bash
  wget https://github.com/jntass/TASSL-1.1.1b/archive/V_1.4.tar.gz
  ```

- 编译安装

  ```bash
  mkdir -p $HOME/.local/TAGMSSL;
  tar -xvf TASSL-1.1.1b-V_1.4.tar.gz;
  cd TASSL-1.1.1b-V_1.4;
  chmod u+x ./config;
  ./config --prefix=$HOME/.local/TAGMSSL;
  make;
  sudo make install;
  $HOME/.local/TAGMSSL/bin/openssl version;
  ```

    > 注意：
    >
    >
    > 安装路径和grpc的源码安装路径不能一样，因为grpc需要安装ssl的依赖库（libssl.so）和TaSSL安装的libssl.so会有冲突；