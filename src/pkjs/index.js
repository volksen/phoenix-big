var Clay = require('pebble-clay');
var clayConfig = require('./config.js');
var clay = new Clay(clayConfig);

///changed from dark sky to open-meteo
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


//--- Helper Functions ---
var xhrRequest = function (url, type, callback) {
    var xhr = new XMLHttpRequest();
    xhr.onload = function () { callback(this.responseText); };
    xhr.open(type, url);
    xhr.send();
};

// --- Main Logic Functions ---

/**
 * Fetches and processes weather data for a given provider.
 */
function fetchWeatherData(pos) {
    var lat, lon;
    if (pos) { // GPS
        lat = pos.coords.latitude;
        lon = pos.coords.longitude;
        localStorage.setItem('lat', lat);
        localStorage.setItem('lon', lon);
    } else { // Last known position
        lat = localStorage.getItem('lat');
        lon = localStorage.getItem('lon');
    }

    if (!lat || !lon) return;

    var url, parseFunc;

    url = "https://api.open-meteo.com/v1/forecast?latitude=" + lat + "&longitude=" + lon +
        "&models=best_match&daily=weather_code,temperature_2m_max,temperature_2m_min,sunrise,sunset,uv_index_max,precipitation_sum,precipitation_hours,precipitation_probability_mean&current=temperature_2m,precipitation,weather_code,is_day&forecast_days=1&timeformat=unixtime";
    parseFunc = function (json) {
        var tempf = Math.round((json.current.temperature_2m * 9 / 5) + 32);
        var tempc = Math.round(json.current.temperature_2m);
        var temp = String(temptousewu(units, tempf, tempc)) + '\xB0';
        var icon_now = ds_iconToId[String(json.current.weather_code) + ',' + String(json.current.is_day)];
        var forecast_icon = ds_iconToId[String(json.daily.weather_code[0]) + ',1'];
        var forecast_high_tempf = Math.round((json.daily.temperature_2m_max[0] * 9 / 5) + 32);
        var forecast_low_tempf = Math.round((json.daily.temperature_2m_min[0] * 9 / 5) + 32);
        var forecast_high_tempc = Math.round(json.daily.temperature_2m_max[0]);
        var forecast_low_tempc = Math.round(json.daily.temperature_2m_min[0]);
        var high = String(temptousewu(units, forecast_high_tempf, forecast_high_tempc));
        var low = String(temptousewu(units, forecast_low_tempf, forecast_low_tempc));
        var highlow = high + '|' + low;
        var aux_time = new Date(json.current.time * 1000);
        var weather_time = aux_time.getHours() * 100 + aux_time.getMinutes();

        console.log(weatherprov);
        console.log(weather_time);
        console.log(temp);
        console.log(icon_now);
        console.log(highlow);
        console.log(forecast_icon);

        return {
            "WeatherTemp": temp,
            "IconNow": icon_now,
            "Weathertime": weather_time,
            "IconFore": forecast_icon,
            "TempFore": highlow
        };
    };


    xhrRequest(encodeURI(url), 'GET', function (responseText) {
        try {
            var json = JSON.parse(responseText);
            var weatherDict = parseFunc(json);
            var finalDictionary = {};
            var dictsToMerge = [astroData.dictionary, weatherDict];
            var i, key, source;
            for (i = 0; i < dictsToMerge.length; i++) {
                source = dictsToMerge[i];
                for (key in source) {
                    // Check to ensure the property belongs to the object (not inherited)
                    if (Object.prototype.hasOwnProperty.call(source, key)) {
                        finalDictionary[key] = source[key];
                    }
                }
            }

            Pebble.sendAppMessage(finalDictionary,
                function () { console.log("Weather sent successfully!"); },
                function () { console.log("Error sending weather info!"); }
            );
        } catch (e) {
            console.log("Error parsing weather response: " + e);
        }
    });

}

function locationError(err) {
    console.log("Error requesting geolocation!");
    Pebble.sendAppMessage({ "NameLocation": "" },
        function () { },
        function () { console.log("Null key error sending to Pebble!"); }
    );
}

function getinfo() {
    navigator.geolocation.getCurrentPosition(
        function (pos) { fetchWeatherData(pos); },
        locationError,
        { enableHighAccuracy: true, timeout: 15000, maximumAge: 1000 }
    );
}

// Listeners
Pebble.addEventListener('ready', function () {
    console.log("Starting Watchface!");
    getinfo();
});

Pebble.addEventListener('appmessage', function () {
    console.log("Requesting geoposition!");
    getinfo();
});
Pebble.addEventListener('webviewclosed', function () {
    console.log("Updating config!");
    getinfo();
});

