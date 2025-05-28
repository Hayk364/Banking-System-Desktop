#include "backend.h"
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
#include <QSettings>
#include "json.hpp"
#include <variant>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>
#include <QStringList>


using json = nlohmann::json;

Backend::Backend() {}

size_t WriteCallBack(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

TokenBody Backend::Register(const std::string& username,const std::string& password){
    CURL* curl = curl_easy_init();

    if(!curl) return TokenBody(false,"",-1);
    std::string response;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers,"Content-Type: application/json");

    json j;
    j["username"] = username;
    j["password"] = password;

    std::string jsonStr = j.dump();

    curl_easy_setopt(curl,CURLOPT_URL,"http://127.0.0.1:4000/register");
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,WriteCallBack);
    curl_easy_setopt(curl,CURLOPT_WRITEDATA,&response);
    curl_easy_setopt(curl,CURLOPT_POSTFIELDS,jsonStr.c_str());
    curl_easy_setopt(curl,CURLOPT_POSTFIELDSIZE,jsonStr.size());
    curl_easy_setopt(curl,CURLOPT_HTTPHEADER,headers);

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        curl_easy_cleanup(curl);
        return TokenBody(false,"",-1);
    }
    json result = json::parse(response);

    TokenBody userResult(result["success"],result["token"],result["id"]);
    curl_easy_cleanup(curl);

    return userResult;

}

TokenBody Backend::Login(const std::string& username,const std::string& password){
    CURL* curl = curl_easy_init();
    if(!curl) return TokenBody(false,"",-1);
    std::string response;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers,"Content-Type: application/json");

    json j;
    j["username"] = username;
    j["password"] = password;

    std::string jsonStr = j.dump();

    curl_easy_setopt(curl,CURLOPT_URL,"http://127.0.0.1:4000/login");
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,WriteCallBack);
    curl_easy_setopt(curl,CURLOPT_WRITEDATA,&response);
    curl_easy_setopt(curl,CURLOPT_POSTFIELDS,jsonStr.c_str());
    curl_easy_setopt(curl,CURLOPT_POSTFIELDSIZE,jsonStr.size());
    curl_easy_setopt(curl,CURLOPT_HTTPHEADER,headers);

    CURLcode res = curl_easy_perform(curl);

    if(res != CURLE_OK) {
        curl_easy_cleanup(curl);
        return TokenBody(false,"",-1);
    }
    json result = json::parse(response);

    TokenBody userResult(result["success"],result["token"],result["id"]);
    curl_easy_cleanup(curl);
    return userResult;
}

bool Backend::LoginForAdmin(const std::string& username,const std::string& password){
    CURL* curl = curl_easy_init();
    if(!curl) return false;
    std::string response;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers,"Content-Type: application/json");

    json j;
    j["username"] = username;
    j["password"] = password;

    std::string jsonStr = j.dump();

    curl_easy_setopt(curl,CURLOPT_URL,"http://127.0.0.1:4000/login");
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,WriteCallBack);
    curl_easy_setopt(curl,CURLOPT_WRITEDATA,&response);
    curl_easy_setopt(curl,CURLOPT_POSTFIELDS,jsonStr.c_str());
    curl_easy_setopt(curl,CURLOPT_POSTFIELDSIZE,jsonStr.size());
    curl_easy_setopt(curl,CURLOPT_HTTPHEADER,headers);

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        curl_easy_cleanup(curl);
        return false;
    }
    json result = json::parse(response);
    curl_easy_cleanup(curl);
    return result["success"];
}

bool Backend::Createwallet(const std::string& nameAndsurname,const std::string& email){
    CURL* curl = curl_easy_init();
    if(!curl) return false;
    std::string response;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers,"Content-Type: application/json");

    json j;
    j["nameAndsurname"] = nameAndsurname;
    j["email"] = email;

    std::string jsonStr = j.dump();

    curl_easy_setopt(curl,CURLOPT_URL,"http://127.0.0.1:4000/create-wallet");
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,WriteCallBack);
    curl_easy_setopt(curl,CURLOPT_WRITEDATA,&response);
    curl_easy_setopt(curl,CURLOPT_POSTFIELDS,jsonStr.c_str());
    curl_easy_setopt(curl,CURLOPT_POSTFIELDSIZE,jsonStr.size());
    curl_easy_setopt(curl,CURLOPT_HTTPHEADER,headers);

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        curl_easy_cleanup(curl);
        return false;
    }

    json result = json::parse(response);

    bool success = result["success"];
    curl_easy_cleanup(curl);
    return success;
}


MainBody Backend::GetMainBody() {
    CURL* curl = curl_easy_init();
    if (!curl) return MainBody("", "", 0.0);

    QSettings settings("LuX", "Bank");

    std::string response;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    json j;
    j["username"] = settings.value("username").toString().toStdString();
    std::string jsonStr = j.dump();

    curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:4000/get-user-data");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallBack);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jsonStr.size());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);  // Освобождаем ресурсы

    if (res != CURLE_OK) {
        std::cerr << "Ошибка curl: " << curl_easy_strerror(res) << std::endl;
        return MainBody("", "", 0.0);
    }

    try {
        json result = json::parse(response);

        std::string token = result.contains("token") ? result["token"].get<std::string>() : "";
        float balance = result.contains("balance") ? result["balance"].get<float>() : 0.0;

        MainBody data(
            settings.value("nameAndSurname").toString().toStdString(),
            token,
            balance
            );

        std::cout << "Token: " << token << std::endl;
        return data;

    } catch (std::exception& e) {
        std::cerr << "Ошибка при разборе JSON: " << e.what() << std::endl;
        return MainBody("", "", 0.0);
    }
}

