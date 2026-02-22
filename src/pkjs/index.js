//function () { "use strict";

// shortcuts for easier to read formulas



var PI   = Math.PI,
    sin  = Math.sin,
    cos  = Math.cos,
    tan  = Math.tan,
    asin = Math.asin,
    atan = Math.atan2,
    acos = Math.acos,
    rad  = PI / 180;

// sun calculations are based on http://aa.quae.nl/en/reken/zonpositie.html formulas


// date/time constants and conversions

var dayMs = 1000 * 60 * 60 * 24,
    J1970 = 2440588,
    J2000 = 2451545;

function toJulian(date) {
    return date.valueOf() / dayMs - 0.5 + J1970;
}
function fromJulian(j) {
    return new Date((j + 0.5 - J1970) * dayMs);
}
function toDays(date) {
    return toJulian(date) - J2000;
}


// general calculations for position

var e = rad * 23.4397; // obliquity of the Earth

function getRightAscension(l, b) {
    return atan(sin(l) * cos(e) - tan(b) * sin(e), cos(l));
}
function getDeclination(l, b) {
    return asin(sin(b) * cos(e) + cos(b) * sin(e) * sin(l));
}
function getAzimuth(H, phi, dec) {
    return atan(sin(H), cos(H) * sin(phi) - tan(dec) * cos(phi));
}
function getAltitude(H, phi, dec) {
    return asin(sin(phi) * sin(dec) + cos(phi) * cos(dec) * cos(H));
}
function getSiderealTime(d, lw) {
    return rad * (280.16 + 360.9856235 * d) - lw;
}


// general sun calculations

function getSolarMeanAnomaly(d) {
    return rad * (357.5291 + 0.98560028 * d);
}
function getEquationOfCenter(M) {
    return rad * (1.9148 * sin(M) + 0.02 * sin(2 * M) + 0.0003 * sin(3 * M));
}
function getEclipticLongitude(M, C) {
    var P = rad * 102.9372; // perihelion of the Earth
    return M + C + P + PI;
}
function getSunCoords(d) {

    var M = getSolarMeanAnomaly(d),
        C = getEquationOfCenter(M),
        L = getEclipticLongitude(M, C);

    return {
        dec: getDeclination(L, 0),
        ra: getRightAscension(L, 0)
    };
}


var SunCalc = {};


// calculates sun position for a given date and latitude/longitude

SunCalc.getPosition = function (date, lat, lng) {

    var lw  = rad * -lng,
        phi = rad * lat,
        d   = toDays(date),

        c  = getSunCoords(d),
        H  = getSiderealTime(d, lw) - c.ra;

    return {
        azimuth: getAzimuth(H, phi, c.dec),
        altitude: getAltitude(H, phi, c.dec)
    };
};


// sun times configuration (angle, morning name, evening name)

var times = [
    [-0.83, 'sunrise',       'sunset'      ],
    [ -0.3, 'sunriseEnd',    'sunsetStart' ],
    [   -6, 'dawn',          'dusk'        ],
    [  -12, 'nauticalDawn',  'nauticalDusk'],
    [  -18, 'nightEnd',      'night'       ],
    [    6, 'goldenHourEnd', 'goldenHour'  ]
];

// adds a custom time to the times config

SunCalc.addTime = function (angle, riseName, setName) {
    times.push([angle, riseName, setName]);
};


// calculations for sun times

var J0 = 0.0009;

function getJulianCycle(d, lw) {
    return Math.round(d - J0 - lw / (2 * PI));
}
function getApproxTransit(Ht, lw, n) {
    return J0 + (Ht + lw) / (2 * PI) + n;
}
function getSolarTransitJ(ds, M, L) {
    return J2000 + ds + 0.0053 * sin(M) - 0.0069 * sin(2 * L);
}
function getHourAngle(h, phi, d) {
    return acos((sin(h) - sin(phi) * sin(d)) / (cos(phi) * cos(d)));
}


// calculates sun times for a given date and latitude/longitude

SunCalc.getTimes = function (date, lat, lng) {

    var lw  = rad * -lng,
        phi = rad * lat,
        d   = toDays(date),

        n  = getJulianCycle(d, lw),
        ds = getApproxTransit(0, lw, n),

        M = getSolarMeanAnomaly(ds),
        C = getEquationOfCenter(M),
        L = getEclipticLongitude(M, C),

        dec = getDeclination(L, 0),

        Jnoon = getSolarTransitJ(ds, M, L);


    // returns set time for the given sun altitude
    function getSetJ(h) {
        var w = getHourAngle(h, phi, dec),
            a = getApproxTransit(w, lw, n);

        return getSolarTransitJ(a, M, L);
    }


    var result = {
        solarNoon: fromJulian(Jnoon),
        nadir: fromJulian(Jnoon - 0.5)
    };

    var i, len, time, angle, morningName, eveningName, Jset, Jrise;

    for (i = 0, len = times.length; i < len; i += 1) {
        time = times[i];

        Jset = getSetJ(time[0] * rad);
        Jrise = Jnoon - (Jset - Jnoon);

        result[time[1]] = fromJulian(Jrise);
        result[time[2]] = fromJulian(Jset);
    }

    return result;
};


// moon calculations, based on http://aa.quae.nl/en/reken/hemelpositie.html formulas

function getMoonCoords(d) { // geocentric ecliptic coordinates of the moon

    var L = rad * (218.316 + 13.176396 * d), // ecliptic longitude
        M = rad * (134.963 + 13.064993 * d), // mean anomaly
        F = rad * (93.272 + 13.229350 * d),  // mean distance

        l  = L + rad * 6.289 * sin(M), // longitude
        b  = rad * 5.128 * sin(F),     // latitude
        dt = 385001 - 20905 * cos(M);  // distance to the moon in km

    return {
        ra: getRightAscension(l, b),
        dec: getDeclination(l, b),
        dist: dt
    };
}

SunCalc.getMoonPosition = function (date, lat, lng) {

    var lw  = rad * -lng,
        phi = rad * lat,
        d   = toDays(date),

        c = getMoonCoords(d),
        H = getSiderealTime(d, lw) - c.ra,
        h = getAltitude(H, phi, c.dec);

    // altitude correction for refraction
    h = h + rad * 0.017 / tan(h + rad * 10.26 / (h + rad * 5.10));

    return {
        azimuth: getAzimuth(H, phi, c.dec),
        altitude: h,
        distance: c.dist
    };
};


