[中文](./language/Chinese.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;
[English](./README.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;
[한국어](./language/Korean.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;
[بالعربية](./language/Arabic.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;

<hr/>

# ARTrade (Tentative name)
> meaning: A trading system that always runs without downtime

### Latest Branch
[develop_2310](https://github.com/hulingyue/ARTrade/tree/develop_2310)

### how to use? (Take the **Ubuntu** as an example)
1. download cmake
    ``` shell
    sudo apt install cmake
    ```
2. download g++
    ```shell
    sudo apt install g++
    ```
3. download OpenSSL
    ```shell
    sudo apt-get install libssl-dev
    ```
4. download Boost
    ```shell
    sudo apt-get install libboost-all-dev
    ```
5. compile project
    ``` shell
    compile bybit
    > sh compile.sh -p bybit
    ```
6. run (Take the **bybit** as an example)
    ```shell
    > cd bybit
    > ./bybit -r ./
    ```