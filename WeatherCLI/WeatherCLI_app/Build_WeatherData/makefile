CC = g++
INC_D = -L/usr/lib/x86_64-linux-gnu
CFLAGS = -Wall -g

WeatherData: WeatherData.o ConnectionWeatherAPI.o DatabaseWeather.o
	$(CC) $(CFLAGS) WeatherData.o ConnectionWeatherAPI.o DatabaseWeather.o $(INC_D) -lsqlite3 -lcurl -ljsoncpp -lboost_system -o WeatherData
  
WeatherData.o: ../WeatherData.cpp ../ConnectionWeatherAPI.h ../DatabaseWeather.h
	$(CC) $(CFLAGS) -c ../WeatherData.cpp

ConnectionWeatherAPI.o: ../ConnectionWeatherAPI.cpp ../ConnectionWeatherAPI.h ../DatabaseWeather.h
	$(CC) $(CFLAGS) -c ../ConnectionWeatherAPI.cpp

DatabaseWeather.o: ../DatabaseWeather.cpp ../DatabaseWeather.h
	$(CC) $(CFLAGS) -c ../DatabaseWeather.cpp

clean:
	rm *.o WeatherData
