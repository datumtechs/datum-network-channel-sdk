### 说明

由于对外的接口单独使用时，必须要导出`IChannel`， 但是Rosetta已经导出，再导出会有冲突；所以编译时需要添加编译选项：

```cmake
cmake -DUSE_ALONE=1
```

> 如果是给rosetta用，即不用加此参数；

