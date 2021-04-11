#include <iostream>
#include <string>

#ifndef INTERFACE_H
#define INTERFACE_H


class ipAddress
{
private:
    std::string ip;
    uint16_t mask;
    std::string formatSubnet(uint16_t bits);

public:
    ipAddress(std::string Iaddr, std::string Imask);
    std::string calculateBroadcast();
    std::string calculateNetwork();
    std::string calculateCidr();
};


#endif