#include <pebble.h>
#include "main.h"
#include "weekday.h"
#include "effect_layer.h"
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/fpath.h>
#include <pebble-fctx/ffont.h>

static uint32_t s_last_tap_time = 0;
#define TAP_COOLDOWN_MS 1000 // 1.5 second cooldown

// Static and initial vars
static GFont
    FontDateNumber,
    FontDay, FontRain,
    FontRainProb, FontIcon, FontIcon2, FontWeatherIcons, FontFore;

FFont *time_font;

static Window *s_window;

static Layer *s_canvas_background;
static Layer *s_canvas_date;
static Layer *s_canvas_bt_icon;
static Layer *s_canvas_qt_icon;
static Layer *s_canvas_rain;
static Layer *s_canvas_weather;

Layer *time_area_layer;

static int s_hours, s_minutes, s_weekday, s_day;

static char *weather_conditions[] = {
    "\U0000F07B",  // 'unknown': 0,
    "\U0000f00e",  // thunderstorm with light rain: 1
    "\U0000f02c",  // thunderstorm with light rain: 2
    "\U0000f010",  // thunderstorm with rain: 3
    "\U0000f02d",  // thunderstorm with rain: 4
    "\U0000f01e",  // thunderstorm with heavy rain: 5
    "\U0000f01e",  // thunderstorm with heavy rain: 6
    "\U0000f005",  // light thunderstorm: 7
    "\U0000f025",  // light thunderstorm: 8
    "\U0000f01e",  // thunderstorm: 9
    "\U0000f01e",  // thunderstorm: 10
    "\U0000f01e",  // heavy thunderstorm: 11
    "\U0000f01e",  // heavy thunderstorm: 12
    "\U0000f01e",  // ragged thunderstorm: 13
    "\U0000f01e",  // ragged thunderstorm: 14
    "\U0000f00e",  // thunderstorm with light drizzle: 15
    "\U0000f02c",  // thunderstorm with light drizzle: 16
    "\U0000f00e",  // thunderstorm with drizzle: 17
    "\U0000f02c",  // thunderstorm with drizzle: 18
    "\U0000f01d",  // thunderstorm with heavy drizzle: 19
    "\U0000f01d",  // thunderstorm with heavy drizzle: 20
    "\U0000f00b",  // light intensity drizzle: 21
    "\U0000f02b",  // light intensity drizzle: 22
    "\U0000f01c",  // drizzle: 23
    "\U0000f01c",  // drizzle: 24
    "\U0000f01a",  // heavy intensity drizzle: 25
    "\U0000f01a",  // heavy intensity drizzle: 26
    "\U0000f00b",  // light intensity drizzle rain: 27
    "\U0000f02b",  // light intensity drizzle rain: 28
    "\U0000f00b",  // drizzle rain: 29
    "\U0000f029",  // drizzle rain: 30
    "\U0000f019",  // heavy intensity drizzle rain: 31
    "\U0000f019",  // heavy intensity drizzle rain: 32
    "\U0000f01a",  // shower rain and drizzle: 33
    "\U0000f01a",  // shower rain and drizzle: 34
    "\U0000f01a",  // heavy shower rain and drizzle: 35
    "\U0000f01a",  // heavy shower rain and drizzle: 36
    "\U0000f00b",  // shower drizzle: 37
    "\U0000f02b",  // shower drizzle: 38
    "\U0000f01a",  // light rain: 39
    "\U0000f01a",  // light rain: 40
    "\U0000f019",  // moderate rain: 41
    "\U0000f019",  // moderate rain: 42
    "\U0000f019",  // heavy intensity rain: 43
    "\U0000f019 ", // heavy intensity rain: 44
    "\U0000f019",  // very heavy rain: 45
    "\U0000f019",  // very heavy rain: 46
    "\U0000f018",  // extreme rain: 47
    "\U0000f018",  // extreme rain: 48
    "\U0000f017",  // freezing rain: 49
    "\U0000f017",  // freezing rain: 50
    "\U0000f01a",  // light intensity shower rain: 51
    "\U0000f01a",  // light intensity shower rain: 52
    "\U0000f01a",  // shower rain: 53
    "\U0000f01a",  // shower rain: 54
    "\U0000f01a",  // heavy intensity shower rain: 55
    "\U0000f01a",  // heavy intensity shower rain: 56
    "\U0000f018",  // ragged shower rain: 57
    "\U0000f018",  // ragged shower rain: 58
    "\U0000f00a",  // light snow: 59
    "\U0000f02a",  // light snow: 60
    "\U0000f01b",  // Snow: 61
    "\U0000f01b",  // Snow: 62
    "\U0000f076",  // Heavy snow: 63
    "\U0000f076",  // Heavy snow: 64
    "\U0000f017",  // Sleet: 65
    "\U0000f017",  // Sleet: 66
    "\U0000f0b2",  // Light shower sleet: 67
    "\U0000f0b4",  // Light shower sleet: 68
    "\U0000f0b5",  // Shower sleet: 69
    "\U0000f0b5",  // Shower sleet: 70
    "\U0000f006",  // Light rain and snow: 71
    "\U0000f026",  // Light rain and snow: 72
    "\U0000f017",  // Rain and snow: 73
    "\U0000f017",  // Rain and snow: 74
    "\U0000f00a",  // Light shower snow: 75
    "\U0000f02a",  // Light shower snow: 76
    "\U0000f00a",  // Shower snow: 77
    "\U0000f02a",  // Shower snow: 78
    "\U0000f076",  // Heavy shower snow: 79
    "\U0000f076",  // Heavy shower snow: 80
    "\U0000f003",  // mist: 81
    "\U0000f04a",  // mist: 82
    "\U0000f062",  // Smoke: 83
    "\U0000f062",  // Smoke: 84
    "\U0000f0b6",  // Haze: 85
    "\U0000f023",  // Haze: 86
    "\U0000f082",  // sand/ dust whirls: 87
    "\U0000f082",  // sand/ dust whirls: 88
    "\U0000f014",  // fog: 89
    "\U0000f014",  // fog: 90
    "\U0000f082",  // sand: 91
    "\U0000f082",  // sand: 92
    "\U0000f082",  // dust: 93
    "\U0000f082",  // dust: 94
    "\U0000f0c8",  // volcanic ash: 95
    "\U0000f0c8",  // volcanic ash: 96
    "\U0000f011",  // squalls: 97
    "\U0000f011",  // squalls: 98
    "\U0000f056",  // tornado: 99
    "\U0000f056",  // tornado: 100
    "\U0000f00d",  // clear sky: 101
    "\U0000f02e",  // clear sky: 102
    "\U0000f00c",  // few clouds: 11-25%: 103
    "\U0000f081",  // few clouds: 11-25%: 104
    "\U0000f002",  // scattered clouds: 25-50%: 105
    "\U0000f086",  // scattered clouds: 25-50%: 106
    "\U0000f041",  // broken clouds: 51-84%: 107
    "\U0000f041",  // broken clouds: 51-84%: 108
    "\U0000f013",  // overcast clouds: 85-100%: 109
    "\U0000f013",  // overcast clouds: 85-100%: 110
    "\U0000f056",  // tornado: 111
    "\U0000f01d",  // storm-showers: 112
    "\U0000f073",  // hurricane: 113
    "\U0000f076",  // snowflake-cold: 114
    "\U0000f072",  // hot: 115
    "\U0000f050",  // windy: 116
    "\U0000f015",  // hail: 117
    "\U0000f050",  // strong-wind: 118
};
//////Init Configuration///
// Init Clay
ClaySettings settings;
// Initialize the default settings
static void prv_default_settings()
{
  settings.AddZero12h = false;
  settings.RemoveZero24h = true;
  settings.MinBackColor1 = GColorBlack;
  settings.HourBackColor2 = GColorBlack;
  settings.HourColor = GColorWhite;
  settings.MinColor = GColorWhite;
  settings.TextDayColor = GColorWhite;
  settings.TextDateColor = GColorWhite;
  settings.TextRainColor = GColorWhite;
  settings.TextRainProbColor = GColorWhite;
  settings.TextWeatherColor = GColorWhite;
  settings.TextTempColor = GColorWhite;
  settings.UpdateSlider = 30;
  strncpy(settings.tempForeString, "NA", sizeof(settings.tempForeString) - 1);
  strncpy(settings.iconNowString, "\U0000F07B", sizeof(settings.iconNowString) - 1);
  strncpy(settings.iconForeString, "\U0000F07B", sizeof(settings.iconForeString) - 1);
  strncpy(settings.rainNowString, "NA", sizeof(settings.rainNowString) - 1);
  strncpy(settings.rainSumForeString, "NA", sizeof(settings.rainSumForeString) - 1);
  strncpy(settings.rainProbNowString, "NA", sizeof(settings.rainProbNowString) - 1);
  strncpy(settings.rainProbMaxForeString, "NA", sizeof(settings.rainProbMaxForeString) - 1);
}
bool BTOn = true;
int s_countdown = 0;
int showForecast = 0;

