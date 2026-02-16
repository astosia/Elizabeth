#include <pebble.h>
#include "main.h"
#include "weekday.h"
//#include "effect_layer.h"
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/fpath.h>
#include <pebble-fctx/ffont.h>

//Static and initial vars
static GFont
  FontDayOfTheWeekShortName, FontBTQTIcons, FontTemp, FontTempFore, FontUV, FontWeatherIcons, FontWeatherCondition;// FontDate, FontSunset, FontMoonPhase,FontWindDirection;

FFont* time_font;

char  citistring[24];

static Window * s_window;

static Layer * s_canvas_background;
static Layer * s_canvas_weather_section;
static Layer * s_canvas_bt_icon;
static Layer * s_canvas_qt_icon;
Layer * time_area_layer;

//#ifdef PBL_MICROPHONE
static Layer * s_canvas_rain;

static GPath *s_rain_path = NULL;
//#endif

static int s_hours, s_minutes, s_weekday, s_day;
//#ifdef PBL_MICROPHONE
static int s_uvmax_level, s_uvnow_level;// last one changed, s_loop;
//#endif

typedef struct {
  int TimebandHeight;
  int HourX;
  int HourY;
  int MinuteX;
  int MinuteY;
  int DateX;
  int DateY;
  int BatteryBarX;
  int BatteryBarY;
  int BatteryBarW;
  int BatteryBarH;
  int MinFontEM;
  int HourFontEM;
  int DateFontEM;
  GRect RainRect[1];
  GRect DateRect[1];
  GRect SunsetRect[1];
  GRect SunriseRect[1];
  GRect MoonRect[1];
  GRect StepsRect[1];
  GRect SunsetIconRect[1];
  GRect SunriseIconRect[1];
  GRect IconNowRect[1];
  GRect IconForeRect[1];
  GRect WindKtsRect[1];
  GRect WindForeKtsRect[1];
  GRect TempRect[1];
  GRect PrecipRect[1];
  GRect TempForeRect[1];
  GRect WindDirNowRect[1];
  GRect WindDirForeRect[1];
  GRect BatteryRect[1];
  GRect RainRateRect[1];
  GRect PressureRect[1];
  GRect BTIconRect[1];
  GRect QTIconRect[1];
  GRect UVDayValueRect[1];
  GRect arc_bounds[1];
  GRect arc_bounds_max[1];
  GRect arc_bounds_now[1];
} UIConfig;

#ifdef PBL_PLATFORM_EMERY //emery aka Pebble Time 2
static const UIConfig config = {
  .TimebandHeight = 94,
  .HourX = 79,
  .HourY = 56-2,
  .MinuteX = 78,
  .MinuteY = 109,
  .DateX = 62,
  .DateY = 92,
  .BatteryBarX = 0,
  .BatteryBarY = 93,
  .BatteryBarW = 200,
  .BatteryBarH = 3,
  .MinFontEM = 116, //ok
  .HourFontEM = 54,
  .DateFontEM = 30, //ok was 30 down to here
  .RainRect = {{{25,106},{50,50}}}, //ok
  .DateRect = {{{1,45},{62,48}}}, //ok
  .SunsetRect = {{{133,200-4+2},{200/3,36}}}, //ok
  .SunriseRect = {{{200/6,200-4+2},{200/3,36}}}, //ok
  .MoonRect = {{{100,105+2+2},{100,76}}}, //ok
  .StepsRect = {{{100,156+5},{100,36}}}, //ok
  .SunsetIconRect = {{{100,200},{200/6,52}}}, //ok
  .SunriseIconRect = {{{0,200},{200/6,52}}}, //ok
  .IconNowRect = {{{0,105+2+2},{100,76}}}, //ok
  .IconForeRect = {{{100,105+2+2},{100,76}}}, //ok
  .WindKtsRect = {{{25,200-4+2},{75,36}}}, //ok
  .WindForeKtsRect = {{{125,200-4+2},{75,36}}}, //ok
  .TempRect = {{{-5,156-1},{111,36}}}, //ok
  .PrecipRect = {{{0,161},{100,36}}}, //ok
  .TempForeRect = {{{100,161},{100,36}}}, //ok
  .WindDirNowRect = {{{0,185+8},{200/6,52}}}, //ok
  .WindDirForeRect = {{{100,185+8},{200/6,52}}}, //ok
  .BatteryRect = {{{0,93},{200,3}}}, //ok
  .RainRateRect = {{{100,198},{100,36}}}, //ok
  .PressureRect = {{{100,161},{100,36}}}, //ok
  .BTIconRect ={{{68,109},{65,27}}}, //ok
  .QTIconRect ={{{68,132},{65,27}}}, //ok
 // .UVDayValueRect = {{{138,102},{25,25}}}, //ok
  .UVDayValueRect = {{{136,117-2},{25,25}}},
  .arc_bounds = {{{130,114-2},{38,38}}}, //ok
  .arc_bounds_max = {{{127,111-2},{44,44}}}, //ok
  .arc_bounds_now = {{{121,105-2},{56,56}}} //ok
};
#elif defined(PBL_BW) //aplite and diorite
static const UIConfig config = {
  .TimebandHeight = 69,
  .HourX = 57,
  .HourY = 41,
  .MinuteX = 56,
  .MinuteY = 80,
  .DateX = 43-9+10,
  .DateY = 70-3,
  .BatteryBarX = 0,
  .BatteryBarY = 68,
  .BatteryBarW = 144,
  .BatteryBarH = 2,
  .MinFontEM = 85,
  .HourFontEM = 40,
  .DateFontEM = 22,
  .RainRect = {{{18,78},{36,36}}},
  .DateRect = {{{-2,33},{45,35}}},
  .SunsetRect = {{{96,147},{144/3,27}}},
  .SunriseRect = {{{144/6,144+3},{144/3,27}}},
  .MoonRect = {{{72,77},{72,56}}},
  .StepsRect = {{{72,115},{72,27}}},
  .SunsetIconRect = {{{72,148},{144/6,38}}},
  .SunriseIconRect = {{{0,148},{144/6,38}}},
  .IconNowRect = {{{0,77},{72,56}}},
  .IconForeRect = {{{72,77},{72,56}}},
  .WindKtsRect = {{{18,146},{54,27}}},
  .WindForeKtsRect = {{{90,146},{54,27}}},
  .TempRect = {{{-4,115},{80,27}}},
  .PrecipRect = {{{0,119},{72,27}}},
  .TempForeRect = {{{72,119},{72,27}}},
  .WindDirNowRect = {{{0,136},{144/6,38}}},
  .WindDirForeRect = {{{72,136},{144/6,38}}},
  .BatteryRect = {{{0,68},{144,2}}},
  .RainRateRect = {{{72,146},{72,27}}},
  .PressureRect = {{{72,119},{72,27}}},
  .BTIconRect ={{{49,80},{47,20}}},
  .QTIconRect ={{{49,97},{47,20}}}, //ok
  .UVDayValueRect = {{{99,86},{18,18}}},
  .arc_bounds = {{{94,84},{28,28}}},
  .arc_bounds_max = {{{92,82},{32,32}}},
  .arc_bounds_now = {{{88,78},{40,40}}}
};
#elif defined(PBL_ROUND) //chalk - NOT gabbro
static const UIConfig config = {
  .TimebandHeight = 88,
  .HourX = 96-23-2+4,
  .HourY = 127-34+2+2-35,
  .MinuteX = 95-23-2+4,
  .MinuteY = 127+39-34+2+2-35,
  .DateX = 150-74-23+9-2-2+4+1,
  .DateY = 133-13+2+2-35,
  .BatteryBarX = 0,
  .BatteryBarY = 50+75-2-35,
  .BatteryBarW = 180,
  .BatteryBarH = 2,
  .MinFontEM = 85,
  .HourFontEM = 40,
  .DateFontEM = 22,
  .RainRect = {{{49,106}, {32,32}}}, // The single GRect at index 0
  .DateRect = {{{25-2-10-2-2+4+1,89-35},{48,20}}},
  .SunsetRect = {{{99-4+27,133-8-37},{61,14}}},
  .SunriseRect = {{{20+6-31+2,133-8-37},{61,14}}},
  .MoonRect = {{{90-9-27+29-4,132-5+12-37+1-2},{18+54,56}}},
  .StepsRect = {{{90-8,33+105},{90-16,30}}},
  .SunsetIconRect = {{{113+30,150-3-36},{24,24}}},
  .SunriseIconRect = {{{42-30,150-3-36},{24,24}}},
  .IconNowRect = {{{0-26-2, 8+91-2+2},{180,32}}},
  .IconForeRect = {{{0+26+3,130-31-2+2},{180,32}}},
  .WindKtsRect = {{{94-115+2,26+6+2+56},{90-4,30}}},
  .WindForeKtsRect = {{{94+20-1+1,180-30-24-2-34},{90-4,30}}},
  .TempRect = {{{0+20-1,24+2+109},{90,30}}},
  .PrecipRect = {{{16+12,33+105},{90-16,30}}},
  .TempForeRect = {{{0+72+5,180-30-24-2+14},{90,30}}},
  .WindDirNowRect = {{{78-99,12-4+95},{90,32}}},
  .WindDirForeRect = {{{78+33+1,180-32-12-33},{90,32}}},
  .BatteryRect = {{{0,50+75-2-35},{180,2}}},
  .RainRateRect = {{{99-4+27,133-8-37},{61,14}}},
  .PressureRect = {{{0+72+5,180-30-24-2+14},{90,30}}},
  .BTIconRect ={{{90,4},{24,20}}},
  .QTIconRect ={{{90-24,4},{24,20}}}, //ok
  .UVDayValueRect = {{{0,90},{18,18}}},
  .arc_bounds = {{{4,60},{28,28}}},
  .arc_bounds_max = {{{2,58},{32,32}}},
  .arc_bounds_now = {{{-2,54},{40,40}}}
};
#else // Default for other platforms, should only apply to Basalt
static const UIConfig config = {
  .TimebandHeight = 69,
  .HourX = 57,
  .HourY = 41,
  .MinuteX = 56,
  .MinuteY = 80,
  .DateX = 43-9+10,
  .DateY = 70-3,
  .BatteryBarX = 0,
  .BatteryBarY = 68,
  .BatteryBarW = 144,
  .BatteryBarH = 2,
  .MinFontEM = 85,
  .HourFontEM = 40,
  .DateFontEM = 22,
  .RainRect = {{{18,78},{36,36}}},
  .DateRect = {{{-2,33},{45,35}}},
  .SunsetRect = {{{96,147},{144/3,27}}},
  .SunriseRect = {{{144/6,144+3},{144/3,27}}},
  .MoonRect = {{{72,77},{72,56}}},
  .StepsRect = {{{72,115},{72,27}}},
  .SunsetIconRect = {{{72,148},{144/6,38}}},
  .SunriseIconRect = {{{0,148},{144/6,38}}},
  .IconNowRect = {{{0,77},{72,56}}},
  .IconForeRect = {{{72,77},{72,56}}},
  .WindKtsRect = {{{18,146},{54,27}}},
  .WindForeKtsRect = {{{90,146},{54,27}}},
  .TempRect = {{{-4,115},{80,27}}},
  .PrecipRect = {{{0,119},{72,27}}},
  .TempForeRect = {{{72,119},{72,27}}},
  .WindDirNowRect = {{{0,136},{144/6,38}}},
  .WindDirForeRect = {{{72,136},{144/6,38}}},
  .BatteryRect = {{{0,68},{144,2}}},
  .RainRateRect = {{{72,146},{72,27}}},
  .PressureRect = {{{72,119},{72,27}}},
  .BTIconRect ={{{49,80},{47,20}}},
  .QTIconRect ={{{49,97},{47,20}}}, //ok
  .UVDayValueRect = {{{99,86},{18,18}}},
  .arc_bounds = {{{94,84},{28,28}}},
  .arc_bounds_max = {{{92,82},{32,32}}},
  .arc_bounds_now = {{{88,78},{40,40}}}
};
#endif

