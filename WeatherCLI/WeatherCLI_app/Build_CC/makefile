CC = g++
INC_D = -L/usr/lib/x86_64-linux-gnu
CFLAGS = -Wall -g

WeatherCLI: CC.o Command.o DatabaseWeather.o
	$(CC) $(CFLAGS) CC.o Command.o DatabaseWeather.o $(INC_D) -lsqlite3 -o WeatherCLI
  
CC.o: ../CC.cpp ../Command.h ../DatabaseWeather.h
	$(CC) $(CFLAGS) -c ../CC.cpp

Command.o: ../Command.cpp ../Command.h ../DatabaseWeather.h
	$(CC) $(CFLAGS) -c ../Command.cpp

DatabaseWeather.o: ../DatabaseWeather.cpp ../DatabaseWeather.h
	$(CC) $(CFLAGS) -c ../DatabaseWeather.cpp

clean:
	rm *.o WeatherCLI
