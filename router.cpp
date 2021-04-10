#include <iostream>
#include <arpa/inet.h>
#include <string>
#include "utility.h"
#include "interface.h"
using namespace std;


int main()
{
    string ip_addr = "8.8.8.8/12";
    vector<string> cidr = tokenize(ip_addr, "/");
    Interface i = Interface(cidr[0],cidr[1]);

    cout << i.calculateBroadcast();

    return 0;
}