[中文](./Chinese.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;
[English](./../README.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;
[한국어](./Korean.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;
[بالعربية](Arabic.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;

<hr/>

# ARTrade (名称暂定)
> 含义: 永远运行，无需停机的交易系统

### 最新分支
[develop_2311](https://github.com/hulingyue/ARTrade/tree/develop_2311)

### 如何使用？（以 **Ubuntu** 环境为例）
1. 下载 cmake
    ``` shell
    sudo apt install cmake
    ```
2. 下载 g++
    ```shell
    sudo apt install g++
    ```
3. 下载 OpenSSL
    ```shell
    sudo apt-get install libssl-dev
    ```
4. 下载 Boost
    ```shell
    sudo apt-get install libboost-all-dev
    ```
5. 如果需要进行单元测试
    ```
    sudo apt-get install libgtest-dev
    ```
6. 编译程序
    ``` shell
    compile bybit
    > sh compile.sh -p bybit
    ```
7. 执行 (以 **bybit** 为例)
    ```shell
    > cd bybit
    > ./bybit -r ./
    ```