// calculations for illuminated fraction of the moon,
// based on http://idlastro.gsfc.nasa.gov/ftp/pro/astro/mphase.pro formulas

SunCalc.getMoonIllumination = function (date) {

    var d = toDays(date || new Date()),
        s = getSunCoords(d),
        m = getMoonCoords(d),

        sdist = 149598000, // distance from Earth to Sun in km

        phi = acos(sin(s.dec) * sin(m.dec) + cos(s.dec) * cos(m.dec) * cos(s.ra - m.ra)),
        inc = atan(sdist * sin(phi), m.dist - sdist * cos(phi)),
        angle = atan(cos(s.dec) * sin(s.ra - m.ra), sin(s.dec) * cos(m.dec) -
                cos(s.dec) * sin(m.dec) * cos(s.ra - m.ra));

    return {
        fraction: (1 + cos(inc)) / 2,
        phase: 0.5 + 0.5 * inc * (angle < 0 ? -1 : 1) / Math.PI,
        angle: angle
    };
};

var WEATHER_MAX  = 118;  // weather_conditions array last index
var WIND_MAX     = 16;   // wind_direction array last index  (16 = Variable)
var MOON_MAX     = 28;   // moon_phase array last index

function clampWeather(idx) {
  var n = parseInt(idx, 10);
  if (isNaN(n) || n < 0) { return 0; }
  if (n > WEATHER_MAX)   { return 0; }  // 0 = unknown icon
  return n;
}

function clampWind(idx) {
  var n = parseInt(idx, 10);
  if (isNaN(n) || n < 0) { return WIND_MAX; }  // WIND_MAX = Variable
  if (n > WIND_MAX)       { return WIND_MAX; }
  return n;
}

function clampMoon(idx) {
  var n = parseInt(idx, 10);
  if (isNaN(n) || n < 0) { return 0; }
  if (n > MOON_MAX)       { return 0; }
  return n;
}

function owmWindDegToIndex(deg) {
  var n = parseInt(deg, 10);
  if (isNaN(n) || n < 0 || n > 360) { return 16; } // 16 = Variable
  // Each of the 8 directions spans 45°, mapped to even indices 0–14.
  // We add 22.5 (half a segment) so that 0° North is centred correctly,
  // then wrap with modulo to handle 360° == 0° (North).
  return (Math.round(n / 45) % 8) * 2;
}
function safeWindId(degStr) {
  return owmWindDegToIndex(degStr);
}

var owm_iconToId = {
  '20011d':1, //thunderstorm with light rain
  '20011n':2, //thunderstorm with light rain
  '20111d':3, //thunderstorm with rain
  '20111n':4, //thunderstorm with rain
  '20211d':5, //thunderstorm with heavy rain
  '20211n':6, //thunderstorm with heavy rain
  '21011d':7, //light thunderstorm
  '21011n':8, //light thunderstorm
  '21111d':9, //thunderstorm
  '21111n':10, //thunderstorm
  '21211d':11, //heavy thunderstorm
  '21211n':12, //heavy thunderstorm
  '22111d':13, //ragged thunderstorm
  '22111n':14, //ragged thunderstorm
  '23011d':15, //thunderstorm with light drizzle
  '23011n':16, //thunderstorm with light drizzle
  '23111d':17, //thunderstorm with drizzle
  '23111n':18, //thunderstorm with drizzle
  '23211d':19, //thunderstorm with heavy drizzle
  '23211n':20, //thunderstorm with heavy drizzle
  '30009d':21, //light intensity drizzle
  '30009n':22, //light intensity drizzle
  '30109d':23, //drizzle
  '30109n':24, //drizzle
  '30209d':25, //heavy intensity drizzle
  '30209n':26, //heavy intensity drizzle
  '31009d':27, //light intensity drizzle rain
  '31009n':28, //light intensity drizzle rain
  '31109d':29, //drizzle rain
  '31109n':30, //drizzle rain
  '31209d':31, //heavy intensity drizzle rain
  '31209n':32, //heavy intensity drizzle rain
  '31309d':33, //shower rain and drizzle
  '31309n':34, //shower rain and drizzle
  '31409d':35, //heavy shower rain and drizzle
  '31409n':36, //heavy shower rain and drizzle
  '32109d':37, //shower drizzle
  '32109n':38, //shower drizzle
  '50010d':39, //light rain
  '50010n':40, //light rain
  '50110d':41, //moderate rain
  '50110n':42, //moderate rain
  '50210d':43, //heavy intensity rain
  '50210n':44, //heavy intensity rain
  '50310d':45, //very heavy rain
  '50310n':46, //very heavy rain
  '50410d':47, //extreme rain
  '50410n':48, //extreme rain
  '51113d':49, //freezing rain
  '51113n':50, //freezing rain
  '52009d':51, //light intensity shower rain
  '52009n':52, //light intensity shower rain
  '52109d':53, //shower rain
  '52109n':54, //shower rain
  '52209d':55, //heavy intensity shower rain
  '52209n':56, //heavy intensity shower rain
  '53109d':57, //ragged shower rain
  '53109n':58, //ragged shower rain
  '60013d':59, //light snow
  '60013n':60, //light snow
  '60113d':61, //Snow
  '60113n':62, //Snow
  '60213d':63, //Heavy snow
  '60213n':64, //Heavy snow
  '61113d':65, //Sleet
  '61113n':66, //Sleet
  '61213d':67, //Light shower sleet
  '61213n':68, //Light shower sleet
  '61313d':69, //Shower sleet
  '61313n':70, //Shower sleet
  '61513d':71, //Light rain and snow
  '61513n':72, //Light rain and snow
  '61613d':73, //Rain and snow
  '61613n':74, //Rain and snow
  '62013d':75, //Light shower snow
  '62013n':76, //Light shower snow
  '62113d':77, //Shower snow
  '62113n':78, //Shower snow
  '62213d':79, //Heavy shower snow
  '62213n':80, //Heavy shower snow
  '70150d':81, //mist
  '70150n':82, //mist
  '71150d':83, //Smoke
  '71150n':84, //Smoke
  '72150d':85, //Haze
  '72150n':86, //Haze
  '73150d':87, //sand/ dust whirls
  '73150n':88, //sand/ dust whirls
  '74150d':89, //fog
  '74150n':90, //fog
  '75150d':91, //sand
  '75150n':92, //sand
  '76150d':93, //dust
  '76150n':94, //dust
  '76250d':95, //volcanic ash
  '76250n':96, //volcanic ash
  '77150d':97, //squalls
  '77150n':98, //squalls
  '78150d':99, //tornado
  '78150n':100, //tornado
  '80001d':101, //clear sky
  '80001n':102, //clear sky
  '80102d':103, //few clouds: 11-25%
  '80102n':104, //few clouds: 11-25%
  '80203d':105, //scattered clouds: 25-50%
  '80203n':106, //scattered clouds: 25-50%
  '80304d':107, //broken clouds: 51-84%
  '80304n':108, //broken clouds: 51-84%
  '80404d':109, //overcast clouds: 85-100%
  '80404n':110, //overcast clouds: 85-100%
  '900':111, //tornado
  '901':112, //storm-showers
  '902':113, //hurricane
  '903':114, //snowflake-cold
  '904':115, //hot
  '905':116, //windy
  '906':117, //hail
  '957':118, //strong-wind
};

