#include <iostream>
#include <string>
#include <vector>
#ifndef UTILITY_H
#define UTILITY_H


std::string DecimalToBinaryString(int a);
std::string formatSubnet(uint32_t decimal);
std::string calculateBroadcast(std::string currentIP, std::string ipNetMask);
std::vector<std::string> tokenize(std::string s, std::string del);
#endif