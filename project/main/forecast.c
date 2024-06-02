#include <math.h>

int oldestPressure = 0;

char* forecastLUT(int computedForecast)
{
    switch(computedForecast)
    {
        case 1:case 10:case 20:
            return "Settled Fine";
        case 2:case 11:case 21:
            return "Fine Weather";
        case 3:
            return "Fine, Becoming Less Settled";
        case 4:
            return "Fairly Fine Showery Later";
        case 5:
            return "Showery Becoming more unsettled";
        case 6:
            return "Unsettled, Rain Later";
        case 7:
            return "Rain at Times, Worse Later";
        case 8:
            return "Rain at Times, Becoming Very Unsettled";
        case 9:
            return "Very Unsettled, Rain";
        case 12:
            return "Fine, Possibly Showers";
        case 13:
            return "Fairly Fine, Showers Likely";
        case 14:
            return "Showery Bright Intervals";
        case 15:
            return "Changeable some rain";
        case 16:
            return "Unsettled, rain at times";
        case 17:
            return "Rain at Frequent Intervals";
        case 18:
            return "Very Unsettled, rain";
        case 19:
            return "Stormy, much rain";
        case 22:
            return "Becoming Fine";
        case 23:
            return "Fairly Fine, Improving";
        case 24:  
            return "Fairly Fine, Possibly Showers Early";
        case 25:
            return "Showery Early, Improving";
        case 26:    
            return "Changeable, Mending";
        case 27:
            return "Rather Unsettled Clearing Later";
        case 28:
            return "Unsettled, Probably Improving";
        case 29:    
            return "Unsettled, short fine Intervals";
        case 30:
            return "Very Unsettled, Finer at Times";
        case 31:
            return "Stormy, Possibly Improving";
        case 32:    
            return "Stormy, Much Rain";
        default:
            return "Unknown";
    }
}

char* computeForecast(float temperature, float pressure, float altitude, int winds, int season)
{
    static int firstIterationFlag = 1;

    if (firstIterationFlag)
    {
        oldestPressure = (int)pressure;
        firstIterationFlag = 0;
        return "Too early to forecast.";
    }

    float Z = 0;

    float p0;
    p0 = (0.0065 * altitude) / (temperature + (0.0065 * altitude) + 273.15);
    p0 = powf((1 - p0), (-5.257));
    p0 = p0 * pressure;

    if (pressure - oldestPressure > 160)
        Z = 185 - 0.16 * p0;
    else if (pressure - oldestPressure < -160)
        Z = 127 - 0.12 * p0;
    else
        Z = 144 - 0.13 * p0;

    // Adjust the forecast based on the winds and season
    // Z = winds ? Z + 1 : Z - 2;
    // Z = season ? Z + 1 : Z - 1;

    return forecastLUT((int)roundf(Z));
}