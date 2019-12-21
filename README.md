# Meteor

Visualize and analyze Hangzhou Metro data with Qt

## Building

On macOS, use CMake to build this project.

```bash
cmake -H. -Bbuild -DCMAKE_PREFIX_PATH=$QT_ROOT_PATH/5.12.6/clang_64/lib/cmake
cd build && cmake --build .
```

## Running



In `build` folder,

```bash
./Meteor
```

Copy `station_line.csv` to `dataset_CS241/adjacency_adjacency/`. Select dataset folder and fields in wizard. The program will automatically check if this file is present in wizard.

<img width="400" alt="Setup Wizard" src="https://user-images.githubusercontent.com/4198311/71302743-f4d25a80-23e9-11ea-9e08-94eb985b70d2.png">

## Passenger Traffic

<img width="1280" alt="Screen Shot 2019-12-21 at 12 00 12 PM" src="https://user-images.githubusercontent.com/4198311/71302744-f4d25a80-23e9-11ea-9a1e-f2c4cde096ff.png">

## Route Planning

Check "Meteor Adviser" to show ETA and crowded rate.

<img width="1280" alt="Screen Shot 2019-12-21 at 12 00 45 PM" src="https://user-images.githubusercontent.com/4198311/71302746-f56af100-23e9-11ea-9686-9e47a24d5098.png">

## Flow Analysis

<img width="1280" alt="Screen Shot 2019-12-21 at 12 01 00 PM" src="https://user-images.githubusercontent.com/4198311/71302747-f56af100-23e9-11ea-9a3e-97369d8f2c70.png">

## License

GPL-3.0