static char* weather_conditions[] = {
  "\U0000F07B", // 'unknown': 0,
  "\U0000f00e", //thunderstorm with light rain: 1
  "\U0000f02c", //thunderstorm with light rain: 2
  "\U0000f010", //thunderstorm with rain: 3
  "\U0000f02d", //thunderstorm with rain: 4
  "\U0000f01e", //thunderstorm with heavy rain: 5
  "\U0000f01e", //thunderstorm with heavy rain: 6
  "\U0000f005", //light thunderstorm: 7
  "\U0000f025", //light thunderstorm: 8
  "\U0000f01e", //thunderstorm: 9
  "\U0000f01e", //thunderstorm: 10
  "\U0000f01e", //heavy thunderstorm: 11
  "\U0000f01e", //heavy thunderstorm: 12
  "\U0000f01e", //ragged thunderstorm: 13
  "\U0000f01e", //ragged thunderstorm: 14
  "\U0000f00e", //thunderstorm with light drizzle: 15
  "\U0000f02c", //thunderstorm with light drizzle: 16
  "\U0000f00e", //thunderstorm with drizzle: 17
  "\U0000f02c", //thunderstorm with drizzle: 18
  "\U0000f01d", //thunderstorm with heavy drizzle: 19
  "\U0000f01d", //thunderstorm with heavy drizzle: 20
  "\U0000f00b", //light intensity drizzle: 21
  "\U0000f02b", //light intensity drizzle: 22
  "\U0000f01c", //drizzle: 23
  "\U0000f01c", //drizzle: 24
  "\U0000f01a", //heavy intensity drizzle: 25
  "\U0000f01a", //heavy intensity drizzle: 26
  "\U0000f00b", //light intensity drizzle rain: 27
  "\U0000f02b", //light intensity drizzle rain: 28
  "\U0000f00b", //drizzle rain: 29
  "\U0000f029", //drizzle rain: 30
  "\U0000f019", //heavy intensity drizzle rain: 31
  "\U0000f019", //heavy intensity drizzle rain: 32
  "\U0000f01a", //shower rain and drizzle: 33
  "\U0000f01a", //shower rain and drizzle: 34
  "\U0000f01a", //heavy shower rain and drizzle: 35
  "\U0000f01a", //heavy shower rain and drizzle: 36
  "\U0000f00b", //shower drizzle: 37
  "\U0000f02b", //shower drizzle: 38
  "\U0000f01a", //light rain: 39
  "\U0000f01a", //light rain: 40
  "\U0000f019", //moderate rain: 41
  "\U0000f019", //moderate rain: 42
  "\U0000f019", //heavy intensity rain: 43
  "\U0000f019", //heavy intensity rain: 44
  "\U0000f019", //very heavy rain: 45
  "\U0000f019", //very heavy rain: 46
  "\U0000f018", //extreme rain: 47
  "\U0000f018", //extreme rain: 48
  "\U0000f017", //freezing rain: 49
  "\U0000f017", //freezing rain: 50
  "\U0000f01a", //light intensity shower rain: 51
  "\U0000f01a", //light intensity shower rain: 52
  "\U0000f01a", //shower rain: 53
  "\U0000f01a", //shower rain: 54
  "\U0000f01a", //heavy intensity shower rain: 55
  "\U0000f01a", //heavy intensity shower rain: 56
  "\U0000f018", //ragged shower rain: 57
  "\U0000f018", //ragged shower rain: 58
  "\U0000f00a", //light snow: 59
  "\U0000f02a", //light snow: 60
  "\U0000f01b", //Snow: 61
  "\U0000f01b", //Snow: 62
  "\U0000f076", //Heavy snow: 63
  "\U0000f076", //Heavy snow: 64
  "\U0000f017", //Sleet: 65
  "\U0000f017", //Sleet: 66
  "\U0000f0b2", //Light shower sleet: 67
  "\U0000f0b4", //Light shower sleet: 68
  "\U0000f0b5", //Shower sleet: 69
  "\U0000f0b5", //Shower sleet: 70
  "\U0000f006", //Light rain and snow: 71
  "\U0000f026", //Light rain and snow: 72
  "\U0000f017", //Rain and snow: 73
  "\U0000f017", //Rain and snow: 74
  "\U0000f00a", //Light shower snow: 75
  "\U0000f02a", //Light shower snow: 76
  "\U0000f00a", //Shower snow: 77
  "\U0000f02a", //Shower snow: 78
  "\U0000f076", //Heavy shower snow: 79
  "\U0000f076", //Heavy shower snow: 80
  "\U0000f003", //mist: 81
  "\U0000f04a", //mist: 82
  "\U0000f062", //Smoke: 83
  "\U0000f062", //Smoke: 84
  "\U0000f0b6", //Haze: 85
  "\U0000f023", //Haze: 86
  "\U0000f082", //sand/ dust whirls: 87
  "\U0000f082", //sand/ dust whirls: 88
  "\U0000f014", //fog: 89
  "\U0000f014", //fog: 90
  "\U0000f082", //sand: 91
  "\U0000f082", //sand: 92
  "\U0000f082", //dust: 93
  "\U0000f082", //dust: 94
  "\U0000f0c8", //volcanic ash: 95
  "\U0000f0c8", //volcanic ash: 96
  "\U0000f011", //squalls: 97
  "\U0000f011", //squalls: 98
  "\U0000f056", //tornado: 99
  "\U0000f056", //tornado: 100
  "\U0000f00d", //clear sky: 101
  "\U0000f02e", //clear sky: 102
  "\U0000f00c", //few clouds: 11-25%: 103
  "\U0000f081", //few clouds: 11-25%: 104
  "\U0000f002", //scattered clouds: 25-50%: 105
  "\U0000f086", //scattered clouds: 25-50%: 106
  "\U0000f041", //broken clouds: 51-84%: 107
  "\U0000f041", //broken clouds: 51-84%: 108
  "\U0000f013", //overcast clouds: 85-100%: 109
  "\U0000f013", //overcast clouds: 85-100%: 110
  "\U0000f056", //tornado: 111
  "\U0000f01d", //storm-showers: 112
  "\U0000f073", //hurricane: 113
  "\U0000f076", //snowflake-cold: 114
  "\U0000f072", //hot: 115
  "\U0000f050", //windy: 116
  "\U0000f015", //hail: 117
  "\U0000f050", //strong-wind: 118
};

static char* wind_direction[] = {
    "\U0000F044",//'North': 0,
    "\U0000F043", //'NNE': 1,
     "\U0000F043",//'NE': 2,
     "\U0000F043",//'ENE': 3,
     "\U0000F048",//'East': 4,
     "\U0000F087",//'ESE': 5,
     "\U0000F087",//'SE': 6,
     "\U0000F087",//'SSE': 7,
     "\U0000F058",//'South': 8,
     "\U0000F057",//'SSW': 9,
     "\U0000F057",//'SW': 10,
     "\U0000F057",//'WSW': 11,
     "\U0000F04D",//'West': 12,
     "\U0000F088",//'WNW': 13,
     "\U0000F088",//'NW': 14,
     "\U0000F088",//'NNW': 15,
     "\U0000F04B", //'Variable': 16
};

static char* moon_phase[] ={
  "\U0000F095",//'wi-moon-new':0,
  "\U0000F096",//'wi-moon-waxing-crescent-1',1,
  "\U0000F097",//'wi-moon-waxing-crescent-2',2,
  "\U0000F098",//'wi-moon-waxing-crescent-3',3,
  "\U0000F099",//'wi-moon-waxing-crescent-4',4,
  "\U0000F09A",//'wi-moon-waxing-crescent-5',5,
  "\U0000F09B",//'wi-moon-waxing-crescent-6',6,
  "\U0000F09C",//'wi-moon-first-quarter',7,
  "\U0000F09D",//'wi-moon-waxing-gibbous-1',8,
  "\U0000F09E",//'wi-moon-waxing-gibbous-2',9,
  "\U0000F09F",//'wi-moon-waxing-gibbous-3',10,
  "\U0000F0A0",//'wi-moon-waxing-gibbous-4',11,
  "\U0000F0A1",//'wi-moon-waxing-gibbous-5',12,
  "\U0000F0A2",//'wi-moon-waxing-gibbous-6',13,
  "\U0000F0A3",//'wi-moon-full',14,
  "\U0000F0A4",//'wi-moon-waning-gibbous-1',15,
  "\U0000F0A5",//'wi-moon-waning-gibbous-2',16,
  "\U0000F0A6",//'wi-moon-waning-gibbous-3',17,
  "\U0000F0A7",//'wi-moon-waning-gibbous-4',18,
  "\U0000F0A8",//'wi-moon-waning-gibbous-5',19,
  "\U0000F0A9",//'wi-moon-waning-gibbous-6',20,
  "\U0000F0AA",//'wi-moon-third-quarter',21,
  "\U0000F0AB",//'wi-moon-waning-crescent-1',22,
  "\U0000F0AC",//'wi-moon-waning-crescent-2',23,
  "\U0000F0AD",//'wi-moon-waning-crescent-3',24,
  "\U0000F0AE",//'wi-moon-waning-crescent-4',25,
  "\U0000F0AF",//'wi-moon-waning-crescent-5',26,
  "\U0000F0B0",//'wi-moon-waning-crescent-6',27,
  "\U0000F095",//'wi-moon-new',28,
};
//////Init Configuration///
//Init Clay
ClaySettings settings;
// Initialize the default settings
static void prv_default_settings(){
  settings.HourColorN = GColorBlack;
  settings.MinColorN = GColorBlack;
  settings.DateColorN = GColorBlack;
  settings.FrameColor2N = GColorWhite;
  settings.FrameColorN = GColorWhite;
  settings.Text1ColorN = GColorBlack;
  settings.Text2ColorN = GColorBlack;
  settings.Text3ColorN = GColorBlack;
  settings.Text4ColorN = GColorBlack;
  settings.Text5ColorN = GColorBlack;
  settings.Text6ColorN = GColorBlack;
  settings.Text7ColorN = GColorBlack;
  settings.Text8ColorN = GColorBlack;
  settings.Text9ColorN = GColorBlack;
  settings.Text10ColorN = GColorBlack;
  settings.FrameColor2 = GColorBlack;
  settings.FrameColor = GColorBlack;
  settings.Text1Color = GColorWhite;
  settings.Text2Color = GColorWhite;
  settings.Text3Color = GColorWhite;
  settings.Text4Color = GColorWhite;
  settings.Text5Color = GColorWhite;
  settings.Text6Color = GColorWhite;
  settings.Text7Color = GColorWhite;
  settings.Text8Color = GColorWhite;
  settings.Text9Color = GColorWhite;
  settings.Text10Color = GColorWhite;
  settings.UVMaxColor = GColorWhite;
  settings.UVNowColor = GColorDarkGray;
  settings.UVArcColor = GColorLightGray;
  settings.UVValColor = GColorWhite;
  settings.UVMaxColorN = GColorBlack;
  settings.UVNowColorN = GColorDarkGray;
  settings.UVArcColorN = GColorLightGray;
  settings.UVValColorN = GColorBlack;
  settings.HourColor = GColorWhite;
  settings.MinColor = GColorWhite;
  settings.DateColor = GColorWhite;
  settings.WeatherUnit = 0;
  settings.WindUnit = 0;
  settings.RainUnit = 0;
  settings.UpSlider = 30;
  settings.Rainmultiplier = 3;
  settings.NightTheme = false;
  settings.HealthOff = true;
  settings.UseUVI = true;
  #ifdef PBL_MICROPHONE
  settings.PressureUnit = 0;
  settings.UsePWS = false;
  #endif
  settings.VibeOn = false;
  settings.AddZero12h = false;
  settings.RemoveZero24h = false;
  }
int HourSunrise=700;
int HourSunset=2200;
int moonphase=0;
bool BTOn=true;
bool GPSOn=true;
#ifdef PBL_MICROPHONE
bool PWSDataExists=false;
#endif
bool IsNightNow=false;
int showForecastWeather = 0;
int s_countdown = 29;
int s_loop = 0;


static GColor ColorSelect(GColor ColorDay, GColor ColorNight){
  if (settings.NightTheme && IsNightNow && GPSOn){
    return ColorNight;
  }
  else{
    return ColorDay;
  }
}

///BT Connection
static void bluetooth_callback(bool connected){
  BTOn = connected;
}

static void bluetooth_vibe_icon (bool connected) {

  layer_set_hidden(s_canvas_bt_icon, connected);

  if(!connected && !quiet_time_is_active()) {
    // Issue a vibrating alert
    vibes_double_pulse();
  }
  else{
  if(!connected && quiet_time_is_active() && settings.VibeOn) {
      // Issue a vibrating alert
      vibes_double_pulse();
    }
  }
}

///Show sound off icon if Quiet time is active
static void quiet_time_icon () {
  if(!quiet_time_is_active()) {
  layer_set_hidden(s_canvas_qt_icon,true);
  }
}


static void accel_tap_handler(AccelAxisType axis, int32_t direction) {
  // A tap event occured
  //showForecastWeather = !showForecastWeather;

 if (showForecastWeather ==5){
   showForecastWeather = 0;
 }
 else {
   showForecastWeather = showForecastWeather + 1;
 }
  layer_mark_dirty (s_canvas_weather_section);
  layer_mark_dirty (s_canvas_rain);
//APP_LOG(APP_LOG_LEVEL_DEBUG, "show forecast weather is %d", showForecastWeather);
}

//Add in HEALTH to the watchface

static char s_current_steps_buffer[8];
static int s_step_count = 0;

// Is step data available?
bool step_data_is_available(){
    return HealthServiceAccessibilityMaskAvailable &
      health_service_metric_accessible(HealthMetricStepCount,
        time_start_of_today(), time(NULL));
      }

// Todays current step count
static void get_step_count() {
//  if (!settings.HealthOff) {
    s_step_count = (int)health_service_sum_today(HealthMetricStepCount);/* code */
//  }
}

static void display_step_count() {
  int thousands = s_step_count / 1000;
  int hundreds = (s_step_count % 1000)/100;
  int hundreds2 = (s_step_count % 1000);

  snprintf(s_current_steps_buffer, sizeof(s_current_steps_buffer),
   "%s", "");

  if(thousands > 9) {
      snprintf(s_current_steps_buffer, sizeof(s_current_steps_buffer),
      "%d.%d%s", thousands, hundreds, "k");
  //      "%d,%d", thousands, hundreds2);
  }
  else if(thousands > 0) {
      snprintf(s_current_steps_buffer, sizeof(s_current_steps_buffer),
  //   "%d.%d%s", thousands, hundreds, "k");
        "%d,%03d", thousands, hundreds2);
  }
  else {
    snprintf(s_current_steps_buffer, sizeof(s_current_steps_buffer),
      "%d", hundreds2);
  }
//  layer_set_text(s_step_layer, s_current_steps_buffer);
}


