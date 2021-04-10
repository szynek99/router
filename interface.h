#include <iostream>
#include <string>

#ifndef INTERFACE_H
#define INTERFACE_H


class Interface
{
private:
    std::string ip;
    uint16_t mask;
    std::string formatSubnet(uint16_t bits);

public:
    Interface(std::string Iaddr, std::string Imask);
    std::string calculateBroadcast();
    std::string calculateNetwork();
};


#endif