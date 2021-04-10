#include <iostream>
#include <arpa/inet.h>
#include "utility.h"
#include <string>
using namespace std;

int main()
{
    string ip_addr = "8.8.8.8/12";
    vector<string> cidr = tokenize(ip_addr,"/");
    cout << cidr[0] << " " << formatSubnet(std::stoi(cidr[1])) << " " << calculateBroadcast(cidr[0],formatSubnet(std::stoi(cidr[1]))) << "\n";
    return 0;
}