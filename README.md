# Meteor

Visualize and analyze Hangzhou Metro data with Qt

## Building

On macOS, use CMake to build this project.

```bash
cmake -H. -Bbuild -DCMAKE_PREFIX_PATH=$QT_ROOT_PATH/5.12.6/clang_64/lib/cmake
cd build && cmake --build .
```

## Running

Copy `station_line.csv` to `dataset_CS241/adjacency_adjacency/`. The program will automatically check if this file is present in wizard.

In `build` folder,

```bash
./Meteor
```

## Passenger Traffic

## Route Planning

## Flow Analysis

## Smart Travel Time

## License

GPL-3.0