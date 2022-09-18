/*
 *
 * ConnectionWeatherAPI.cpp
 * Author: Aikaterini Karanasiou
 * It provides functions for connecting to the Open Meteo API (https://open-meteo.com/en).
 *
 */

#include "ConnectionWeatherAPI.h"
#include "DatabaseWeather.h"

ConnectionWeatherAPI::ConnectionWeatherAPI(){
}

ConnectionWeatherAPI::~ConnectionWeatherAPI() {
}

size_t ConnectionWeatherAPI::callback(const char* inputJSONdata, size_t size, size_t nmemb, string* outputJSONdata)
{
    const size_t realSize(size * nmemb);
    outputJSONdata->append(inputJSONdata, realSize);
    return realSize;
}

void ConnectionWeatherAPI::SendGETrequest(CURL* curl, string url, const string &receivedData, long &result_httpCode, CURLcode &result_curlCode)
{

    // Connect to the URL and wait maximum 20 secs for receiving the reply
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &receivedData);

    result_curlCode = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &result_httpCode);

}

void ConnectionWeatherAPI::GetHourlyAverageTemperature(string prefixUrl, string startDate, string endDate){

    string str_date;
    string str_temperature;

    cout << endl << "** GetHourlyAverageTemperature() function called **" << endl;

    DatabaseWeather *db = DatabaseWeather::GetInstance();

    // URL that includes information for the hourly temperature in Athens
    const string url(prefixUrl + startDate + "&end_date=" + endDate + "&hourly=temperature_2m");

    // Initialize a new curl session
    CURL* curl = curl_easy_init();
    if(curl!=NULL)
    {
        // Send HTTP Get request to the Weather API for receiving data
        long result_httpCode(0);
        CURLcode result_curlCode(CURLE_UNSUPPORTED_PROTOCOL);
        string receivedData;
        SendGETrequest(curl, url, receivedData, result_httpCode, result_curlCode);

        curl_easy_cleanup(curl);

        // Check the reply from the Weather API
        if (result_httpCode == 200 && result_curlCode == CURLE_OK)
        {
            cout << endl << "Response received successfully from the Weather API." << endl;
            Json::Value jsonValues;
            Json::Reader jsonReader;
            // Parse JSON data of the reply
            if (jsonReader.parse(receivedData, jsonValues))
            {
                cout << endl << "JSON data were parsed successfully." << endl;
                const Json::Value& temperatures = jsonValues["hourly"]["temperature_2m"];
                const Json::Value& dates = jsonValues["hourly"]["time"];
                for (unsigned int i = 0; i < temperatures.size(); i++)
                {
                    // Check if the JSON entry has both temperature and date values
                    if(!temperatures[i].isNull() && !dates[i].isNull())
                    {
                        // Store temperature
                        str_temperature = temperatures[i].asString();
                        cout << endl << str_temperature << endl;
                        // Parse and store the date
                        str_date = dates[i].asString();
                        tm time;
                        memset(&time, 0, sizeof(struct tm));
                        strptime(str_date.c_str(), "%Y-%m-%eT%H:%M", &time);
                        db->writeTemperatureDataInDB(time.tm_year+1900, time.tm_mon+1, time.tm_mday, time.tm_hour, str_temperature, true);
                    }
                }
            }
            else
            {
                cerr << endl << "Failed to parse response from the Weather API." << endl;
            }
        }
        else
        {
            cerr << endl << "Failed to receive response from the Weather API." << endl;
        }
    }

}

void ConnectionWeatherAPI::GetCurrentTemperature(){

    cout << endl << "** GetCurrentTemperature() function called **" << endl;

    DatabaseWeather *db = DatabaseWeather::GetInstance();

    // URL that includes information for current temperature in Athens
    const string url("https://api.open-meteo.com/v1/forecast?latitude=37.9792&longitude=23.7166&current_weather=TRUE");
    // Initialize a new curl session
    CURL* curl = curl_easy_init();
    if(curl!=NULL)
    {
        // Send HTTP Get request to the Weather API for receiving data
        long result_httpCode(0);
        CURLcode result_curlCode(CURLE_UNSUPPORTED_PROTOCOL);
        string receivedData;
        SendGETrequest(curl, url, receivedData, result_httpCode, result_curlCode);

        // Parse JSON data of the reply
        if (result_httpCode == 200 && result_curlCode == CURLE_OK)
        {
            cout << endl << "Response received successfully from the Weather API." << endl;
            Json::Value jsonValues;
            Json::Reader jsonReader;
            if (jsonReader.parse(receivedData, jsonValues))
            {
                // Store temperature
                cout << endl << "JSON data were parsed successfully." << endl;
                const string temperature(jsonValues["current_weather"]["temperature"].asString());
                // Parse and Store the date
                const string date(jsonValues["current_weather"]["time"].asString());
                tm time;
                memset(&time, 0, sizeof(struct tm));
                strptime(date.c_str(), "%Y-%m-%eT%H:%M", &time);
                db->writeTemperatureDataInDB(time.tm_year+1900, time.tm_mon+1, time.tm_mday, -1, temperature, false);
            }
            else
            {
                cerr << endl << "Failed to parse response from the Weather API." << endl;
            }
        }
        else
        {
            cerr << endl << "Failed to receive response from the Weather API." << endl;
        }
    }

}
