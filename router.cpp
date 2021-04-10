#include <iostream>
#include <arpa/inet.h>
#include <string>
#include "utility.h"
#include "interface.h"
using namespace std;


int main()
{
    vector<Interface> interfaces;
    string str;
    int no_interfaces;
    cin >> no_interfaces;
    getline(cin, str);
    for(int i = 0 ; i < no_interfaces; i++)
    {
        getline(cin, str);
        vector<string> input = tokenize(str, " ");
        vector<string> cidr = tokenize(input[0], "/");
        interfaces.push_back(Interface(cidr[0],cidr[1]));
    }
    for(int i = 0; i < no_interfaces; i++)
    {
        Interface xd = interfaces[i];
        cout << "Interface: " << xd.calculateCidr() << "\n";
        cout << "Network: " << xd.calculateNetwork() << "\n";
        cout << "Brodcast: " << xd.calculateBroadcast() << "\n\n";
    }


    return 0;
}