//////End Configuration///
///////////////////////////

// Callback for js request
void request_watchjs()
{
  // Begin dictionary
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  // Add a key-value pair
  dict_write_uint8(iter, MESSAGE_KEY_REQUEST_WEATHER, 1);
  // Send the message!
  app_message_outbox_send();
}

//////////changed function to avoid false/rebound taps on Pebble Time 2 alpha unit
static void accel_tap_handler(AccelAxisType axis, int32_t direction)
{

  time_t seconds;
  uint16_t milliseconds;
  time_ms(&seconds, &milliseconds);

  uint32_t now = (uint32_t)seconds * TAP_COOLDOWN_MS + milliseconds;

  // Check cooldown
  if (s_last_tap_time != 0 && (now - s_last_tap_time < TAP_COOLDOWN_MS))
  {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Tap ignored");
    return;
  }

  s_last_tap_time = now;
  showForecast = (showForecast >= 1) ? 0 : showForecast + 1;

  layer_mark_dirty(s_canvas_weather);
  layer_mark_dirty(s_canvas_rain);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Valid Tap! showForecast: %d", (int)showForecast);
}

/// BT Connection
static void bluetooth_callback(bool connected)
{
  BTOn = connected;
}

static void bluetooth_vibe_icon(bool connected)
{
  layer_set_hidden(s_canvas_bt_icon, connected);
}

