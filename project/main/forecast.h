#ifndef __FORECAST_H__INCLUDED__
#define __FORECAST_H__INCLUDED__

#define MINUTES_BETWEEN_FORECASTS 10

#define DETI_ALTITUDE 3

#define NORTH_WINDS 1
#define SOUTH_WINDS 0
#define SUMMER 1
#define WINTER 0

char* forecastLUT(int computedForecast);
char* computeForecast(float temperature, float pressure, float altitude, int winds, int season, int* bin7seg);

#endif // __FORECAST_H__INCLUDED__