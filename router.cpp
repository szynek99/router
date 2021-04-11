#include <iostream>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include "utility.h"
#include "ipAddress.h"
#include <unordered_map>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>

#define INF_REACH 6000
#define ROUND_TIME_MSEC 15000
#define ROUND_TIME_SEC 15

#define bzero(b, len) (memset((b), '\0', (len)), (void)0)

using namespace std;

class Router
{
private:
    vector<ipAddress> interfaces;
    unordered_map<string, pair<uint32_t, string>> r_table;

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
    void processRoutingInformation(string message, string sender_ip)
    {
        vector<string> message_elems = tokenize(message, ":");
        string network_addr = message_elems[0];
        uint32_t cost = stoi(message_elems[1]);
        auto search = r_table.find(network_addr);
        if (search != r_table.end())
        {
            pair<uint32_t, string> entry_found = search->second;
            if (cost >= INF_REACH)
            {
                if (entry_found.second.compare(sender_ip) == 0)
                {
                    entry_found.first = INF_REACH;
                }
            }
            else if (cost < entry_found.first)
            {
                r_table.insert({network_addr, pair<uint32_t, string>(cost, sender_ip)});
            }
        }
        else
        {
            r_table.insert({network_addr, pair<uint32_t, string>(cost, sender_ip)});
        }
    }
    void sendRoutingTable(int sockfd)
    {
        char message[9];
        ssize_t message_len = 9;

        struct sockaddr_in server_address;
        bzero(&server_address, sizeof(server_address));
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(54321);

        for (auto &elem : r_table)
        {
            encodeToBytes(elem.first, elem.second.first, message);
            for (auto &elem : interfaces)
            {
                inet_pton(AF_INET, elem.calculateBroadcast().c_str(), &server_address.sin_addr);
                if (sendto(sockfd, message, message_len, 0, (struct sockaddr *)&server_address, sizeof(server_address)) != message_len)
                {
                    throw runtime_error("sendto");
                }
            }
        }
    };

public:
    Router()
    {
        interfaces = vector<ipAddress>();
        r_table = unordered_map<string, pair<uint32_t, string>>();
    }
    void addInterface(ipAddress i_new, uint32_t cost)
    {
        interfaces.push_back(i_new);
        string ip_network = i_new.calculateNetworkToCidr();
        auto search = r_table.find(ip_network);
        if (search != r_table.end())
        {
            pair<uint32_t, string> entry_found = search->second;
            if (entry_found.first > cost)
            {
                r_table.insert({ip_network, pair<uint32_t, string>(cost, i_new.getLocalhost())});
            }
        }
        else
        {
            r_table.insert({ip_network, pair<uint32_t, string>(cost, i_new.getLocalhost())});
        }
    };
    void removeInteface(ipAddress i);

    void run(int sockfd)
    {
        struct sockaddr_in server_address;
        bzero(&server_address, sizeof(server_address));
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(54321);
        server_address.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
            throw runtime_error("bind");
        long long begin_time = current_timestamp();
        long long round_time = 0;
        while (true)
        {
            fd_set descriptors;
            FD_ZERO(&descriptors);
            FD_SET(sockfd, &descriptors);
            struct timeval tv;
            tv.tv_sec = ROUND_TIME_SEC;
            tv.tv_usec = 0;
            int ready = select(sockfd + 1, &descriptors, NULL, NULL, &tv);

            if (ready < 0)
                throw runtime_error("select");
            else if (ready == 0)
            {
                begin_time = current_timestamp();
                round_time = 0;
                sendRoutingTable(sockfd);
                continue;
            }
            long long curr_time = current_timestamp();

            struct sockaddr_in sender;
            socklen_t sender_len = sizeof(sender);
            char buffer[9];

            ssize_t datagram_len = recvfrom(sockfd, buffer, IP_MAXPACKET, 0, (struct sockaddr *)&sender, &sender_len);
            round_time += curr_time - begin_time;
            if (datagram_len < 0)
            {
                throw runtime_error("recfrom");
            }
            
            char sender_ip_str[20];
            string message = decodeFromBytes(buffer);
            inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));
            processRoutingInformation(message, sender_ip_str);

            if (round_time >= ROUND_TIME_MSEC)
            {
                sendRoutingTable(sockfd);
                begin_time = current_timestamp();
                round_time = 0;
            }
        }
    }
};
int main()
{
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

    try
    {
        router.run(sockfd);
    }
    catch (const exception &e)
    {
        cerr << "error: " << e.what() << "\n";
    }

    close(sockfd);
    return 0;
}