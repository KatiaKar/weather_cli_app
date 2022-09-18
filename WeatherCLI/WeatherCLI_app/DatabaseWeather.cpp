/*
 * DatabaseWeather.cpp
 * Author: Aikaterini Karanasiou
 * It provides functions for creating a database and tables as well as functions for reading/writing on the tables.
 */

#include "DatabaseWeather.h"
DatabaseWeather *DatabaseWeather::dbInstance = nullptr;
double DatabaseWeather::res_temperature = 0;

DatabaseWeather *DatabaseWeather::GetInstance(){
    if(dbInstance==nullptr){
        dbInstance = new DatabaseWeather("../../Database/WeatherDatabase.db");
    }
    return dbInstance;
}

void DatabaseWeather::DeleteInstance()
{
    if(dbInstance!=nullptr)
    {
        delete dbInstance;
        dbInstance = nullptr;
    }
}

DatabaseWeather::DatabaseWeather(const string dbName): db_name(dbName){
}

int DatabaseWeather::createDatabaseAndTables(){

    cout << endl << "** createDatabaseAndTables() function called **" << endl;

    // Create the database
    int rc_sqlite_comm = sqlite3_open(db_name.c_str(), &DB);
    string sql_query_journal = "PRAGMA journal_mode = WAL";
    sqlite3_exec(DB, sql_query_journal.c_str(), NULL, 0, NULL);
    if (rc_sqlite_comm)
    {
        cerr << endl << "Failed to open the database." << sqlite3_errmsg(DB) << endl;
        return rc_sqlite_comm;
    }
    else
    {
        cout << endl << "Succeeded to open the Database." << endl;
    }

    // Create table for the hourly temperature for each day of each year
    string table_avg_temperature =  "CREATE TABLE AVG_TEMPERATURE("
            "YEAR		INT     CHECK(YEAR>1900), "
            "MONTH	    INT		CHECK(MONTH>0 AND MONTH<13), "
            "DAY		INT		CHECK(DAY>0 AND DAY<32), "
            "HOUR	    INT		CHECK(HOUR>=0 AND HOUR<24), "
            "TEMPERATURE	REAL, "
            "PRIMARY KEY	(YEAR, MONTH, DAY, HOUR));";
    rc_sqlite_comm = sqlite3_exec(DB, table_avg_temperature.c_str(), NULL, 0, NULL);
    // in case of error return the error code and close the DB
    if (rc_sqlite_comm != SQLITE_OK)
    {
        cerr << endl << "Failed to create the table for hourly temperatures. Error: " << sqlite3_errmsg(DB) << endl;
        sqlite3_close(DB);
        return rc_sqlite_comm;
    }
    else
    {
        cout << endl << "Succeeded to create the table for hourly temperatures." << endl;
    }

    // Create table for the daily temperature
    string table_daily_temperature = "CREATE TABLE DAILY_TEMPERATURE("
            "YEAR	INT     CHECK(YEAR>1900), "
            "MONTH	INT		CHECK(MONTH>0 AND MONTH<13), "
            "DAY	INT		CHECK(DAY>0 AND DAY<32), "
            "TEMPERATURE	REAL, "
            "PRIMARY KEY	(YEAR, MONTH, DAY));";
    rc_sqlite_comm = sqlite3_exec(DB, table_daily_temperature.c_str(), NULL, 0, NULL);
    // in case of error return the error code and close the DB
    if (rc_sqlite_comm)
    {
        cerr << endl << "Failed to create the table for daily temperatures. Error: " << sqlite3_errmsg(DB) << endl;
        sqlite3_close(DB);
        return rc_sqlite_comm;
    }
    else
    {
        cout << endl << "Succeeded to create the table for daily temperatures." << endl;
    }
    // close the database
    sqlite3_close(DB);
    return rc_sqlite_comm;
}

