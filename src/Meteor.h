//
// Created by Alex Chi on 2019/12/23.
//

#ifndef METRO_METEOR_H
#define METRO_METEOR_H

#ifdef DO_NOT_COMPILE_WITH_QMAKE
template <class T>
struct do_not_compile_with_qmake {
    static_assert(false,
                  "Please build with project with CMake. Refer to README.md for instructions.");
};
#endif

#endif //METRO_METEOR_H