static void health_handler(HealthEventType event, void *context) {
    if(event != HealthEventSleepUpdate) {
      get_step_count();
      display_step_count();
    }
  }


void layer_update_proc_background (Layer * back_layer, GContext * ctx){
  // Create Rects
  GRect bounds = layer_get_bounds(back_layer);

  GRect ComplicationsBand =
     PBL_IF_ROUND_ELSE(
       GRect(0, 0, bounds.size.w, bounds.size.h),
       GRect(0, 0, bounds.size.w, bounds.size.h));

  GRect TimeBand =
    PBL_IF_ROUND_ELSE(
      GRect(0, 0, bounds.size.w, config.TimebandHeight),
      GRect(0, 0, bounds.size.w, config.TimebandHeight));

      graphics_context_set_fill_color(ctx, ColorSelect(settings.FrameColor, settings.FrameColorN));
      graphics_fill_rect(ctx, ComplicationsBand,0,GCornersAll);
      graphics_context_set_fill_color(ctx, ColorSelect(settings.FrameColor2, settings.FrameColor2N));
      graphics_fill_rect(ctx, TimeBand,0,GCornersAll);
}

void update_time_area_layer(Layer *l, GContext* ctx) {

  GRect bounds = layer_get_bounds(l);
  // check layer bounds
  #ifdef PBL_ROUND
     bounds = GRect(0, 0,bounds.size.w, bounds.size.h);
  #else
    //  GRect bounds = GRect (0,0,144,80);
     bounds = GRect(0,0,bounds.size.w,config.TimebandHeight);
  #endif

  FContext fctx;

  fctx_init_context(&fctx, ctx);
  fctx_set_color_bias(&fctx, 0);

  int font_size_hour = config.HourFontEM;
  int font_size_min = config.MinFontEM;
  int font_size_date = config.DateFontEM;
  
  #ifdef PBL_COLOR
    fctx_enable_aa(true);
  #endif

  time_t temp = time(NULL);
  struct tm *time_now = localtime(&temp);

  char hourdraw[8];
  if(clock_is_24h_style() && settings.RemoveZero24h){
      strftime(hourdraw, sizeof(hourdraw),"%k:",time_now);
  } else if (clock_is_24h_style() && !settings.RemoveZero24h) {
      strftime(hourdraw, sizeof(hourdraw),"%H:",time_now);
  } else if (settings.AddZero12h) {
    strftime(hourdraw, sizeof(hourdraw),"%I:",time_now);
  } else {
    strftime(hourdraw, sizeof(hourdraw),"%l:",time_now);
  }

  int minnow;
  minnow = s_minutes;
  char mindraw [8];
  snprintf(mindraw, sizeof(mindraw), "%02d", minnow);

  int daynow;
  daynow = s_day;
  char daydraw[8];
  snprintf(daydraw, sizeof(daydraw), "%d", daynow);

  //draw hours
  fctx_set_fill_color(&fctx, ColorSelect(settings.HourColor, settings.HourColorN));

  FPoint hour_pos;
  fctx_begin_fill(&fctx);
  fctx_set_text_em_height(&fctx, time_font, font_size_hour);

  hour_pos.x = INT_TO_FIXED(config.HourX);
  hour_pos.y = INT_TO_FIXED(config.HourY);

  fctx_set_offset(&fctx, hour_pos);

  fctx_draw_string(&fctx, hourdraw, time_font, GTextAlignmentRight, FTextAnchorBottom);
  fctx_end_fill(&fctx);

//draw minutes
  fctx_set_fill_color(&fctx, ColorSelect(settings.MinColor, settings.MinColorN));

  FPoint min_pos;
  fctx_begin_fill(&fctx);

  fctx_set_text_em_height(&fctx, time_font, font_size_min);

  min_pos.x = INT_TO_FIXED(config.MinuteX);
  min_pos.y = INT_TO_FIXED(config.MinuteY);

  fctx_set_offset(&fctx, min_pos);

  fctx_draw_string(&fctx, mindraw, time_font, GTextAlignmentLeft, FTextAnchorBottom);
  fctx_end_fill(&fctx);

//draw date
  fctx_set_fill_color(&fctx, ColorSelect(settings.DateColor, settings.DateColorN));

  FPoint date_pos;
  fctx_begin_fill(&fctx);

  fctx_set_text_em_height(&fctx, time_font, font_size_date);

  date_pos.x = INT_TO_FIXED(config.DateX);
  date_pos.y = INT_TO_FIXED(config.DateY);

  fctx_set_offset(&fctx, date_pos);

  fctx_draw_string(&fctx, daydraw, time_font, GTextAlignmentRight, FTextAnchorBottom);
  fctx_end_fill(&fctx);

  fctx_deinit_context(&fctx);
}


