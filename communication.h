#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#include <iostream>
using namespace std; 

class Communication 
{ 
public:
    Communication(int socketfd);
    int socketfd;
    void send_message(const string& message);
    string receive_message();
};

#endif