// Clay Config: see https://github.com/pebble/clay
module.exports = [
  {
    "type": "heading",
    "defaultValue": "Settings"
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Theme settings"
      }
      {
        "type": "toggle",
        "messageKey": "AddZero12h",
        "label": "Add leading zero to 12h time",
        "defaultValue": false
      },
      {
        "type": "toggle",
        "messageKey": "RemoveZero24h",
        "label": "Remove leading zero from 24h time",
        "defaultValue": true
      },
      {
        "type": "color",
        "messageKey": "RoundLeftFrameColor1",
        "defaultValue": "0x000000",
        "label": "Left Sidebar Colour",
        "capabilities": ["ROUND"]
      },
      {
        "type": "color",
        "messageKey": "MinBackColor1",
        "defaultValue": "0x000000",
        "label": "Minute Background Colour",
        "allowGray": true
      },
      {
        "type": "color",
        "messageKey": "HourBackColor2",
        "defaultValue": "0x000000",
        "label": "Hour Background Colour",
        "allowGray": true
      },
      {
        "type": "color",
        "messageKey": "HourColor",
        "defaultValue": "0xFFFFFF",
        "label": "Hour Text Colour",
        "allowGray": true
      },
      {
        "type": "color",
        "messageKey": "MinColor",
        "defaultValue": "0xFFFFFF",
        "label": "Minute Text Colour",
        "allowGray": true
      },
      {
        "type": "color",
        "messageKey": "TextDayColor",
        "defaultValue": "0xFFFFFF",
        "label": "Day Colour",
        "allowGray": true
      },
      {
        "type": "color",
        "messageKey": "TextDateColor",
        "defaultValue": "0xFFFFFF",
        "label": "Date Colour",
        "allowGray": true
      },
      {
        "type": "color",
        "messageKey": "TextRainColor",
        "defaultValue": "0xFFFFFF",
        "label": "Rain Colour",
        "allowGray": true
      },
      {
        "type": "color",
        "messageKey": "TextRainProbColor",
        "defaultValue": "0xFFFFFF",
        "label": "Rain Prob Colour",
        "allowGray": true
      },
      {
        "type": "color",
        "messageKey": "TextWeatherColor",
        "defaultValue": "0xFFFFFF",
        "label": "Weather Icon Colour",
        "allowGray": true
      },
      {
        "type": "color",
        "messageKey": "TextTempColor",
        "defaultValue": "0xFFFFFF",
        "label": "Temperature Colour",
        "allowGray": true
      },
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Weather settings",
        "description": "Shake or tap to change view between Current Weather and Forecast Weather",
      },
     
      {
        "type": "slider",
        "messageKey": "UpdateSlider",
        "defaultValue": 30,
        "label": "Update frequency (minutes)",
        "description": "More frequent requests will drain your phone battery more quickly",
        "min": 15,
        "max": 120,
        "step": 15
      },
    ]
  },
  {
    "type": "submit",
    "defaultValue": "SAVE"
  }
];