static void quiet_time_icon()
{
  if (!quiet_time_is_active())
  {
    layer_set_hidden(s_canvas_qt_icon, true);
  }
}

static void onreconnection(bool before, bool now)
{
  if (!before && now)
  {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "BT reconnected, requesting weather at %d", s_minutes);
    request_watchjs();
  }
}

void layer_update_proc_background(Layer *back_layer, GContext *ctx)
{
  // Create Rects
  GRect bounds = layer_get_bounds(back_layer);
  // pebble round
  GRect RoundLeftBand =
      PBL_IF_ROUND_ELSE(
          GRect(0, 0, 38, bounds.size.h),
          GRect(0, 0, 0, 0));

  GRect HourBand =
      PBL_IF_ROUND_ELSE(
          GRect(38, 0, 100 - 38, bounds.size.h),
          GRect(0, 0, bounds.size.w / 2, bounds.size.h));

  GRect MinBand =
      PBL_IF_ROUND_ELSE(
          GRect(100, 0, 80, bounds.size.h),
          GRect(bounds.size.w / 2, 0, bounds.size.w / 2, bounds.size.h));

  graphics_context_set_fill_color(ctx, settings.RoundLeftFrameColor1);
  graphics_fill_rect(ctx, RoundLeftBand, 0, GCornersAll);
  graphics_context_set_fill_color(ctx, settings.HourBackColor2);
  graphics_fill_rect(ctx, HourBand, 0, GCornersAll);
  graphics_context_set_fill_color(ctx, settings.MinBackColor1);
  graphics_fill_rect(ctx, MinBand, 0, GCornersAll);
}

