#include <pebble.h>
#include "main.h"
#include "weekday.h"
//#include "effect_layer.h"
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/fpath.h>
#include <pebble-fctx/ffont.h>

//Static and initial vars
static GFont
  FontDayOfTheWeekShortName, FontBTQTIcons, FontTemp, FontTempFore, FontWeatherIcons, FontWeatherCondition;// FontDate, FontSunset, FontMoonPhase,FontWindDirection;

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

static int s_hours, s_minutes, s_weekday, s_day;// last one changed, s_loop;

static char* weather_conditions[] = {
    "\U0000F07B", // 'unknown': 0,
    "\U0000F00D", // 'clear': 1,
    "\U0000F00D", // 'sunny': 2,
    "\U0000F002", // 'partlycloudy': 3,
    "\U0000F041", // 'mostlycloudy': 4,
    "\U0000F00C", // 'mostlysunny': 5,
    "\U0000F002", // 'partlysunny': 6,
    "\U0000F013", // 'cloudy': 7,
    "\U0000F019", // 'rain': 8,
    "\U0000F01B", // 'snow': 9,
    "\U0000F01D", // 'tstorms': 10,
    "\U0000F0b5", // 'sleat': 11,
    "\U0000F00A", // 'flurries': 12,
    "\U0000F0b6", // 'hazy': 13,
    "\U0000F01D", // 'chancetstorms': 14,
    "\U0000F01B", // 'chancesnow': 15,
    "\U0000F0b5", // 'chancesleat': 16,
    "\U0000F008", // 'chancerain': 17,
    "\U0000F01B", // 'chanceflurries': 18,
    "\U0000F07B", // 'nt_unknown': 19,
    "\U0000F02E", // 'nt_clear': 20,
    "\U0000F02E", // 'nt_sunny': 21,
    "\U0000F083", // 'nt_partlycloudy': 22,
    "\U0000F086", // 'nt_mostlycloudy': 23,
    "\U0000F081", // 'nt_mostlysunny': 24,
    "\U0000F086", // 'nt_partlysunny': 25,
    "\U0000F013", // 'nt_cloudy': 26,
    "\U0000F019", // 'nt_rain': 27,
    "\U0000F01B", // 'nt_snow': 28,
    "\U0000F01D", // 'nt_tstorms': 29,
    "\U0000F0b5", // 'nt_sleat': 30,
    "\U0000F038", // 'nt_flurries': 31,
    "\U0000F04A", // 'nt_hazy': 32,
    "\U0000F01D", // 'nt_chancetstorms': 33,
    "\U0000F038", // 'nt_chancesnow': 34,
    "\U0000F0B3", // 'nt_chancesleat': 35,
    "\U0000F036", // 'nt_chancerain': 36,
    "\U0000F038", // 'nt_chanceflurries': 37,
    "\U0000F003", // 'fog': 38,
    "\U0000F04A", // 'nt_fog': 39,
    "\U0000F050", // 'strong wind': 40,
    "\U0000F015", // 'hail': 41,
    "\U0000F056", // 'tornado': 42,
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
int s_countdown = 0;
int s_loop = 0;


static GColor ColorSelect(GColor ColorDay, GColor ColorNight){
  if (settings.NightTheme && IsNightNow && GPSOn){
    return ColorNight;
  }
  else{
    return ColorDay;
  }
}

/*void request_watchjs(){
  //Starting loop at 0
  s_loop = 0;
  // Begin dictionary
  DictionaryIterator * iter;
  app_message_outbox_begin( & iter);
  // Add a key-value pair
  dict_write_uint8(iter, 0, 0);
  // Send the message!
  app_message_outbox_send();
}*/


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


//static void onreconnection(bool before, bool now){
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "void before BTOn is %d", before);
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "void Now BT connection is %d", now);
//}

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
      GRect(0, 0, bounds.size.w, 88),
      GRect(0, 0, bounds.size.w, 69));

      graphics_context_set_fill_color(ctx, ColorSelect(settings.FrameColor, settings.FrameColorN));
      graphics_fill_rect(ctx, ComplicationsBand,0,GCornersAll);
      graphics_context_set_fill_color(ctx, ColorSelect(settings.FrameColor2, settings.FrameColor2N));
      graphics_fill_rect(ctx, TimeBand,0,GCornersAll);

}

