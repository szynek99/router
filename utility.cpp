#include <iostream>
#include <string>
#include <vector>

using namespace std;

vector<string> tokenize(string s, string del)
{
    int start = 0;
    int end = s.find(del);
    vector<string> result;
    while (end != -1) {
        result.push_back(s.substr(start, end - start));
        start = end + del.size();
        end = s.find(del, start);
    }
    result.push_back(s.substr(start, end - start));
    return result;
}
string formatSubnet(uint32_t bits)
{
    uint64_t ip = ~((1 << (32 - bits)) - 1);
    unsigned char bytes[4];
    bytes[0] = ip & 0xFF;
    bytes[1] = (ip >> 8) & 0xFF;
    bytes[2] = (ip >> 16) & 0xFF;
    bytes[3] = (ip >> 24) & 0xFF;
    string result = "";
    for (int i = 3; i >= 0; i--)
    {
        result.append(to_string(bytes[i]));
        if (i != 0)
            result.append(".");
    }
    return result;
}
string calculateBroadcast(string currentIP, string ipNetMask)
{
    vector<string> arrIP = tokenize(currentIP, ".");
    vector<string> arrMask = tokenize(ipNetMask, ".");
    string result = "";

    for (int i = 0; i < 4; i++)
    {
        uint16_t byte = stoi(arrIP[i]) | (stoi(arrMask[i]) ^ 255);
        result.append(to_string(byte));
        if (i != 3)
            result.append(".");
    }
    return result;
}