function safeIconId(key) {
  var result = owm_iconToId[String(key)];
  return clampWeather(result !== undefined ? result : 0);
}

var ds_iconToId = {
  //daytime
    '0,1': 101, //0 = clear sky ok
    '1,1': 103, //1 = Mainly Clear ok
    '2,1': 105, //2 = partly cloudy ok
    '3,1': 110, //3 = Overcast ok
    '55,1': 34, //55 = Drizzle dense
    '57,1': 34, //57 = Freezing drizzle dense
    '61,1': 34, //61 = Slight Rain
    '80,1': 34, //80 = Slight Rain showers
    '63,1': 46, //63 = Moderate Rain
    '81,1': 46, //81 = Moderate Rain showers
    '73,1': 114, //73 = Moderate Snow
    '75,1': 63, //75 = Heavy Snow
    '86,1': 79, //86 = Heavy Snow showers
    '95,1': 17, //95 = Slight or moderate thunderstorm
    '45,1': 90, //45 = Fog
    '48,1': 90, //48 = Depositing rime fog (freezing fog)
    '51,1': 37, //51 = Drizzle light
    '53,1': 24, //53 = Drizzle moderate
    '56,1': 24, //56 = Freezing drizzle light
    '65,1': 46, //65 = Heavy Rain
    '82,1': 58, //82 = Violent Rain showers
    '66,1': 67, //66 = Light Freezing rain (Sleet)
    '67,1': 66, //67 = Heavy Freezing rain   (sleet)
    '71,1': 77, //71 = Slight Snow
    '77,1': 117, //77 = Snow grains (hail?)
    '85,1': 77, //85 = Slight Snow showers
    '96,1': 20, //96 = Thunderstorm with slight hail
    '99,1': 20, //99 = Thunderstorn with heavy hail
    //night
    '0,0': 102, //0 = clear sky ok
    '1,0': 104, //1 = Mainly Clear ok
    '2,0': 106, //2 = partly cloudy ok
    '3,0': 110, //3 = Overcast ok
    '55,0': 34, //55 = Drizzle dense
    '57,0': 34, //57 = Freezing drizzle dense
    '61,0': 40, //61 = Slight Rain
    '80,0': 40, //80 = Slight Rain showers
    '63,0': 46, //63 = Moderate Rain
    '81,0': 46, //81 = Moderate Rain showers
    '73,0': 114, //73 = Moderate Snow
    '75,0': 63, //75 = Heavy Snow
    '86,0': 79, //86 = Heavy Snow showers
    '95,0': 2, //95 = Slight or moderate thunderstorm
    '45,0': 90, //45 = Fog
    '48,0': 90, //48 = Depositing rime fog (freezing fog)
    '51,0': 38, //51 = Drizzle light
    '53,0': 24, //53 = Drizzle moderate
    '56,0': 24, //56 = Freezing drizzle light
    '65,0': 46, //65 = Heavy Rain
    '82,0': 58, //82 = Violent Rain showers
    '66,0': 68, //66 = Light Freezing rain (Sleet)
    '67,0': 66, //67 = Heavy Freezing rain   (sleet)
    '71,0': 78, //71 = Slight Snow
    '77,0': 117, //77 = Snow grains (hail?)
    '85,0': 78, //85 = Slight Snow showers
    '96,0': 20, //96 = Thunderstorm with slight hail
    '99,0': 20, //99 = Thunderstorn with heavy hail
};

function safeDsIconId(key) {
  var result = ds_iconToId[String(key)];
  return clampWeather(result !== undefined ? result : 0);
}

//var Clay = require('pebble-clay');
var Clay = require('@rebble/clay');
var clayConfig = require('./config');
var clay = new Clay(clayConfig);

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

// ─── Shared helpers ──────────────────────────────────────────────────────────

// Compute suncalc / moon data for a given lat/lon and return a plain object
// with all the fields that every weather provider needs to include.
function computeSuncalc(lat, lon) {
  var d = new Date();
  var sunTimes = SunCalc.getTimes(d, lat, lon);

  var sunsetStrhr  = ('0' + sunTimes.sunset.getHours()).substr(-2);
  var sunsetStrmin = ('0' + sunTimes.sunset.getMinutes()).substr(-2);
  var sunriseStrhr  = ('0' + sunTimes.sunrise.getHours()).substr(-2);
  var sunriseStrmin = ('0' + sunTimes.sunrise.getMinutes()).substr(-2);

  var sunsetStr  = sunsetStrhr  + ':' + sunsetStrmin;
  var sunriseStr = sunriseStrhr + ':' + sunriseStrmin;

  var sunsetHr12  = parseInt(sunTimes.sunset.getHours());
  var sunriseHr12 = parseInt(sunTimes.sunrise.getHours());
  var sunsetStr12h  = (sunsetHr12  > 12 ? sunsetHr12  - 12 : sunsetHr12)  + ':' + sunsetStrmin;
  var sunriseStr12h = (sunriseHr12 > 12 ? sunriseHr12 - 12 : sunriseHr12) + ':' + sunriseStrmin;

  var moonmetrics = SunCalc.getMoonIllumination(d);
  var moonphase   = clampMoon(Math.round(moonmetrics.phase * 28));

  return {
    sunsetStr:    sunsetStr,
    sunriseStr:   sunriseStr,
    sunsetStr12h: sunsetStr12h,
    sunriseStr12h: sunriseStr12h,
    moonphase:    moonphase
  };
}