void update_time_area_layer(Layer *l, GContext *ctx)
{
  // 1. Get bounds

#if defined(PBL_ROUND)
  // Round: offset slightly to look better in the circle
  GRect bounds = layer_get_unobstructed_bounds(l);
  GRect hour_bounds = GRect(0, 0, bounds.size.w / 3 * 2, bounds.size.h);
#else
  GRect bounds = layer_get_unobstructed_bounds(l);
  GRect boundsobs = layer_get_bounds(l);
#endif

  // GRect minute_bounds = GRect(bounds.size.w/2,0,bounds.size.w/2, bounds.size.h);

  // 2. Initialize FCTX (Only once for both hour and minute)
  FContext fctx;
  fctx_init_context(&fctx, ctx);
  fctx_set_color_bias(&fctx, 0);
#if defined(PBL_COLOR)
  fctx_enable_aa(true);
#endif

  // 3. Get Time Data
  time_t temp = time(NULL);
  struct tm *time_now = localtime(&temp);
  char hourdraw[8];
  char mindraw[8];

  // Format Hours
  if (clock_is_24h_style() && settings.RemoveZero24h)
  {
    strftime(hourdraw, sizeof(hourdraw), "%k", time_now);
  }
  else if (clock_is_24h_style() && !settings.RemoveZero24h)
  {
    strftime(hourdraw, sizeof(hourdraw), "%H", time_now);
  }
  else if (settings.AddZero12h)
  {
    strftime(hourdraw, sizeof(hourdraw), "%I", time_now);
  }
  else
  {
    strftime(hourdraw, sizeof(hourdraw), "%l", time_now);
  }

  // Format Minutes
  strftime(mindraw, sizeof(mindraw), "%M", time_now);

  // ==========================================
  // DRAW HOURS
  // ==========================================
  fctx_begin_fill(&fctx);
  fctx_set_fill_color(&fctx, settings.HourColor);

#ifdef PBL_ROUND
  int font_size_h = 168;
#else
  int font_size_h = boundsobs.size.h;
#endif

  fctx_set_text_em_height(&fctx, time_font, font_size_h);

  FPoint hour_pos;
#ifdef PBL_PLATFORM_EMERY
  hour_pos.x = INT_TO_FIXED((bounds.size.w / 4) + 1);
  hour_pos.y = INT_TO_FIXED(1);
#else
  hour_pos.x = INT_TO_FIXED(PBL_IF_ROUND_ELSE(hour_bounds.size.w / 2 + 8, bounds.size.w / 4 + 1));
  hour_pos.y = INT_TO_FIXED(PBL_IF_ROUND_ELSE(hour_bounds.size.h / 2 + 1, 1));
#endif

  int h_width = fctx_string_width(&fctx, hourdraw, time_font);
#if defined(PBL_PLATFORM_EMERY)
  fctx_set_scale(&fctx, FPoint(h_width * 0.7, font_size_h),
                 FPoint(200 * 16, -font_size_h * 3.6 * bounds.size.h / boundsobs.size.h));
#elif defined(PBL_PLATFORM_CHALK)

  int hour_len = strlen(hourdraw);

  if (hour_len == 1)
  {
    // DYNAMIC SCALE FOR SINGLE DIGIT:
    // We increase the width relative to height (e.g., 1.5x wider)
    // and adjust the reference width (100*16) to keep it centered properly
    fctx_set_scale(&fctx, FPoint(h_width * 0.75, font_size_h), FPoint(144 * 16, -font_size_h * 2.25));
  }
  else
  {
    // STANDARD SCALE FOR DOUBLE DIGITS (10, 11, 12 or 00-24)
    fctx_set_scale(&fctx, FPoint(h_width, font_size_h), FPoint(144 * 16, -font_size_h * 2.25));
  }

#else
  int h_ref_width = 144;
  fctx_set_scale(&fctx, FPoint(h_width, font_size_h),
                 FPoint(h_ref_width * 16, -font_size_h * 2.65 * bounds.size.h / boundsobs.size.h));
#endif

  fctx_set_offset(&fctx, hour_pos);
  fctx_draw_string(&fctx, hourdraw, time_font, GTextAlignmentCenter, PBL_IF_ROUND_ELSE(FTextAnchorMiddle, FTextAnchorTop));
  fctx_end_fill(&fctx);

  // ==========================================
  // DRAW MINUTES
  // ==========================================
  fctx_begin_fill(&fctx);
  fctx_set_fill_color(&fctx, settings.MinColor);

#ifdef PBL_ROUND
  int font_size_m = bounds.size.h * 0.55;
#elif defined(PBL_PLATFORM_EMERY)
  int font_size_m = 148;
#else
  int font_size_m = 109;
#endif

  // Reset scale to default before repositioning or apply specific minute scaling
  fctx_set_scale(&fctx, FPoint(INT_TO_FIXED(1), INT_TO_FIXED(1)), FPoint(INT_TO_FIXED(1), INT_TO_FIXED(1)));

  fctx_set_text_em_height(&fctx, time_font, font_size_m);

  FPoint min_pos;
#ifdef PBL_PLATFORM_EMERY
  min_pos.x = INT_TO_FIXED(150);
  min_pos.y = INT_TO_FIXED(boundsobs.size.h / 4 + 10);
#else
  min_pos.x = INT_TO_FIXED(PBL_IF_ROUND_ELSE(135, 108));
  min_pos.y = INT_TO_FIXED(PBL_IF_ROUND_ELSE(bounds.size.h / 2, boundsobs.size.h / 4 + 6));
#endif

  fctx_set_offset(&fctx, min_pos);
  fctx_draw_string(&fctx, mindraw, time_font, GTextAlignmentCenter, FTextAnchorMiddle);
  fctx_end_fill(&fctx);

  // 4. Deinit (Frees memory only once)
  fctx_deinit_context(&fctx);
}

static void layer_update_date_proc(Layer *layer, GContext *ctx)
{
#if defined(PBL_PLATFORM_EMERY)
  GRect DateRect = GRect(100, 128, 50, 22);
  GRect DateDayRect2 = GRect(100, 148, 48, 54);

#else

  GRect DateRect = PBL_IF_ROUND_ELSE(
      GRect(22 - 22 + 6, 26 + 40 + 2, 32, 40),
      GRect(0 + 108 - 19 - 18, 0 + 94, 38, 16));

  GRect DateDayRect2 =
      PBL_IF_ROUND_ELSE(
          GRect(22 - 22 + 6, 16 + 22 + 40 + 2, 32, 40),
          GRect(0 + 108 - 19 - 18, 16 + 90, 38, 40));

#endif

  // Date
  //  Local language
  const char *sys_locale = i18n_get_system_locale();
  char datedraw[10];
  fetchwday(s_weekday, sys_locale, datedraw);
  char datenow[10];
  snprintf(datenow, sizeof(datenow), "%s", datedraw);

  int daydraw;
  daydraw = s_day;
  char daynow[8];
  snprintf(daynow, sizeof(daynow), "%02d", daydraw);

  graphics_context_set_text_color(ctx, settings.TextDayColor);
  graphics_draw_text(ctx, datenow, FontDay, DateRect, GTextOverflowModeWordWrap, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentCenter), NULL);

  graphics_context_set_text_color(ctx, settings.TextDateColor);
  graphics_draw_text(ctx, daynow, PBL_IF_ROUND_ELSE(FontDateNumber, FontDateNumber), DateDayRect2, GTextOverflowModeWordWrap, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentCenter), NULL);
}

