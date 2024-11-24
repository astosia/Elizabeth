// Clay Config: see https://github.com/pebble/clay
module.exports = [
  {
    "type": "heading",
    "defaultValue": "Settings"
  },
  {
  "type": "heading",
  "defaultValue": "Weather will not show unless a weather provider is selected and set up.",
  "size":6
  },
  {
  "type": "heading",
  "defaultValue": "Set complications to the same colour as the background to hide them.",
  "size":6
  },
  {
    "type": "section",
    "items": [
      {
        "type": "toggle",
        "messageKey": "HealthOff",
        "label": "Switch off Steps",
        "defaultValue": true,
        "capabilities":["HEALTH"]
      },
      {
        "type": "toggle",
        "messageKey": "VibeOn",
        "label": "Switch on vibrate on Bluetooth disconnect during Quiet Time",
        "defaultValue": false
      },
      {
        "type": "heading",
        "defaultValue": "Time Section",
        "size":4
      },
      {
        "type": "color",
        "messageKey": "FrameColor2",
        "defaultValue": "0x000000",
        "label": "Background Colour",
        "allowGray":true
      },
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
        "defaultValue": false
      },
      {
        "type": "color",
        "messageKey": "HourColor",
        "defaultValue": "0xFFFFFF",
        "label": "Hour Colour",
        "allowGray":true
      },
      {
        "type": "color",
        "messageKey": "MinColor",
        "defaultValue": "0xFFFFFF",
        "label": "Minute Colour",
        "allowGray":true
      },
      {
        "type": "color",
        "messageKey": "DateColor",
        "defaultValue": "0xFFFFFF",
        "label": "Date Colour",
        "allowGray":true
      },
      {
        "type": "color",
        "messageKey": "Text3Color",
        "defaultValue": "0xFFFFFF",
        "label": "Day of the Week Colour",
        "allowGray":true
      },
      {
        "type": "color",
        "messageKey": "Text6Color",
        "defaultValue": "0xFFFFFF",
        "label": "Battery Bar Colour",
        "allowGray":true
      },
      {
        "type": "heading",
        "defaultValue": "Complications Section",
        "size":4
      },
      {
        "type": "color",
        "messageKey": "FrameColor",
        "defaultValue": "0x000000",
        "label": "Background Colour",
        "allowGray":true
      },
      {
        "type": "color",
        "messageKey": "Text7Color",
        "defaultValue": "0xFFFFFF",
        "label": "Weather icon & Rain Graph Colour",
        "allowGray":true
      },
      {
        "type": "color",
        "messageKey": "Text8Color",
        "defaultValue": "0xFFFFFF",
        "label": "Temperature & Rain Colour",
        "allowGray":true
      },
      {
        "type": "color",
        "messageKey": "Text9Color",
        "defaultValue": "0xFFFFFF",
        "label": "Wind Speed Colour",
        "allowGray":true
      },
      {
        "type": "color",
        "messageKey": "Text10Color",
        "defaultValue": "0xFFFFFF",
        "label": "Wind Direction Colour",
        "allowGray":true
      },
      {
        "type": "color",
        "messageKey": "Text5Color",
        "defaultValue": "0xFFFFFF",
        "label": "BT & QT Colour",
        "allowGray":true
      },
       {
        "type": "color",
        "messageKey": "Text2Color",
        "defaultValue": "0xFFFFFF",
        "label": "Sunrise & Sunset Colour",
        "allowGray":true
      },
      {
        "type": "color",
        "messageKey": "Text1Color",
        "defaultValue": "0xFFFFFF",
        "label": "Steps Colour",
        "allowGray":true,
        "capabilities":["HEALTH"]
      },
      {
        "type":"color",
        "messageKey":"Text4Color",
        "defaultValue":"0xFFFFFF",
        "label":"Moon Colour",
        "allowGray":true
      },
    ]
  },
  {
    "type": "section",
       "items": [
         {
           "type": "heading",
           "defaultValue": "Night Theme",
           "size":4
         } ,
         {
         "type": "heading",
         "defaultValue": "Changes colours at night (between sunset and sunrise, or from 22:00 to 07:00 if no GPS or location set).",
         "size":6
         },
         {
           "type": "toggle",
           "messageKey": "NightTheme",
           "label": "Activate Night Theme",
           "defaultValue": false,
         },
         {
           "type": "heading",
           "defaultValue": "Time Section Night",
           "size":4
         },
         {
            "type": "color",
            "messageKey": "FrameColor2N",
            "defaultValue": "0xFFFFFF",
            "label": "Background Colour",
            "allowGray":true
         },
         {
           "type": "color",
           "messageKey": "HourColorN",
           "defaultValue": "0x000000",
          "label": "Hour Colour",
          "allowGray":true
         },
         {
           "type": "color",
           "messageKey": "MinColorN",
           "defaultValue": "0x000000",
          "label": "Minute Colour",
          "allowGray":true
         },
         {
          "type": "color",
          "messageKey": "DateColorN",
          "defaultValue": "0x000000",
          "label": "Date Colour",
          "allowGray":true
         },
         {
           "type": "color",
           "messageKey": "Text3ColorN",
           "defaultValue": "0x000000",
           "label": "Day of the Week Colour",
           "allowGray":true
         },
         {
          "type": "color",
          "messageKey": "Text6ColorN",
          "defaultValue": "0x000000",
          "label": "Battery Bar Colour",
          "allowGray":true
         },
         {
           "type": "heading",
           "defaultValue": "Complications Section Night",
           "size":4
         },
         {
            "type": "color",
            "messageKey": "FrameColorN",
            "defaultValue": "0xFFFFFF",
            "label": "Background Colour",
            "allowGray":true
         },
         {
           "type": "color",
           "messageKey": "Text7ColorN",
           "defaultValue": "0x000000",
           "label": "Weather icon & Rain Graph Colour",
           "allowGray":true
         },
         {
           "type": "color",
           "messageKey": "Text8ColorN",
           "defaultValue": "0x000000",
           "label": "Temperature & Rain Colour",
           "allowGray":true
         },
         {
           "type": "color",
           "messageKey": "Text9ColorN",
           "defaultValue": "0x000000",
           "label": "Wind Speed Colour",
           "allowGray":true
         },
         {
           "type": "color",
           "messageKey": "Text10ColorN",
           "defaultValue": "0x000000",
           "label": "Wind Direction Colour",
           "allowGray":true
         },
         {
          "type": "color",
          "messageKey": "Text5ColorN",
          "defaultValue": "0x000000",
          "label": "BT & QT Colour",
          "allowGray":true
         },
         {
          "type": "color",
          "messageKey": "Text2ColorN",
          "defaultValue": "0x000000",
          "label": "Sunrise & Sunset Colour",
          "allowGray":true
         },
         {
          "type": "color",
          "messageKey": "Text1ColorN",
          "defaultValue": "0x000000",
          "label": "Steps Colour",
          "allowGray":true,
          "capabilities":["HEALTH"]
         },
         {
          "type":"color",
          "messageKey":"Text4ColorN",
          "defaultValue":"0x000000",
          "label":"Moon Colour",
          "allowGray":true
        }
       ]
     },
  {
    "type": "section",
    "items": [
          {
            "type": "heading",
            "defaultValue": "Weather settings"
          },
          {
            "type": "slider",
            "messageKey": "UpSlider",
            "defaultValue": 30,
            "label": "Weather update frequency (minutes)",
            "description": "More frequent requests will drain your phone battery more quickly",
            "min": 15,
            "max": 120,
            "step": 15
          },
          {
            "type": "toggle",
            "messageKey": "WeatherUnit",
            "label": "Temperature in Fahrenheit",
            "defaultValue": false,
          },
            {
            "type": "select",
            "messageKey": "WindUnit",
            "label": "Wind speed unit to display",
            "defaultValue": "knots",
               "options": [
              {
                "label": "knots",
                "value": "kts"
              },
              {
                "label": "miles per hour",
                "value": "mph"
              },
                 {
                "label": "metres per second",
                "value": "ms"
              },
              {
                "label": "kilometres per hour",
                "value": "kph"
              }
            ]
          },
          {
          "type": "select",
          "messageKey": "RainUnit",
          "label": "Rain quantity unit to display",
          "defaultValue": "mm",
             "options": [
            {
              "label": "millimeters | mm/h",
              "value": "mm"
            },
            {
              "label": "inches | in/h",
              "value": "in"
            }
          ]
        },
        {
          "type": "slider",
          "messageKey": "Rainmultiplier",
          "defaultValue": 3,
          "label": "Rain bar length (relative)",
          "description": "Increase or decrease the size of the rain bars",
          "min": 1,
          "max": 10,
          "step": 1
        },
          {
            "type": "select",
            "messageKey": "WeatherProv",
            "defaultValue": "owm",
            "label": "Weather Provider",
            "options": [
              {
                "label": "OpenWeatherMap",
                "value": "owm"
              }
            ]
          },
          {
             "type": "input",
             "messageKey": "Lat",
             "defaultValue": "",
             "label": "Manual Location - Latitude",
             "attributes": {
             "placeholder": "eg: 51.4962"
             }
           },
           {
              "type": "input",
              "messageKey": "Long",
              "defaultValue": "",
              "label": "Manual Location - Longitude",
              "description": "Leave both blank to use GPS location for sunrise & sunset times and weather. You can use <a href =https://www.google.com/maps>Google Maps</a> or <a href =https://www.openstreetmap.org/>OpenStreetMap</a> to find latitude & longitude.",
              "attributes": {
                "placeholder": "eg: -0.0989"
              }
            },
         {
            "type": "input",
            "messageKey": "APIKEY_User",
            "defaultValue": "",
            "label": "API Key",
            "description": "If you don't have an api key, weather data will not be displayed. You can register for a free personal API key for <a href =https://home.openweathermap.org/users/sign_up/>OpenWeatherMap here</a>.",
            "attributes": {
              "placeholder": "Paste your API Key here"
            }
          },
        ]
          },
          {
            "type": "section",
            "items": [
              {
                "type": "heading",
                "defaultValue": "Personal Weather Station settings",
                "capabilities":["MICROPHONE"]
              },
              {
                "type": "toggle",
                "messageKey": "UsePWS",
                "label": "Show Personal Weather Station data.",
                "defaultValue": false,
                "capabilities":["MICROPHONE"]
              },
              {
              "type": "select",
              "messageKey": "PressureUnit",
              "capabilities":["MICROPHONE"],
              "label": "Barometric Pressure Unit",
              "defaultValue": "mb",
                 "options": [
                {
                  "label": "millibars (mb)",
                  "value": "mb"
                },
                {
                  "label": "inches of Mercury (Hg)",
                  "value": "hg"
                },
                {
                  "label": "millimeters of Mercury (mmHg)",
                  "value": "tor"
                },
                {
                  "label": "Pascals (P)",
                  "value": "ap"
                },
                {
                  "label": "standard atmospheric (atm)",
                  "value": "atm"
                }
                ]
              },
              {
                 "type": "input",
                 "messageKey": "PWSAPIKEY_User",
                 "defaultValue": "",
                 "label": "Personal Weather Station API Key",
                 "description": "Use this to access your Personal Weather Station data via Weather Underground/The Weather Company. If left blank, the watch will attempt to request the Weather Underground API from your pmkey.xyz",
                 "attributes": {
                   "placeholder": "Paste PWS API Key, or blank if using pmkey"
                 },
                 "capabilities":["MICROPHONE"]
               },
               {
                  "type": "input",
                  "messageKey": "PWSStationID_User",
                  "defaultValue": "",
                  "label": "Personal Weather Station ID",
                  "description": "Enter your Personal Weather Station Station ID",
                  "attributes": {
                    "placeholder": "eg: KMAHANOV10"
                  },
                  "capabilities":["MICROPHONE"]
                },
            ]
          },
        {
          "type": "submit",
          "defaultValue":"SAVE"
          },
          {
          "type": "heading",
          "defaultValue": "version v4.0",
          "size":6
          },
          {
          "type": "heading",
          "defaultValue": "Made in the UK",
          "size":6
          }
        ];