static void layer_update_proc_rain (Layer * layer, GContext * ctx){

  #ifdef PBL_MICROPHONE
    if ((showForecastWeather == 5) ||
        (showForecastWeather == 2 && PWSDataExists && settings.UsePWS) ||
        (showForecastWeather == 1 && (!settings.UsePWS || !PWSDataExists)) ||
        (showForecastWeather == 3 && (!settings.UsePWS || !PWSDataExists))) {
  #else
    if ((showForecastWeather == 1 ||showForecastWeather == 3 || showForecastWeather == 5)) {
  #endif
    
    GRect bounds = config.RainRect[0];
    
    GPoint center = grect_center_point(&bounds);
    const int16_t rain_bar_max_length = bounds.size.w/2;
    int multiplier = settings.Rainmultiplier;
    int multiplier2 = 3;

    // Use an array to store the rain data to avoid code repetition
    int rain_data[] = {
      settings.rain0, settings.rain5, settings.rain10, settings.rain15,
      settings.rain20, settings.rain25, settings.rain30, settings.rain35,
      settings.rain40, settings.rain45, settings.rain50, settings.rain55,
      settings.rain60
    };

    int num_points = 0;
    GPoint points[13];

    #ifdef PBL_MICROPHONE
      // All 13 points are used for this version
      num_points = 13;
    #else
      // Only 7 points (0, 10, 20, 30, 40, 50, 60) are used for this version
      num_points = 7;
    #endif

    // Calculate the GPoints dynamically in a loop
    for (int i = 0; i < num_points; ++i) {
      int minutes_step;
      #ifdef PBL_MICROPHONE
        minutes_step = i * 5;
      #else
        minutes_step = i * 10;
      #endif

      int rain_value = rain_data[minutes_step / 5];
      int rain_length = (rain_value * multiplier / 100);
      int32_t rain_angle = TRIG_MAX_ANGLE * minutes_step / 60;

      GPoint base_point = {
        .x = (int16_t)(sin_lookup(rain_angle) * rain_bar_max_length / multiplier2 / TRIG_MAX_RATIO) + center.x,
        .y = (int16_t)(-cos_lookup(rain_angle) * rain_bar_max_length / multiplier2 / TRIG_MAX_RATIO) + center.y,
      };

      points[i] = (GPoint){
        .x = (int16_t)(sin_lookup(rain_angle) * rain_length / TRIG_MAX_RATIO) + base_point.x,
        .y = (int16_t)(-cos_lookup(rain_angle) * rain_length / TRIG_MAX_RATIO) + base_point.y,
      };
    }

    graphics_context_set_stroke_color(ctx, ColorSelect(settings.Text7Color, settings.Text7ColorN));
    graphics_context_set_fill_color(ctx, ColorSelect(settings.Text7Color, settings.Text7ColorN));

    const GPathInfo Rain_path_info = {
      .num_points = num_points,
      .points = points
    };

    s_rain_path = gpath_create(&Rain_path_info);

    gpath_draw_filled(ctx, s_rain_path);
    gpath_draw_outline(ctx, s_rain_path);

    graphics_context_set_fill_color(ctx, ColorSelect(settings.FrameColor, settings.FrameColorN));
    graphics_fill_circle(ctx, center, rain_bar_max_length / multiplier2);

    graphics_context_set_stroke_color(ctx, ColorSelect(settings.Text7Color, settings.Text7ColorN));
    graphics_context_set_stroke_width(ctx, 1);
    graphics_draw_circle(ctx, center, rain_bar_max_length / multiplier2);

    gpath_destroy(s_rain_path);
  }
}



static void layer_update_proc(Layer * layer, GContext * ctx){

GRect DateRect = config.DateRect[0];
GRect SunsetRect = config.SunsetRect[0];
GRect SunriseRect = config.SunriseRect[0];
GRect MoonRect = config.MoonRect[0]; 
GRect StepsRect = config.StepsRect[0]; 
GRect SunsetIconRect = config.SunsetIconRect[0];
GRect SunriseIconRect = config.SunriseIconRect[0];
GRect IconNowRect = config.IconNowRect[0]; 
GRect IconForeRect = config.IconForeRect[0];
GRect WindKtsRect = config.WindKtsRect[0];
GRect WindForeKtsRect = config.WindForeKtsRect[0];
GRect TempRect = config.TempRect[0];
GRect PrecipRect = config.PrecipRect[0];
GRect TempForeRect = config.TempForeRect[0];
GRect WindDirNowRect = config.WindDirNowRect[0];
GRect WindDirForeRect = config.WindDirForeRect[0];
GRect BatteryRect = config.BatteryRect[0]; 


//add in weather info
// GRect DateRect =
//   (PBL_IF_ROUND_ELSE(
//      GRect(25-2-10-2-2+4+1, 89-35, 48, 20),
//      GRect(0-2, 36-4+1, 45, 35)));

// GRect SunsetRect =
//   (PBL_IF_ROUND_ELSE(
//      GRect(99-4+27,133-8-37,61,14),
//      GRect(96,144+3,144/3,27)));

// GRect SunriseRect =
//    (PBL_IF_ROUND_ELSE(
//      GRect(20+6-31+2,133-8-37,61,14),
//      GRect(144/6,144+3,144/3,27)));

//      GRect MoonRect =
//          (PBL_IF_ROUND_ELSE(
//            GRect(90-9-27+29-4,132-5+12-37+1-2,18+54,56),
//            GRect(72, 68+9, 72,56)));

//  GRect StepsRect =
//    (PBL_IF_ROUND_ELSE(
//      (GRect(90-8,33+105,90-16,30)),
//      (GRect(72,118-3+4,144/2,27))));

//  GRect SunsetIconRect =
//     (PBL_IF_ROUND_ELSE(
//       GRect(113+30,150-3-36,24,24),
//       GRect(72,146+2,144/6,38)));

//   GRect SunriseIconRect =
//      (PBL_IF_ROUND_ELSE(
//        GRect(42-30,150-3-36,24,24),
//        GRect(0,146+2,144/6,38)));


// GRect IconNowRect = //weather condition icon
//      (PBL_IF_ROUND_ELSE(
//      GRect(0-26-2, 8+91-2+2,180,32),
//      GRect(0, 68+9, 72,56)));

// GRect IconForeRect = //weather condition icon
//       (PBL_IF_ROUND_ELSE(
//       GRect(0+26+3,130-31-2+2,180,32),
//       GRect(72, 68+9, 72,56)));


// /*GRect Icon1hRect = //weather condition icon next hour INSTEAD OF RAIN
//       (PBL_IF_ROUND_ELSE(
//       GRect(0-26-2, 8+91-2+2,180,32),
//       GRect(0, 68+9, 72,56)));*/

// GRect WindKtsRect =  //windspeed number
//      (PBL_IF_ROUND_ELSE(
//      (GRect(94-115+2,26+6+2+56,90-4,30)),
//      (GRect(24-6,144+2,48+6,27))));

// GRect WindForeKtsRect =  //windspeed number
//     (PBL_IF_ROUND_ELSE(
//       (GRect(94+20-1+1,180-30-24-2-34,90-4,30)),
//       (GRect(96-6,144+2,48+6,27))));

// GRect TempRect =  //temperature number
//     (PBL_IF_ROUND_ELSE(
//        (GRect(0+20-1,24+2+109,90,30)),
//        (GRect(-4,118-3,(144/2)+8,27))));

// GRect PrecipRect =  //rain chance next hour
//     (PBL_IF_ROUND_ELSE(
//       (GRect(16+12,33+105,90-16,30)),
//       (GRect(0,118-3+4,(144/2),27))));

//  GRect TempForeRect =  //temperature number
//     (PBL_IF_ROUND_ELSE(
//       (GRect(0+72+5,180-30-24-2+14,90,30)),
//       (GRect(72,118+1,144/2,27))));

// GRect WindDirNowRect =  //wind direction icon
//     (PBL_IF_ROUND_ELSE(
//       (GRect(78-99,12-4+95,90,32)),
//       (GRect(0,136,144/6,38))));

// GRect WindDirForeRect =  //wind direction icon
//    (PBL_IF_ROUND_ELSE(
//      (GRect(78+33+1,180-32-12-33,90,32)),
//      (GRect(144/2,136,144/6,38))));

// GRect BatteryRect =
//     (PBL_IF_ROUND_ELSE(
//       GRect(0,50+75-2-35,180,2),
//       GRect(0,68,144,2)));

    //Battery
  int s_battery_level = battery_state_service_peek().charge_percent;

//#ifdef PBL_ROUND
  int width_battery = (s_battery_level * config.BatteryBarW) / 100;
// #else
//   int width_rect = (s_battery_level * 144) / 100;
// #endif

GRect BatteryFillRect = GRect(config.BatteryBarX, config.BatteryBarY, width_battery, config.BatteryBarH);

// GRect BatteryFillRect =
//     (PBL_IF_ROUND_ELSE(
//       GRect(0,50+75-2-35,width_battery,2),
//       GRect(0,68,width_battery,2)));

  char battperc[6];
  snprintf(battperc, sizeof(battperc), "%d", s_battery_level);
  strcat(battperc, "%");

 //Date
 // Local language
 const char * sys_locale = i18n_get_system_locale();
 char datedraw[6];
 fetchwday(s_weekday, sys_locale, datedraw);
 char datenow[6];
 snprintf(datenow, sizeof(datenow), "%s", datedraw);

 //draw day of the week
 graphics_context_set_text_color(ctx, ColorSelect(settings.Text3Color, settings.Text3ColorN));
 graphics_draw_text(ctx, datenow, FontDayOfTheWeekShortName, DateRect, GTextOverflowModeWordWrap, PBL_IF_ROUND_ELSE(GTextAlignmentRight,GTextAlignmentRight), NULL);

 // Draw the battery bar background
 graphics_context_set_fill_color(ctx, ColorSelect(settings.FrameColor2,settings.FrameColor2N));// GColorBlack);
 graphics_fill_rect(ctx, BatteryRect, 0, GCornerNone);

 // Draw the battery bar
 graphics_context_set_fill_color(ctx, ColorSelect(settings.Text6Color, settings.Text6ColorN));
 graphics_fill_rect(ctx,BatteryFillRect, 0, GCornerNone);

  char TempForeToDraw[10];
  snprintf(TempForeToDraw, sizeof(TempForeToDraw), "%s",settings.temphistring);

  #ifdef PBL_MICROPHONE
  //show either wind, current & forecast or the or sunset/sunrise,  steps & next 60 mins conditions & rain
  if (//show current weather every 2 taps if PWS not available, every 3 taps if PWS is available
      (showForecastWeather==0) ||
      (showForecastWeather == 3 && PWSDataExists && settings.UsePWS) ||
      (showForecastWeather == 2 && (!settings.UsePWS || !PWSDataExists)) ||
      (showForecastWeather == 4 && (!settings.UsePWS || !PWSDataExists))
     )
      {
        char CondToDraw[4];
        char CondForeToDraw[4];
        char TempToDraw[8];

        char SpeedToDraw[10];
        char SpeedForeToDraw[10];
        char DirectionToDraw[4];
        char DirectionForeToDraw[4];

        snprintf(SpeedToDraw,sizeof(SpeedToDraw),"%s",settings.windstring);
        snprintf(SpeedForeToDraw,sizeof(SpeedForeToDraw),"%s",settings.windavestring);
        snprintf(DirectionToDraw,sizeof(DirectionToDraw),"%s",settings.windiconnowstring);
        snprintf(DirectionForeToDraw,sizeof(DirectionForeToDraw),"%s", settings.windiconavestring);
        snprintf(CondToDraw, sizeof(CondToDraw), "%s",settings.iconnowstring);

        snprintf(TempToDraw, sizeof(TempToDraw), "%s",settings.tempstring);

        snprintf(CondForeToDraw, sizeof(CondForeToDraw), "%s",settings.iconforestring);


        //Wind speed
        graphics_context_set_text_color(ctx,ColorSelect(settings.Text9Color,settings.Text9ColorN));
        graphics_draw_text(ctx, SpeedToDraw, FontTempFore, WindKtsRect, GTextOverflowModeWordWrap, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
        graphics_draw_text(ctx, SpeedForeToDraw, FontTempFore, WindForeKtsRect, GTextOverflowModeWordWrap, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
        //Wind Direction
        graphics_context_set_text_color(ctx,ColorSelect(settings.Text10Color,settings.Text10ColorN));
        graphics_draw_text(ctx, DirectionToDraw, FontWeatherCondition, WindDirNowRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
        graphics_draw_text(ctx, DirectionForeToDraw, FontWeatherCondition, WindDirForeRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);

        //Weathericons
        graphics_context_set_text_color(ctx,ColorSelect(settings.Text7Color,settings.Text7ColorN));
        graphics_draw_text(ctx, CondToDraw, FontWeatherCondition, IconNowRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);

        //weather temps
        graphics_context_set_text_color(ctx,ColorSelect(settings.Text8Color,settings.Text8ColorN));
        graphics_draw_text(ctx, TempToDraw, FontTemp, TempRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);


        graphics_context_set_text_color(ctx,ColorSelect(settings.Text7Color,settings.Text7ColorN));
        graphics_draw_text(ctx, CondForeToDraw, FontWeatherCondition, IconForeRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);

        graphics_context_set_text_color(ctx,ColorSelect(settings.Text8Color,settings.Text8ColorN));
        graphics_draw_text(ctx, TempForeToDraw, FontTempFore, TempForeRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);


      }//show Personal Weather station data on tap, if available
  else if (
      (showForecastWeather == 1 || showForecastWeather == 4) && PWSDataExists && settings.UsePWS)//showPWS data
         {
          GRect RainRateRect = config.RainRateRect[0];
          GRect PressureRect = config.PressureRect[0];
      //  GRect RainRateRect =  //temperature number
      //      (PBL_IF_ROUND_ELSE(
      //        (GRect(99-4+27,133-8-37,61,14)),
      //        (GRect(72,144+2,144/2,27))));

      //  GRect PressureRect =  //rain chance next hour
      //      (PBL_IF_ROUND_ELSE(
      //        (GRect(0+72+5,180-30-24-2+14,90,30)),
      //        (GRect(72,118+1,144/2,27))));

       char TempPWSToDraw[8];
       char SpeedPWSToDraw[10];
       char DirectionPWSToDraw[5];

       snprintf(SpeedPWSToDraw,sizeof(SpeedPWSToDraw),"%s",settings.windstringpws);
       snprintf(DirectionPWSToDraw,sizeof(DirectionPWSToDraw),"%s",settings.windiconnowstringpws);
       snprintf(TempPWSToDraw, sizeof(TempPWSToDraw), "%s",settings.tempstringpws);


       char RainTodayToDraw[12];
       char PressureToDraw[10];
       char RainNowIconToDraw[5];
       char RainTodayIconToDraw[5];

       //snprintf(RainRateToDraw, sizeof(RainRateToDraw), "%s",settings.precipratestringpws);
       snprintf(RainTodayToDraw, sizeof(RainTodayToDraw), "%s",settings.preciptotalstringpws);
       APP_LOG(APP_LOG_LEVEL_DEBUG, "preciptotalstringpws is %s",settings.preciptotalstringpws);
       snprintf(PressureToDraw,sizeof(PressureToDraw),"%s",settings.pressurestringpws);
       //snprintf(DirectionForeToDraw,sizeof(DirectionForeToDraw),"%s", settings.windiconavestring);
       if(strcmp(settings.preciptotalstringpws, "0") != 0){
         snprintf(RainTodayIconToDraw, sizeof(RainTodayIconToDraw), "%s","\U0000F078");
       } else {
         snprintf(RainTodayIconToDraw, sizeof(RainTodayIconToDraw), "%s","\U0000F079");
       }

       if(strcmp(settings.precipratestringpws, "0") != 0){
         APP_LOG(APP_LOG_LEVEL_DEBUG, "raining, precipratestringpws is %s",settings.precipratestringpws);
         snprintf(RainNowIconToDraw, sizeof(RainNowIconToDraw), "%s","\U0000F084");
       } else {
          APP_LOG(APP_LOG_LEVEL_DEBUG, "not raining, precipratestringpws is %s",settings.precipratestringpws);
         snprintf(RainNowIconToDraw, sizeof(RainNowIconToDraw), "%s","\U0000F055");
       }

       //PWS Wind speed
       graphics_context_set_text_color(ctx,ColorSelect(settings.Text9Color,settings.Text9ColorN));
       graphics_draw_text(ctx, SpeedPWSToDraw, FontTempFore, WindKtsRect, GTextOverflowModeWordWrap, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
       //graphics_draw_text(ctx, SpeedForeToDraw, FontTempFore, WindForeKtsRect, GTextOverflowModeWordWrap, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);

       //PWS Wind Direction
       graphics_context_set_text_color(ctx,ColorSelect(settings.Text10Color,settings.Text10ColorN));
       graphics_draw_text(ctx, DirectionPWSToDraw, FontWeatherCondition, WindDirNowRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
      // graphics_draw_text(ctx, DirectionForeToDraw, FontWeatherCondition, WindDirForeRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);

       //Weathericons
       graphics_context_set_text_color(ctx,ColorSelect(settings.Text9Color,settings.Text9ColorN));
       graphics_draw_text(ctx, RainTodayToDraw, FontTempFore, RainRateRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
       //graphics_draw_text(ctx, RainTodayToDraw, FontTempFore, RainTodayRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);

       //weather temps
       graphics_context_set_text_color(ctx,ColorSelect(settings.Text8Color,settings.Text8ColorN));
       graphics_draw_text(ctx, TempPWSToDraw, FontTemp, TempRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
       graphics_draw_text(ctx, PressureToDraw, FontTempFore, PressureRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);

       //Weathericons
       graphics_context_set_text_color(ctx,ColorSelect(settings.Text7Color,settings.Text7ColorN));
       graphics_draw_text(ctx, RainNowIconToDraw, FontWeatherCondition, IconNowRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
       graphics_draw_text(ctx, RainTodayIconToDraw, FontWeatherCondition, IconForeRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);


     }//show forecast weather on tap (every 2 taps if PWS not available, every 3 taps if PWS is available)
  else //if(showForecastWeather ==1 || showForecastWeather == 3 || showForecastWeather == 5)//showForecastWeather
      {
        //#ifndef PBL_MICROPHONE  //draw rain1h icon instead of rain graph on aplite
        //char Cond1hToDraw[4];
        //snprintf(Cond1hToDraw, sizeof(Cond1hToDraw), "%s",settings.icon1hstring);

        //Weathericon1h
        //graphics_context_set_text_color(ctx,ColorSelect(settings.Text7Color,settings.Text7ColorN));
        //graphics_draw_text(ctx, Cond1hToDraw, FontWeatherCondition, Icon1hRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
        //#endif

        char SunsetIconToShow[4];

        snprintf(SunsetIconToShow, sizeof(SunsetIconToShow),  "%s", "\U0000F052");

        char SunriseIconToShow[4];

        snprintf(SunriseIconToShow, sizeof(SunriseIconToShow),  "%s",  "\U0000F051");

        //sunsettime variable by clock setting
        char SunsetToDraw[9];
        if (clock_is_24h_style()){
          snprintf(SunsetToDraw, sizeof(SunsetToDraw), "%s",settings.sunsetstring);
        }
        else {
          snprintf(SunsetToDraw, sizeof(SunsetToDraw), "%s",settings.sunsetstring12);
        }

        char SunriseToDraw[9];
        if (clock_is_24h_style()){
           snprintf(SunriseToDraw, sizeof(SunriseToDraw), "%s",settings.sunrisestring);
         }
        else {
           snprintf(SunriseToDraw, sizeof(SunriseToDraw), "%s",settings.sunrisestring12);
         }

         char MoonToDraw[4];
         snprintf(MoonToDraw, sizeof(MoonToDraw), "%s",settings.moonstring);

         int s_uvmax_level = settings.UVIndexMax;
         int s_uvnow_level = settings.UVIndexNow;
         int s_uvday_level = settings.UVIndexDay;
         //int s_uvday_level = 14;

         APP_LOG(APP_LOG_LEVEL_DEBUG, "s_uvday_level is %d",s_uvday_level);

              if(settings.UseUVI){

                GRect UVDayValueRect = config.UVDayValueRect[0];
               
                // GRect UVDayValueRect =
                //   (PBL_IF_ROUND_ELSE(
                //     GRect(0,90,18,18),
                //     GRect(9+84+6,62+30-6,18,18)));

                graphics_context_set_fill_color(ctx, ColorSelect(settings.UVArcColor,settings.UVArcColorN));
                
                GRect arc_bounds = config.arc_bounds[0];

                // GRect arc_bounds =
                //   PBL_IF_ROUND_ELSE(
                //     GRect (4,8+52,28,28),
                //     GRect (90+4,8+80-4,28,28)
                //     );

                int32_t angle_start = DEG_TO_TRIGANGLE(180+30);
                int32_t angle_end = DEG_TO_TRIGANGLE(360+180-30);
                uint16_t inset_thickness = 2;
                graphics_fill_radial(ctx,arc_bounds,GOvalScaleModeFitCircle,inset_thickness,angle_start,angle_end);

                graphics_context_set_fill_color(ctx, ColorSelect(settings.UVMaxColor,settings.UVMaxColorN));// GColorBlack);
                //graphics_fill_rect(ctx, UVMaxRect, 0, GCornerNone);

                
                GRect arc_bounds_max = config.arc_bounds_max[0];
                
                // GRect arc_bounds_max =
                //     PBL_IF_ROUND_ELSE(
                //       GRect (2,6+52,32,32),
                //       GRect (88+4,6+80-4,32,32)
                //     );

                int32_t angle_start_max = DEG_TO_TRIGANGLE(180+30);
                int32_t angle_end_max = DEG_TO_TRIGANGLE((180+30)+ ((360-60)*s_uvmax_level/10));
                uint16_t inset_thickness_max = 4;
                graphics_fill_radial(ctx,arc_bounds_max,GOvalScaleModeFitCircle,inset_thickness_max,angle_start_max,angle_end_max);

                graphics_context_set_fill_color(ctx, ColorSelect(settings.UVNowColor, settings.UVNowColorN));
                //graphics_fill_rect(ctx,UVNowRect, 3, GCornersAll);

                
                GRect arc_bounds_now = config.arc_bounds_now[0];
                
                // GRect arc_bounds_now =
                //     PBL_IF_ROUND_ELSE(
                //       GRect (-2,2+52,40,40),
                //       GRect (84+4,2+80-4,40,40)
                //     );

                    int32_t angle_start_now = DEG_TO_TRIGANGLE((180+30)+((360-60)*s_uvnow_level/10)-3);
                    int32_t angle_end_now = DEG_TO_TRIGANGLE((180+30)+((360-60)*s_uvnow_level/10)+3);

                uint16_t inset_thickness_now = 10;
                graphics_fill_radial(ctx,arc_bounds_now,GOvalScaleModeFitCircle,inset_thickness_now,angle_start_now,angle_end_now);


                char UVValueToDraw[4];
                snprintf(UVValueToDraw, sizeof(UVValueToDraw), "%d",s_uvday_level);
                graphics_context_set_text_color(ctx,ColorSelect(settings.UVValColor,settings.UVValColorN));
                if (s_uvday_level<20){
                  graphics_draw_text(ctx, UVValueToDraw, FontTempFore, UVDayValueRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
                }
                else{
                  graphics_draw_text(ctx, UVValueToDraw, FontUV, UVDayValueRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
                }
              }
              else{




                graphics_context_set_text_color(ctx,ColorSelect(settings.Text4Color,settings.Text4ColorN));
                graphics_draw_text(ctx, MoonToDraw, FontWeatherCondition, MoonRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);



              }



        char StepsToDraw[10];
        snprintf(StepsToDraw, sizeof(StepsToDraw), "%s",s_current_steps_buffer);

        char precipToDraw[10];
        //snprintf(precipToDraw, sizeof(precipToDraw), "%s",settings.popstring);
        snprintf(precipToDraw, sizeof(precipToDraw), "%s",settings.rainstring);




  //precip chance
        graphics_context_set_text_color(ctx,ColorSelect(settings.Text8Color,settings.Text8ColorN));
        graphics_draw_text(ctx, precipToDraw, FontTempFore, PrecipRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
    //    graphics_draw_text(ctx, RainTimeToDraw, FontTempFore, PrecipRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);

        graphics_context_set_text_color(ctx, ColorSelect(settings.Text2Color, settings.Text2ColorN));
        graphics_draw_text(ctx, SunsetIconToShow, FontWeatherIcons, SunsetIconRect, GTextOverflowModeFill,PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
        graphics_draw_text(ctx, SunriseIconToShow, FontWeatherIcons, SunriseIconRect, GTextOverflowModeFill,PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);

        graphics_context_set_text_color(ctx, ColorSelect(settings.Text2Color, settings.Text2ColorN));
        if (clock_is_24h_style()){
          graphics_draw_text(ctx, SunsetToDraw, FontTempFore, SunsetRect, GTextOverflowModeWordWrap, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
          graphics_draw_text(ctx, SunriseToDraw, FontTempFore, SunriseRect, GTextOverflowModeWordWrap, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
          }
        else {
          graphics_draw_text(ctx, SunsetToDraw, FontTempFore, SunsetRect, GTextOverflowModeWordWrap, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
          graphics_draw_text(ctx, SunriseToDraw, FontTempFore, SunriseRect, GTextOverflowModeWordWrap, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
          }


        if (!settings.HealthOff && step_data_is_available())  {
          graphics_context_set_text_color(ctx,ColorSelect(settings.Text1Color,settings.Text1ColorN));
          graphics_draw_text(ctx, StepsToDraw, FontTempFore, StepsRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentCenter), NULL);
          }
          else{
            graphics_context_set_text_color(ctx,ColorSelect(settings.Text8Color,settings.Text8ColorN));
            graphics_draw_text(ctx, TempForeToDraw, FontTempFore, PBL_IF_ROUND_ELSE(StepsRect,TempForeRect), GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
        }
      }
  #else
      if ((showForecastWeather==0 ||showForecastWeather==2 || showForecastWeather==4))
          {
            char CondToDraw[4];
            char CondForeToDraw[4];
            char TempToDraw[8];

            char SpeedToDraw[10];
            char SpeedForeToDraw[10];
            char DirectionToDraw[4];
            char DirectionForeToDraw[4];

            snprintf(SpeedToDraw,sizeof(SpeedToDraw),"%s",settings.windstring);
            snprintf(SpeedForeToDraw,sizeof(SpeedForeToDraw),"%s",settings.windavestring);
            snprintf(DirectionToDraw,sizeof(DirectionToDraw),"%s",settings.windiconnowstring);
            snprintf(DirectionForeToDraw,sizeof(DirectionForeToDraw),"%s", settings.windiconavestring);
            snprintf(CondToDraw, sizeof(CondToDraw), "%s",settings.iconnowstring);

            snprintf(TempToDraw, sizeof(TempToDraw), "%s",settings.tempstring);

            snprintf(CondForeToDraw, sizeof(CondForeToDraw), "%s",settings.iconforestring);


            //Wind speed
            graphics_context_set_text_color(ctx,ColorSelect(settings.Text9Color,settings.Text9ColorN));
            graphics_draw_text(ctx, SpeedToDraw, FontTempFore, WindKtsRect, GTextOverflowModeWordWrap, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
            graphics_draw_text(ctx, SpeedForeToDraw, FontTempFore, WindForeKtsRect, GTextOverflowModeWordWrap, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
            //Wind Direction
            graphics_context_set_text_color(ctx,ColorSelect(settings.Text10Color,settings.Text10ColorN));
            graphics_draw_text(ctx, DirectionToDraw, FontWeatherCondition, WindDirNowRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
            graphics_draw_text(ctx, DirectionForeToDraw, FontWeatherCondition, WindDirForeRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);

            //Weathericons
            graphics_context_set_text_color(ctx,ColorSelect(settings.Text7Color,settings.Text7ColorN));
            graphics_draw_text(ctx, CondToDraw, FontWeatherCondition, IconNowRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);

            //weather temps
            graphics_context_set_text_color(ctx,ColorSelect(settings.Text8Color,settings.Text8ColorN));
            graphics_draw_text(ctx, TempToDraw, FontTemp, TempRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);


            graphics_context_set_text_color(ctx,ColorSelect(settings.Text7Color,settings.Text7ColorN));
            graphics_draw_text(ctx, CondForeToDraw, FontWeatherCondition, IconForeRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);

            graphics_context_set_text_color(ctx,ColorSelect(settings.Text8Color,settings.Text8ColorN));
            graphics_draw_text(ctx, TempForeToDraw, FontTempFore, TempForeRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);


          }
      else
          {
            //#ifndef PBL_MICROPHONE  //draw rain1h icon instead of rain graph on aplite
            //char Cond1hToDraw[4];
            //snprintf(Cond1hToDraw, sizeof(Cond1hToDraw), "%s",settings.icon1hstring);

            //Weathericon1h
            //graphics_context_set_text_color(ctx,ColorSelect(settings.Text7Color,settings.Text7ColorN));
            //graphics_draw_text(ctx, Cond1hToDraw, FontWeatherCondition, Icon1hRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
            //#endif

            char SunsetIconToShow[4];

            snprintf(SunsetIconToShow, sizeof(SunsetIconToShow),  "%s", "\U0000F052");

            char SunriseIconToShow[4];

            snprintf(SunriseIconToShow, sizeof(SunriseIconToShow),  "%s",  "\U0000F051");

            //sunsettime variable by clock setting
            char SunsetToDraw[8];
            if (clock_is_24h_style()){
              snprintf(SunsetToDraw, sizeof(SunsetToDraw), "%s",settings.sunsetstring);
            }
            else {
              snprintf(SunsetToDraw, sizeof(SunsetToDraw), "%s",settings.sunsetstring12);
            }

            char SunriseToDraw[8];
            if (clock_is_24h_style()){
               snprintf(SunriseToDraw, sizeof(SunriseToDraw), "%s",settings.sunrisestring);
             }
            else {
               snprintf(SunriseToDraw, sizeof(SunriseToDraw), "%s",settings.sunrisestring12);
             }

             char MoonToDraw[4];
             snprintf(MoonToDraw, sizeof(MoonToDraw), "%s",settings.moonstring);

             int s_uvmax_level = settings.UVIndexMax;
             int s_uvnow_level = settings.UVIndexNow;
             int s_uvday_level = settings.UVIndexDay;
             //int s_uvday_level = 14;

             APP_LOG(APP_LOG_LEVEL_DEBUG, "s_uvday_level is %d",s_uvday_level);

                  if(settings.UseUVI){



                    GRect UVDayValueRect =
                      (PBL_IF_ROUND_ELSE(
                        GRect(0,90,18,18),
                        GRect(9+84,62,18,18)));

                    graphics_context_set_fill_color(ctx, ColorSelect(settings.UVArcColor,settings.UVArcColor));
                    GRect arc_bounds =
                      PBL_IF_ROUND_ELSE(
                        GRect (4,8+52,28,28),
                        GRect (90,8+84,28,28)
                        );

                    int32_t angle_start = DEG_TO_TRIGANGLE(180+30);
                    int32_t angle_end = DEG_TO_TRIGANGLE(360+180-30);
                    uint16_t inset_thickness = 2;
                    graphics_fill_radial(ctx,arc_bounds,GOvalScaleModeFitCircle,inset_thickness,angle_start,angle_end);

                    graphics_context_set_fill_color(ctx, ColorSelect(settings.UVMaxColor,settings.UVMaxColor));// GColorBlack);
                    //graphics_fill_rect(ctx, UVMaxRect, 0, GCornerNone);

                    GRect arc_bounds_max =
                        PBL_IF_ROUND_ELSE(
                          GRect (2,6+52,32,32),
                          GRect (88,6+84,32,32)
                        );

                    int32_t angle_start_max = DEG_TO_TRIGANGLE(180+30);
                    int32_t angle_end_max = DEG_TO_TRIGANGLE((180+30)+ ((360-60)*s_uvmax_level/10));
                    uint16_t inset_thickness_max = 4;
                    graphics_fill_radial(ctx,arc_bounds_max,GOvalScaleModeFitCircle,inset_thickness_max,angle_start_max,angle_end_max);

                    graphics_context_set_fill_color(ctx, ColorSelect(settings.UVNowColor, settings.UVNowColor));
                    //graphics_fill_rect(ctx,UVNowRect, 3, GCornersAll);

                    GRect arc_bounds_now =
                        PBL_IF_ROUND_ELSE(
                          GRect (-2,2+52,40,40),
                          GRect (84,2+84,40,40)
                        );
                        int32_t angle_start_now = DEG_TO_TRIGANGLE((180+30)+((360-60)*s_uvnow_level/10)-3);
                        int32_t angle_end_now = DEG_TO_TRIGANGLE((180+30)+((360-60)*s_uvnow_level/10)+3);

                    uint16_t inset_thickness_now = 10;
                    graphics_fill_radial(ctx,arc_bounds_now,GOvalScaleModeFitCircle,inset_thickness_now,angle_start_now,angle_end_now);


                    char UVValueToDraw[4];
                    snprintf(UVValueToDraw, sizeof(UVValueToDraw), "%d",s_uvday_level);
                    graphics_context_set_text_color(ctx,ColorSelect(settings.UVValColor,settings.UVValColor));
                    if (s_uvday_level<20){
                      graphics_draw_text(ctx, UVValueToDraw, FontTempFore, UVDayValueRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
                    }
                    else{
                      graphics_draw_text(ctx, UVValueToDraw, FontUV, UVDayValueRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
                    }
                  }
                  else{




                    graphics_context_set_text_color(ctx,ColorSelect(settings.Text4Color,settings.Text4ColorN));
                    graphics_draw_text(ctx, MoonToDraw, FontWeatherCondition, MoonRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);



                  }


            char StepsToDraw[10];
            snprintf(StepsToDraw, sizeof(StepsToDraw), "%s",s_current_steps_buffer);

            char precipToDraw[10];
            //snprintf(precipToDraw, sizeof(precipToDraw), "%s",settings.popstring);
            snprintf(precipToDraw, sizeof(precipToDraw), "%s",settings.rainstring);




      //precip chance
            graphics_context_set_text_color(ctx,ColorSelect(settings.Text8Color,settings.Text8ColorN));
            graphics_draw_text(ctx, precipToDraw, FontTempFore, PrecipRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
        //    graphics_draw_text(ctx, RainTimeToDraw, FontTempFore, PrecipRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);

            graphics_context_set_text_color(ctx, ColorSelect(settings.Text2Color, settings.Text2ColorN));
            graphics_draw_text(ctx, SunsetIconToShow, FontWeatherIcons, SunsetIconRect, GTextOverflowModeFill,PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
            graphics_draw_text(ctx, SunriseIconToShow, FontWeatherIcons, SunriseIconRect, GTextOverflowModeFill,PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);

            graphics_context_set_text_color(ctx, ColorSelect(settings.Text2Color, settings.Text2ColorN));
            if (clock_is_24h_style()){
              graphics_draw_text(ctx, SunsetToDraw, FontTempFore, SunsetRect, GTextOverflowModeWordWrap, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
              graphics_draw_text(ctx, SunriseToDraw, FontTempFore, SunriseRect, GTextOverflowModeWordWrap, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
              }
            else {
              graphics_draw_text(ctx, SunsetToDraw, FontTempFore, SunsetRect, GTextOverflowModeWordWrap, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
              graphics_draw_text(ctx, SunriseToDraw, FontTempFore, SunriseRect, GTextOverflowModeWordWrap, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
              }

            graphics_context_set_text_color(ctx,ColorSelect(settings.Text8Color,settings.Text8ColorN));
            graphics_draw_text(ctx, TempForeToDraw, FontTempFore, PBL_IF_ROUND_ELSE(StepsRect,TempForeRect), GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
          }
  #endif
}



static void layer_update_proc_bt(Layer * layer3, GContext * ctx3){
  
  GRect BTIconRect = config.BTIconRect[0];
  
  // GRect BTIconRect =
  //   (PBL_IF_ROUND_ELSE(
  //     GRect(90,4,24,20),
  //     GRect(49,80,47,20)));

 graphics_context_set_text_color(ctx3, ColorSelect(settings.Text5Color, settings.Text5ColorN));
 graphics_draw_text(ctx3, "z", FontBTQTIcons, BTIconRect, GTextOverflowModeFill,GTextAlignmentCenter, NULL);

}

static void layer_update_proc_qt(Layer * layer4, GContext * ctx4){
  GRect QTIconRect = config.QTIconRect[0];
  
  // GRect QTIconRect =
  //   (PBL_IF_ROUND_ELSE(
  //     GRect(90-24,4,24,20),
  //     GRect(49,97,47,20)));

 quiet_time_icon();

 graphics_context_set_text_color(ctx4, ColorSelect(settings.Text5Color, settings.Text5ColorN));
 graphics_draw_text(ctx4, "\U0000E061", FontBTQTIcons, QTIconRect, GTextOverflowModeFill,GTextAlignmentCenter, NULL);

}


/////////////////////////////////////////
////Init: Handle Settings and Weather////
/////////////////////////////////////////
// Read settings from persistent storage
static void prv_load_settings(){
  // Load the default settings
  prv_default_settings();
  // Read settings from persistent storage, if they exist
  persist_read_data(SETTINGS_KEY, & settings, sizeof(settings));
}
// Save the settings to persistent storage
static void prv_save_settings(){
  persist_write_data(SETTINGS_KEY, & settings, sizeof(settings));

}
// Handle the response from AppMessage
static void prv_inbox_received_handler(DictionaryIterator * iter, void * context){
  s_loop = s_loop + 1;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "prv_inbox_received_handler s_loop is %d",s_loop);

  if (s_loop == 1){
    //  strcpy(settings.icon1hstring, "\U0000F04C");
    //  strcpy(settings.moonstring, "\U0000F04C");
      strcpy(settings.iconnowstring, "\U0000F03e");

    if(clock_is_24h_style()){
        strcpy(settings.rainstring, settings.raintime24h);

    }
    else{
        strcpy(settings.rainstring, settings.raintime12h);

    }
  }
  //  Colours

  Tuple * sd1_color_t = dict_find(iter, MESSAGE_KEY_FrameColor2);
  if (sd1_color_t){
    settings.FrameColor2 = GColorFromHEX(sd1_color_t-> value -> int32);
  }
  Tuple * nsd1_color_t = dict_find(iter, MESSAGE_KEY_FrameColor2N);
  if (nsd1_color_t){
    settings.FrameColor2N = GColorFromHEX(nsd1_color_t-> value -> int32);
  }
   Tuple * sd2_color_t = dict_find(iter, MESSAGE_KEY_FrameColor);
  if (sd2_color_t){
    settings.FrameColor = GColorFromHEX(sd2_color_t-> value -> int32);
  }
  Tuple * nsd2_color_t = dict_find(iter, MESSAGE_KEY_FrameColorN);
  if (nsd2_color_t){
    settings.FrameColorN = GColorFromHEX(nsd2_color_t-> value -> int32);
  }
  ////////////
  Tuple * tx1_color_t = dict_find(iter, MESSAGE_KEY_Text1Color);
  if (tx1_color_t){
    settings.Text1Color = GColorFromHEX(tx1_color_t-> value -> int32);
  }
  Tuple * ntx1_color_t = dict_find(iter, MESSAGE_KEY_Text1ColorN);
  if (ntx1_color_t){
    settings.Text1ColorN = GColorFromHEX(ntx1_color_t-> value -> int32);
  }
  ///////////////////////////////
  Tuple * hr_color_t = dict_find(iter, MESSAGE_KEY_HourColor);
  if (hr_color_t){
    settings.HourColor = GColorFromHEX(hr_color_t-> value -> int32);
  }
  Tuple * nthr_color_t = dict_find(iter, MESSAGE_KEY_HourColorN);
  if (nthr_color_t){
    settings.HourColorN = GColorFromHEX(nthr_color_t-> value -> int32);
  }
  Tuple * min_color_t = dict_find(iter, MESSAGE_KEY_MinColor);
  if (min_color_t){
    settings.MinColor = GColorFromHEX(min_color_t-> value -> int32);
  }
  Tuple * ntmin_color_t = dict_find(iter, MESSAGE_KEY_MinColorN);
  if (ntmin_color_t){
    settings.MinColorN = GColorFromHEX(ntmin_color_t-> value -> int32);
  }
  Tuple * date_color_t = dict_find(iter, MESSAGE_KEY_DateColor);
  if (date_color_t){
    settings.DateColor = GColorFromHEX(date_color_t-> value -> int32);
  }
  Tuple * ntdate_color_t = dict_find(iter, MESSAGE_KEY_DateColorN);
  if (ntdate_color_t){
    settings.DateColorN = GColorFromHEX(ntdate_color_t-> value -> int32);
  }
  ///////////////////////////////
  Tuple * tx2_color_t = dict_find(iter, MESSAGE_KEY_Text2Color);
  if (tx2_color_t){
    settings.Text2Color = GColorFromHEX(tx2_color_t-> value -> int32);
  }
  Tuple * ntx2_color_t = dict_find(iter, MESSAGE_KEY_Text2ColorN);
  if (ntx2_color_t){
    settings.Text2ColorN = GColorFromHEX(ntx2_color_t-> value -> int32);
  }
   Tuple * tx3_color_t = dict_find(iter, MESSAGE_KEY_Text3Color);
  if (tx3_color_t){
    settings.Text3Color = GColorFromHEX(tx3_color_t-> value -> int32);
  }
  Tuple * ntx3_color_t = dict_find(iter, MESSAGE_KEY_Text3ColorN);
  if (ntx3_color_t){
    settings.Text3ColorN = GColorFromHEX(ntx3_color_t-> value -> int32);
  }
  Tuple * tx4_color_t = dict_find(iter,MESSAGE_KEY_Text4Color);
  if (tx4_color_t){
    settings.Text4Color = GColorFromHEX(tx4_color_t-> value -> int32);
    }
  Tuple * ntx4_color_t = dict_find(iter, MESSAGE_KEY_Text4ColorN);
  if(ntx4_color_t){
    settings.Text4ColorN = GColorFromHEX(ntx4_color_t-> value -> int32);
  }
  Tuple * tx5_color_t = dict_find(iter,MESSAGE_KEY_Text5Color);
  if (tx5_color_t){
    settings.Text5Color = GColorFromHEX(tx5_color_t-> value -> int32);
    }
  Tuple * ntx5_color_t = dict_find(iter, MESSAGE_KEY_Text5ColorN);
  if(ntx5_color_t){
    settings.Text5ColorN = GColorFromHEX(ntx5_color_t-> value -> int32);
  }
   Tuple * tx6_color_t = dict_find(iter,MESSAGE_KEY_Text6Color);
  if (tx6_color_t){
    settings.Text6Color = GColorFromHEX(tx6_color_t-> value -> int32);
    }
  Tuple * ntx6_color_t = dict_find(iter, MESSAGE_KEY_Text6ColorN);
  if(ntx6_color_t){
    settings.Text6ColorN = GColorFromHEX(ntx6_color_t-> value -> int32);
  }
  if (tx6_color_t){
    settings.Text6Color = GColorFromHEX(tx6_color_t-> value -> int32);
    }

  Tuple * tx7_color_t = dict_find(iter,MESSAGE_KEY_Text7Color);
 if (tx7_color_t){
   settings.Text7Color = GColorFromHEX(tx7_color_t-> value -> int32);
   }
 Tuple * ntx7_color_t = dict_find(iter, MESSAGE_KEY_Text7ColorN);
 if(ntx7_color_t){
   settings.Text7ColorN = GColorFromHEX(ntx7_color_t-> value -> int32);
 }
 if (tx7_color_t){
   settings.Text7Color = GColorFromHEX(tx7_color_t-> value -> int32);
   }

 Tuple * tx8_color_t = dict_find(iter,MESSAGE_KEY_Text8Color);
if (tx8_color_t){
  settings.Text8Color = GColorFromHEX(tx8_color_t-> value -> int32);
  }
Tuple * ntx8_color_t = dict_find(iter, MESSAGE_KEY_Text8ColorN);
if(ntx8_color_t){
  settings.Text8ColorN = GColorFromHEX(ntx8_color_t-> value -> int32);
}
if (tx8_color_t){
  settings.Text8Color = GColorFromHEX(tx8_color_t-> value -> int32);
  }

Tuple * tx9_color_t = dict_find(iter,MESSAGE_KEY_Text9Color);
if (tx9_color_t){
 settings.Text9Color = GColorFromHEX(tx9_color_t-> value -> int32);
 }
Tuple * ntx9_color_t = dict_find(iter, MESSAGE_KEY_Text9ColorN);
if(ntx9_color_t){
 settings.Text9ColorN = GColorFromHEX(ntx9_color_t-> value -> int32);
}
if (tx9_color_t){
 settings.Text9Color = GColorFromHEX(tx9_color_t-> value -> int32);
 }

Tuple * tx10_color_t = dict_find(iter,MESSAGE_KEY_Text10Color);
if (tx10_color_t){
settings.Text10Color = GColorFromHEX(tx10_color_t-> value -> int32);
}
Tuple * ntx10_color_t = dict_find(iter, MESSAGE_KEY_Text10ColorN);
if(ntx10_color_t){
settings.Text10ColorN = GColorFromHEX(ntx10_color_t-> value -> int32);
}
if (tx10_color_t){
settings.Text10Color = GColorFromHEX(tx10_color_t-> value -> int32);
}

Tuple * uvarccol_t = dict_find(iter,MESSAGE_KEY_UVArcColor);
if (uvarccol_t){
settings.UVArcColor = GColorFromHEX(uvarccol_t-> value -> int32);
}

Tuple * uvmaxcol_t = dict_find(iter,MESSAGE_KEY_UVMaxColor);
if (uvmaxcol_t){
settings.UVMaxColor = GColorFromHEX(uvmaxcol_t-> value -> int32);
}

Tuple * uvvalcol_t = dict_find(iter,MESSAGE_KEY_UVValColor);
if (uvvalcol_t){
settings.UVValColor = GColorFromHEX(uvvalcol_t-> value -> int32);
}

Tuple * uvnowcol_t = dict_find(iter,MESSAGE_KEY_UVNowColor);
if (uvnowcol_t){
settings.UVNowColor = GColorFromHEX(uvnowcol_t-> value -> int32);
}

Tuple * uvarccol_nt = dict_find(iter,MESSAGE_KEY_UVArcColorN);
if (uvarccol_nt){
settings.UVArcColorN = GColorFromHEX(uvarccol_nt-> value -> int32);
}

Tuple * uvmaxcol_nt = dict_find(iter,MESSAGE_KEY_UVMaxColorN);
if (uvmaxcol_nt){
settings.UVMaxColorN = GColorFromHEX(uvmaxcol_nt-> value -> int32);
}

Tuple * uvvalcol_nt = dict_find(iter,MESSAGE_KEY_UVValColorN);
if (uvvalcol_nt){
settings.UVValColorN = GColorFromHEX(uvvalcol_nt-> value -> int32);
}

Tuple * uvnowcol_nt = dict_find(iter,MESSAGE_KEY_UVNowColorN);
if (uvnowcol_nt){
settings.UVNowColorN = GColorFromHEX(uvnowcol_nt-> value -> int32);
}

// Weather conditions
Tuple * uvi_t = dict_find(iter, MESSAGE_KEY_UseUVI);
if (uvi_t){
  if (uvi_t -> value -> int32 == 0){
    settings.UseUVI = true;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "UVI switched on");
  } else {
    settings.UseUVI = false;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "MoonPhase switched on");
  }
}

Tuple * uvmax_tuple = dict_find(iter, MESSAGE_KEY_UVIndexMax);
if (uvmax_tuple){
  settings.UVIndexMax = (int) uvmax_tuple -> value -> int32;
APP_LOG(APP_LOG_LEVEL_DEBUG, "UVIndexMax is %d",settings.UVIndexMax);
}

Tuple * uvday_tuple = dict_find(iter, MESSAGE_KEY_UVIndexDay);
if (uvday_tuple){
  settings.UVIndexDay = (int) uvday_tuple -> value -> int32;
APP_LOG(APP_LOG_LEVEL_DEBUG, "UVIndexDay is %d",settings.UVIndexDay);
}

Tuple * uvnow_tuple = dict_find(iter, MESSAGE_KEY_UVIndexNow);
if (uvnow_tuple){
  settings.UVIndexNow = (int) uvnow_tuple -> value -> int32;
  s_uvnow_level = settings.UVIndexNow;
}

  Tuple * wtemp_t = dict_find(iter, MESSAGE_KEY_WeatherTemp);
  if (wtemp_t){
  snprintf(settings.tempstring, sizeof(settings.tempstring), "%s", wtemp_t -> value -> cstring);
  }

    #ifdef PBL_ROUND
      Tuple * wwind_t = dict_find(iter, MESSAGE_KEY_WeatherWindRound);
      if (wwind_t){
      snprintf(settings.windstring, sizeof(settings.windstring), "%s", wwind_t -> value -> cstring);
      }
    #else
      Tuple * wwind_t = dict_find(iter, MESSAGE_KEY_WeatherWind);
      if (wwind_t){
      snprintf(settings.windstring, sizeof(settings.windstring), "%s", wwind_t -> value -> cstring);
      }
    #endif

#ifdef PBL_MICROPHONE
    #ifdef PBL_ROUND
      Tuple * wwindpws_t = dict_find(iter, MESSAGE_KEY_WeatherWindRoundPWS);
      if (wwindpws_t){
      snprintf(settings.windstringpws, sizeof(settings.windstringpws), "%s", wwindpws_t -> value -> cstring);
      }
    #else
      Tuple * wwindpws_t = dict_find(iter, MESSAGE_KEY_WeatherWindPWS);
      if (wwindpws_t){
      snprintf(settings.windstringpws, sizeof(settings.windstringpws), "%s", wwindpws_t -> value -> cstring);
      }
    #endif
#endif

  Tuple * iconwinddirnow_tuple = dict_find(iter, MESSAGE_KEY_WindIconNow);
  if (iconwinddirnow_tuple){
  snprintf(settings.windiconnowstring,sizeof(settings.windiconnowstring),"%s",wind_direction[(int)iconwinddirnow_tuple->value->int32]);
  }

#ifdef PBL_MICROPHONE
  Tuple * iconwinddirnowpws_tuple = dict_find(iter, MESSAGE_KEY_WindIconNowPWS);
  if (iconwinddirnowpws_tuple){
  snprintf(settings.windiconnowstringpws,sizeof(settings.windiconnowstringpws),"%s",wind_direction[(int)iconwinddirnowpws_tuple->value->int32]);
  }
#endif

  Tuple * iconnow_tuple = dict_find(iter, MESSAGE_KEY_IconNow);
    if (iconnow_tuple){
      snprintf(settings.iconnowstring,sizeof(settings.iconnowstring),"%s",weather_conditions[(int)iconnow_tuple->value->int32]);
  }

  Tuple * icon1h_tuple = dict_find(iter, MESSAGE_KEY_Cond1h);
    if (icon1h_tuple){
      snprintf(settings.icon1hstring,sizeof(settings.icon1hstring),"%s",weather_conditions[(int)icon1h_tuple->value->int32]);
  }


  #ifdef PBL_ROUND
  Tuple * wwindave_t = dict_find(iter, MESSAGE_KEY_WindForeRound);
  if (wwindave_t){
    snprintf(settings.windavestring, sizeof(settings.windavestring), "%s", wwindave_t -> value -> cstring);
  }
  #else
  Tuple * wwindave_t = dict_find(iter, MESSAGE_KEY_WindFore);
  if (wwindave_t){
    snprintf(settings.windavestring, sizeof(settings.windavestring), "%s", wwindave_t -> value -> cstring);
  }
  #endif
    //Hour Sunrise and Sunset
  Tuple * sunrise_t = dict_find(iter, MESSAGE_KEY_HourSunrise);
  if (sunrise_t){
    HourSunrise = (int) sunrise_t -> value -> int32;
  }
  Tuple * sunset_t = dict_find(iter, MESSAGE_KEY_HourSunset);
  if (sunset_t){
    HourSunset = (int) sunset_t -> value -> int32;
  }

  Tuple * sunset_dt = dict_find(iter, MESSAGE_KEY_WEATHER_SUNSET_KEY);
  if (sunset_dt){
     snprintf(settings.sunsetstring, sizeof(settings.sunsetstring), "%s", sunset_dt -> value -> cstring);
  }
   Tuple * sunrise_dt = dict_find(iter, MESSAGE_KEY_WEATHER_SUNRISE_KEY);
  if (sunrise_dt){
     snprintf(settings.sunrisestring, sizeof(settings.sunrisestring), "%s", sunrise_dt -> value -> cstring);
  }

  //12hr version of sunset & sunrise
  Tuple * sunset12_dt = dict_find(iter, MESSAGE_KEY_WEATHER_SUNSET_KEY_12H);
  if (sunset12_dt){
     snprintf(settings.sunsetstring12, sizeof(settings.sunsetstring12), "%s", sunset12_dt -> value -> cstring);
  }
  Tuple * sunrise12_dt = dict_find(iter, MESSAGE_KEY_WEATHER_SUNRISE_KEY_12H);
  if (sunrise12_dt){
     snprintf(settings.sunrisestring12, sizeof(settings.sunrisestring12), "%s", sunrise12_dt -> value -> cstring);
  }


  Tuple * iconfore_tuple = dict_find(iter, MESSAGE_KEY_IconFore);
  if (iconfore_tuple){
    snprintf(settings.iconforestring,sizeof(settings.iconforestring),"%s",weather_conditions[(int)iconfore_tuple->value->int32]);
  }

   Tuple * iconwinddirave_tuple = dict_find(iter, MESSAGE_KEY_WindIconAve);
  if (iconwinddirave_tuple){
    snprintf(settings.windiconavestring,sizeof(settings.windiconavestring),"%s",wind_direction[(int)iconwinddirave_tuple->value->int32]);
  }

  Tuple * wforetemp_t = dict_find(iter, MESSAGE_KEY_TempFore);
  if (wforetemp_t){
    snprintf(settings.temphistring, sizeof(settings.temphistring), "%s", wforetemp_t -> value -> cstring);
  }

  Tuple * rain1h_t = dict_find(iter, MESSAGE_KEY_rain1h);
  if (rain1h_t){
    snprintf(settings.rainstring, sizeof(settings.rainstring), "%s", rain1h_t -> value -> cstring);
  }

  Tuple * pop1h_t = dict_find(iter, MESSAGE_KEY_pop1h);
  if (pop1h_t){
    snprintf(settings.popstring, sizeof(settings.popstring), "%s", pop1h_t -> value -> cstring);
  }

  Tuple * raintime24_t = dict_find(iter, MESSAGE_KEY_raintime24h);
  if (raintime24_t){
     snprintf(settings.raintime24h, sizeof(settings.raintime24h), "%s", raintime24_t -> value -> cstring);
     //APP_LOG(APP_LOG_LEVEL_DEBUG, "Rain data captured at time %s",settings.raintime24h);
  }

  Tuple * raintime12_t = dict_find(iter, MESSAGE_KEY_raintime12h);
  if (raintime12_t){
     snprintf(settings.raintime12h, sizeof(settings.raintime12h), "%s", raintime12_t -> value -> cstring);
  }

  //#ifdef PBL_MICROPHONE
  Tuple * rain0_t = dict_find(iter, MESSAGE_KEY_rain0);
  if (rain0_t){
  settings.rain0 = (int) rain0_t -> value -> int32;
  }

  Tuple * rain10_t = dict_find(iter, MESSAGE_KEY_rain10);
  if (rain10_t){
  settings.rain10 = (int) rain10_t -> value -> int32;
  }
  Tuple * rain20_t = dict_find(iter, MESSAGE_KEY_rain20);
  if (rain20_t){
  settings.rain20 = (int) rain20_t -> value -> int32;
  }

  Tuple * rain30_t = dict_find(iter, MESSAGE_KEY_rain30);
  if (rain30_t){
  settings.rain30 = (int) rain30_t -> value -> int32;
  }

  Tuple * rain40_t = dict_find(iter, MESSAGE_KEY_rain40);
  if (rain40_t){
  settings.rain40 = (int) rain40_t -> value -> int32;
  }

  Tuple * rain50_t = dict_find(iter, MESSAGE_KEY_rain50);
  if (rain50_t){
  settings.rain50 = (int) rain50_t -> value -> int32;
  }

  Tuple * rain60_t = dict_find(iter, MESSAGE_KEY_rain60);
  if (rain60_t){
  settings.rain60 = (int) rain60_t -> value -> int32;
  }

//#ifdef PBL_MICROPHONE
  Tuple * rain5_t = dict_find(iter, MESSAGE_KEY_rain5);
  if (rain5_t){
  settings.rain5 = (int) rain5_t -> value -> int32;
  }

  Tuple * rain15_t = dict_find(iter, MESSAGE_KEY_rain15);
  if (rain15_t){
  settings.rain15 = (int) rain15_t -> value -> int32;
  }

  Tuple * rain25_t = dict_find(iter, MESSAGE_KEY_rain25);
  if (rain25_t){
  settings.rain25 = (int) rain25_t -> value -> int32;
  }

  Tuple * rain35_t = dict_find(iter, MESSAGE_KEY_rain35);
  if (rain35_t){
  settings.rain35 = (int) rain35_t -> value -> int32;
  }

  Tuple * rain45_t = dict_find(iter, MESSAGE_KEY_rain45);
  if (rain45_t){
  settings.rain45 = (int) rain45_t -> value -> int32;
  }

  Tuple * rain55_t = dict_find(iter, MESSAGE_KEY_rain55);
  if (rain55_t){
  settings.rain55 = (int) rain55_t -> value -> int32;
  }
//#endif



  Tuple * rainsize = dict_find(iter, MESSAGE_KEY_Rainmultiplier);
  if (rainsize){
    settings.Rainmultiplier = (int) rainsize -> value -> int32;

  }
  //#endif

  Tuple * wforetemplow_t = dict_find(iter, MESSAGE_KEY_TempForeLow);
  if (wforetemplow_t){
    snprintf(settings.templowstring, sizeof(settings.templowstring), "%s", wforetemplow_t -> value -> cstring);
  }

  Tuple * moon_tuple = dict_find(iter, MESSAGE_KEY_MoonPhase);
  if (moon_tuple){
    snprintf(settings.moonstring,sizeof(settings.moonstring),"%s",moon_phase[(int)moon_tuple->value->int32]);
  }

  Tuple * weather_t = dict_find(iter, MESSAGE_KEY_Weathertime);
  if (weather_t){
    settings.Weathertimecapture = (int) weather_t -> value -> int32;
  //   snprintf(settings.weathertimecapture, sizeof(settings.weathertimecapture), "%s", weather_t -> value -> cstring);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Weather data captured at time %d",settings.Weathertimecapture);
  }

  Tuple * neigh_t = dict_find(iter, MESSAGE_KEY_NameLocation);
  if (neigh_t){
    snprintf(citistring, sizeof(citistring), "%s", neigh_t -> value -> cstring);
  }
  //Control of data gathered for http
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Location Timezone is %s", citistring);
  if (strcmp(citistring, "") == 0 || strcmp(citistring,"NotSet") == 0 ){
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "Missing info at loop %d, GPS false, citistring is %s, 1 is %d, 2 is %d", s_loop, citistring, strcmp(citistring, ""),strcmp(citistring,"NotSet"));
    GPSOn = false;
  } else{
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "GPS working loop %d, GPS on, citistring is %s, 1 is %d, 2 is %d", s_loop, citistring, strcmp(citistring, ""),strcmp(citistring,"NotSet"));
    GPSOn = true;
  }

  Tuple * frequpdate = dict_find(iter, MESSAGE_KEY_UpSlider);
  if (frequpdate){
    settings.UpSlider = (int) frequpdate -> value -> int32;
    //Restart the counter
    s_countdown = settings.UpSlider;
  }


  Tuple * disntheme_t = dict_find(iter, MESSAGE_KEY_NightTheme);
  if (disntheme_t){
    if (disntheme_t -> value -> int32 == 0){
      settings.NightTheme = false;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "NTHeme off");
    } else {
    settings.NightTheme = true;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "NTHeme on");
      }
    }

  Tuple * health_t = dict_find(iter, MESSAGE_KEY_HealthOff);
  if (health_t){
    if (health_t -> value -> int32 == 0){
      settings.HealthOff = false;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Health on");
      get_step_count();
      display_step_count();
    } else {
      settings.HealthOff = true;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Health off");
      snprintf(s_current_steps_buffer, sizeof(s_current_steps_buffer),
       "%s", "");
    }
  }



#ifdef PBL_MICROPHONE
  Tuple * pws_t = dict_find(iter, MESSAGE_KEY_UsePWS);
  if (pws_t){
    if (pws_t -> value -> int32 == 0){
      settings.UsePWS = false;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "PWS switched on");
    } else {
      settings.UsePWS = true;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "PWS switched off");
    }
  }

  Tuple * wtemppws_t = dict_find(iter, MESSAGE_KEY_WeatherTempPWS);
  if (wtemppws_t){
  snprintf(settings.tempstringpws, sizeof(settings.tempstringpws), "%s", wtemppws_t -> value -> cstring);
  }
  if (strcmp(settings.tempstringpws, "") == 0){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Missing PWS info at loop %d, PWS false, tempstringpws is %s, UsePWS is %d, 1 is %d", s_loop, settings.tempstringpws, settings.UsePWS, strcmp(settings.tempstringpws, ""));
    PWSDataExists = false;
  } else{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "PWS working loop %d, PWS on, tempstringpws is %s, UsePWS is %d, 1 is %d", s_loop, settings.tempstringpws, settings.UsePWS, strcmp(settings.tempstringpws, ""));
    PWSDataExists = true;
  }

  Tuple * raintotalpws_t = dict_find(iter, MESSAGE_KEY_RainTotalTodayPWS);
  if (raintotalpws_t){
    snprintf(settings.preciptotalstringpws, sizeof(settings.preciptotalstringpws), "%s", raintotalpws_t -> value -> cstring);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Rain Total today PWS is %s",settings.preciptotalstringpws);
  }

  Tuple * rainratepws_t = dict_find(iter, MESSAGE_KEY_RainRatePWS);
  if (rainratepws_t){
    snprintf(settings.precipratestringpws, sizeof(settings.precipratestringpws), "%s", rainratepws_t -> value -> cstring);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Rain Rate PWS is %s",settings.precipratestringpws);
  }

  Tuple * pressurepws_t = dict_find(iter, MESSAGE_KEY_PressurePWS);
  if (pressurepws_t){
    snprintf(settings.pressurestringpws, sizeof(settings.pressurestringpws), "%s", pressurepws_t -> value -> cstring);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Pressure PWS is %s",settings.pressurestringpws);
  }

#endif

  Tuple * vibe_t = dict_find(iter, MESSAGE_KEY_VibeOn);
  if (vibe_t){
    if (vibe_t -> value -> int32 == 0){
      settings.VibeOn = false;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Vibe off");
    } else {
      settings.VibeOn = true;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Vibe on");
    }
  }

  Tuple * addzero12_t = dict_find(iter, MESSAGE_KEY_AddZero12h);
  if (addzero12_t){
    if (addzero12_t -> value -> int32 == 0){
      settings.AddZero12h = false;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Add Zero 12h off");
    } else {
    settings.AddZero12h = true;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Add Zero 12h on");
      }
    }

  Tuple * remzero24_t = dict_find(iter, MESSAGE_KEY_RemoveZero24h);
  if (remzero24_t){
    if (remzero24_t -> value -> int32 == 0){
      settings.RemoveZero24h = false;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Remove Zero 24h off");
    } else {
    settings.RemoveZero24h = true;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Remove Zero 24h off");
      }
    }

  layer_mark_dirty(s_canvas_weather_section);
  layer_mark_dirty(time_area_layer);
  layer_mark_dirty(s_canvas_rain);
  layer_mark_dirty(s_canvas_bt_icon);
  layer_mark_dirty(s_canvas_qt_icon);

  prv_save_settings();
}

//Load main layer
static void window_load(Window * window){
  Layer * window_layer = window_get_root_layer(window);
  GRect bounds4 = layer_get_bounds(window_layer);

  s_canvas_background = layer_create(bounds4);
    layer_set_update_proc(s_canvas_background, layer_update_proc_background);
    layer_add_child(window_layer, s_canvas_background);

  time_area_layer = layer_create(bounds4);
     layer_add_child(window_layer, time_area_layer);
     layer_set_update_proc(time_area_layer, update_time_area_layer);

    // #ifdef PBL_MICROPHONE
     s_canvas_rain = layer_create(bounds4);
           layer_set_update_proc (s_canvas_rain, layer_update_proc_rain);
           layer_add_child(window_layer, s_canvas_rain);
    // #endif


  s_canvas_bt_icon = layer_create(bounds4);
    layer_set_update_proc (s_canvas_bt_icon, layer_update_proc_bt);
    layer_add_child(window_layer, s_canvas_bt_icon);

  s_canvas_qt_icon = layer_create(bounds4);
    layer_set_update_proc (s_canvas_qt_icon, layer_update_proc_qt);
    layer_add_child(window_layer, s_canvas_qt_icon);

  s_canvas_weather_section = layer_create(bounds4);
    layer_set_update_proc(s_canvas_weather_section, layer_update_proc);
    layer_add_child(window_layer, s_canvas_weather_section);
  }


static void window_unload(Window * window){
  layer_destroy (s_canvas_background);
  layer_destroy(s_canvas_weather_section);
  layer_destroy(time_area_layer);
 //#ifdef PBL_MICROPHONE
  layer_destroy(s_canvas_rain);
//#endif
  layer_destroy(s_canvas_bt_icon);
  layer_destroy(s_canvas_qt_icon);
  window_destroy(s_window);

  ffont_destroy(time_font);
  fonts_unload_custom_font(FontDayOfTheWeekShortName);
  fonts_unload_custom_font(FontBTQTIcons);
  fonts_unload_custom_font(FontTemp);
  fonts_unload_custom_font(FontTempFore);
  fonts_unload_custom_font(FontUV);
  fonts_unload_custom_font(FontWeatherIcons);
  fonts_unload_custom_font(FontWeatherCondition);

}

void main_window_push(){
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers){
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);
}

void main_window_update(int hours, int minutes, int weekday, int day){
  s_hours = hours;
  s_minutes = minutes;
  s_day = day;
  s_weekday = weekday;

  layer_mark_dirty(time_area_layer);
}

static void tick_handler(struct tm * time_now, TimeUnits changed){

  main_window_update(time_now -> tm_hour, time_now -> tm_min, time_now -> tm_wday, time_now -> tm_mday);
  //update_time();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Tick at %d", time_now -> tm_min);
  //s_loop = 0;
  if (s_countdown == 0){
    //Reset
    s_countdown = settings.UpSlider;
  } else{
    s_countdown = s_countdown - 1;
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Countdown to update %d loop is %d, GPS is %d", s_countdown, s_loop, GPSOn);


  // Evaluate if is day or night
  int nowthehouris = s_hours * 100 + s_minutes;
  if (HourSunrise <= nowthehouris && nowthehouris <= HourSunset){
    IsNightNow = false;
  } else{
    IsNightNow = true;
  }
  // Extra catch on sunrise and sunset
  if (nowthehouris == HourSunrise || nowthehouris == HourSunset){
    s_countdown = 1;
  }
  if (GPSOn && settings.NightTheme){
    //Extra catch around 1159 to gather information of today
    if (nowthehouris == 1159 && s_countdown > 5){
      s_countdown = 1;
    };
    // Change Colors for night/day
    layer_mark_dirty(s_canvas_background);
    layer_mark_dirty(s_canvas_weather_section);
    layer_mark_dirty(time_area_layer);
    //#ifdef PBL_MICROPHONE
    layer_mark_dirty(s_canvas_rain);
    //#endif
    layer_mark_dirty(s_canvas_bt_icon);
    layer_mark_dirty(s_canvas_qt_icon);

  }
  // Get weather update every requested minutes and extra request 5 minutes earlier
  if (s_countdown == 0 || s_countdown == 5){
      APP_LOG(APP_LOG_LEVEL_DEBUG, "countdown is 0, updated weather at %d", time_now -> tm_min);
      s_loop = 0;
      // Begin dictionary
      DictionaryIterator * iter;
      app_message_outbox_begin( & iter);
      // Add a key-value pair
      dict_write_uint8(iter, 0, 0);
      // Send the message!
      app_message_outbox_send();
  }

//If GPS was off request weather every 15 minutes
  if (!GPSOn){
      if (settings.UpSlider > 15){
        if (s_countdown % 15 == 0){
          APP_LOG(APP_LOG_LEVEL_DEBUG, "Attempt to request GPS on %d", time_now -> tm_min);
          s_loop = 0;
          // Begin dictionary
          DictionaryIterator * iter;
          app_message_outbox_begin( & iter);
          // Add a key-value pair
          dict_write_uint8(iter, 0, 0);
          // Send the message!
          app_message_outbox_send();
        }
      }
    }

  //onreconnection(BTOn, connection_service_peek_pebble_app_connection());
  if (!BTOn && connection_service_peek_pebble_app_connection()){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "BT reconnected, requesting weather at %d", time_now -> tm_min);
    s_loop = 0;
    // Begin dictionary
    DictionaryIterator * iter;
    app_message_outbox_begin( & iter);
    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);
    // Send the message!
    app_message_outbox_send();
  }

  bluetooth_callback(connection_service_peek_pebble_app_connection());

 }

static void init(){

  prv_load_settings();

  // Listen for AppMessages

  s_countdown = settings.UpSlider;
  //Clean vars
  //strcpy(citistring, "NotSet");
  //strcpy(settings.icon1hstring, "\U0000F03D");
  strcpy(settings.iconnowstring, "\U0000F03D");

  s_loop = 0;

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  s_hours=t->tm_hour;
  s_minutes=t->tm_min;
  s_day=t->tm_mday;
  s_weekday=t->tm_wday;
  //Register and open
  app_message_register_inbox_received(prv_inbox_received_handler);
#ifdef PBL_MICROPHONE
  app_message_open(1024,1024);
#else
  app_message_open(512,512);
#endif

// Load Fonts
   // allocate fonts
  time_font =  ffont_create_from_resource(RESOURCE_ID_FONT_LIZ);

  #ifdef PBL_PLATFORM_EMERY
  FontDayOfTheWeekShortName = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LIZ_16));  //mod, was 12, font done
  FontBTQTIcons = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DRIPICONS_16)); //original
  FontTemp = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LIZ_37));  //mod, was 27 font done
  FontTempFore = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LIZ_24));  //mod, was 18 font done
  FontUV = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LIZ_19));  //mod, was 14 font done
  FontWeatherIcons = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHERICONS_20)); //mod was 16 font done
  FontWeatherCondition = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHERICONS_35));  //mod want 42 was 30 font done
  
  #else
  FontDayOfTheWeekShortName = fonts_load_custom_font(resource_get_handle(PBL_IF_ROUND_ELSE(RESOURCE_ID_FONT_LIZ_12, RESOURCE_ID_FONT_LIZ_12)));
  FontBTQTIcons = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DRIPICONS_16));
  FontTemp = fonts_load_custom_font(resource_get_handle(PBL_IF_ROUND_ELSE(RESOURCE_ID_FONT_LIZ_27, RESOURCE_ID_FONT_LIZ_27)));
  FontTempFore = fonts_load_custom_font(resource_get_handle(PBL_IF_ROUND_ELSE(RESOURCE_ID_FONT_LIZ_18, RESOURCE_ID_FONT_LIZ_18)));
  FontUV = fonts_load_custom_font(resource_get_handle(PBL_IF_ROUND_ELSE(RESOURCE_ID_FONT_LIZ_14, RESOURCE_ID_FONT_LIZ_14)));
  FontWeatherIcons = fonts_load_custom_font(resource_get_handle(PBL_IF_ROUND_ELSE(RESOURCE_ID_FONT_WEATHERICONS_16,RESOURCE_ID_FONT_WEATHERICONS_16)));
  FontWeatherCondition = fonts_load_custom_font(resource_get_handle(PBL_IF_ROUND_ELSE(RESOURCE_ID_FONT_WEATHERICONS_30,RESOURCE_ID_FONT_WEATHERICONS_30)));
  #endif

  //weather icons
// #ifdef PBL_MICROPHONE
  
// #else
//   FontWeatherCondition = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHERICONS_30));
// #endif
  main_window_push();
  // Register with Event Services
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  if (!settings.HealthOff && step_data_is_available())  {
      health_service_events_subscribe(health_handler,NULL);
      APP_LOG(APP_LOG_LEVEL_DEBUG, "health is on, and steps data is subscribed");
    }


  connection_service_subscribe((ConnectionHandlers){
    .pebble_app_connection_handler = bluetooth_vibe_icon
  });
  bluetooth_vibe_icon(connection_service_peek_pebble_app_connection());
  accel_tap_service_subscribe(accel_tap_handler);
}
static void deinit(){
  tick_timer_service_unsubscribe();
  app_message_deregister_callbacks();
  battery_state_service_unsubscribe();
  connection_service_unsubscribe();
  health_service_events_unsubscribe();
  accel_tap_service_unsubscribe();
}
int main(){
  init();
  app_event_loop();
  deinit();
}
