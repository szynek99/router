#include <iostream>
#include <arpa/inet.h>
#include <string>
#include "utility.h"
#include "interface.h"
using namespace std;


int main()
{
    string ip_addr = "254.254.0.0/12";
    vector<string> cidr = tokenize(ip_addr, "/");
    Interface i = Interface(cidr[0],cidr[1]);

    cout << "Network: " << i.calculateNetwork() << "\n";
    cout << "Brodcast: " << i.calculateBroadcast();

    return 0;
}