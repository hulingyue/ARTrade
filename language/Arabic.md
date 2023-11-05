[中文](./Chinese.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;
[English](./../README.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;
[한국어](./Korean.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;
[بالعربية](Arabic.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;

<div style="color: red">الترجمة الكورية تأتي من ترجمة جوجل، يرجى الاعتذار إذا كان هناك أي أخطاء! في الوقت نفسه، يسعدني أيضًا قبول آراء الجميع والترحيب بتعليقات الأخطاء بالنسبة لي.</div>
<hr/>

# ARTrade (اسم مبدئي)
> المعنى: نظام تداول يعمل دائمًا دون توقف

### أحدث فرع
[develop_2311](https://github.com/hulingyue/ARTrade/tree/develop_2311)

### كيف تستعمل؟ (خذ بيئة أوبونتو كمثال)
1. تحميل cmake  
    ``` shell
    sudo apt install cmake
    ```
2. تحميل g++  
    ```shell
    sudo apt install g++
    ```
3. تحميل OpenSSL  
    ```shell
    sudo apt-get install libssl-dev
    ```
4. تحميل Boost  
    ```shell
    sudo apt-get install libboost-all-dev
    ```
5. مترجم   
    ``` shell
    <!-- compile bybit -->
    > sh compile.sh -p bybit
    ```
6. التنفيذ (خذ **bybit** كمثال)  

    ```shell
    > cd bybit
    > ./bybit -r ./
    ```