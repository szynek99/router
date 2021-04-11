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
    unordered_map<string, pair<uint32_t, ipAddress>> r_table;

public:
    Router()
    {
        interfaces = vector<ipAddress>();
        r_table = unordered_map<string, pair<uint32_t, ipAddress>>();
    }
    void addInterface(ipAddress i_new, uint32_t cost)
    {
        interfaces.push_back(i_new);
        string ip_network = i_new.calculateNetworkToCidr();
        auto search = r_table.find(ip_network);
        if (search != r_table.end())
        {
            pair<uint32_t, ipAddress> i_found = search->second;
            if (search->second.first > cost)
            {
                r_table.insert({ip_network, pair<uint32_t, ipAddress>(cost, i_new)});
            }
        }
        else
        {
            r_table.insert({ip_network, pair<uint32_t, ipAddress>(cost, i_new)});
        }
    };
    void removeInteface(ipAddress i);
    void encodeToBytes(string network, uint32_t cost, char byteArray[8])
    {
        cost = htonl(cost);
        vector<string> ip_addr_elems = tokenize(network, "/");
        vector<string> ip_bytes = tokenize(ip_addr_elems[0], ".");
        for (int i = 0; i < 4; i++)
        {
            byteArray[i] = stoi(ip_bytes[i]);
        }
        byteArray[4] = stoi(ip_addr_elems[1]);
        byteArray[5] = (char)(cost >> 24);
        byteArray[6] = (char)(cost >> 16);
        byteArray[7] = (char)(cost >> 8);
        byteArray[8] = (char)cost;
    }
    string decodeFromBytes(char byteArray[9])
    {
        string message = "";
        for (int i = 0; i < 4; i++)
        {
            message.append(to_string((uint8_t)byteArray[i]));
            if (i != 3)
                message.push_back('.');
        }
        message.push_back('/');
        message.append(to_string((uint8_t)byteArray[4]));
        message.push_back(':');
        uint32_t cost = 0;
        cost |= byteArray[5] << 24;
        cost |= byteArray[6] << 16;
        cost |= byteArray[7] << 8;
        cost |= byteArray[8];
        cost = ntohl(cost);
        message.append(to_string(cost));
        return message;
    }
    void sendEcho(int sockfd)
    {
        char echo_message[9];
        encodeToBytes("8.8.8.8/24", 2137, echo_message);
        decodeFromBytes(echo_message);
        struct sockaddr_in server_address;
        bzero(&server_address, sizeof(server_address));
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(54321);
        ssize_t message_len = 9;
        for (auto &elem : interfaces)
        {
            inet_pton(AF_INET, elem.calculateBroadcast().c_str(), &server_address.sin_addr);
            if (sendto(sockfd, echo_message, message_len, 0, (struct sockaddr *)&server_address, sizeof(server_address)) != message_len)
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