void update_time_area_layer(Layer *l, GContext* ctx) {
  // check layer bounds
  #ifdef PBL_ROUND
     GRect bounds = layer_get_unobstructed_bounds(l);
     bounds = GRect(0, 0,bounds.size.w, bounds.size.h);
  #else
     GRect bounds = GRect (0,0,144,80);
     bounds = GRect(0,0,bounds.size.w,bounds.size.h);
  #endif

  FContext fctx;

  fctx_init_context(&fctx, ctx);
  fctx_set_color_bias(&fctx, 0);

  #ifdef PBL_MICROPHONE
  int font_size_hour = 40;
  int font_size_min = 85;
  int font_size_date = 22;
  #else
  int font_size_hour = 40;
  int font_size_min = 85;
  int font_size_date = 22;
  #endif

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

  hour_pos.x = INT_TO_FIXED(PBL_IF_ROUND_ELSE(96-23-2+4, 57) );
  hour_pos.y = INT_TO_FIXED(PBL_IF_ROUND_ELSE(127-34+2+2-35, 41));

  fctx_set_offset(&fctx, hour_pos);

  fctx_draw_string(&fctx, hourdraw, time_font, GTextAlignmentRight, FTextAnchorBottom);
  fctx_end_fill(&fctx);

//draw minutes
  fctx_set_fill_color(&fctx, ColorSelect(settings.MinColor, settings.MinColorN));

  FPoint min_pos;
  fctx_begin_fill(&fctx);

  fctx_set_text_em_height(&fctx, time_font, font_size_min);

  min_pos.x = INT_TO_FIXED(PBL_IF_ROUND_ELSE(95-23-2+4, 56));
  min_pos.y = INT_TO_FIXED(PBL_IF_ROUND_ELSE(127+39-34+2+2-35 , 80));

  fctx_set_offset(&fctx, min_pos);

  fctx_draw_string(&fctx, mindraw, time_font, GTextAlignmentLeft, FTextAnchorBottom);
  fctx_end_fill(&fctx);

//draw date
  fctx_set_fill_color(&fctx, ColorSelect(settings.DateColor, settings.DateColorN));

  FPoint date_pos;
  fctx_begin_fill(&fctx);

  fctx_set_text_em_height(&fctx, time_font, font_size_date);

  date_pos.x = INT_TO_FIXED(PBL_IF_ROUND_ELSE(150-74-23+9-2-2+4+1, 43-9+10));
  date_pos.y = INT_TO_FIXED(PBL_IF_ROUND_ELSE(133-13+2+2-35, 70-3));

  fctx_set_offset(&fctx, date_pos);

  fctx_draw_string(&fctx, daydraw, time_font, GTextAlignmentRight, FTextAnchorBottom);
  fctx_end_fill(&fctx);

  fctx_deinit_context(&fctx);
}


