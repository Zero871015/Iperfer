# Iperfer

## How to run

### Compile

* There is a makefile for you.
* Just `make` to compile the program.
* If you want to compile youself, using `g++ Iperfer.cpp -o Iperfer` to do.
* If there is anything failed, make sure
  * you are in Linux
  * g++ version is 7.5.0 or more

### Server

* Using `Iperfer -s -p <port>` to run the server.
* The port must be in the range `1024` to `65535`.

### Client

* Using `Iperfer -c -h <hostname> -p <port> -t <time>` to run client.
* hostname can be the server hostname or IP address.
* The port must be in the range `1024` to `65535`.
* The unit of time is second. Default time is `10` seconds.

## What is Iperfer

* Iperfer is a program can measure network bandwidth like Iperf.
* Run server mode and client mode on different devices, Iperfer will show the rate between them.
