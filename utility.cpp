#include <iostream>
#include <string>
#include <vector>
#include <sys/time.h>

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
long long current_timestamp()
{
	struct timeval te;
	gettimeofday(&te, NULL);
	long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000;
	return milliseconds;
}