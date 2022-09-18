/*
 * DatabaseWeather.h
 * Author: Aikaterini Karanasiou
 * It provides functions for creating a database and tables as well as functions for reading/writing on the tables.
 */

#ifndef DATABASEWEATHER_H_
#define DATABASEWEATHER_H_

#include <iostream>
#include <string>
#include <sqlite3.h>

using namespace std;

class DatabaseWeather {
private:
    DatabaseWeather(const string dbName);
    static DatabaseWeather *dbInstance;
    static double res_temperature;
    string db_name;
    sqlite3 *DB;
public:
    // this singleton instance should not be cloned
    DatabaseWeather(DatabaseWeather &otherInstance) = delete;
    // this singleton instance should not be assigned
    void operator=(const DatabaseWeather &) = delete;
    // It creates a singleton instance
    static DatabaseWeather *GetInstance();
    static void DeleteInstance();
    /*
     * It creates two tables: one table for storing the hourly temperatures
     * and one table for storing the daily temperature in Athens.
     *
     * Returns: It returns either an error code (due to failure of opening the DB or creating a table) or
     * the successful result code of SQL queries
     */
    int createDatabaseAndTables();
    /*
     * Arguments:  day, month, year and each hour of the day. hour is equal to -1 when it constructs INSERT SQL query the current temperature in Athens
     *             temperature: is the hourly average temperature of Athens or the current temperature in Athens
     *             isAVGnetry: is TRUE if it sends SQL INSERT query for the hourly temperature
     *
     * It inserts data either to the table with the hourly temperatures
     * or to the table with the daily temperature.
     *
     * Returns: It returns either an error code (due to failure of opening the DB or inserting data to the table) or
     * a successful result code of INSERT SQL query
     */
    int writeTemperatureDataInDB(int year, int month, int day, int hour, string temperature, bool isAVGnetry);
    /*
     * It stores data of the SELECT SQL query. This function is used as an argument on the sqlite3_exec() function.
     * sqlite3_exec is called by readAverageMonthlyTemperatureFromDB(), readAverageLastTemperatureFromDB() and
     * readCurrentTemperatureFromDB() functions. Return value of the SELECT Query is a double value.
     *
     */
    static int doubleResultFromSelectQuery(void* data, int argc, char** argv, char** resName);
    /*
     * It stores data of the SELECT SQL query. This function is used as an argument on the sqlite3_exec() function.
     * sqlite3_exec is called by readAverageLastTemperatureFromDB()function. Return value of the SELECT Query is a integer value.
     *
     */
    static int intResultFromSelectQuery(void* data, int argc, char** argv, char** resName);
    /*
     * Arguments: (1) year, month, and day. These arguments are defined by the user’s action.
     *            (2) current_temperature: It is the result of a SELECT query.
     *
     * It finds and prints the current temperature in Athens.
     *
     * Returns: It returns either an error code (due to failure of opening the DB or extracting data from the table) or
     * a successful result code of SELECT SQL query
     */
    int readDailyTemperatureFromDB(int year, int month, int day, double &current_temperature);
    /*
     * Arguments: (1) entries_of_last_period is the total number of hours during a year/month/week. These are defined by the user.
     *            (2) average_temperature: It is the result of a SELECT query
     *
     * It finds and prints the average temperature of specific last month/year/week.
     *
     * Returns: It returns either an error code (due to failure of opening the DB or extracting data from the table) or
     * a successful result code of SELECT SQL query
     */
    int readAverageLastTemperatureFromDB(int entries_of_last_period, double &average_temperature);
    /*
     * Arguments: (1) year and month. These are defined by the user.
     *            (2) average_temperature: It is the result of a SELECT query
     *
     * It finds and prints the average temperature of specific month and year.
     *
     * Returns: It returns either an error code (due to failure of opening the DB or extracting data from the table) or
     * a successful result code of SELECT SQL query
     */
    int readAverageMonthlyTemperatureFromDB(int year, int month, double &average_temperature);
};

#endif /* DATABASEWEATHER_H_ */
