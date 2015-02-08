#ifndef DYNAMIC_H_INCLUDED
#define DYNAMIC_H_INCLUDED

#define DYNAMIC_VERSION "0.3.0"
#define DYNAMIC_VERSION_MAJOR 0
#define DYNAMIC_VERSION_MINOR 3
#define DYNAMIC_VERSION_PATCH 0

#ifdef __cplusplus
extern "C" {
#endif

#include <dynamic/buffer.h>
#include <dynamic/vector.h>
#include <dynamic/string.h>
#include <dynamic/cfarmhash.h>
#include <dynamic/map_int.h>
#include <dynamic/map_str.h>
#include <dynamic/map_pair.h>
#include <dynamic/io.h>

#ifdef __cplusplus
}
#endif

#endif /* DYNAMIC_H_INCLUDED */