// Build the 13 rain-minute values from an OWM minutely array (or fall back to
// hourly when minutely is absent).  Returns an object with rain0..rain55,
// rain_next_60 and rain_chance_next_hour.
function computeRainOWM(json, rainunits, rain_chance_next_hour) {
  var minutely = json.minutely;
  var rain0, rain5, rain10, rain15, rain20, rain25, rain30,
      rain35, rain40, rain45, rain50, rain55, rain60;

  if (!minutely) {
    // No minutely data – use hourly[0] and hourly[1] as flat values
    var r0raw = json.hourly[0].rain ? json.hourly[0].rain['1h'] : null;
    var r30raw = json.hourly[1].rain ? json.hourly[1].rain['1h'] : null;
    rain0  = (r0raw  !== null && r0raw  !== undefined) ? Math.round(r0raw  * 100) : 0;
    rain30 = (r30raw !== null && r30raw !== undefined) ? Math.round(r30raw * 100) : 0;
    rain5  = rain10 = rain15 = rain20 = rain25 = rain0;
    rain35 = rain40 = rain45 = rain50 = rain55 = rain60 = rain30;
  } else {
    rain0  = Math.round(minutely[0].precipitation  * 100);
    rain5  = Math.round(minutely[5].precipitation  * 100);
    rain10 = Math.round(minutely[10].precipitation * 100);
    rain15 = Math.round(minutely[15].precipitation * 100);
    rain20 = Math.round(minutely[20].precipitation * 100);
    rain25 = Math.round(minutely[25].precipitation * 100);
    rain30 = Math.round(minutely[30].precipitation * 100);
    rain35 = Math.round(minutely[35].precipitation * 100);
    rain40 = Math.round(minutely[40].precipitation * 100);
    rain45 = Math.round(minutely[45].precipitation * 100);
    rain50 = Math.round(minutely[50].precipitation * 100);
    rain55 = Math.round(minutely[55].precipitation * 100);
    rain60 = Math.round(minutely[59].precipitation * 100);
  }

  var sum13   = rain0+rain5+rain10+rain15+rain20+rain25+rain30+rain35+rain40+rain45+rain50+rain55+rain60;
  var rainn60mm = Math.round(sum13 / 13 / 100 * 10) / 10;
  var rainn60in = Math.round(sum13 / 13 / 100 * 10 / 25.4) / 10;
  var rain_next_60 = String(raintouse(rainunits, rainn60mm, rainn60in)) + '|' + rain_chance_next_hour;

  return {
    rain0: rain0, rain5: rain5, rain10: rain10, rain15: rain15,
    rain20: rain20, rain25: rain25, rain30: rain30, rain35: rain35,
    rain40: rain40, rain45: rain45, rain50: rain50, rain55: rain55,
    rain60: rain60, rain_next_60: rain_next_60
  };
}

// Same for Open-Meteo (minutely_15 or hourly fallback).
function computeRainDS(json, rainunits, rain_chance_next_hour) {
  var minutely = json.minutely_15;
  var rain0, rain5, rain10, rain15, rain20, rain25, rain30,
      rain35, rain40, rain45, rain50, rain55, rain60;

  if (!minutely) {
    var r0raw  = json.hourly.precipitation[0];
    var r30raw = json.hourly.precipitation[1];
    rain0  = (r0raw  !== null && r0raw  !== undefined) ? Math.round(r0raw  * 100) : 0;
    rain30 = (r30raw !== null && r30raw !== undefined) ? Math.round(r30raw * 100) : 0;
    rain5  = rain10 = rain15 = rain20 = rain25 = rain0;
    rain35 = rain40 = rain45 = rain50 = rain55 = rain60 = rain30;
  } else {
    // minutely_15 gives one value per 15 minutes; interpolate 5-min steps
    var m0  = Math.round(minutely.precipitation[0] * 100);
    var m15 = Math.round(minutely.precipitation[1] * 100);
    var m30 = Math.round(minutely.precipitation[2] * 100);
    var m45 = Math.round(minutely.precipitation[3] * 100);
    var m60 = Math.round(minutely.precipitation[4] * 100);
    rain0  = m0;
    rain5  = Math.round((m0  + m0  + m15) / 3);
    rain10 = Math.round((m0  + m15 + m15) / 3);
    rain15 = m15;
    rain20 = Math.round((m15 + m15 + m30) / 3);
    rain25 = Math.round((m15 + m30 + m30) / 3);
    rain30 = m30;
    rain35 = Math.round((m30 + m30 + m45) / 3);
    rain40 = Math.round((m30 + m45 + m45) / 3);
    rain45 = m45;
    rain50 = Math.round((m45 + m45 + m60) / 3);
    rain55 = Math.round((m45 + m60 + m60) / 3);
    rain60 = m60;
  }

  var sum5    = rain0 + rain15 + rain30 + rain45 + rain60;
  var rainn60mm = Math.round(sum5 / 5 / 100 * 10) / 10;
  var rainn60in = Math.round(sum5 / 5 / 100 * 10 / 25.4) / 10;
  var rain_next_60 = String(raintouse(rainunits, rainn60mm, rainn60in)) + '|' + rain_chance_next_hour;

  return {
    rain0: rain0, rain5: rain5, rain10: rain10, rain15: rain15,
    rain20: rain20, rain25: rain25, rain30: rain30, rain35: rain35,
    rain40: rain40, rain45: rain45, rain50: rain50, rain55: rain55,
    rain60: rain60, rain_next_60: rain_next_60
  };
}

// Format a timestamp (JS Date) as "HH:MM" and "H:MM" (12h) strings.
function formatTime(date) {
  var hr  = date.getHours();
  var min = ('0' + date.getMinutes()).substr(-2);
  var hr24 = ('0' + hr).substr(-2);
  var hr12 = hr > 12 ? hr - 12 : hr;
  return {
    str24: hr24 + ':' + min,
    str12: hr12 + ':' + min
  };
}

// ─── Error handler ───────────────────────────────────────────────────────────

function locationError(err) {
  console.log("Error requesting geolocation!");
  isFetching = false;
  Pebble.sendAppMessage(
    { "NameLocation": "" },
    function(e) { console.log("locationError sent to Pebble."); },
    function(e) { console.log("locationError send failed."); }
  );
}

// ─── Single combined fetch function ──────────────────────────────────────────

