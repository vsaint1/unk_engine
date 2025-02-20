# [unk_engine] :dash:

## [Description]

unk_engine is a `CMAKE` and `SDL3` project "framework" used to help create cross-platform applications with `C++` and learn how complex systems works under the hood.

> The goals are supporting only the following platforms: Windows, Macos, Android iOS, Web.

## Development and build

1. Clone the repository

```sh
git clone https://github.com/vsaint1/unk_engine.git
git pull
cd unk_engine
git submodule update --init --recursive
```

2. Install requirements

- All the `requirements` for each platform are listed below with minimal version (major, minor, patch).

### Windows

- Visual Studio [2019/2022](https://visualstudio.microsoft.com/)
  - MSVC [17.11.0](https://en.wikipedia.org/wiki/MSVC)

### Android

> Tested on Android emulator, used Pixel 8 API 30 and Samsung Galaxy A15

- Android Studio [4.1.x](https://developer.android.com/studio/)
  - Android SDK [34.x](https://developer.android.com/studio/)
  - Android NDK [25.x](https://developer.android.com/ndk/downloads/)
  - Android Command-Line Tools [16.x](https://developer.android.com/studio/command-line)

### Mac

- Xcode [15.4](https://developer.apple.com/xcode/)
  - iOS SDK [12.x](https://developer.apple.com/)


### iOS

> Tested on iOS simulator, used iPhone 16 PLUS API 18 and iPhone 15 API 18.3

- Xcode [15.4](https://developer.apple.com/xcode/)
  - iOS SDK [12.x](https://developer.apple.com/)

### Web


- Emscripten [2.0.6](https://emscripten.org/)
  - emcc [3.1.64+](https://emscripten.org/)
  - em++ [3.x+](https://emscripten.org/)
  - nodejs [18.20.3](https://nodejs.org/)


### General

- Tooling
  - Git [2.45.2](https://git-scm.com/)
  - CMake [3.21.x](https://cmake.org/)

# How to build ? :construction_worker:

<img height="32" width="32" src="examples/windows.svg" alt="windows  icon">

```bat
cmake --preset=debug-windows
cmake --build build/windows --preset=windows-d
```

<img height="32" width="32" src="examples/android.svg" alt="android  icon">


```sh
cd android
gradle assembleDebug
```

<img height="32" width="32" src="examples/ios.svg" alt="iOS  icon">
  

1. Generating the `*.xcodeproj`

> DEVICE arm64

```sh
cmake --preset=ios-xcode

open $project_name.xcodeproj

```

> SIMULATOR x86_64

```sh
cmake --preset=ios-simulator-xcode

open $project_name.xcodeproj

```

2. Signing and building
   - Setup the Provisioning signing for iOS
   - Choose the Device
   - Signing and building the application


<img height="32" width="32" src="examples/apple.svg" alt="Apple  icon">

```sh
cmake --preset=mac-xcode

cmake --build build/mac  
# or
open $project_name.xcodeproj

```

<img height="32" width="32" src="examples/webgl.svg" alt="WebGL  icon">


```sh
# CERTIFY THAT EMSDK IS ACTIVED (emsdk activate latest)
emcmake cmake --preset=rel-webgl
emmake cmake --build build/webgl --preset=webgl-r
```

## [DEPENDENCIES]

Currently im managing dependencies through [git-submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules).

- [SDL3](https://github.com/libsdl-org/SDL)
- [SDL3_ttf](https://github.com/libsdl-org/SDL_ttf)
- [SDL3_image](https://github.com/libsdl-org/SDL_image)
- [SDL3_mixer](https://github.com/libsdl-org/SDL_mixer)
- [JSON](https://github.com/nlohmann/json)
- [XML](https://github.com/leethomason/tinyxml2)
- [GLM](https://github.com/g-truc/glm)



1. New submodule
```sh
 git submodule add https://github.com/libsdl-org/SDL.git vendor/SDL
 cd $into_submodule_dir
 git checkout $desired_hash
 # fetch all dependencies/submodules
 git submodule update --init --recursive 
```

2. Removing submodule

`NOTE: on windows use Remove-Item -Recurse -Force`

```sh
git submodule deinit -f -- SDL
rm -rf .git/modules/SDL
rm -rf SDL
```



# Code Style

This project follows some [UE Coding Guidelines](https://dev.epicgames.com/documentation/en-us/unreal-engine/epic-cplusplus-coding-standard-for-unreal-engine).
