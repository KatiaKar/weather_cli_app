/*
 * main.cpp
 * Author: Aikaterini Karanasiou
 */
#include <string.h>
#include "DatabaseWeather.h"
#include "ConnectionWeatherAPI.h"
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

boost::asio::io_service io;
boost::posix_time::hours expiration_time(24);
boost::asio::deadline_timer timer(io, expiration_time);

// t is used for storing the latest current date
static tm t;
static bool isCurrDateExists = false;
/*
 *
 * It finds the current GMT time
 * Returns: the current GMT time (tm type)
 *
 */
bool currentDate(tm &tmp)
{
    time_t current_time;
    current_time = time(NULL);
    tm *tm_gmt = gmtime(&current_time);
    if(tm_gmt==nullptr)
        return false;
    tmp = *tm_gmt;
return true;
}

/*
 *
 * Converts the time from tm to string type
 * Returns: the time with string type
 *
 */
string convertTMtoString(tm &t){
    string strDate = to_string(t.tm_year+1900) + "-";
    if(t.tm_mon+1 < 10)
        strDate = strDate + "0" + to_string(t.tm_mon+1) + "-";
    else
        strDate = strDate + to_string(t.tm_mon+1) + "-";

    if(t.tm_mday < 10)
        strDate = strDate + "0" + to_string(t.tm_mday) + "-";
    else
        strDate = strDate + to_string(t.tm_mday);

    return strDate;
}


void updateDBtables()
{
    string prefixUrl;
    string middleDate, startDate, endDate;
    // Convert the last date (startDate) from tm to string type
    if(isCurrDateExists==true)
        startDate = convertTMtoString(t);
    else
        startDate = "2021-01-01";

    // Find the current date (endDate) and convert it from tm to string type
    tm tmp;
    isCurrDateExists = currentDate(tmp);
    if(isCurrDateExists==true)
    {
        t = tmp;
        endDate = convertTMtoString(t);
        cout << endl << "############################## STORE CURRENT TEMPERATURE ##############################" << endl;
        ConnectionWeatherAPI::GetCurrentTemperature();
        cout << endl << "############################## STORE AVERAGE TEMPERATURE ##############################" << endl;
        // if timer is expired for the first time
        if(!startDate.compare("2021-01-01"))
        {
            // I observed that the open meteo weather website provides two APIS: one for Weather Historical data and one for the Weather Forecast data. Historical data API does not
            // provide data for the latest days. Due to this I used also the Weather Forecast API. So, I extracted data from two URLs. See web page here --> https://open-meteo.com/en/docs
            tm t_middle = t;
            if(t_middle.tm_mon==1)
                t_middle.tm_mon = 12;
            else
                t_middle.tm_mon = t_middle.tm_mon - 1;
            middleDate = convertTMtoString(t_middle);

            cout << endl << "Start date: " << startDate << endl;
            cout << "Middle date: " << middleDate << endl;
            cout << "End date: " << endDate << endl;
            prefixUrl = "https://archive-api.open-meteo.com/v1/era5?latitude=37.9792&longitude=23.7166&start_date=";
            ConnectionWeatherAPI::GetHourlyAverageTemperature(prefixUrl, startDate, middleDate);

            prefixUrl = "https://api.open-meteo.com/v1/forecast?latitude=52.52&longitude=13.41&start_date=";
            ConnectionWeatherAPI::GetHourlyAverageTemperature(prefixUrl, middleDate, endDate);
        }
        else
        {
            cout << endl << "Start date: " << startDate << endl;
            cout << "End date: " << endDate << endl;
            prefixUrl = "https://api.open-meteo.com/v1/forecast?latitude=52.52&longitude=13.41&start_date=";
            ConnectionWeatherAPI::GetHourlyAverageTemperature(prefixUrl, startDate, endDate);
        }
    }
    else
    {
        cerr << endl << "Failed to update the Database. Current data was not received. " << endl;
    }
}

void action(const boost::system::error_code& /*e*/){
    // Update the tables on database every day (timer is expired every 24 hours)
    cout << endl << "############################## TIMER EXPIRED ##############################" << endl;
    updateDBtables();
    // Reinitialize the expiration timer in order to be expired after 24 hour
    timer.expires_at(timer.expires_at() + expiration_time);
    timer.async_wait(action);
}

int main(){
    // Create the database and the tables for storing hourly and daily temperatures
    DatabaseWeather *db = DatabaseWeather::GetInstance();
    db->createDatabaseAndTables();
    updateDBtables();

    // Check and store the current temperature every 24 hours
    // Update the table with historical data every 24 hours (hourly temperatures of last day)
    timer.async_wait(action);
    io.run();

    DatabaseWeather::DeleteInstance();
    return 0;
}

