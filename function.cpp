#include <function.h>

string cutPre(string stream, const string &str)
{
    int nPos = stream.find(str);
    if (nPos != -1)
    {
        stream = stream.substr(0, nPos);
    }
    return stream;
}

string cutNext(string stream, const string &str)
{
    int nPos = stream.find(str);

    if (nPos != -1)
    {
        stream = stream.substr(nPos + str.size(), stream.size());
    }
    return stream;
}
