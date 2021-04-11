#include <iostream>
#include <arpa/inet.h>
#include "utility.h"
#include "ipAddress.h"
#include <unordered_map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

using namespace std;

class Router
{
private:
    vector<ipAddress> interfaces;
    unordered_map<string, pair<uint16_t, ipAddress>> r_table;

public:
    Router()
    {
        interfaces = vector<ipAddress>();
        r_table = unordered_map<string, pair<uint16_t, ipAddress>>();
    }
    void addInterface(ipAddress i_new, uint16_t cost)
    {
        interfaces.push_back(i_new);
        string ip_network = i_new.calculateNetwork();
        auto search = r_table.find(ip_network);
        if (search != r_table.end())
        {
            pair<uint16_t, ipAddress> i_found = search->second;
            if (search->second.first > cost)
            {
                r_table.insert({ip_network, pair<uint16_t, ipAddress>(cost, i_new)});
            }
        }
        else
        {
            r_table.insert({ip_network, pair<uint16_t, ipAddress>(cost, i_new)});
        }
    };
    void removeInteface(ipAddress i);
    void sendEcho(int sockfd)
    {
        string echo_message = "test";
        struct sockaddr_in server_address;
        bzero(&server_address, sizeof(server_address));
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(54321);
        ssize_t message_len = echo_message.length();
        for (auto &elem : interfaces)
        {
            inet_pton(AF_INET, elem.calculateBroadcast().c_str(), &server_address.sin_addr);
            if (sendto(sockfd, echo_message.c_str(), message_len, 0, (struct sockaddr *)&server_address, sizeof(server_address)) != message_len)
            {
                throw runtime_error("sendto");
            }
        }
    };
};
int main()
{
    string odpowiedz_test = "cos";

    Router router = Router();
    string str;
    int no_interfaces;
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0)
    {
        fprintf(stderr, "socket error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    cin >> no_interfaces;
    getline(cin, str);
    for (int i = 0; i < no_interfaces; i++)
    {
        getline(cin, str);
        vector<string> input = tokenize(str, " ");
        vector<string> cidr = tokenize(input[0], "/");
        router.addInterface(ipAddress(cidr[0], cidr[1]), stoi(input[2]));
    }

    router.sendEcho(sockfd);

    close(sockfd);
    return 0;
}