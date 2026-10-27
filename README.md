# C++ Weather Forecast App 🌦️

A beginner-friendly C++ console application that fetches **live weather and a 5-day forecast** for a city you enter.

## Features

- Search weather by city name
- Current temperature and feels-like temperature
- Humidity, wind speed, and precipitation
- 5-day forecast with daily high/low temperatures and rain probability
- Simple weather-code descriptions
- Handles unknown cities and common network errors

## Built with

- C++17
- libcurl for HTTP requests
- [Open-Meteo Geocoding API](https://open-meteo.com/en/docs/geocoding-api)
- [Open-Meteo Forecast API](https://open-meteo.com/en/docs)

No API key is required for this project. An internet connection is required.

## Folder structure

```text
cpp-weather-forecast/
├── main.cpp
├── CMakeLists.txt
├── README.md
└── .gitignore
```

## Requirements

- A C++17 compiler (GCC, Clang, or MSVC)
- CMake 3.16 or newer
- libcurl development files
- Internet connection

## Build and run

### Option A: Linux (Ubuntu/Debian)

Install the compiler, CMake, and libcurl development package:

```bash
sudo apt update
sudo apt install build-essential cmake libcurl4-openssl-dev
```

From the project folder:

```bash
cmake -S . -B build
cmake --build build
./build/weather_app
```

### Option B: macOS

Install the tools with Homebrew:

```bash
brew install cmake curl
```

Then build:

```bash
cmake -S . -B build
cmake --build build
./build/weather_app
```

If CMake cannot locate curl, install the Xcode Command Line Tools and check your Homebrew environment.

### Option C: Windows

The easiest reliable setup is Visual Studio 2022 with the **Desktop development with C++** workload, CMake, and vcpkg.

1. Install Visual Studio and CMake.
2. Install [vcpkg](https://github.com/microsoft/vcpkg) and integrate it with Visual Studio.
3. Install curl using vcpkg:

```powershell
vcpkg install curl:x64-windows
```

4. From the project folder, configure CMake. Replace `C:/path/to/vcpkg` with your actual vcpkg folder:

```powershell
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
```

5. Run the executable from the generated build output. With a Visual Studio generator, it is commonly:

```powershell
.uild\Release\weather_app.exe
```

The exact executable location can vary by Visual Studio version and generator.

## Example

```text
------------------------------------------------------------
              C++ WEATHER FORECAST APP
          Live data powered by Open-Meteo
------------------------------------------------------------
Enter a city name (example: Karur, Chennai, London): Chennai

------------------------------------------------------------
  WEATHER FOR: Chennai, India
------------------------------------------------------------
Conditions              : Partly cloudy
Temperature             : 31.2 C
Feels like               : 36.1 C
Humidity                 : 65 %
Wind speed               : 14.0 km/h
Precipitation            : 0 mm

5-DAY FORECAST
------------------------------------------------------------
Date         Condition    High (C)    Low (C)     Rain chance
...
```

*Example values are illustrative only; the app fetches live data when you run it.*

## Upload to GitHub

1. Sign in to [GitHub](https://github.com/).
2. Click **New repository**.
3. Name it `cpp-weather-forecast`.
4. Choose Public if you want it visible on your profile.
5. Create the repository.
6. Upload `main.cpp`, `CMakeLists.txt`, `README.md`, and `.gitignore` using **Add file → Upload files**. Do not upload the `build` folder.
7. Click **Commit changes**.

### Upload with Git (optional)

From the project folder, run:

```bash
git init
git add main.cpp CMakeLists.txt README.md .gitignore
git commit -m "Add C++ weather forecast app"
git branch -M main
git remote add origin https://github.com/YOUR-USERNAME/cpp-weather-forecast.git
git push -u origin main
```

Replace `YOUR-USERNAME` with your GitHub username.

## Notes

- Weather data and geocoding are provided by Open-Meteo; see its website for usage terms and attribution requirements.
- This project is for learning and demonstration. Weather values and availability depend on the API and your internet connection.
- If you are new to C++, make sure you understand the input, HTTP request, JSON parsing, and forecast-printing sections before presenting this project.
