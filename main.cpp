#include <curl/curl.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <regex>
#include <sstream>
#include <algorithm>
#include <cctype>

using namespace std;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userData) {
    size_t totalSize = size * nmemb;
    static_cast<string*>(userData)->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

string urlEncode(const string& value) {
    CURL* curl = curl_easy_init();
    if (!curl) return value;
    char* encoded = curl_easy_escape(curl, value.c_str(), static_cast<int>(value.length()));
    string result = encoded ? encoded : value;
    if (encoded) curl_free(encoded);
    curl_easy_cleanup(curl);
    return result;
}

string httpGet(const string& url) {
    CURL* curl = curl_easy_init();
    if (!curl) throw runtime_error("Could not initialize the network connection.");

    string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "BeginnerCppWeatherApp/1.0");

    CURLcode result = curl_easy_perform(curl);
    long statusCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);
    curl_easy_cleanup(curl);

    if (result != CURLE_OK) {
        throw runtime_error(string("Network error: ") + curl_easy_strerror(result));
    }
    if (statusCode < 200 || statusCode >= 300) {
        throw runtime_error("Weather service returned HTTP status " + to_string(statusCode) + ".");
    }
    return response;
}

string jsonString(const string& json, const string& key) {
    regex pattern("\"" + key + R"REGEX(\s*:\s*"([^"]*)")REGEX");
    smatch match;
    if (regex_search(json, match, pattern)) return match[1].str();
    return "";
}

string jsonNumber(const string& json, const string& key) {
    regex pattern("\"" + key + R"("\s*:\s*(-?[0-9]+(?:\.[0-9]+)?))");
    smatch match;
    if (regex_search(json, match, pattern)) return match[1].str();
    return "N/A";
}

string jsonObject(const string& json, const string& key) {
    size_t keyPos = json.find("\"" + key + "\"");
    if (keyPos == string::npos) return "";
    size_t start = json.find('{', keyPos);
    if (start == string::npos) return "";
    int depth = 0;
    bool inString = false, escaped = false;
    for (size_t i = start; i < json.size(); ++i) {
        char c = json[i];
        if (inString) {
            if (escaped) escaped = false;
            else if (c == '\\') escaped = true;
            else if (c == '"') inString = false;
            continue;
        }
        if (c == '"') inString = true;
        else if (c == '{') ++depth;
        else if (c == '}' && --depth == 0) return json.substr(start, i - start + 1);
    }
    return "";
}

vector<string> jsonArrayValues(const string& json, const string& key) {
    vector<string> values;
    size_t keyPos = json.find("\"" + key + "\"");
    if (keyPos == string::npos) return values;
    size_t start = json.find('[', keyPos);
    size_t end = json.find(']', start);
    if (start == string::npos || end == string::npos) return values;
    string inside = json.substr(start + 1, end - start - 1);
    regex item(R"REGEX("([^"]*)"|(-?[0-9]+(\.[0-9]+)?|null))REGEX");
    for (sregex_iterator it(inside.begin(), inside.end(), item), last; it != last; ++it) {
        if ((*it)[1].matched) values.push_back((*it)[1].str());
        else values.push_back((*it)[2].str());
    }
    return values;
}

string weatherDescription(int code) {
    if (code == 0) return "Clear sky";
    if (code == 1) return "Mainly clear";
    if (code == 2) return "Partly cloudy";
    if (code == 3) return "Overcast";
    if (code == 45 || code == 48) return "Fog";
    if (code >= 51 && code <= 57) return "Drizzle";
    if (code >= 61 && code <= 67) return "Rain";
    if (code >= 71 && code <= 77) return "Snow";
    if (code >= 80 && code <= 82) return "Rain showers";
    if (code >= 85 && code <= 86) return "Snow showers";
    if (code == 95) return "Thunderstorm";
    if (code == 96 || code == 99) return "Thunderstorm with hail";
    return "Unknown conditions";
}

void printLine() {
    cout << "------------------------------------------------------------\n";
}

int main() {
    cout << "\n";
    printLine();
    cout << "              C++ WEATHER FORECAST APP\n";
    cout << "          Live data powered by Open-Meteo\n";
    printLine();
    cout << "Enter a city name (example: Karur, Chennai, London): ";

    string city;
    getline(cin, city);
    if (city.empty()) {
        cout << "\nPlease enter a city name and try again.\n";
        return 0;
    }

    try {
        curl_global_init(CURL_GLOBAL_DEFAULT);

        string geoUrl = "https://geocoding-api.open-meteo.com/v1/search?name="
                      + urlEncode(city) + "&count=1&language=en&format=json";
        string geoJson = httpGet(geoUrl);

        if (geoJson.find("\"results\"") == string::npos ||
            geoJson.find("\"latitude\"") == string::npos) {
            cout << "\nNo matching city was found. Check the spelling and try again.\n";
            curl_global_cleanup();
            return 0;
        }

        string placeName = jsonString(geoJson, "name");
        string country = jsonString(geoJson, "country");
        string latitude = jsonNumber(geoJson, "latitude");
        string longitude = jsonNumber(geoJson, "longitude");

        string forecastUrl = "https://api.open-meteo.com/v1/forecast?latitude="
            + latitude + "&longitude=" + longitude
            + "&current=temperature_2m,relative_humidity_2m,apparent_temperature,"
              "is_day,precipitation,weather_code,wind_speed_10m"
              "&daily=weather_code,temperature_2m_max,temperature_2m_min,"
              "precipitation_probability_max&forecast_days=5&timezone=auto";

        string forecastJson = httpGet(forecastUrl);
        string current = jsonObject(forecastJson, "current");
        string daily = jsonObject(forecastJson, "daily");

        string codeText = jsonNumber(current, "weather_code");
        int code = codeText == "N/A" ? -1 : stoi(codeText);

        printLine();
        cout << "  WEATHER FOR: " << placeName;
        if (!country.empty()) cout << ", " << country;
        cout << "\n";
        printLine();
        cout << left << setw(25) << "Conditions" << ": " << weatherDescription(code) << "\n";
        cout << left << setw(25) << "Temperature" << ": " << jsonNumber(current, "temperature_2m") << " C\n";
        cout << left << setw(25) << "Feels like" << ": " << jsonNumber(current, "apparent_temperature") << " C\n";
        cout << left << setw(25) << "Humidity" << ": " << jsonNumber(current, "relative_humidity_2m") << " %\n";
        cout << left << setw(25) << "Wind speed" << ": " << jsonNumber(current, "wind_speed_10m") << " km/h\n";
        cout << left << setw(25) << "Precipitation" << ": " << jsonNumber(current, "precipitation") << " mm\n";

        vector<string> dates = jsonArrayValues(daily, "time");
        vector<string> highs = jsonArrayValues(daily, "temperature_2m_max");
        vector<string> lows = jsonArrayValues(daily, "temperature_2m_min");
        vector<string> rain = jsonArrayValues(daily, "precipitation_probability_max");
        vector<string> codes = jsonArrayValues(daily, "weather_code");

        cout << "\n5-DAY FORECAST\n";
        printLine();
        cout << left << setw(13) << "Date" << setw(13) << "Condition"
             << setw(12) << "High (C)" << setw(12) << "Low (C)" << "Rain chance\n";
        printLine();

        size_t count = min({dates.size(), highs.size(), lows.size(), rain.size(), codes.size()});
        for (size_t i = 0; i < count; ++i) {
            string condition = weatherDescription(stoi(codes[i]));
            if (condition.size() > 12) condition = condition.substr(0, 11) + ".";
            cout << left << setw(13) << dates[i]
                 << setw(13) << condition
                 << setw(12) << highs[i]
                 << setw(12) << lows[i]
                 << rain[i] << "%\n";
        }

        printLine();
        cout << "Data source: Open-Meteo (https://open-meteo.com/)\n";
        cout << "Note: Forecasts can change. Check local alerts before travel.\n\n";
        curl_global_cleanup();
    } catch (const exception& ex) {
        cerr << "\nSorry, the forecast could not be loaded.\n";
        cerr << "Details: " << ex.what() << "\n";
        cerr << "Check your internet connection and try again.\n\n";
        curl_global_cleanup();
        return 1;
    }

    return 0;
}