static void layer_update_rain(Layer *layer, GContext *ctx)
{
#if defined(PBL_PLATFORM_EMERY)
  GRect RainRect = GRect(87, 180, 75, 27);
  GRect RainProbRect = GRect(100, 200, 50, 27);
#else
  GRect RainRect = PBL_IF_ROUND_ELSE(
      GRect(100, 28, 72, 20),
      GRect(0 + 108 - 27 - 18, 132, 54, 20));

  GRect RainProbRect = PBL_IF_ROUND_ELSE(
      GRect(100, 136 + 12, 50, 20),
      GRect(0 + 108 - 19 - 18, 150, 38, 20));
#endif
  char RainProbToDraw[22];


  if (showForecast == 0) // show current weather
  {
   
    snprintf(RainProbToDraw, sizeof(RainProbToDraw), "%s%%", settings.rainProbNowString);

#ifdef PBL_ROUND
    graphics_context_set_text_color(ctx, settings.TextRainColor);
    graphics_draw_text(ctx, settings.rainNowString, FontRain, RainRect, GTextOverflowModeFill,
                       PBL_IF_ROUND_ELSE(GTextAlignmentLeft, GTextAlignmentCenter), NULL);
    graphics_context_set_text_color(ctx, settings.TextRainProbColor);
    graphics_draw_text(ctx, RainProbToDraw, FontRainProb, RainProbRect, GTextOverflowModeFill,
                       PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentCenter), NULL);
#else

    graphics_context_set_text_color(ctx, settings.TextRainColor);
    graphics_draw_text(ctx, settings.rainNowString, FontRain, RainRect, GTextOverflowModeFill,
                       PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentCenter), NULL);
    graphics_context_set_text_color(ctx, settings.TextRainProbColor);
    graphics_draw_text(ctx, RainProbToDraw, FontRainProb, RainProbRect, GTextOverflowModeFill,
                       PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentCenter), NULL);
#endif
  }
  else if (showForecast == 1) // show forecast weather
  {
    snprintf(RainProbToDraw, sizeof(RainProbToDraw), "%s%%", settings.rainProbMaxForeString);

  

#ifdef PBL_ROUND
    graphics_context_set_text_color(ctx, settings.TextRainColor);
    graphics_draw_text(ctx, settings.rainSumForeString, FontWeatherIcons, RainRect, GTextOverflowModeFill,
                       PBL_IF_ROUND_ELSE(GTextAlignmentLeft, GTextAlignmentCenter), NULL);
    graphics_context_set_text_color(ctx, settings.TextRainProbColor);
    graphics_draw_text(ctx, RainProbToDraw, FontDay, RainProbRect, GTextOverflowModeFill,
                       PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentCenter), NULL);
#else
  graphics_context_set_text_color(ctx, settings.TextRainColor);
    graphics_draw_text(ctx, settings.rainSumForeString, FontRain, RainRect, GTextOverflowModeFill,
                       PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentCenter), NULL);
    graphics_context_set_text_color(ctx, settings.TextRainProbColor);
    graphics_draw_text(ctx, RainProbToDraw, FontRainProb, RainProbRect, GTextOverflowModeFill,
                       PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentCenter), NULL);
#endif
  }
}

static void layer_update_proc_weather(Layer *layer2, GContext *ctx2)
{

#if defined(PBL_PLATFORM_EMERY)
  GRect TempRect = GRect(136, 199, 78, 28);
  GRect IconRect = GRect(150, 161, 52, 54);

#else
  // temperature number
  GRect TempRect = PBL_IF_ROUND_ELSE(
      GRect(8, 106, 32, 40),
      GRect(0 + 108 + 18 - 28, 146, 54, 20));

  // weather condition icon
  GRect IconRect = PBL_IF_ROUND_ELSE(
      GRect(28 - 20, 139 - 100 + 6, 28, 20),
      GRect(0 + 108 - 19 + 18, 16 + 93 + 10, 38, 40));
#endif
  char CondToDraw[20];
  char TempToDraw[20];
  char ForeToDraw[20];
  char HiLowToDraw[20];

  snprintf(CondToDraw, sizeof(CondToDraw), "%s", settings.iconNowString);
  snprintf(TempToDraw, sizeof(TempToDraw), "%s°", settings.tempNowString);
  snprintf(ForeToDraw, sizeof(ForeToDraw), "%s", settings.iconForeString);
  snprintf(HiLowToDraw, sizeof(HiLowToDraw), "%s", settings.tempForeString);

  if (showForecast == 0) // show current weather
  {
#ifdef PBL_ROUND
    graphics_context_set_text_color(ctx2, settings.TextTempColor);
    graphics_draw_text(ctx2, TempToDraw, FontWeatherIcons, TempRect, GTextOverflowModeFill,
                       PBL_IF_ROUND_ELSE(GTextAlignmentLeft, GTextAlignmentCenter), NULL);

    graphics_context_set_text_color(ctx2, settings.TextWeatherColor);
    graphics_draw_text(ctx2, CondToDraw, FontDay, IconRect, GTextOverflowModeFill,
                       PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentCenter), NULL);
#else
    graphics_context_set_text_color(ctx2, settings.TextTempColor);
    graphics_draw_text(ctx2, TempToDraw, FontDay, TempRect, GTextOverflowModeFill,
                       PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentCenter), NULL);

    graphics_context_set_text_color(ctx2, settings.TextWeatherColor);
    graphics_draw_text(ctx2, CondToDraw, FontIcon, IconRect, GTextOverflowModeFill,
                       PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentCenter), NULL);
