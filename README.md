# Meteor

[![Build Status](https://travis-ci.com/skyzh/Meteor.svg?token=szB6fz2m5vb2KyfAiZ3B&branch=master)](https://travis-ci.com/skyzh/Meteor)

Visualize and analyze Hangzhou Metro data with Qt.

## Building

On macOS, use CMake to build this project. Replace `$QT_ROOT_PATH` with Qt folder. It's usually `~/Qt`.

```bash
cmake -H. -Bbuild -DCMAKE_PREFIX_PATH=$QT_ROOT_PATH/5.12.6/clang_64/lib/cmake -DCMAKE_BUILD_TYPE=RELEASE
cd build && cmake --build .
```

Or you may open this project with CLion, configure Qt in CMake settings, and build with `Release` target.

## Running

In `build` folder,

```bash
./Meteor
```

Copy `station_line.csv` to `dataset_CS241/adjacency_adjacency/`. Select dataset folder and fields in wizard. The program will automatically check if this file is present in wizard.

<img width="400" alt="Setup Wizard" src="https://user-images.githubusercontent.com/4198311/71302743-f4d25a80-23e9-11ea-9e08-94eb985b70d2.png">

Uncheck "Persist Analysis Cache" will make data loading faster in current session, but data will be loaded again if you restart the app.

## Functionalities

### Passenger Traffic

<img width="1280" alt="Screen Shot 2019-12-21 at 12 00 12 PM" src="https://user-images.githubusercontent.com/4198311/71302744-f4d25a80-23e9-11ea-9a1e-f2c4cde096ff.png">

### Route Planning

Select "Meteor Adviser" to show ETA and crowded rate.

<img width="1280" alt="Screen Shot 2019-12-21 at 12 00 45 PM" src="https://user-images.githubusercontent.com/4198311/71302746-f56af100-23e9-11ea-9686-9e47a24d5098.png">

### Flow Analysis for Segment Pressure

You may use the slider on screen or on Touch Bar to navigate time through a day.

<img width="1280" alt="Screen Shot 2019-12-21 at 12 01 00 PM" src="https://user-images.githubusercontent.com/4198311/71302747-f56af100-23e9-11ea-9a3e-97369d8f2c70.png">

## License

GPL-3.0
