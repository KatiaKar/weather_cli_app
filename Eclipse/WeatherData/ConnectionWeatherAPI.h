/*
 *
 * ConnectionWeatherAPI.h
 * Author: Aikaterini Karanasiou
 * It provides functions for connecting to the Open Meteo API (https://open-meteo.com/en).
 *
 */
#ifndef CONNECTIONWEATHERAPI_H_
#define CONNECTIONWEATHERAPI_H_
#include <iostream>
#include <string>
#include <memory>
#include <curl/curl.h>
#include <json/json.h>
#include <string.h>
#include <cstdlib>
#include <ctime>

using namespace std;

class ConnectionWeatherAPI {
private:
    const string URL;
public:
    ConnectionWeatherAPI();
    virtual ~ConnectionWeatherAPI();

    /*
     * It receives all the JSON data of one connection.
     * Returns: The total size of the received data.
     */
    static size_t callback(const char* inputJSONdata, size_t size, size_t nmemb, std::string* outputJSONdata);
    /*
     * Arguments: (1) curl points to the new curl session, (2) url is the HTTP address that is used for receiving weather data, (3) receivedData is the reply (with JSON format) from GET HTTP request
     * (4) httpCode is the reply code from the HTTP protocol, (5) curlCode is the received reply code after a HTTP response has been received.
     * It sends and HTTP GET request to weather API and waits for a reply
     */
    static void SendGETrequest(CURL* curl, string url, const string &receivedData, long &result_httpCode, CURLcode &result_curlCode);
    /*
     * Arguments: (1) prefixUrl: is the address of the Weather API. startDate and endDate define the amount of weather data that should be retrieved and stored.
     * It sends HTTP GET requests to a Weather API for requesting the hourly temperature from historical data and it stores them to the database.
     *
     */
    static void GetHourlyAverageTemperature(string prefixUrl, string startDate, string endDate);
    /*
     * It sends HTTP GET requests to a Weather API for requesting the daily temperature in Athens and stores this information to the DB
     */
    static  void GetCurrentTemperature();
};

#endif /* CONNECTIONWEATHERAPI_H_ */