#endif
  }
  else if (showForecast == 1) // show forecast weather
  {
#ifdef PBL_ROUND
    graphics_context_set_text_color(ctx2, settings.TextTempColor);
    graphics_draw_text(ctx2, HiLowToDraw, FontWeatherIcons, TempRect, GTextOverflowModeFill,
                       PBL_IF_ROUND_ELSE(GTextAlignmentLeft, GTextAlignmentCenter), NULL);
    graphics_context_set_text_color(ctx2, settings.TextWeatherColor);
    graphics_draw_text(ctx2, ForeToDraw, FontDay, IconRect, GTextOverflowModeFill,
                       PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentCenter), NULL);
#else
    graphics_context_set_text_color(ctx2, settings.TextTempColor);
    graphics_draw_text(ctx2, HiLowToDraw, FontFore, TempRect, GTextOverflowModeFill,
                       PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentCenter), NULL);
    graphics_context_set_text_color(ctx2, settings.TextWeatherColor);
    graphics_draw_text(ctx2, ForeToDraw, FontIcon, IconRect, GTextOverflowModeFill,
                       PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentCenter), NULL);
#endif
  }
}

static void layer_update_proc_bt(Layer *layer3, GContext *ctx3)
{
  // Create Rects

#if defined(PBL_PLATFORM_EMERY)
  GRect BTIconRect = GRect(150, 136, 25, 27);
#else
  GRect BTIconRect =
      (PBL_IF_ROUND_ELSE(
          GRect(100, 16, 72, 20),
          GRect(126 - 18, 100 - 2, 18, 20)));

#endif

  onreconnection(BTOn, connection_service_peek_pebble_app_connection());
  bluetooth_callback(connection_service_peek_pebble_app_connection());

  graphics_context_set_text_color(ctx3, settings.TextDayColor);
  graphics_draw_text(ctx3, "z", FontIcon2, BTIconRect, GTextOverflowModeFill, GTextAlignmentCenter, NULL);
}

static void layer_update_proc_qt(Layer *layer4, GContext *ctx4)
{
  // Create Rects
#if defined(PBL_PLATFORM_EMERY)
  GRect QTIconRect = GRect(175, 136, 25, 27);
#else
  GRect QTIconRect = PBL_IF_ROUND_ELSE(
      GRect(8, 126, 32, 20),
      GRect(126, 100 - 2, 18, 20));

#endif

  quiet_time_icon();

  graphics_context_set_text_color(ctx4, settings.TextDayColor);
  graphics_draw_text(ctx4, "\U0000E061", FontIcon2, QTIconRect, GTextOverflowModeFill, GTextAlignmentCenter, NULL);
}