function fetchWeather(lat, lon) {
  var cfg         = JSON.parse(localStorage.getItem('clay-settings')) || {};
  var weatherprov = cfg.WeatherProv;
  var units       = unitsToString(cfg.WeatherUnit);
  var windunits   = windunitsToString(cfg.WindUnit);
  var rainunits   = rainunitsToString(cfg.RainUnit);
  var langtouse   = translate(navigator.language);

  // 1. Suncalc – computed locally, no XHR needed
  var sc = computeSuncalc(lat, lon);
  console.log("suncalc: sunset=" + sc.sunsetStr + " sunrise=" + sc.sunriseStr + " moon=" + sc.moonphase);

  // 2. PWS (Weather Underground) – XHR runs in parallel but we hold the result
  //    and only send it AFTER the main weather message is acknowledged, to avoid
  //    two AppMessages arriving simultaneously and crashing the watch.

  // var pendingPWSDict = null;
  // var pwsID = cfg.PWSStationID_User;
  // var hasPWS = (cfg.UsePWS !== undefined && cfg.UsePWS !== null) &&
  //              (pwsID !== undefined && pwsID !== null && pwsID !== '');
  // if (hasPWS) {
  //   var pressureunits = pressureunitsToString(cfg.PressureUnit);
  //   var keyAPIwu  = localStorage.getItem('wuKey');
  //   var endkeypws = apikeytouse(cfg.PWSAPIKEY_User, keyAPIwu);
  //   var urlPWS = "https://api.weather.com/v2/pws/observations/current?stationId=" +
  //                pwsID + '&format=json&units=s&apiKey=' + endkeypws;
  //   console.log("PWSUrl= " + urlPWS);
  //   xhrRequest(encodeURI(urlPWS), 'GET', function(responseText) {
  //     var json = JSON.parse(responseText);
  //     var obs  = json.observations[0];
  //     var tempf = Math.round((obs.metric_si.temp * 1.8) + 32);
  //     var tempc = Math.round(obs.metric_si.temp);
  //     var temppws = String(temptousewu(units, tempf, tempc)) + '\xB0';

  //     var windkts = Math.round(obs.metric_si.windSpeed * 1.9438444924574);
  //     var windkph = Math.round(obs.metric_si.windSpeed * 3.6);
  //     var windms  = Math.round(obs.metric_si.windSpeed);
  //     var windmph = Math.round(obs.metric_si.windSpeed * 2.2369362920544);
  //     var windpws      = String(windtousewu(windunits, windkph, windmph, windms, windkts)) + windunits;
  //     var windroundpws = String(windtousewu(windunits, windkph, windmph, windms, windkts));
  //     var winddir_numpws = safeWindId(String(obs.winddir));

  //     var precip_today_mm = Math.round(obs.metric_si.precipTotal * 10) / 10;
  //     var precip_today_in = Math.round(obs.metric_si.precipTotal / 25.4 * 10) / 10;
  //     var precip_today_pws = String(raintouse(rainunits, precip_today_mm, precip_today_in));
  //     var precip_rate_mm  = Math.round(obs.metric_si.precipRate * 10) / 10;
  //     var precip_rate_in  = Math.round(obs.metric_si.precipRate / 25.4 * 10) / 10;
  //     var precip_rate_pws = String(raintouse(rainunits, precip_rate_mm, precip_rate_in));

  //     var pressuremb  = String(Math.round(obs.metric_si.pressure));
  //     var pressurehg  = String(Math.round(obs.metric_si.pressure / 33.8639 * 10) / 10);
  //     var pressuretor = String(Math.round(obs.metric_si.pressure / 1.333));
  //     var pressureap  = String(Math.round(obs.metric_si.pressure * 100));
  //     var pressureatm = String(Math.round(obs.metric_si.pressure / 1013 * 1000) / 1000);
  //     var pressurepws = String(pressuretouse(pressureunits, pressuremb, pressurehg, pressuretor, pressureap, pressureatm));

  //     console.log("PWS data ready: temp=" + temppws);
  //     console.log("PWS wind=" + windpws);
  //     console.log("PWS pressure=" + pressurepws);
  //     // Store for sending after the main message is confirmed
  //     pendingPWSDict = {
  //       "WeatherTempPWS":      temppws,
  //       "WeatherWindPWS":      windpws,
  //       "WeatherWindRoundPWS": windroundpws,
  //       "WindIconNowPWS":      winddir_numpws,
  //       "RainTotalTodayPWS":   precip_today_pws,
  //       "RainRatePWS":         precip_rate_pws,
  //       "PressurePWS":         pressurepws
  //     };
  //   });
  // }

  // 3. Main weather provider
  if (weatherprov === 'ds') {
    // ── Open-Meteo ──────────────────────────────────────────────────────────
    var urlds = "https://api.open-meteo.com/v1/forecast?" +
      "latitude=" + lat + "&longitude=" + lon +
      "&models=ukmo_seamless" +
      "&minutely_15=precipitation" +
      "&hourly=uv_index,precipitation,precipitation_probability" +
      "&daily=weather_code,temperature_2m_max,temperature_2m_min,sunrise,sunset," +
             "uv_index_max,wind_direction_10m_dominant,wind_speed_10m_mean," +
             "precipitation_sum,precipitation_hours,precipitation_probability_mean" +
      "&current=temperature_2m,precipitation,uv_index,weather_code," +
               "wind_speed_10m,wind_direction_10m,is_day" +
      "&forecast_hours=24&forecast_days=1&forecast_minutely_15=6" +
      "&timeformat=unixtime&wind_speed_unit=ms";

    console.log("DSUrl= " + urlds);
    xhrRequest(encodeURI(urlds), 'GET', function(responseText) {
      var json = JSON.parse(responseText);

      // Temperature
      var tempf = Math.round((json.current.temperature_2m * 9/5) + 32);
      var tempc = Math.round(json.current.temperature_2m);
      var tempds = String(temptousewu(units, tempf, tempc)) + '\xB0';
      var cityds = String((Math.round(lat*100))/100 + ',' + (Math.round(lon*100))/100);

      // Current weather icon
      var icon_ds = safeDsIconId(String(json.current.weather_code) + ',' + String(json.current.is_day));

      // Sunrise / sunset from API (for HourSunrise/HourSunset integer fields)
      var sunriseds = new Date(json.daily.sunrise[0] * 1000);
      var sunsetds  = new Date(json.daily.sunset[0]  * 1000);
      var sunriseds_int = sunriseds.getHours() * 100 + sunriseds.getMinutes();
      var sunsetds_int  = sunsetds.getHours()  * 100 + sunsetds.getMinutes();

      // Wind
      var windkts = Math.round(json.current.wind_speed_10m * 1.9438444924574);
      var windkph = Math.round(json.current.wind_speed_10m * 3.6);
      var windms  = Math.round(json.current.wind_speed_10m);
      var windmph = Math.round(json.current.wind_speed_10m * 2.2369362920544);
      var wind      = String(windtousewu(windunits, windkph, windmph, windms, windkts)) + windunits;
      var windround = String(windtousewu(windunits, windkph, windmph, windms, windkts));
      var winddir_num = safeWindId(String(json.current.wind_direction_10m));

      // Forecast
      var forecast_icon_ds = safeDsIconId(String(json.daily.weather_code[0]) + ',1');
      var fhighf = Math.round((json.daily.temperature_2m_max[0] * 9/5) + 32);
      var flowf  = Math.round((json.daily.temperature_2m_min[0] * 9/5) + 32);
      var fhighc = Math.round(json.daily.temperature_2m_max[0]);
      var flowc  = Math.round(json.daily.temperature_2m_min[0]);
      var highds    = String(temptousewu(units, fhighf, fhighc));
      var lowds     = String(temptousewu(units, flowf,  flowc));
      var highlowds = highds + '|' + lowds + '\xB0';

      var fwindkts = Math.round(json.daily.wind_speed_10m_mean[0] * 1.9438444924574);
      var fwindkph = Math.round(json.daily.wind_speed_10m_mean[0] * 3.6);
      var fwindms  = Math.round(json.daily.wind_speed_10m_mean[0]);
      var fwindmph = Math.round(json.daily.wind_speed_10m_mean[0] * 2.2369362920544);
      var forecast_ave_wind_ds = String(windtousewu(windunits, fwindkph, fwindmph, fwindms, fwindkts)) + windunits;
      var forecast_wind_dir_num = safeWindId(String(json.daily.wind_direction_10m_dominant[0]));

      // Timestamp
      var auxtimeds = new Date(json.current.time * 1000);
      var dstime    = auxtimeds.getHours() * 100 + auxtimeds.getMinutes();
      var tds       = formatTime(auxtimeds);

      // UV
      var uv_index_max_ds      = Math.min(Math.round(json.daily.uv_index_max[0]), 10);
      var uv_index_day_ds      = Math.round(json.daily.uv_index_max[0]);
      var uv_index_next_hour_ds = Math.min(Math.round(json.hourly.uv_index[0]), 10);

      // Rain
      var rain_chance_next_hour = Math.round(json.hourly.precipitation_probability[0]) + '\x25';
      var rain = computeRainDS(json, rainunits, rain_chance_next_hour);

      console.log("DS temp=" + tempds);
      console.log("DS icon=" + icon_ds);
      console.log("DS wind=" + wind);

      Pebble.sendAppMessage(
        {
          "WeatherTemp":        tempds,
          "WeatherCond":        json.current.weather_code,
          "HourSunset":         sunsetds_int,
          "HourSunrise":        sunriseds_int,
          "WeatherWind":        wind,
          "WeatherWindRound":   windround,
          "WEATHER_SUNSET_KEY":     sc.sunsetStr,
          "WEATHER_SUNRISE_KEY":    sc.sunriseStr,
          "WEATHER_SUNSET_KEY_12H": sc.sunsetStr12h,
          "WEATHER_SUNRISE_KEY_12H": sc.sunriseStr12h,
          "IconNow":            icon_ds,
          "IconFore":           forecast_icon_ds,
          "TempFore":           highlowds,
          "TempForeLow":        lowds,
          "WindFore":           forecast_ave_wind_ds,
          "WindIconNow":        winddir_num,
          "WindIconAve":        forecast_wind_dir_num,
          "Weathertime":        dstime,
          "MoonPhase":          sc.moonphase,
          "UVIndexMax":         uv_index_max_ds,
          "UVIndexNow":         uv_index_next_hour_ds,
          "UVIndexDay":         uv_index_day_ds,
          "NameLocation":       cityds,
          "pop1h":              rain_chance_next_hour,
          "rain1h":             rain.rain_next_60,
          "raintime24h":        tds.str24,
          "raintime12h":        tds.str12,
          "rain0":  rain.rain0,  "rain5":  rain.rain5,  "rain10": rain.rain10,
          "rain15": rain.rain15, "rain20": rain.rain20, "rain25": rain.rain25,
          "rain30": rain.rain30, "rain35": rain.rain35, "rain40": rain.rain40,
          "rain45": rain.rain45, "rain50": rain.rain50, "rain55": rain.rain55,
          "rain60": rain.rain60
        },
        function(e) {
          console.log("Open-Meteo sent to Pebble.");
          // if (pendingPWSDict) {
          //   Pebble.sendAppMessage(
          //     pendingPWSDict,
          //     function(e) { console.log("PWS sent to Pebble."); isFetching = false; },
          //     function(e) { console.log("PWS send failed."); isFetching = false; }
          //   );
          //   pendingPWSDict = null;
          // } else {
          //   isFetching = false;
          // }
        },
        function(e) { console.log("Open-Meteo send failed."); isFetching = false; }
      );
    });

  } else if (weatherprov === 'owm') {
    // ── OpenWeatherMap ───────────────────────────────────────────────────────
    var keyAPIowm = localStorage.getItem('owmKey');
    var endkeyowm = apikeytouse(cfg.APIKEY_User, keyAPIowm);
    var urlOWM = "https://api.openweathermap.org/data/3.0/onecall?lat=" +
      lat + "&lon=" + lon +
      '&appid=' + endkeyowm + "&exclude=alerts" +
      '&lang=' + langtouse;

    console.log("OWMUrl= " + urlOWM);
    xhrRequest(encodeURI(urlOWM), 'GET', function(responseText) {
      var json = JSON.parse(responseText);

      // Temperature
      var tempf = Math.round((json.current.temp * 1.8) - 459.67);
      var tempc = Math.round(json.current.temp - 273.15);
      var tempowm  = String(temptousewu(units, tempf, tempc)) + '\xB0';
      var cityowm  = String(json.timezone);
      var condclean = replaceDiacritics(json.current.weather[0].main);

      // Current icon
      var id_owm   = parseInt(json.current.weather[0].id);
      var icon_owm = String(json.current.weather[0].icon);
      var icon2_owm = (id_owm > 899)
        ? safeIconId(String(id_owm))
        : safeIconId(String(id_owm) + icon_owm);

      // Sunrise / sunset
      var sunriseowm = new Date(json.current.sunrise * 1000);
      var sunsetowm  = new Date(json.current.sunset  * 1000);
      var sunriseowm_int = sunriseowm.getHours() * 100 + sunriseowm.getMinutes();
      var sunsetowm_int  = sunsetowm.getHours()  * 100 + sunsetowm.getMinutes();

      // Wind
      var windkts = Math.round(json.current.wind_speed * 1.9438444924574);
      var windkph = Math.round(json.current.wind_speed * 3.6);
      var windms  = Math.round(json.current.wind_speed);
      var windmph = Math.round(json.current.wind_speed * 2.2369362920544);
      var wind      = String(windtousewu(windunits, windkph, windmph, windms, windkts)) + windunits;
      var windround = String(windtousewu(windunits, windkph, windmph, windms, windkts));
      var winddir_num = safeWindId(String(json.current.wind_deg));

      // Forecast
      var forecast_id_owm   = parseInt(json.daily[0].weather[0].id);
      var forecast_icon_owm = String(json.daily[0].weather[0].icon);
      var forecast_icon2_owm = (forecast_id_owm > 899)
        ? safeIconId(String(forecast_id_owm))
        : safeIconId(String(forecast_id_owm) + forecast_icon_owm);

      var fhighf = Math.round((json.daily[0].temp.max * 1.8) - 459.67);
      var flowf  = Math.round((json.daily[0].temp.min * 1.8) - 459.67);
      var fhighc = Math.round(json.daily[0].temp.max - 273.15);
      var flowc  = Math.round(json.daily[0].temp.min - 273.15);
      var highowm    = String(temptousewu(units, fhighf, fhighc));
      var lowowm     = String(temptousewu(units, flowf,  flowc));
      var highlowowm = highowm + '|' + lowowm + '\xB0';

      var fwindkts = Math.round(json.daily[0].wind_speed * 1.9438444924574);
      var fwindkph = Math.round(json.daily[0].wind_speed * 3.6);
      var fwindms  = Math.round(json.daily[0].wind_speed);
      var fwindmph = Math.round(json.daily[0].wind_speed * 2.2369362920544);
      var forecast_ave_wind_owm       = String(windtousewu(windunits, fwindkph, fwindmph, fwindms, fwindkts)) + windunits;
      var forecast_ave_wind_owm_round = String(windtousewu(windunits, fwindkph, fwindmph, fwindms, fwindkts));
      var forecast_wind_dir_num = safeWindId(String(json.daily[0].wind_deg));

      // Timestamp
      var auxtimeowm = new Date(json.current.dt * 1000);
      var owmtime    = auxtimeowm.getHours() * 100 + auxtimeowm.getMinutes();
      var towm       = formatTime(auxtimeowm);

      // UV
      var uv_index_max_owm      = Math.min(Math.round(json.daily[0].uvi), 10);
      var uv_index_day_owm      = Math.round(json.daily[0].uvi);
      var uv_index_next_hour_owm = Math.min(Math.round(json.current.uvi), 10);

      // Next-hour conditions and rain chance
      var rain_chance_next_hour = String(Math.round(json.hourly[1].pop * 100)) + '\x25';
      var icon_next_hour = safeIconId(json.hourly[1].weather[0].icon);

      // Rain
      var rain = computeRainOWM(json, rainunits, rain_chance_next_hour);

      console.log("OWM temp=" + tempowm);
      console.log("OWM icon=" + icon2_owm);
      console.log("OWM wind=" + wind);

      Pebble.sendAppMessage(
        {
          "WeatherTemp":        tempowm,
          "WeatherCond":        condclean,
          "HourSunset":         sunsetowm_int,
          "HourSunrise":        sunriseowm_int,
          "WeatherWind":        wind,
          "WeatherWindRound":   windround,
          "WEATHER_SUNSET_KEY":      sc.sunsetStr,
          "WEATHER_SUNRISE_KEY":     sc.sunriseStr,
          "WEATHER_SUNSET_KEY_12H":  sc.sunsetStr12h,
          "WEATHER_SUNRISE_KEY_12H": sc.sunriseStr12h,
          "IconNow":            icon2_owm,
          "IconFore":           forecast_icon2_owm,
          "TempFore":           highlowowm,
          "TempForeLow":        lowowm,
          "WindFore":           forecast_ave_wind_owm,
          "WindForeRound":      forecast_ave_wind_owm_round,
          "WindIconNow":        winddir_num,
          "WindIconAve":        forecast_wind_dir_num,
          "Weathertime":        owmtime,
          "MoonPhase":          sc.moonphase,
          "Cond1h":             icon_next_hour,
          "pop1h":              rain_chance_next_hour,
          "rain1h":             rain.rain_next_60,
          "raintime24h":        towm.str24,
          "raintime12h":        towm.str12,
          "NameLocation":       cityowm,
          "UVIndexMax":         uv_index_max_owm,
          "UVIndexDay":         uv_index_day_owm,
          "UVIndexNow":         uv_index_next_hour_owm,
          "rain0":  rain.rain0,  "rain5":  rain.rain5,  "rain10": rain.rain10,
          "rain15": rain.rain15, "rain20": rain.rain20, "rain25": rain.rain25,
          "rain30": rain.rain30, "rain35": rain.rain35, "rain40": rain.rain40,
          "rain45": rain.rain45, "rain50": rain.rain50, "rain55": rain.rain55,
          "rain60": rain.rain60
        },
        function(e) {
          console.log("OWM sent to Pebble.");
          // if (pendingPWSDict) {
          //   Pebble.sendAppMessage(
          //     pendingPWSDict,
          //     function(e) { console.log("PWS sent to Pebble."); isFetching = false; },
          //     function(e) { console.log("PWS send failed."); isFetching = false; }
          //   );
          //   pendingPWSDict = null;
          // } else {
          //   isFetching = false;
          // }
        },
        function(e) { console.log("OWM send failed."); isFetching = false; }
      );
    });

  } else {
    // ── No weather provider – send suncalc only ──────────────────────────────
    console.log("No weather provider, sending suncalc only.");
    Pebble.sendAppMessage(
      {
        "WEATHER_SUNSET_KEY":      sc.sunsetStr,
        "WEATHER_SUNRISE_KEY":     sc.sunriseStr,
        "WEATHER_SUNSET_KEY_12H":  sc.sunsetStr12h,
        "WEATHER_SUNRISE_KEY_12H": sc.sunriseStr12h,
        "MoonPhase":               sc.moonphase
      },
      function(e) { console.log("Suncalc-only sent to Pebble."); isFetching = false; },
      function(e) { console.log("Suncalc-only send failed."); isFetching = false; }
    );
  }
}

