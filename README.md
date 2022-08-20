# Elizabeth
![Banner Elizabeth](https://user-images.githubusercontent.com/39700835/183258123-45612506-ca07-4eff-bf98-44bef5031de2.png)


Elizabeth is an easy to read pebble watchface with weather & steps, using the Elizabeth Line font from the London Underground.

Features:
- Fully configurable colours
- Toggle on configuration screen to turn off pebble health (turns off steps & the pebble health warning banner)
- Weather (based on phone GPS or a manually entered latitude & longitude)
- Sunrise & sunset times (based on phone GPS or a manually entered latitude & longitude)
- Radial graph of rain in the next hour (if available)
- Auto-translation of weekday supported for: English, Spanish, German, French, Portuguese, Italian
- Battery bar
- Auto-detection of 12h/24h based on your watch settings - Bluetooth disconnect vibe warning and icon (no vibe in quiet time)
- Optional night theme colours between sunset and sunrise

Weather:
- Choice of two weather providers: Dark Sky (existing api keys only) or OpenWeathermap (link to sign up for a new api key on the settings page for the watchface)
- Current & forecast conditions icon and current & forecast high/low temperatures in °C or °F
- Current & forecast average wind direction & speed in knots, kph, mph or ms
- Option to show Personal Weather Station data*

Tap or shake for:
- Rain info for the next hour, showing rain graph, rain quantity (mm per hour or inches per hour), and chance of rain (%).  Change the size/length of the rain graph in the settings.
- Moonphase
- Steps (on Time Series and Pebble 2). Steps/health can be turned off via config page.  If steps are off, shows high/low temperature instead.

Implementation of pmkey.xyz to store your api keys & retrieve them with a simple pin number.
Make elements the same colour as the background to hide them

* V3 adds in an option on Time Series & Pebble 2 watches to show local Personal Weather Station data from Weather Underground/The Weather Company.  Set up on the config page, and then shake or tap to display:
- Current Temperature in °C or °F
- Current Barometric Pressure in millibars (mb), inches of Mercury (Hg), millimeters of Mercury (mmHG), Pascals or Standard atmospheric (atm)
- Total Rain amount today in mm or inches
- If it's not raining currently, the left hand icon shows a thermometer.  If it is raining, it shows an umbrella
- If there has been no rain today, right hand icon shows a barometer.  If it has rained today, shows a raindrop
