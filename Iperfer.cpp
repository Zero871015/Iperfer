#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <chrono>

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>

using namespace std;

#define MAX_BUF 1000

void errorMsg(int);
void runServer(int);
void runClient(string, int, int);

int main(int argc, char *argv[])
{
    if(argc <= 1)
    {
        errorMsg(1);
    }

    string temp(argv[1]);
    if(temp == "-c")
    {
        string hostname = "";
        int port = -1;
        int time = 10;
        for(int i = 2; i < argc; i++)
        {
            temp = string(argv[i]);
            if(temp == "-h")
            {
                i++;
                if(i >= argc)
                    errorMsg(1);
                hostname = string(argv[i]);
            }
            else if(temp == "-p")
            {
                i++;
                if(i >= argc || !isdigit(argv[i][0]))
                    errorMsg(1);
                port = atoi(argv[i]);
            }
            else if(temp == "-t" || !isdigit(argv[i][0]))
            {
                i++;
                if(i >= argc)
                    errorMsg(1);
                time = atoi(argv[i]);
            }
            else
            {
                errorMsg(1);
            }
        }
        if(hostname == "" || port == -1)
            errorMsg(1);
        if(port < 1024 || port > 65535)
            errorMsg(2);

        runClient(hostname, port, time);
    }
    else if(temp == "-s")
    {
        if(argc != 4 || !isdigit(argv[3][0]))
            errorMsg(1);

        int port = -1;
        temp = string(argv[2]);
        if(temp != "-p")
            errorMsg(1);
        port = atoi(argv[3]);
        if(port < 1024 || port > 65535)
            errorMsg(2);

        runServer(port);
    }
    else
        errorMsg(1);


    return 0;    
}

void errorMsg(int type)
{
    if(type == 1)
        cerr << "Error: missing or additional arguments\n";
    else if(type == 2)
        cerr << "Error: port number must be in the range 1024 to 65535\n";
    else if(type == 3)
        cerr << "Error: socket create failed\n";
    else if(type == 4)
        cerr << "Error: bind failed\n";
    else if(type == 5)
        cerr << "Error: listen failed\n";
    else if(type == 6)
        cerr << "Error: connect error\n";
    else if(type == 7)
        cerr << "Error: hostname or ip error\n";
    else if(type == 8)
        cerr << "Error: send/recv msg error\n";
    exit(-1);
}

void runServer(int port)
{
    int sockfd, new_fd;
    struct sockaddr_in my_addr, client_addr;
    socklen_t sin_size = 0;
    int numbytes;
    vector<char> buffer(MAX_BUF);

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        errorMsg(3);

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(my_addr.sin_zero), 0, 8);

    if(bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
        errorMsg(4);
    if(listen(sockfd, 5) == -1)
        errorMsg(5);

    sin_size = sizeof(struct sockaddr_in);
    if((new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size)) == -1)
        errorMsg(6);
    
    chrono::steady_clock::time_point start = chrono::steady_clock::now();
    int num = 0;
    bool flag = false;
    while(!flag)
    {
        if((numbytes = recv(new_fd, &buffer[0], buffer.size(), 0)) == -1)
            errorMsg(8);
        num++;
        for(auto b : buffer)
        {
            if(b == 'E')
            {
                flag = true;
                num -= 1;
                break;
            }
        }
    }
    double diff = chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - start).count() / 1000000.0;
    cout << "sent=" << num << " KB rate=" << double(num / 1000 * 8 / diff) << " Mbps" << endl;
    
    close(new_fd);
}

void runClient(string hostname, int port, int time)
{
    int sockfd;
    struct sockaddr_in server_addr;
    string ip;

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        errorMsg(3);
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    in_addr *address;
    hostent *record = gethostbyname(hostname.c_str());
    if(record == NULL)
        errorMsg(7);
    else
        address = (in_addr *)record->h_addr;
    ip = string(inet_ntoa(* address));
    
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    memset(&(server_addr.sin_zero), 0, 8);
    if(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
        errorMsg(6);
    

    chrono::steady_clock::time_point start = chrono::steady_clock::now();
    double diff = 0;
    int num = 0;
    char chunk[1000] = {0};
    while(diff < time)
    {
        if(send(sockfd, chunk, 1000, 0) == -1)
            errorMsg(8);
        diff = chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - start).count() / 1000000.0;
        num++;
    }
    send(sockfd, "E", 1, 0);
    
    
    
    cout << "sent=" << num << " KB rate=" << double(num / 1000 * 8 / diff) << " Mbps" << endl;
    close(sockfd);
}