int DatabaseWeather::writeTemperatureDataInDB(int year, int month, int day, int hour, string temperature, bool isAVGnetry){

    int rc_sqlite_comm;
    string sql_query;

    cout << endl << "** writeTemperatureDataInDB() function called **" << endl;

    // Create INSERT query either for the hourly temperatures of several years (AVG_TEMPERATURE) or the daily temperature (DAILY_TEMPERATURE)
    // Insert year, month, day and temperature (hourly or daily temperature)
    // Insert hour only in the table with the hourly temperatures
    if(isAVGnetry)
        sql_query = "INSERT INTO AVG_TEMPERATURE VALUES(";
    else
        sql_query = "INSERT INTO DAILY_TEMPERATURE VALUES(";

    sql_query = sql_query + to_string(year) + ", " + to_string(month) + ", " + to_string(day);
    if(isAVGnetry)
    {
        sql_query = sql_query + ", " + to_string(hour);
    }
    sql_query = sql_query + ", " + temperature + ");";

    // Open the database
    rc_sqlite_comm = sqlite3_open(db_name.c_str(), &DB);
    string sql_query_journal = "PRAGMA journal_mode = WAL";
    sqlite3_exec(DB, sql_query_journal.c_str(), NULL, 0, NULL);
    if (rc_sqlite_comm != SQLITE_OK)
    {
        cerr << endl << "Failed to open the database. " << sqlite3_errmsg(DB)<< endl;
        return rc_sqlite_comm;
    }

    // Execute the query for inserting data on the table
    cout << endl << "Current INSERT SLQ command is: " << sql_query << endl;
    rc_sqlite_comm = sqlite3_exec(DB, sql_query.c_str(), NULL, 0, NULL);
    if (rc_sqlite_comm != SQLITE_OK)
    {
        // in case of error return the error code and close the DB
        cerr << endl << "Failed to INSERT weather data into the table. Error: " << sqlite3_errmsg(DB) << endl;
        sqlite3_close(DB);
        return rc_sqlite_comm;
    }
    else
    {
        cout << endl << "Succeeded to INSERT weather data into the table." << endl;
    }

    // Close the database
    sqlite3_close(DB);
    return rc_sqlite_comm;
}

int DatabaseWeather::doubleResultFromSelectQuery(void* data, int argc, char** argv, char** resName)
{
    // Expect to receive only one value (average temperature of a year or daily temperature)
    if (argc == 1)
    {
        char *res;
        cout << endl << resName[0] << " = " << ((argv[0]) ? argv[0] : "NULL") << endl;
        errno = 0;
        res_temperature = strtod(argv[0],&res);
        if (errno != 0 || *res != '\0') {
            cerr << endl << "Failed to SELECT the correct weather data from the table. strtod() failed." << endl;
        }
    }
    else
    {
        cerr << endl << "Failed to SELECT the correct weather data from the table." << endl;
    }
    return 0;
}

int DatabaseWeather::intResultFromSelectQuery(void* data, int argc, char** argv, char** resName)
{
    int *c = (int *)data;
    // Expect to receive only one value (number of entries)
    if (argc == 1)
    {
        cout << endl << resName[0] << " = " << ((argv[0]) ? argv[0] : "NULL") << endl;
        errno = 0;
        *c = atoi(argv[0]);
    }
    else
    {
        cerr << endl << "Failed to SELECT the correct weather data from the table." << endl;
    }
    return 0;
}

int DatabaseWeather::readDailyTemperatureFromDB(int year, int month, int day, double &current_temperature){
    int rc_sqlite_comm;
    string sql_query;

    cout << endl << "** readAverageDailyTemperatureFromDB() function called **" << endl;

    // Create SELECT query
    sql_query = "SELECT TEMPERATURE FROM DAILY_TEMPERATURE WHERE YEAR  = " + to_string(year) + " AND MONTH = " + to_string(month) + " AND DAY = " + to_string(day);

    // Open the database
    rc_sqlite_comm = sqlite3_open(db_name.c_str(), &DB);
    string sql_query_journal = "PRAGMA journal_mode = WAL";
    sqlite3_exec(DB, sql_query_journal.c_str(), NULL, 0, NULL);
    if (rc_sqlite_comm != SQLITE_OK)
    {
        cerr << endl << "Failed to open the database. " << sqlite3_errmsg(DB)<< endl;
        return rc_sqlite_comm;
    }

    // Execute the query for extracting data from the table
    cout << endl << "Current SELECT SLQ command is: " << sql_query << endl;
    rc_sqlite_comm = sqlite3_exec(DB, sql_query.c_str(), doubleResultFromSelectQuery, NULL, NULL);
    if (rc_sqlite_comm != SQLITE_OK)
    {
        // in case of error return the error code and close the DB
        cerr << endl << "Failed to SELECT weather data from the table. Error: " << sqlite3_errmsg(DB) << endl;
        sqlite3_close(DB);
        return rc_sqlite_comm;
    }
    else
    {
        cout << endl << "Succeeded to SELECT weather data from the table." << endl;
        current_temperature = res_temperature;
    }

    // Close the database
    sqlite3_close(DB);
    return rc_sqlite_comm;
}

