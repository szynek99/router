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
