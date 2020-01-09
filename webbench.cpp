#include <iostream>
#include <stdio.h>
#include <string>
using namespace std;

enum Method
{
    GET,
    POST,
    HEAD
};

string buildRequest(string url, Method method);



int main(int argc, char** argv)
{
    buildRequest("http://127.0.0.1:8080", GET);

    return 0;
}

string buildRequest(string url, Method method)
{
    if (0 != url.find("http://"))
    {
        cout << "invalid url: " << url << endl;
    }
    if (url.length() > 1500)
    {
        cout << "url is too long, length: " << url.length() << endl;
    }

    string request = "";
    switch(method)
    {
        case GET:
            request += "GET";
        break;
        case POST:
        break;
        case HEAD:
        break;
    }
    request += " ";

    string uri = url.substr(url.find("://") + 3, url.length() - 7);
    cout << uri << endl;

    string host = "";
    string port = "";
    // 默认不使用代理
    if (url.npos != uri.find(":") && )
    return request;
}