var Clay = require('pebble-clay');
var clayConfig = require('./config.js');
var clay = new Clay(clayConfig);

///changed from dark sky to open-meteo
// TODO check weather code from openmeteo and font /day night
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

// --- Helper Functions ---
var xhrRequest = function (url, type, callback) {
    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
        // Only trigger callback if server responded with a success status (200-299)
        if (this.status >= 200 && this.status < 300) {
            callback(this.responseText);
        } else {
            console.log("Network error: Server responded with status " + this.status);
        }
    };
    xhr.onerror = function () {
        console.log("Network error: Connection failed completely.");
    };
    xhr.open(type, url);
    xhr.send();
};


// --- Main Logic Functions ---

function fetchWeatherData(pos) {
    var lat, lon;
    if (pos) { // GPS
        lat = pos.coords.latitude;
        lon = pos.coords.longitude;
        localStorage.setItem('lat', lat);
        localStorage.setItem('lon', lon);
    } else { // Last known position
        lat = localStorage.getItem('lat'); // Returns null if not set
        lon = localStorage.getItem('lon');
    }

    if (!lat || !lon) return;

    var url = "https://api.open-meteo.com/v1/forecast?latitude=" + lat + "&longitude=" + lon +
        "&daily=precipitation_sum,precipitation_probability_max,temperature_2m_max,temperature_2m_min,weather_code&current=weather_code,precipitation,precipitation_probability,temperature_2m,is_day&timezone=auto&forecast_days=1";

    xhrRequest(url, 'GET', function (responseText) {
        try {
            var json = JSON.parse(responseText);

            // Build the clean Pebble dictionary directly in place
            var pData = {
                "TempNow": Math.round(json.current.temperature_2m),
                "IconNow": ds_iconToId[json.current.weather_code + ',' + json.current.is_day] || 101,
                "TempFore": Math.round(json.daily.temperature_2m_min[0]) + '|' + Math.round(json.daily.temperature_2m_max[0]),
                "IconFore": ds_iconToId[json.daily.weather_code[0] + ',1'] || 101,
                "RainNow": json.current.precipitation !== null ? Math.round(json.current.precipitation * 10) : 0,
                "RainProbNow": json.current.precipitation_probability !== null ? Math.round(json.current.precipitation_probability) : 0,
                "RainSumFore": json.daily.precipitation_sum[0] !== null ? Math.round(json.daily.precipitation_sum[0] * 10) : 0,
                "RainProbMaxFore": json.daily.precipitation_probability_max[0] !== null ? Math.round(json.daily.precipitation_probability_max[0]) : 0
            };

            console.log("Current Rain scaled: " + pData.RainNow + ", Rain daily Sum scaled: " + pData.RainSumFore);

            Pebble.sendAppMessage(pData,
                function () { console.log("Weather sent successfully!"); },
                function () { console.log("Error sending weather info!"); }
            );
        } catch (e) {
            console.log("Error parsing weather response: " + e);
        }
    });
}


function locationError(err) {
    console.warn(`Error requesting geolocation! ERROR(${err.code}): ${err.message}`);
    fetchWeatherData(null);
}

function getWeather() {
    navigator.geolocation.getCurrentPosition(
        fetchWeatherData,
        locationError,
        {
            enableHighAccuracy: false,
            timeout: 15000,
            maximumAge: 60000 // 1 minute (allows a slightly cached location to save battery)
        }
    );
}


// --- Listeners ---
Pebble.addEventListener('ready', 
    function (e) {
    console.log("Starting Watchface!");
    getWeather();
}
);

Pebble.addEventListener('appmessage',
    function (e) {
        console.log("Requesting weather!");
        if (e.payload['REQUEST_WEATHER']) {
            getWeather();
        }
    }
);

Pebble.addEventListener('webviewclosed',
    function (e) {
        console.log("Updating config!");
        getWeather();
    }
);