int DatabaseWeather::readAverageLastTemperatureFromDB(int entries_of_last_period, double &average_temperature){
    int rc_sqlite_comm;
    string sql_query;
    int total_entries = 0;

    cout << endl << "** readAverageLastTemperatureFromDB() function called **" << endl;

    // Open the database
    rc_sqlite_comm = sqlite3_open(db_name.c_str(), &DB);
    string sql_query_journal = "PRAGMA journal_mode = WAL";
    sqlite3_exec(DB, sql_query_journal.c_str(), NULL, 0, NULL);
    if (rc_sqlite_comm != SQLITE_OK)
    {
        cerr << endl << "Failed to open the database. " << sqlite3_errmsg(DB)<< endl;
        return rc_sqlite_comm;
    }

    // Execute the query for finding the number of total entries
    // Result of this sql command is used for the next sql command (for estimating the average temperature)
    sql_query = "SELECT COUNT(*) FROM AVG_TEMPERATURE";
    cout << endl << "Current SELECT SLQ command is: " << sql_query << endl;
    rc_sqlite_comm = sqlite3_exec(DB, sql_query.c_str(), intResultFromSelectQuery, &total_entries, NULL);
    if (rc_sqlite_comm != SQLITE_OK)
    {
        // in case of error return the error code and close the DB
        cerr << endl << "Failed to SELECT weather data from the table. Error: " << sqlite3_errmsg(DB) << endl;
        sqlite3_close(DB);
        return rc_sqlite_comm;
    }
    else
    {
        cout << endl << "Succeeded to SELECT weather data from the table." << endl;
    }

    // Estimate the average temperature of the last period (month, year, week)
    sql_query = "SELECT AVG(TEMPERATURE) FROM (SELECT TEMPERATURE FROM AVG_TEMPERATURE LIMIT " + to_string(entries_of_last_period) + " OFFSET " + to_string(total_entries-(entries_of_last_period)) + ")";
    cout << endl << "Current SELECT SLQ command is: " << sql_query << endl;
    rc_sqlite_comm = sqlite3_exec(DB, sql_query.c_str(), doubleResultFromSelectQuery, NULL, NULL);
    if (rc_sqlite_comm != SQLITE_OK)
    {
        // in case of error return the error code and close the DB
        cerr << endl << "Failed to SELECT weather data from the table. Error: " << sqlite3_errmsg(DB) << endl;
        sqlite3_close(DB);
        return rc_sqlite_comm;
    }
    else
    {
        cout << endl << "Succeeded to SELECT weather data from the table." << endl;
        average_temperature = res_temperature;
    }

    // Close the database
    sqlite3_close(DB);
    return rc_sqlite_comm;

    return 0;
}

int DatabaseWeather::readAverageMonthlyTemperatureFromDB(int year, int month, double &average_temperature){
    int rc_sqlite_comm;
    string sql_query;

    cout << endl << "** readAverageLastTemperatureFromDB() function called **" << endl;

    // Create SELECT query
    sql_query = "SELECT AVG(case when MONTH  = " + to_string(month) + " AND YEAR = " + to_string(year) + " then TEMPERATURE end) as AVG_MONTHLY_TEMPERATURE FROM AVG_TEMPERATURE";

    // Open the database
    rc_sqlite_comm = sqlite3_open(db_name.c_str(), &DB);
    string sql_query_journal = "PRAGMA journal_mode = WAL";
    sqlite3_exec(DB, sql_query_journal.c_str(), NULL, 0, NULL);
    if (rc_sqlite_comm != SQLITE_OK)
    {
        cerr << endl << "Failed to open the database. " << sqlite3_errmsg(DB)<< endl;
        return rc_sqlite_comm;
    }

    // Execute the query for extracting data from the table
    cout << endl << "Current SELECT SLQ command is: " << sql_query << endl;
    rc_sqlite_comm = sqlite3_exec(DB, sql_query.c_str(), doubleResultFromSelectQuery, NULL, NULL);
    if (rc_sqlite_comm != SQLITE_OK)
    {
        // in case of error return the error code and close the DB
        cerr << endl << "Failed to SELECT weather data from the table. Error: " << sqlite3_errmsg(DB) << endl;
        sqlite3_close(DB);
        return rc_sqlite_comm;
    }
    else
    {
        cout << endl << "Succeeded to SELECT weather data from the table." << endl;
        average_temperature = res_temperature;
    }

    // Close the database
    sqlite3_close(DB);
    return rc_sqlite_comm;
}