// ─── Entry point ─────────────────────────────────────────────────────────────

// Guard flag: prevents concurrent fetchWeather() calls.
// Problem: 'ready' + multiple 'appmessage' events fire almost simultaneously at startup,
// AND getCurrentPosition with enableHighAccuracy fires its callback multiple times.
// Each concurrent call fires independent XHRs and all try to sendAppMessage to the watch
// simultaneously, overflowing the AppMessage queue and crashing the watch.
var isFetching = false;

function fetchWeatherOnce(lat, lon) {
  if (isFetching) {
    console.log("fetchWeather: already in progress, dropping duplicate.");
    return;
  }
  isFetching = true;
  console.log("fetchWeather: starting (" + lat + "," + lon + ")");
  fetchWeather(lat, lon);
  // Release the guard after a safe window. 30s covers all XHR timeouts.
  // This ensures a genuine retry is possible if the fetch fails silently.
  setTimeout(function() {
    isFetching = false;
    console.log("fetchWeather: guard released.");
  }, 30000);
}

function getinfo() {
  var cfg        = JSON.parse(localStorage.getItem('clay-settings')) || {};
  var manuallat  = cfg.Lat;
  var manuallong = cfg.Long;

  if (manuallat !== null && manuallat !== undefined && manuallat !== '' &&
      manuallong !== null && manuallong !== undefined && manuallong !== '') {
    // Manual coordinates – no geolocation needed
    console.log("Using manual lat/lon: " + manuallat + ", " + manuallong);
    fetchWeatherOnce(parseFloat(manuallat), parseFloat(manuallong));
  } else {
    // Request device location.
    // enableHighAccuracy can cause the callback to fire multiple times as GPS
    // accuracy improves - fetchWeatherOnce() drops all but the first call.
    navigator.geolocation.getCurrentPosition(
      function(pos) {
        fetchWeatherOnce(pos.coords.latitude, pos.coords.longitude);
      },
      locationError,
      { enableHighAccuracy: true, timeout: 15000, maximumAge: 60000 }
    );
  }
}