#ifdef PBL_MICROPHONE
static void layer_update_proc_rain (Layer * layer, GContext * ctx){

if (
    (showForecastWeather == 5) ||
    (showForecastWeather == 2 && PWSDataExists && settings.UsePWS) ||
    (showForecastWeather == 1 && (!settings.UsePWS || !PWSDataExists)) ||
    (showForecastWeather == 3 && (!settings.UsePWS || !PWSDataExists))
    )
    {
      GRect bounds =
         (PBL_IF_ROUND_ELSE(
           GRect(90-16-29+4,4+101+1,32,32),
           GRect(18,78,36,36)));
      GPoint center = grect_center_point(&bounds);
      const int16_t rain_bar_max_length = bounds.size.w/2;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "rain_bar is %d",rain_bar_max_length);
          //30 min rain amount
      int multiplier = settings.Rainmultiplier;
      int multiplier2 = 3;
      int s_rain0 = settings.rain0;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "rain0 is %d",s_rain0);
      int s_0_len = (s_rain0 * multiplier/100);
      APP_LOG(APP_LOG_LEVEL_DEBUG, "s_0_len is %d",s_0_len);
      int32_t rain_0_angle = TRIG_MAX_ANGLE * 60/60;

      int s_rain5 = settings.rain5;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "rain5 is %d",s_rain5);
      int s_5_len = (s_rain5 * multiplier/100);
      int32_t rain_5_angle = TRIG_MAX_ANGLE * 5/60;

      int s_rain10 = settings.rain10;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "rain10 is %d",s_rain10);
      int s_10_len = (s_rain10 * multiplier/100);
      int32_t rain_10_angle = TRIG_MAX_ANGLE * 10/60;

      int s_rain15 = settings.rain15;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "rain15 is %d",s_rain15);
      int s_15_len = (s_rain15 * multiplier/100);
      int32_t rain_15_angle = TRIG_MAX_ANGLE * 15/60;

      int s_rain20 = settings.rain20;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "rain20 is %d",s_rain20);
      int s_20_len = (s_rain20 * multiplier/100);
      int32_t rain_20_angle = TRIG_MAX_ANGLE * 20/60;

      int s_rain25 = settings.rain25;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "rain25 is %d",s_rain25);
      int s_25_len = (s_rain25 * multiplier/100);
      int32_t rain_25_angle = TRIG_MAX_ANGLE * 25/60;

      int s_rain30 = settings.rain30;
            APP_LOG(APP_LOG_LEVEL_DEBUG, "rain30 is %d",s_rain30);
      int s_30_len = (s_rain30 * multiplier/100);
      int32_t rain_30_angle = TRIG_MAX_ANGLE * 30/60;

      int s_rain35 = settings.rain35;
      int s_35_len = (s_rain35 * multiplier/100);
      int32_t rain_35_angle = TRIG_MAX_ANGLE * 35/60;

      int s_rain40 = settings.rain40;
      int s_40_len = (s_rain40 * multiplier/100);
      int32_t rain_40_angle = TRIG_MAX_ANGLE * 40/60;

      int s_rain45 = settings.rain45;
      int s_45_len = (s_rain45 * multiplier/100);
      int32_t rain_45_angle = TRIG_MAX_ANGLE * 45/60;

      int s_rain50 = settings.rain50;
      int s_50_len = (s_rain50 * multiplier/100);
      int32_t rain_50_angle = TRIG_MAX_ANGLE * 50/60;

      int s_rain55 = settings.rain55;
      int s_55_len = (s_rain55 * multiplier/100);
      int32_t rain_55_angle = TRIG_MAX_ANGLE * 55/60;

      int s_rain60 = settings.rain60;
      int s_60_len = (s_rain60 * multiplier/100);
      int32_t rain_60_angle = TRIG_MAX_ANGLE * 60/60;

    GPoint rain_0_1 = {
      .x = (int16_t)(sin_lookup(rain_0_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.x,
      .y = (int16_t)(-cos_lookup(rain_0_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.y,
    };

    GPoint rain_0 = {
      .x = (int16_t)(sin_lookup(rain_0_angle)*(int32_t)s_0_len/TRIG_MAX_RATIO) + rain_0_1.x,
      .y = (int16_t)(-cos_lookup(rain_0_angle)*(int32_t)s_0_len/TRIG_MAX_RATIO) + rain_0_1.y,
    };

    GPoint rain_5_1 = {
      .x = (int16_t)(sin_lookup(rain_5_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.x,
      .y = (int16_t)(-cos_lookup(rain_5_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.y,
    };

    GPoint rain_5 = {
      .x = (int16_t)(sin_lookup(rain_5_angle)*(int32_t)s_5_len/TRIG_MAX_RATIO) + rain_5_1.x,
      .y = (int16_t)(-cos_lookup(rain_5_angle)*(int32_t)s_5_len/TRIG_MAX_RATIO) + rain_5_1.y,
    };

    GPoint rain_10_1 = {
      .x = (int16_t)(sin_lookup(rain_10_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.x,
      .y = (int16_t)(-cos_lookup(rain_10_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.y,
    };

    GPoint rain_10 = {
      .x = (int16_t)(sin_lookup(rain_10_angle)*(int32_t)s_10_len/TRIG_MAX_RATIO) + rain_10_1.x,
      .y = (int16_t)(-cos_lookup(rain_10_angle)*(int32_t)s_10_len/TRIG_MAX_RATIO) + rain_10_1.y,
    };

    GPoint rain_15_1 = {
      .x = (int16_t)(sin_lookup(rain_15_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.x,
      .y = (int16_t)(-cos_lookup(rain_15_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.y,
    };

    GPoint rain_15 = {
      .x = (int16_t)(sin_lookup(rain_15_angle)*(int32_t)s_15_len/TRIG_MAX_RATIO) + rain_15_1.x,
      .y = (int16_t)(-cos_lookup(rain_15_angle)*(int32_t)s_15_len/TRIG_MAX_RATIO) + rain_15_1.y,
    };

    GPoint rain_20_1 = {
      .x = (int16_t)(sin_lookup(rain_20_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.x,
      .y = (int16_t)(-cos_lookup(rain_20_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.y,
    };

    GPoint rain_20 = {
      .x = (int16_t)(sin_lookup(rain_20_angle)*(int32_t)s_20_len/TRIG_MAX_RATIO) + rain_20_1.x,
      .y = (int16_t)(-cos_lookup(rain_20_angle)*(int32_t)s_20_len/TRIG_MAX_RATIO) + rain_20_1.y,
    };

    GPoint rain_25_1 = {
      .x = (int16_t)(sin_lookup(rain_25_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.x,
      .y = (int16_t)(-cos_lookup(rain_25_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.y,
    };

    GPoint rain_25 = {
      .x = (int16_t)(sin_lookup(rain_25_angle)*(int32_t)s_25_len/TRIG_MAX_RATIO) + rain_25_1.x,
      .y = (int16_t)(-cos_lookup(rain_25_angle)*(int32_t)s_25_len/TRIG_MAX_RATIO) + rain_25_1.y,
    };

    GPoint rain_30_1 = {
      .x = (int16_t)(sin_lookup(rain_30_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.x,
      .y = (int16_t)(-cos_lookup(rain_30_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.y,
    };

    GPoint rain_30 = {
      .x = (int16_t)(sin_lookup(rain_30_angle)*(int32_t)s_30_len/TRIG_MAX_RATIO) + rain_30_1.x,
      .y = (int16_t)(-cos_lookup(rain_30_angle)*(int32_t)s_30_len/TRIG_MAX_RATIO) + rain_30_1.y,
    };

    GPoint rain_35_1 = {
      .x = (int16_t)(sin_lookup(rain_35_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.x,
      .y = (int16_t)(-cos_lookup(rain_35_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.y,
    };

    GPoint rain_35 = {
      .x = (int16_t)(sin_lookup(rain_35_angle)*(int32_t)s_35_len/TRIG_MAX_RATIO) + rain_35_1.x,
      .y = (int16_t)(-cos_lookup(rain_35_angle)*(int32_t)s_35_len/TRIG_MAX_RATIO) + rain_35_1.y,
    };

    GPoint rain_40_1 = {
      .x = (int16_t)(sin_lookup(rain_40_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.x,
      .y = (int16_t)(-cos_lookup(rain_40_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.y,
    };

    GPoint rain_40 = {
      .x = (int16_t)(sin_lookup(rain_40_angle)*(int32_t)s_40_len/TRIG_MAX_RATIO) + rain_40_1.x,
      .y = (int16_t)(-cos_lookup(rain_40_angle)*(int32_t)s_40_len/TRIG_MAX_RATIO) + rain_40_1.y,
    };

    GPoint rain_45_1 = {
      .x = (int16_t)(sin_lookup(rain_45_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.x,
      .y = (int16_t)(-cos_lookup(rain_45_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.y,
    };

    GPoint rain_45 = {
      .x = (int16_t)(sin_lookup(rain_45_angle)*(int32_t)s_45_len/TRIG_MAX_RATIO) + rain_45_1.x,
      .y = (int16_t)(-cos_lookup(rain_45_angle)*(int32_t)s_45_len/TRIG_MAX_RATIO) + rain_45_1.y,
    };

    GPoint rain_50_1 = {
      .x = (int16_t)(sin_lookup(rain_50_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.x,
      .y = (int16_t)(-cos_lookup(rain_50_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.y,
    };

    GPoint rain_50 = {
      .x = (int16_t)(sin_lookup(rain_50_angle)*(int32_t)s_50_len/TRIG_MAX_RATIO) + rain_50_1.x,
      .y = (int16_t)(-cos_lookup(rain_50_angle)*(int32_t)s_50_len/TRIG_MAX_RATIO) + rain_50_1.y,
    };

    GPoint rain_55_1 = {
      .x = (int16_t)(sin_lookup(rain_55_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.x,
      .y = (int16_t)(-cos_lookup(rain_55_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.y,
    };

    GPoint rain_55 = {
      .x = (int16_t)(sin_lookup(rain_55_angle)*(int32_t)s_55_len/TRIG_MAX_RATIO) + rain_55_1.x,
      .y = (int16_t)(-cos_lookup(rain_55_angle)*(int32_t)s_55_len/TRIG_MAX_RATIO) + rain_55_1.y,
    };

    GPoint rain_60_1 = {
      .x = (int16_t)(sin_lookup(rain_60_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.x,
      .y = (int16_t)(-cos_lookup(rain_60_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.y,
    };

    GPoint rain_60 = {
      .x = (int16_t)(sin_lookup(rain_60_angle)*(int32_t)s_60_len/TRIG_MAX_RATIO) + rain_60_1.x,
      .y = (int16_t)(-cos_lookup(rain_60_angle)*(int32_t)s_60_len/TRIG_MAX_RATIO) + rain_60_1.y,
    };

    graphics_context_set_stroke_color(ctx,ColorSelect(settings.Text7Color,settings.Text7ColorN));
    graphics_context_set_fill_color(ctx,ColorSelect(settings.Text7Color,settings.Text7ColorN));

    const GPathInfo Rain_path_info = {
      .num_points = 13,
      .points = (GPoint []) {
        {rain_0.x,rain_0.y},
        {rain_5.x,rain_5.y},
        {rain_10.x,rain_10.y},
        {rain_15.x,rain_15.y},
        {rain_20.x,rain_20.y},
        {rain_25.x,rain_25.y},
        {rain_30.x,rain_30.y},
        {rain_35.x,rain_35.y},
        {rain_40.x,rain_40.y},
        {rain_45.x,rain_45.y},
        {rain_50.x,rain_50.y},
        {rain_55.x,rain_55.y},
        {rain_60.x,rain_60.y}}
    };

    s_rain_path = gpath_create(&Rain_path_info);

    gpath_draw_filled(ctx,s_rain_path);
    gpath_draw_outline(ctx,s_rain_path);


    graphics_context_set_fill_color(ctx,ColorSelect(settings.FrameColor,settings.FrameColorN));
    graphics_fill_circle(ctx,center,rain_bar_max_length/multiplier2);

    graphics_context_set_stroke_color(ctx,ColorSelect(settings.Text7Color,settings.Text7ColorN));
    graphics_context_set_stroke_width(ctx,1);
    graphics_draw_circle(ctx,center,rain_bar_max_length/multiplier2);

    gpath_destroy(s_rain_path);
}
}

#else
static void layer_update_proc_rain (Layer * layer, GContext * ctx){

if ((showForecastWeather == 1 ||showForecastWeather == 3 || showForecastWeather == 5))
    {
      GRect bounds = GRect(18,78,36,36);
      GPoint center = grect_center_point(&bounds);
      const int16_t rain_bar_max_length = bounds.size.w/2;
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "rain_bar is %d",rain_bar_max_length);
          //30 min rain amount
      int multiplier = settings.Rainmultiplier;
      int multiplier2 = 3;

      int s_rain0 = settings.rain0;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "rain0 is %d",s_rain0);
      int s_0_len = (s_rain0 * multiplier/100);
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "s_0_len is %d",s_0_len);
      int32_t rain_0_angle = TRIG_MAX_ANGLE * 60/60;

      int s_rain10 = settings.rain10;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "rain10 is %d",s_rain10);
      int s_10_len = (s_rain10 * multiplier/100);
      int32_t rain_10_angle = TRIG_MAX_ANGLE * 10/60;

      int s_rain20 = settings.rain20;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "rain20 is %d",s_rain20);
      int s_20_len = (s_rain20 * multiplier/100);
      int32_t rain_20_angle = TRIG_MAX_ANGLE * 20/60;

      int s_rain30 = settings.rain30;
      int s_30_len = (s_rain30 * multiplier/100);
      int32_t rain_30_angle = TRIG_MAX_ANGLE * 30/60;

      int s_rain40 = settings.rain40;
      int s_40_len = (s_rain40 * multiplier/100);
      int32_t rain_40_angle = TRIG_MAX_ANGLE * 40/60;

      int s_rain50 = settings.rain50;
      int s_50_len = (s_rain50 * multiplier/100);
      int32_t rain_50_angle = TRIG_MAX_ANGLE * 50/60;

      int s_rain60 = settings.rain60;
      int s_60_len = (s_rain60 * multiplier/100);
      int32_t rain_60_angle = TRIG_MAX_ANGLE * 60/60;



    GPoint rain_0_1 = {
      .x = (int16_t)(sin_lookup(rain_0_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.x,
      .y = (int16_t)(-cos_lookup(rain_0_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.y,
    };

    GPoint rain_0 = {
      .x = (int16_t)(sin_lookup(rain_0_angle)*(int32_t)s_0_len/TRIG_MAX_RATIO) + rain_0_1.x,
      .y = (int16_t)(-cos_lookup(rain_0_angle)*(int32_t)s_0_len/TRIG_MAX_RATIO) + rain_0_1.y,
    };

    GPoint rain_10_1 = {
      .x = (int16_t)(sin_lookup(rain_10_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.x,
      .y = (int16_t)(-cos_lookup(rain_10_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.y,
    };

    GPoint rain_10 = {
      .x = (int16_t)(sin_lookup(rain_10_angle)*(int32_t)s_10_len/TRIG_MAX_RATIO) + rain_10_1.x,
      .y = (int16_t)(-cos_lookup(rain_10_angle)*(int32_t)s_10_len/TRIG_MAX_RATIO) + rain_10_1.y,
    };

    GPoint rain_20_1 = {
      .x = (int16_t)(sin_lookup(rain_20_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.x,
      .y = (int16_t)(-cos_lookup(rain_20_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.y,
    };

    GPoint rain_20 = {
      .x = (int16_t)(sin_lookup(rain_20_angle)*(int32_t)s_20_len/TRIG_MAX_RATIO) + rain_20_1.x,
      .y = (int16_t)(-cos_lookup(rain_20_angle)*(int32_t)s_20_len/TRIG_MAX_RATIO) + rain_20_1.y,
    };

    GPoint rain_30_1 = {
      .x = (int16_t)(sin_lookup(rain_30_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.x,
      .y = (int16_t)(-cos_lookup(rain_30_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.y,
    };

    GPoint rain_30 = {
      .x = (int16_t)(sin_lookup(rain_30_angle)*(int32_t)s_30_len/TRIG_MAX_RATIO) + rain_30_1.x,
      .y = (int16_t)(-cos_lookup(rain_30_angle)*(int32_t)s_30_len/TRIG_MAX_RATIO) + rain_30_1.y,
    };

    GPoint rain_40_1 = {
      .x = (int16_t)(sin_lookup(rain_40_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.x,
      .y = (int16_t)(-cos_lookup(rain_40_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.y,
    };

    GPoint rain_40 = {
      .x = (int16_t)(sin_lookup(rain_40_angle)*(int32_t)s_40_len/TRIG_MAX_RATIO) + rain_40_1.x,
      .y = (int16_t)(-cos_lookup(rain_40_angle)*(int32_t)s_40_len/TRIG_MAX_RATIO) + rain_40_1.y,
    };

    GPoint rain_50_1 = {
      .x = (int16_t)(sin_lookup(rain_50_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.x,
      .y = (int16_t)(-cos_lookup(rain_50_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.y,
    };

    GPoint rain_50 = {
      .x = (int16_t)(sin_lookup(rain_50_angle)*(int32_t)s_50_len/TRIG_MAX_RATIO) + rain_50_1.x,
      .y = (int16_t)(-cos_lookup(rain_50_angle)*(int32_t)s_50_len/TRIG_MAX_RATIO) + rain_50_1.y,
    };

    GPoint rain_60_1 = {
      .x = (int16_t)(sin_lookup(rain_60_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.x,
      .y = (int16_t)(-cos_lookup(rain_60_angle)*(int32_t)rain_bar_max_length/multiplier2/TRIG_MAX_RATIO)+ center.y,
    };

    GPoint rain_60 = {
      .x = (int16_t)(sin_lookup(rain_60_angle)*(int32_t)s_60_len/TRIG_MAX_RATIO) + rain_60_1.x,
      .y = (int16_t)(-cos_lookup(rain_60_angle)*(int32_t)s_60_len/TRIG_MAX_RATIO) + rain_60_1.y,
    };

    graphics_context_set_stroke_color(ctx,ColorSelect(settings.Text7Color,settings.Text7ColorN));
    graphics_context_set_fill_color(ctx,ColorSelect(settings.Text7Color,settings.Text7ColorN));

    const GPathInfo Rain_path_info = {
      .num_points = 7,
      .points = (GPoint []) {
        {rain_0.x,rain_0.y},
        {rain_10.x,rain_10.y},
        {rain_20.x,rain_20.y},
        {rain_30.x,rain_30.y},
        {rain_40.x,rain_40.y},
        {rain_50.x,rain_50.y},
        {rain_60.x,rain_60.y}}
    };

    s_rain_path = gpath_create(&Rain_path_info);

    gpath_draw_filled(ctx,s_rain_path);
    gpath_draw_outline(ctx,s_rain_path);

    graphics_context_set_fill_color(ctx,ColorSelect(settings.FrameColor,settings.FrameColorN));
    graphics_fill_circle(ctx,center,rain_bar_max_length/multiplier2);

    graphics_context_set_stroke_color(ctx,ColorSelect(settings.Text7Color,settings.Text7ColorN));
    graphics_context_set_stroke_width(ctx,1);
    graphics_draw_circle(ctx,center,rain_bar_max_length/multiplier2);

    gpath_destroy(s_rain_path);

    }
  }

#endif


static void layer_update_proc(Layer * layer, GContext * ctx){
//add in weather info
GRect DateRect =
  (PBL_IF_ROUND_ELSE(
     GRect(25-2-10-2-2+4+1, 89-35, 48, 20),
     GRect(0-2, 36-4+1, 45, 35)));

GRect SunsetRect =
  (PBL_IF_ROUND_ELSE(
     GRect(99-4+27,133-8-37,61,14),
     GRect(96,144+3,144/3,27)));

GRect SunriseRect =
   (PBL_IF_ROUND_ELSE(
     GRect(20+6-31+2,133-8-37,61,14),
     GRect(144/6,144+3,144/3,27)));

GRect MoonRect =
    (PBL_IF_ROUND_ELSE(
      GRect(90-9-27+29-4,132-5+12-37+1-2,18+54,56),
      GRect(72, 68+9, 72,56)));

 GRect StepsRect =
   (PBL_IF_ROUND_ELSE(
     (GRect(90-8,33+105,90-16,30)),
     (GRect(72,118-3+4,144/2,27))));

 GRect SunsetIconRect =
    (PBL_IF_ROUND_ELSE(
      GRect(113+30,150-3-36,24,24),
      GRect(72,146+2,144/6,38)));

  GRect SunriseIconRect =
     (PBL_IF_ROUND_ELSE(
       GRect(42-30,150-3-36,24,24),
       GRect(0,146+2,144/6,38)));


GRect IconNowRect = //weather condition icon
     (PBL_IF_ROUND_ELSE(
     GRect(0-26-2, 8+91-2+2,180,32),
     GRect(0, 68+9, 72,56)));

GRect IconForeRect = //weather condition icon
      (PBL_IF_ROUND_ELSE(
      GRect(0+26+3,130-31-2+2,180,32),
      GRect(72, 68+9, 72,56)));


/*GRect Icon1hRect = //weather condition icon next hour INSTEAD OF RAIN
      (PBL_IF_ROUND_ELSE(
      GRect(0-26-2, 8+91-2+2,180,32),
      GRect(0, 68+9, 72,56)));*/

GRect WindKtsRect =  //windspeed number
     (PBL_IF_ROUND_ELSE(
     (GRect(94-115+2,26+6+2+56,90-4,30)),
     (GRect(24-6,144+2,48+6,27))));

GRect WindForeKtsRect =  //windspeed number
    (PBL_IF_ROUND_ELSE(
      (GRect(94+20-1+1,180-30-24-2-34,90-4,30)),
      (GRect(96-6,144+2,48+6,27))));

GRect TempRect =  //temperature number
    (PBL_IF_ROUND_ELSE(
       (GRect(0+20-1,24+2+109,90,30)),
       (GRect(-4,118-3,(144/2)+8,27))));

GRect PrecipRect =  //rain chance next hour
    (PBL_IF_ROUND_ELSE(
      (GRect(16+12,33+105,90-16,30)),
      (GRect(0,118-3+4,(144/2),27))));

 GRect TempForeRect =  //temperature number
    (PBL_IF_ROUND_ELSE(
      (GRect(0+72+5,180-30-24-2+14,90,30)),
      (GRect(72,118+1,144/2,27))));

GRect WindDirNowRect =  //wind direction icon
    (PBL_IF_ROUND_ELSE(
      (GRect(78-99,12-4+95,90,32)),
      (GRect(0,136,144/6,38))));

GRect WindDirForeRect =  //wind direction icon
   (PBL_IF_ROUND_ELSE(
     (GRect(78+33+1,180-32-12-33,90,32)),
     (GRect(144/2,136,144/6,38))));

GRect BatteryRect =
    (PBL_IF_ROUND_ELSE(
      GRect(0,50+75-2-35,180,2),
      GRect(0,68,144,2)));

    //Battery
  int s_battery_level = battery_state_service_peek().charge_percent;

#ifdef PBL_ROUND
  int width_round = (s_battery_level * 180) / 100;
#else
  int width_rect = (s_battery_level * 144) / 100;
#endif



GRect BatteryFillRect =
    (PBL_IF_ROUND_ELSE(
      GRect(0,50+75-2-35,width_round,2),
      GRect(0,68,width_rect,2)));

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

       GRect RainRateRect =  //temperature number
           (PBL_IF_ROUND_ELSE(
             (GRect(99-4+27,133-8-37,61,14)),
             (GRect(72,144+2,144/2,27))));

       GRect PressureRect =  //rain chance next hour
           (PBL_IF_ROUND_ELSE(
             (GRect(0+72+5,180-30-24-2+14,90,30)),
             (GRect(72,118+1,144/2,27))));

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

        char StepsToDraw[10];
        snprintf(StepsToDraw, sizeof(StepsToDraw), "%s",s_current_steps_buffer);

        char precipToDraw[10];
        //snprintf(precipToDraw, sizeof(precipToDraw), "%s",settings.popstring);
        snprintf(precipToDraw, sizeof(precipToDraw), "%s",settings.rainstring);



        graphics_context_set_text_color(ctx,ColorSelect(settings.Text4Color,settings.Text4ColorN));
        graphics_draw_text(ctx, MoonToDraw, FontWeatherCondition, MoonRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);

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

            char StepsToDraw[10];
            snprintf(StepsToDraw, sizeof(StepsToDraw), "%s",s_current_steps_buffer);

            char precipToDraw[10];
            //snprintf(precipToDraw, sizeof(precipToDraw), "%s",settings.popstring);
            snprintf(precipToDraw, sizeof(precipToDraw), "%s",settings.rainstring);



            graphics_context_set_text_color(ctx,ColorSelect(settings.Text4Color,settings.Text4ColorN));
            graphics_draw_text(ctx, MoonToDraw, FontWeatherCondition, MoonRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);

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
   GRect BTIconRect =
    (PBL_IF_ROUND_ELSE(
      GRect(90,4,24,20),
      GRect(49,80,47,20)));

 graphics_context_set_text_color(ctx3, ColorSelect(settings.Text5Color, settings.Text5ColorN));
 graphics_draw_text(ctx3, "z", FontBTQTIcons, BTIconRect, GTextOverflowModeFill,GTextAlignmentCenter, NULL);

}

static void layer_update_proc_qt(Layer * layer4, GContext * ctx4){
  GRect QTIconRect =
    (PBL_IF_ROUND_ELSE(
      GRect(90-24,4,24,20),
      GRect(49,97,47,20)));

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

// Weather conditions
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
  strcpy(citistring, "NotSet");
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

  FontDayOfTheWeekShortName = fonts_load_custom_font(resource_get_handle(PBL_IF_ROUND_ELSE(RESOURCE_ID_FONT_LIZ_12, RESOURCE_ID_FONT_LIZ_12)));
  FontBTQTIcons = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DRIPICONS_16));
  FontTemp = fonts_load_custom_font(resource_get_handle(PBL_IF_ROUND_ELSE(RESOURCE_ID_FONT_LIZ_27, RESOURCE_ID_FONT_LIZ_27)));
  FontTempFore = fonts_load_custom_font(resource_get_handle(PBL_IF_ROUND_ELSE(RESOURCE_ID_FONT_LIZ_18, RESOURCE_ID_FONT_LIZ_18)));
//sunset sunrise icon font
  FontWeatherIcons = fonts_load_custom_font(resource_get_handle(PBL_IF_ROUND_ELSE(RESOURCE_ID_FONT_WEATHERICONS_16,RESOURCE_ID_FONT_WEATHERICONS_16)));
//weather icons
#ifdef PBL_MICROPHONE
  FontWeatherCondition = fonts_load_custom_font(resource_get_handle(PBL_IF_ROUND_ELSE(RESOURCE_ID_FONT_WEATHERICONS_32,RESOURCE_ID_FONT_WEATHERICONS_32)));
#else
  FontWeatherCondition = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHERICONS_32));
#endif
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
