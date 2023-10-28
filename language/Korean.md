[中文](./Chinese.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;
[English](./../README.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;
[한국어](./Korean.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;
[بالعربية](Arabic.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;

<div style="color: red">한국어 번역은 Google 번역에서 가져왔습니다. 오류가 있으면 사과해주세요! 동시에 모든 사람의 의견을 기꺼이 수용하고 버그 피드백을 환영합니다.</div>
<hr/>

# ARTrade (가칭)
> 의미: 다운타임 없이 항상 실행되는 거래 시스템

### 최신 지점
[develop_2310](https://github.com/hulingyue/ARTrade/tree/develop_2310)

### 사용하는 방법? (우분투 환경을 예로 들어보겠습니다)
1. cmake 다운로드
    ``` shell
    sudo apt install cmake
    ```
2. g++ 다운로드
    ```shell
    sudo apt install g++
    ```
3. OpenSSL 다운로드
    ```shell
    sudo apt-get install libssl-dev
    ```
4. Boost 다운로드
    ```shell
    sudo apt-get install libboost-all-dev
    ```
5. 컴파일러
    ``` shell
    compile bybit
    > sh compile.sh -p bybit
    ```
6. 실행(예를 들어 **비트별** 사용)
    ```shell
    > cd bybit
    > ./bybit -r ./
    ```