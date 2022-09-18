/*
 * main.cpp
 * Author: Aikaterini Karanasiou
 * It starts a command line app for displaying the weather in Athens
 */
#include <string.h>
#include "Command.h"

int main(int argc, char *argv[]){

    std::string dis("-display");
    std::string comp("-compare");
    std::string avg("-average");
    std::string h("--help");
    std::string w("week");
    std::string m("month");
    std::string y("year");

    ControlCommands *cc;
    try
    {
        cc = new ControlCommands();
    }
    catch (bad_alloc& exception)
    {
        cout << endl << "Caught bad_alloc exception: " << exception.what() << endl;
        exit(2);
    }

    if(argc == 1)
    {
        printf("\nCC missing arguments\nTry 'CC --help' for more information.\n");
        exit(1);
    } // --help command
    else if(argc==2 && strcmp(argv[1],h.c_str())==0)
    {
        printf("\n-display                                        Display current temperature in Athens\n"
                "-compare month|year|day                         Compare current temperature with average temperature of last the week/month/year\n"
                "-average NUMBER_OF_MONTH NUMBER_OF_YEAR         Display average temperature of a certain month and year\n");
        exit(0);
    }// -display command
    else if(argc==2 && strcmp(argv[1],dis.c_str())==0)
    {
        cc->setCommand(new (std::nothrow)DisplayCurrentWeather());
    }// -compare command should have year/month/week as argument
    else if(argc==3 && strcmp(argv[1],comp.c_str())==0 && ( strcmp(argv[2],w.c_str())==0 || strcmp(argv[2],m.c_str())==0 || strcmp(argv[2],y.c_str())==0) )
    {
        string timePeriod(argv[2]);
        cc->setCommand(new (std::nothrow)CompareWeather(timePeriod));
    }// -average should have month and year as arguments
    else if(argc==4 && strcmp(argv[1],avg.c_str())==0)
    {
        // convert month and year from char array to integer type
        int Month,Year;
        try
        {
            Month = stoi(argv[2]);
            Year = stoi(argv[3]);
        }
        catch (std::exception& e)
        {
            std::cerr << "Input values cannot converted from char array to int. Exception caught in stoi: " << e.what() << '\n';
            exit(2);
        }
        cc->setCommand(new DisplayAverageWeather(Month,Year));
    }
    cc->Execute();

    DatabaseWeather::DeleteInstance();
    delete cc;
    cc = nullptr;
    return 0;
}

