#pragma once
#include <pebble.h>
#define SETTINGS_KEY 1
// A structure containing our settings
typedef struct ClaySettings
{
  // GColor Back1Color;
  //  GColor Back2Color;
  GColor FrameColor;
  GColor FrameColor1;
  GColor FrameColor2;
  GColor SideColor1;
  GColor SideColor2;
  GColor TextRainColor;
  GColor Text2Color;
  GColor TextDayColor;
  GColor Text4Color;
  GColor TextWeatherColor;
  GColor TextDateColor;
  GColor TextTempColor;
  GColor TextRainProbColor;
  /////////////////
  GColor HourColor;
  GColor MinColor;
  GColor HourColorN;
  GColor MinColorN;
  ////////////////
  // GColor Back1ColorN;
  // GColor Back2ColorN;
  GColor FrameColorN;
  GColor FrameColor1N;
  GColor FrameColor2N;
  GColor SideColor1N;
  GColor SideColor2N;
  GColor Text1ColorN;
  GColor Text2ColorN;
  GColor Text3ColorN;
  GColor Text4ColorN;
  GColor TextWeatherColorN;
  GColor Text6ColorN;
  GColor TextTempColorN;
  GColor TextRainProbColorN;
  int WeatherUnit;
  char *WindUnit;
  int UpSlider;
  char *WeatherTemp;
  char *TempFore;
  bool NightTheme;
  bool AddZero12h;
  bool RemoveZero24h;
  bool WeatherOn;
  bool ForecastWeatherOn;
  bool SunsetOn;
  char FontChoice[12];
  char moonstring[12];
  char sunsetstring[10];
  char sunrisestring[10];
  char sunsetstring12[10];
  char sunrisestring12[10];
  char tempstring[10];
  char iconnowstring[6];
  // char condstring[10];
  // char windstring[10];
  // char windavestring[10];
  char iconforestring[6];
  // char windiconnowstring[6];
  // char windiconavestring[6];
  // char templowstring[10];
  char temphistring[10];
  // bool Rotate;
  // bool RightLeft;
} __attribute__((__packed__)) ClaySettings;
