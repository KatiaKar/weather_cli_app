/*
 * Command.h
 * Author: Aikaterini Karanasiou
 * It includes classes that represent the commands on the Weather Command Line app.
 */
#include <string>
#include <iostream>
#include "DatabaseWeather.h"

using namespace std;

#ifndef COMMAND_H_
#define COMMAND_H_

/* Command (Base) Class*/
/* It includes functions that are used by all the Commands*/
class Command {
public:
    Command();
    virtual ~Command();
    virtual void Execute() = 0;
    /*
     * It returns the current GMT date.
     */
    bool currentDate(tm &date);
protected:
    DatabaseWeather *db;
};

/* Control Commands Class*/
/* It is responsible for executing one command according to the User's input.*/
class ControlCommands
{
public:
    ~ControlCommands();
    /*
     * It takes as input an object that is created by Command subclasses.
     * Sets one of the command types (display current weather, compare weather, display average weather temperature of a month)
     */
    void setCommand(Command *cmd);
    /*
     *  It executes an action (Execute() function) according to the input
     *  command type ("DisplayCurrentWeather", "CompareWeather" or "DisplayAverageWeather")
     *  Input command type (Command subclass) is defined by the setCommand() function.
     */
    void Execute();
private:
    Command *current_Cmd;
};


/* Command Derived Classes */
/* Derived class for the -display command*/
class DisplayCurrentWeather : public Command {
public:
    DisplayCurrentWeather();
    // Prints the current weather in Athens
    void Execute();
};

class CompareWeather : public Command {
private:
    // time_period can be "year","month" or "week"
    string time_period;
public:
    CompareWeather(string timePeriod);
    // Compares the current weather temperature with the average weather temperature of last year/month/week in Athens
    void Execute();
};

class DisplayAverageWeather : public Command {
private:
    int month, year;
public:
    DisplayAverageWeather(int Month, int Year);
    // Prints the average weather temperature of specific month and year in Athens
    void Execute();

};

#endif /* COMMAND_H_ */