/////////////////////////////////////////
////Init: Handle Settings and Weather////
/////////////////////////////////////////
// Read settings from persistent storage
static void prv_load_settings()
{
  // Load the default settings
  prv_default_settings();
  // Read settings from persistent storage, if they exist
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}
// Save the settings to persistent storage
static void prv_save_settings()
{
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
}
// Handle the response from AppMessage
static void prv_inbox_received_handler(DictionaryIterator *iter, void *context)
{
  Tuple *addzero12_t = dict_find(iter, MESSAGE_KEY_AddZero12h);
  if (addzero12_t)
  {
    settings.AddZero12h = addzero12_t->value->int32 == 1;
  }

  Tuple *remzero24_t = dict_find(iter, MESSAGE_KEY_RemoveZero24h);
  if (remzero24_t)
  {
    settings.RemoveZero24h = remzero24_t->value->int32 == 1;
  }

  Tuple *fr1_color_t = dict_find(iter, MESSAGE_KEY_RoundLeftFrameColor1);
  if (fr1_color_t)
  {
    settings.RoundLeftFrameColor1 = GColorFromHEX(fr1_color_t->value->int32);
  }
  ///////////
  Tuple *sd1_color_t = dict_find(iter, MESSAGE_KEY_MinBackColor1);
  if (sd1_color_t)
  {
    settings.MinBackColor1 = GColorFromHEX(sd1_color_t->value->int32);
  }
  Tuple *sd2_color_t = dict_find(iter, MESSAGE_KEY_HourBackColor2);
  if (sd2_color_t)
  {
    settings.HourBackColor2 = GColorFromHEX(sd2_color_t->value->int32);
  }
  ///////////////////////////////
  Tuple *hr_color_t = dict_find(iter, MESSAGE_KEY_HourColor);
  if (hr_color_t)
  {
    settings.HourColor = GColorFromHEX(hr_color_t->value->int32);
  }
  Tuple *min_color_t = dict_find(iter, MESSAGE_KEY_MinColor);
  if (min_color_t)
  {
    settings.MinColor = GColorFromHEX(min_color_t->value->int32);
  }
  //////////////
  Tuple *tx_day_color_t = dict_find(iter, MESSAGE_KEY_TextDayColor);
  if (tx_day_color_t)
  {
    settings.TextDayColor = GColorFromHEX(tx_day_color_t->value->int32);
  }
  Tuple *tx_date_t = dict_find(iter, MESSAGE_KEY_TextDateColor);
  if (tx_date_t)
  {
    settings.TextDateColor = GColorFromHEX(tx_date_t->value->int32);
  }
  Tuple *tx_rain_color_t = dict_find(iter, MESSAGE_KEY_TextRainColor);
  if (tx_rain_color_t)
  {
    settings.TextRainColor = GColorFromHEX(tx_rain_color_t->value->int32);
  }
  ///////////////

  Tuple *tx8_color_t = dict_find(iter, MESSAGE_KEY_TextRainProbColor);
  if (tx8_color_t)
  {
    settings.TextRainProbColor = GColorFromHEX(tx8_color_t->value->int32);
  }
  ///////////////

  Tuple *tx_weather_color_t = dict_find(iter, MESSAGE_KEY_TextWeatherColor);
  if (tx_weather_color_t)
  {
    settings.TextWeatherColor = GColorFromHEX(tx_weather_color_t->value->int32);
  }
  ///////////////////////////////
  Tuple *tx7_color_t = dict_find(iter, MESSAGE_KEY_TextTempColor);
  if (tx7_color_t)
  {
    settings.TextTempColor = GColorFromHEX(tx7_color_t->value->int32);
  }

  // Control of data from http
  // Weather Cond
  //
  Tuple *wtemp_t = dict_find(iter, MESSAGE_KEY_TempNow);
  if (wtemp_t)
  {
    snprintf(settings.tempNowString, sizeof(settings.tempNowString), "%d",  (int)wtemp_t->value->int32);
  }

  Tuple *wforetemp_t = dict_find(iter, MESSAGE_KEY_TempForeString);
  if (wforetemp_t)
  {
    snprintf(settings.tempForeString, sizeof(settings.tempForeString), "%s", wforetemp_t->value->cstring);
  }

  //////////Add in icons and forecast hi/lo temp////////////
  Tuple *iconnow_tuple = dict_find(iter, MESSAGE_KEY_IconNow);
  if (iconnow_tuple)
  {
    snprintf(settings.iconNowString, sizeof(settings.iconNowString), "%s", weather_conditions[(int)iconnow_tuple->value->int32]);
  }

  Tuple *iconfore_tuple = dict_find(iter, MESSAGE_KEY_IconFore);
  if (iconfore_tuple)
  {
    snprintf(settings.iconForeString, sizeof(settings.iconForeString), "%s", weather_conditions[(int)iconfore_tuple->value->int32]);
  }


  Tuple *rain_now_tuple = dict_find(iter, MESSAGE_KEY_RainNow);
  if (rain_now_tuple)
  {
    snprintf(settings.rainNowString, sizeof(settings.rainNowString), "%d.%d",  (int)(rain_now_tuple->value->int32/10), (int)(rain_now_tuple->value->int32%10));
  }
  
 Tuple *rain_sum_tuple = dict_find(iter, MESSAGE_KEY_RainSumFore);
  if (rain_sum_tuple)
  {
    snprintf(settings.rainSumForeString, sizeof(settings.rainSumForeString), "%d.%d", (int) (rain_sum_tuple->value->int32/10), (int)(rain_sum_tuple->value->int32%10));
  }

  Tuple *rain_prob_now = dict_find(iter, MESSAGE_KEY_RainProbNow);
  if (rain_prob_now)
  {
    snprintf(settings.rainProbNowString, sizeof(settings.rainProbNowString), "%d", (int)rain_prob_now->value->int32);
  }
  Tuple *rainprob_max_tuple = dict_find(iter, MESSAGE_KEY_RainProbMaxFore);
  if (rainprob_max_tuple)
  {
    snprintf(settings.rainProbMaxForeString, sizeof(settings.rainProbMaxForeString), "%d", (int)rainprob_max_tuple->value->int32);
  }


  Tuple *frequpdate = dict_find(iter, MESSAGE_KEY_UpdateSlider);
  if (frequpdate)
  {
    settings.UpdateSlider = (int)frequpdate->value->int32;
    // Restart the counter
    s_countdown = settings.UpdateSlider;
  }

  // Update colors
  layer_mark_dirty(s_canvas_date);
  layer_mark_dirty(s_canvas_weather);
  layer_mark_dirty(s_canvas_rain);
  layer_mark_dirty(s_canvas_bt_icon);
  layer_mark_dirty(s_canvas_qt_icon);

  layer_mark_dirty(time_area_layer);
  // Save the new settings to persistent storage

  prv_save_settings();
}

