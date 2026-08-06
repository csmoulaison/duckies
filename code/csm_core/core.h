#ifndef core_h_INCLUDED
#define core_h_INCLUDED

#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dlfcn.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef float    f32;
typedef double   f64;

typedef size_t   usize;

#define KILOBYTE 1000
#define MEGABYTE 1000000
#define GIGABYTE 1000000000

#include "./assert.h"
#include "./log.h"
#include "./string.h"
#include "./buffer.h"
#include "./stack.h"
#include "./rect.h"
#include "./random.h"
#include "./file.h"

#ifdef CSM_MODULE_MATH
#include <math.h>
#include "./math.h"
#endif

#ifdef CSM_MODULE_GL
#include "./gl.h"
#endif

#ifdef CSM_MODULE_WINDOW
#include "./window.h"
#endif

#ifdef CSM_MODULE_FIEDLER
#include "./fiedler.h"
#endif

#ifdef CSM_MODULE_ASSET_BUILDER
#include "./asset_builder.h"
#endif

#ifdef CSM_MODULE_BUILD
#include "./build.h"
#endif

#ifdef CSM_MODULE_DYNAMIC_LIB
#include "./dynamic_library.h"
#endif

#ifdef CSM_MODULE_MESH
#include "./mesh.h"
#endif

#ifdef CSM_MODULE_TEXTURE
#include "./texture.h"
#endif

#ifdef CSM_MODULE_SPRITE
#include "./sprite.h"
#endif

#ifdef CSM_MODULE_SPRITE_ATLAS
#include "./sprite_atlas.h"
#endif

#ifdef CSM_MODULE_FONT
#include "./font.h"
#endif

#ifdef CSM_MODULE_BLENDER
#include "./blender.h"
#endif

#ifdef CSM_MODULE_ASEPRITE
#include "./aseprite.h"
#endif

#ifdef CSM_MODULE_ALSA
#include "./alsa.h"
#endif

#endif
