#ifndef COMMON_H
#define COMMON_H

#include <cstdio>
#include <cstdint>
#include <iostream>
#include <cstring>

typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t uint;

#include <functional>
#include <fstream>

using namespace std;

inline u8* read_file(string path, int* f_length) {
   ifstream f(path, fstream::binary);

   f.seekg(0, ios::end);
   int length = f.tellg();
   f.seekg(0, ios::beg);

   char* buff = new char[length];
   f.read(buff, length);
   f.close();

   *f_length = length;

   return (u8*)buff;
}

#endif //COMMON_H
