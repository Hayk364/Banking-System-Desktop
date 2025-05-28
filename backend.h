#ifndef BACKEND_H
#define BACKEND_H

#include <filesystem>
#include <memory>
#include <coroutine>
#include <mutex>
#include <thread>
#include <unistd.h>
#include <cstring>
#include <dirent.h>
#include <semaphore>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <atomic>
#include <future>
#include <curl/curl.h>
#include <map>
#include <QString>
#include "json.hpp"
#include <variant>
#include <QStringList>

struct DataBody{
    std::string login;
    std::string password;
};

struct TokenBody{
    bool success;
    std::string token;
    std::string nameAndSurname;
    int id;

    TokenBody (bool s,std::string t,int i) : success(s),token(t),id(i){}
};

struct MainBody{
    std::string NameAndSurname;
    std::string token;
    float balance;

    MainBody(std::string nAs,std::string t,float b) : NameAndSurname(nAs), token(t), balance(b){}
};

class Backend
{
public:
    Backend();
    static TokenBody Login(const std::string& username,const std::string& password);
    static TokenBody Register(const std::string& username,const std::string& password);
    static bool Createwallet(const std::string& nameAndsurname,const std::string& email);
    static MainBody GetMainBody();
    static std::variant<bool,std::string> Send(const std::string& token,const float count);
    static std::string GetNameSurname(const int id);
    static float UpdateBalance(const int id);
    static QStringList GetHistoryList(const int userId,const bool isUserList);
    static std::string GetNameForId(const int id);

private:
    static CURL* curl;
    static struct curl_slist* headers;
    static bool LoginForAdmin(const std::string& username,const std::string& password);
};

#endif // BACKEND_H
