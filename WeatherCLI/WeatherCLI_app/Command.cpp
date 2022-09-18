/*
 * Command.cpp
 * Author: Aikaterini Karanasiou
 * It includes classes that represent the commands on the Weather Command Line app.
 */

#include "Command.h"

/* Command (Base) Class */
Command::Command()
{
    db = DatabaseWeather::GetInstance();
}

Command::~Command()
{

}

bool Command::currentDate(tm &date)
{
    time_t current_time;
    current_time = time(NULL);
    tm *tm_gmt = gmtime(&current_time);
    if(tm_gmt==nullptr)
        return false;
    date = *tm_gmt;
return true;
}

/* Control Commands Class*/
ControlCommands::~ControlCommands()
{
    if(current_Cmd)
    {
        delete current_Cmd;
        current_Cmd = nullptr;
    }
}

void ControlCommands::setCommand(Command *cmd)
{
    current_Cmd = cmd;
}

void ControlCommands::Execute()
{
    if(current_Cmd)
        current_Cmd->Execute();
}

/* Command Derived Classes */
DisplayCurrentWeather::DisplayCurrentWeather()
{
}

void DisplayCurrentWeather::Execute()
{
    double current_temperature;
    // Find the current date
    tm t;
    bool isCurrDateExists = currentDate(t);
    if(isCurrDateExists)
    {
        // Find the weather temperature for the current date
        int result_query = db->readDailyTemperatureFromDB(t.tm_year+1900, t.tm_mon+1, t.tm_mday, current_temperature);
        if(result_query != SQLITE_OK)
            cout << endl << "Failed to display the current weather in Athens." << endl;
        else
            cout << endl << "Weather temperature in Athens is: " << current_temperature << endl;
    }
    else
    {
        cout << endl << "Failed to display the current weather in Athens." << endl;
    }
}

/* Derived class for the -compare command*/
CompareWeather::CompareWeather(string timePeriod) : time_period(timePeriod)
{
}

void CompareWeather::Execute()
{
    double average_temperature, current_temperature;
    // Assign number of entries (hours) on the table that correspond to the last period (year, month or week)
    int entries_of_last_period = 0;
    if(time_period.compare("year") == 0)
        entries_of_last_period = 365*24;
    else if(time_period.compare("month") == 0)
        entries_of_last_period = 30*24;
    else if(time_period.compare("week") == 0)
        entries_of_last_period = 7*24;

    // Find average weather temperature of the last period (year, month or week) in Athens
    int result_query_avg_temp = db->readAverageLastTemperatureFromDB(entries_of_last_period, average_temperature);
    // Find the current weather date
    tm t;
    bool isCurrDateExists = currentDate(t);
    if (isCurrDateExists)
    {
        // Find the current weather temperature in Athens
        int results_query_daily_temp = db->readDailyTemperatureFromDB(t.tm_year+1900, t.tm_mon+1, t.tm_mday, current_temperature);
        // Compare average and current weather temperature in Athens
        if(result_query_avg_temp == SQLITE_OK && results_query_daily_temp == SQLITE_OK)
        {
            if(average_temperature > current_temperature)
                cout << endl << "Current Weather temperature in Athens (" << current_temperature << " Celsius) is lower than the average temperature of last " << time_period << " (" << average_temperature << " Celsius)" << endl;
            else if(average_temperature < current_temperature)
                cout << endl << "Current Weather temperature in Athens (" << current_temperature << " Celsius) is higher than the average temperature of last " << time_period << " (" << average_temperature << " Celsius)" << endl;
            else
                cout << endl << "Current Weather temperature in Athens (" << current_temperature << " Celsius) is equal to the average temperature of last " << time_period << " (" << average_temperature << " Celsius)" << endl;
        }
        else
        {
            cout << endl << "Failed to compare the current weather temperature in Athens with the average temperature of last " << time_period << endl;
        }
    }
    else
    {
        cout << endl << "Failed to compare the current weather temperature in Athens with the average temperature of last " << time_period << endl;
    }
}

/* Derived class for the -average command*/
DisplayAverageWeather::DisplayAverageWeather(int Month, int Year) : month(Month), year(Year)
{
}

void DisplayAverageWeather::Execute()
{
    double average_temperature;
    // Find the average weather temperature of a specific month and year
    int result_query = db->readAverageMonthlyTemperatureFromDB(year, month, average_temperature);
    if(result_query != SQLITE_OK)
        cout << endl << "Failed to display the average weather in Athens for Year = " << year << " and Month = " << month << endl;
    else
        cout << endl << "Average Weather temperature in Athens on Year = " << year << " and Month = " << month << " is " << average_temperature << endl;
}

