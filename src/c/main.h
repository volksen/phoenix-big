#pragma once
#include <pebble.h>
#define SETTINGS_KEY 1
// A structure containing our settings
typedef struct ClaySettings
{
  bool AddZero12h;
  bool RemoveZero24h;
  GColor RoundLeftFrameColor1;
  GColor MinBackColor1;
  GColor HourBackColor2;
  GColor HourColor;
  GColor MinColor;
  GColor TextDayColor;
  GColor TextDateColor;
  GColor TextRainColor;
  GColor TextRainProbColor;
  GColor TextWeatherColor;
  GColor TextTempColor;
  int UpdateSlider;
  char tempNowString[10];
  char tempForeString[10];
  char iconNowString[6];
  char iconForeString[6];
  char rainNowString[16];
  char rainSumForeString[16];
  char rainProbNowString[20];
  char rainProbMaxForeString[20];
} __attribute__((__packed__)) ClaySettings;
