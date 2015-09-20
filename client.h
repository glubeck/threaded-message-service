#pragma once

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
using namespace std;

class Client {
public:
    Client();
    ~Client();

    void run();

protected:
    virtual void create();
    virtual void close_socket();
    void echo();
    bool send_request(string);
    bool get_response();
    vector<string> parse_request(string);
    bool isNumber(string);
    int server_;
    int buflen_;
    char* buf_;
};