// Load main layer
static void window_load(Window *window)
{
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_canvas_background = layer_create(bounds);
  layer_set_update_proc(s_canvas_background, layer_update_proc_background);
  layer_add_child(window_layer, s_canvas_background);

  time_area_layer = layer_create(bounds);
  layer_add_child(window_layer, time_area_layer);
  layer_set_update_proc(time_area_layer, update_time_area_layer);

  s_canvas_date = layer_create(bounds);
  layer_set_update_proc(s_canvas_date, layer_update_date_proc);
  layer_add_child(window_layer, s_canvas_date);

  s_canvas_rain = layer_create(bounds);
  layer_set_update_proc(s_canvas_rain, layer_update_rain);
  layer_add_child(window_layer, s_canvas_rain);

  s_canvas_weather = layer_create(bounds);
  layer_set_update_proc(s_canvas_weather, layer_update_proc_weather);
  layer_add_child(window_layer, s_canvas_weather);

  s_canvas_bt_icon = layer_create(bounds);
  layer_set_update_proc(s_canvas_bt_icon, layer_update_proc_bt);
  layer_add_child(window_layer, s_canvas_bt_icon);

  s_canvas_qt_icon = layer_create(bounds);
  layer_set_update_proc(s_canvas_qt_icon, layer_update_proc_qt);
  layer_add_child(window_layer, s_canvas_qt_icon);
}

static void window_unload(Window *window)
{
  layer_destroy(s_canvas_background);
  layer_destroy(s_canvas_date);
  layer_destroy(s_canvas_rain);
  layer_destroy(s_canvas_weather);
  layer_destroy(time_area_layer);
  layer_destroy(s_canvas_bt_icon);
  layer_destroy(s_canvas_qt_icon);
  window_destroy(s_window);
  fonts_unload_custom_font(FontIcon);
  fonts_unload_custom_font(FontIcon2);
  fonts_unload_custom_font(FontWeatherIcons);
  ffont_destroy(time_font);
}

void main_window_push()
{
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers){
                                           .load = window_load,
                                           .unload = window_unload,
                                       });
  window_stack_push(s_window, true);
}

void main_window_update(int hours, int minutes, int weekday, int day)
{
  s_hours = hours;
  s_minutes = minutes;
  s_day = day;
  s_weekday = weekday;

  layer_mark_dirty(s_canvas_background);
  layer_mark_dirty(s_canvas_date);
  layer_mark_dirty(s_canvas_bt_icon);
  layer_mark_dirty(s_canvas_qt_icon);
  layer_mark_dirty(time_area_layer);
}

static void tick_handler(struct tm *time_now, TimeUnits changed)
{

  main_window_update(time_now->tm_hour, time_now->tm_min, time_now->tm_wday, time_now->tm_mday);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Tick at %d", time_now->tm_min);

  if (s_countdown == 0)
  {
    // reset counter
    s_countdown = settings.UpdateSlider;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Update weather at %d", time_now->tm_min);
    request_watchjs();
  }
  else
  {
    s_countdown = s_countdown - 1;
  }
}

static void init()
{
  prv_load_settings();
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  s_hours = t->tm_hour;
  s_minutes = t->tm_min;
  s_day = t->tm_mday;
  s_weekday = t->tm_wday;
  // Register and open
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(512, 512);
  // Load Fonts
  // allocate fonts

  time_font = ffont_create_from_resource(RESOURCE_ID_FONT_STEELFISH);

#ifdef PBL_PLATFORM_EMERY

  FontDay = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  FontDateNumber = fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS);
  FontRain = fonts_get_system_font(FONT_KEY_GOTHIC_24);
  FontRainProb = fonts_get_system_font(FONT_KEY_GOTHIC_24);
  FontFore = fonts_get_system_font(FONT_KEY_GOTHIC_24);
  FontWeatherIcons = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHERICONS_22));
  FontIcon = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHERICONS_28));
  FontIcon2 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DRIPICONS_18));

#else

  FontDay = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  FontDateNumber = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  FontRain = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  FontRainProb = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  FontFore = PBL_IF_ROUND_ELSE(fonts_get_system_font(FONT_KEY_GOTHIC_14), fonts_get_system_font(FONT_KEY_GOTHIC_18));
  FontWeatherIcons = fonts_load_custom_font(resource_get_handle(PBL_IF_ROUND_ELSE(RESOURCE_ID_FONT_WEATHERICONS_12, RESOURCE_ID_FONT_WEATHERICONS_16)));
  FontIcon = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHERICONS_20));
  FontIcon2 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DRIPICONS_16));

#endif

  main_window_push();
  // Register with Event Services
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  connection_service_subscribe((ConnectionHandlers){
      .pebble_app_connection_handler = bluetooth_vibe_icon});
  bluetooth_vibe_icon(connection_service_peek_pebble_app_connection());
  accel_tap_service_subscribe(accel_tap_handler);
}

static void deinit()
{
  tick_timer_service_unsubscribe();
  app_message_deregister_callbacks(); // Destroy the callbacks for clean up
  connection_service_unsubscribe();
  accel_tap_service_unsubscribe();
}
int main()
{
  init();
  app_event_loop();
  deinit();
}
