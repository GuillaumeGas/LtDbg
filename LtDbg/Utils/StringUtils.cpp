#include "StringUtils.hpp"

using namespace std;

vector<string> StringUtils::Split(string str, char delimiter)
{
	size_t pos = 0;
	vector<string> res;
	while ((pos = str.find(delimiter)) != string::npos)
	{
		res.push_back(str.substr(0, pos));
		str.erase(0, pos + 1);
	}
	res.push_back(str);
	return res;
}