std::variant<bool,std::string> Backend::Send(const std::string& token,const float count){
    CURL* curl = curl_easy_init();
    if(!curl) return false;
    std::string response;

    QSettings settings("LuX","Bank");

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers,"Content-Type: application/json");
    json j;
    j["token"] = token;
    j["count"] = count;
    j["id"] = settings.value("id").toInt();
    std::cout << settings.value("id").toInt() << std::endl;

    std::string jsonStr = j.dump();

    curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:4000/send");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallBack);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jsonStr.size());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);  // Освобождаем ресурсы

    if (res != CURLE_OK) {
        std::cerr << "Ошибка curl: " << curl_easy_strerror(res) << std::endl;
        return false;
    }

    try{
        json result = json::parse(response);

        std::string error = result.contains("error") ? result["error"].get<std::string>() : "";
        bool success = result.contains("success") ? result["success"].get<bool>() : false;

        if(success){
            return true;
        }else{
            return error;
        }

    }catch(const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
        return false;
    }

}


std::string Backend::GetNameSurname(const int id){
    CURL* curl = curl_easy_init();
    if(!curl) return "";
    std::string response;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers,"Content-Type: application/json");

    json j;
    j["id"] = id;

    std::string jsonStr = j.dump();

    curl_easy_setopt(curl,CURLOPT_URL,"http://127.0.0.1:4000/get-name-surname");
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,WriteCallBack);
    curl_easy_setopt(curl,CURLOPT_WRITEDATA,&response);
    curl_easy_setopt(curl,CURLOPT_POSTFIELDS,jsonStr.c_str());
    curl_easy_setopt(curl,CURLOPT_POSTFIELDSIZE,jsonStr.size());
    curl_easy_setopt(curl,CURLOPT_HTTPHEADER,headers);

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        curl_easy_cleanup(curl);
        return "";
    }

    json result = json::parse(response);

    std::string name = result["name"];
    curl_easy_cleanup(curl);
    return name;
}

float Backend::UpdateBalance(const int id){
    QNetworkAccessManager manager;
    QNetworkRequest request(QUrl("http://127.0.0.1:4000/get-balance/" + QString::number(id)));
    QNetworkReply* reply = manager.get(request);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QByteArray response = reply->readAll();
    QJsonDocument json = QJsonDocument::fromJson(response);
    float balance = json["balance"].toDouble();

    reply->deleteLater();
    return balance;
}

std::string Backend::GetNameForId(const int id){
    CURL* curl = curl_easy_init();
    if(!curl) return "";
    std::string response;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers,"Content-Type: application/json");

    json j;
    j["id"] = id;

    std::string jsonStr = j.dump();

    curl_easy_setopt(curl,CURLOPT_URL,"http://127.0.0.1:4000/get-name-id");
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,WriteCallBack);
    curl_easy_setopt(curl,CURLOPT_WRITEDATA,&response);
    curl_easy_setopt(curl,CURLOPT_POSTFIELDS,jsonStr.c_str());
    curl_easy_setopt(curl,CURLOPT_POSTFIELDSIZE,jsonStr.size());
    curl_easy_setopt(curl,CURLOPT_HTTPHEADER,headers);

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        curl_easy_cleanup(curl);
        return "";
    }

    json result = json::parse(response);

    std::string name = result["name"];
    curl_easy_cleanup(curl);
    return name;
}

QStringList Backend::GetHistoryList(const int userId,const bool isUserList){
    CURL* curl = curl_easy_init();
    if(!curl) return {};
    std::string response;
    QStringList transaction = {};
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers,"Content-Type: application/json");

    json j;
    j["id"] = userId;

    std::string jsonStr = j.dump();

    curl_easy_setopt(curl,CURLOPT_URL,"http://127.0.0.1:4000/get-history");
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,WriteCallBack);
    curl_easy_setopt(curl,CURLOPT_WRITEDATA,&response);
    curl_easy_setopt(curl,CURLOPT_POSTFIELDS,jsonStr.c_str());
    curl_easy_setopt(curl,CURLOPT_POSTFIELDSIZE,jsonStr.size());
    curl_easy_setopt(curl,CURLOPT_HTTPHEADER,headers);

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        curl_easy_cleanup(curl);
        return {};
    }

    json result = json::parse(response);

    for(auto& line : result){
        if(isUserList){
            if(line["senderId"] == userId){
                QString li = QString("%1 -> %2")
                .arg(QString::number(line["count"].get<double>()))
                    .arg(QString::fromStdString(GetNameForId(line["getterId"])));
                transaction << li;
            }
        } else {
            if(line["getterId"] == userId){
                QString li = QString("%1 <- %2")
                .arg(QString::number(line["count"].get<double>()))
                    .arg(QString::fromStdString(GetNameForId(line["senderId"])));
                transaction << li;
            }
        }
    }

    curl_easy_cleanup(curl);
    return transaction;
}