// ─── Event listeners ─────────────────────────────────────────────────────────

Pebble.addEventListener('ready', function(e) {
  console.log("Starting Watchface!");
  localStorage.setItem("OKAPI", 0);
  //getinfo();
});

Pebble.addEventListener('appmessage', function(e) {
  console.log("Requesting weather update!");
  getinfo();
});

Pebble.addEventListener('webviewclosed', function(e) {
  console.log("Config closed, updating!");
  // Config changed - force a fresh fetch even if guard is set.
  isFetching = false;
  getinfo();
});



function unitsToString(unit) {
  if (unit) {
    return 'F';
  }
  return 'C';
}

function windunitsToString(windunit){
  if (windunit=='kts') {
    return 'kt';
  }
  else if (windunit=='kph'){
    return 'kph';
  }
  else if (windunit=='ms'){
    return 'ms';
  }
  return 'mph';
  }

function rainunitsToString(rainunit){
  if (rainunit=='mm') {
    return 'mm';
  }
  return 'in';
  }

function pressureunitsToString(pressureunit){
  if (pressureunit=='mb') {
    return 'mb';
  }
  else if (pressureunit=='hg'){
    return 'hg';
  }
  else if (pressureunit=='tor'){
    return 'tor';
  }
  else if (pressureunit=='ap'){
    return 'ap';
  }
  return 'atm';
  }


function translate(langloc){
  if (langloc==='es-ES'){
    return 'es';
  }
  else if (langloc==='fr_FR'){
    return 'fr';
  }
  else if (langloc==='de_DE'){
    return 'de';
  }
  else if (langloc==='it_IT'){
    return 'it';
  }
  else if (langloc==='pt_PT'){
    return 'pt';
  }
  else {
    return 'en';
  }
}
function translatewu(langloc){
  if (langloc==='es-ES'){
    return 'SP';
  }
  else if (langloc==='fr_FR'){
    return 'FR';
  }
  else if (langloc==='de_DE'){
    return 'DL';
  }
  else if (langloc==='it_IT'){
    return 'IT';
  }
  else if (langloc==='pt_PT'){
    return 'BR';
  }
  else {
    return 'EN';
  }
}
function temptousewu(unit,tempf,tempc){
  if (unit=="F"){
    return tempf; }
  else return tempc;
}
function windtousewu(windunit,windkph,windmph,windms,windkts){
  if (windunit=="kph"){
    return windkph; }
  else if (windunit=="mph")
    {return windmph; }
  else if (windunit=="ms")
    {return windms; }
  else return windkts;
}

function raintouse(rainunit,rainmm,rainin){
  if (rainunit=="mm"){
    return rainmm; }
  else return rainin;
}

function pressuretouse(pressureunit,pressuremb,pressurehg,pressuretor,pressureap,pressureatm){
  if (pressureunit=="mb"){
    return pressuremb; }
  else if (pressureunit=="hg")
    {return pressurehg; }
  else if (pressureunit=="tor")
    {return pressuretor; }
  else if (pressureunit=="ap")
    {return pressureap; }
  else return pressureatm;
}

function replaceDiacritics(s){
    var diacritics =[
        /[\300-\306]/g, /[\340-\346]/g,  // A, a
        /[\310-\313]/g, /[\350-\353]/g,  // E, e
        /[\314-\317]/g, /[\354-\357]/g,  // I, i
        /[\322-\330]/g, /[\362-\370]/g,  // O, o
        /[\331-\334]/g, /[\371-\374]/g,  // U, u
        /[\321]/g, /[\361]/g, // N, n
        /[\307]/g, /[\347]/g, // C, c
    ];

    var chars = ['A','a','E','e','I','i','O','o','U','u','N','n','C','c'];

    for (var i = 0; i < diacritics.length; i++)
    {
        s = s.replace(diacritics[i],chars[i]);
    }
  var end=s;
  return end;
}

function apikeytouse(APIUser,APIPMKEY){
  if (APIUser===""){
    console.log("Using pmkey");
    return APIPMKEY;
  }
  else {
    console.log("Using Key User");
    return APIUser;
  }
}