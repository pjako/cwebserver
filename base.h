
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifndef _BASE_H_
#define _BASE_H_
#pragma warning(push)
#pragma warning(disable: 4068)

///////////////////////////////////////////
// BASE ///////////////////////////////////
///////////////////////////////////////////

#ifdef __cplusplus
#define CPP_ENV 1
#else
#define CPP_ENV 0
#endif

#if CPP_ENV == 0
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    // C11 or later
#else
    #error "This code requires at least C11 to compile."
#endif
#endif

#pragma mark - Compiler

#define COMPILER_MSVC 0
#define COMPILER_CLANG 0
#define COMPILER_GCC 0

#ifdef _MSC_VER
#undef COMPILER_MSVC
#define COMPILER_MSVC 1
#elif defined(__clang__)
#undef COMPILER_CLANG
#define COMPILER_CLANG 1
#elif defined(__GNUC__)
#undef COMPILER_GCC
#define COMPILER_GCC 1
#endif

////////////////////////////////
// NOTE(pjako): Operation System
#define OS_WIN 0
#define OS_OSX 0
#define OS_IOS 0
#define OS_ANDROID 0
#define OS_LINUX 0
#define OS_UNIX 0
#define OS_EMSCRIPTEN 0

#define KILOBYTE(KIL) (1024LL*(1LL)*(KIL))
#define MEGABYTE(MEG) (1024LL*(KILOBYTE(1LL))*(MEG))
#define GIGABYTE(GIG) (1024LL*(MEGABYTE(1LL))*(GIG))

#define MINUTESTOSECONDS(MIN) ((60LL)*(MIN))
#define HOURSTOSECONDS(MIN) ((60LL)*(60LL)*(MIN))
#define DAYSTOSECONDS(MIN) ((24LL)*(60LL)*(60LL)*(MIN))

#ifdef _WIN32
#undef OS_WIN
#define OS_WIN 1
#define OS_SHORT "win"
#elif defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
#undef OS_OSX
#define OS_OSX 1
#define OS_SHORT "osx"
#elif defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) || defined(__ENVIRONMENT_TV_OS_VERSION_MIN_REQUIRED__)
#undef OS_IOS
#define OS_IOS 1
#define OS_SHORT "ios"
#elif __ANDROID__
#undef OS_ANDROID
#define OS_ANDROID 1
#define OS_SHORT "an"
#elif __linux__
#undef OS_LINUX
#define OS_LINUX 1
#define OS_SHORT "linux"
#elif __unix__
#undef OS_UNIX
#define OS_UNIX 1
#define OS_SHORT "unix"
#elif __EMSCRIPTEN__
#undef OS_EMSCRIPTEN
#define OS_EMSCRIPTEN 1
#define OS_SHORT "ems"
#else
#error "Unknown Operation System"
#endif
#if OS_OSX || OS_IOS
#define OS_APPLE 1
#else
#define OS_APPLE 0
#endif

////////////////////////////////
// NOTE(pjako): Operation System Name
#if OS_WIN
#define OS_NAME "Windows"
#elif OS_OSX
#define OS_NAME "OSX"
#elif  OS_IOS
#define OS_NAME "IOS"
#elif  OS_ANDROID
#define OS_NAME "Android"
#elif  OS_LINUX
#define OS_NAME "Linux"
#elif  OS_UNIX
#define OS_NAME "Unix"
#elif  OS_EMSCRIPTEN
#define OS_NAME "Emscripten"
#else
#error "Unknown OS"
#endif

////////////////////////////////
// NOTE(pjako): Architecture
#define ARCH_X64 0
#define ARCH_ARM64 0

#if defined(_M_X64) || defined(_M_AMD64) || defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
#undef ARCH_X64
#define ARCH_X64 1
#elif defined(__aarch64__) || defined(_M_ARM64)
#undef ARCH_ARM64
#define ARCH_ARM64 1
#else
#error "Unknown CPU architecture"
#endif

#define INLINE static inline
#if COMPILER_MSVC
#define FORCE_INLINE static __forceinline
#else
#define FORCE_INLINE static __attribute__((always_inline))
#endif

#if COMPILER_CLANG || COMPILER_GCC
#define ALIGNED_STRUCT(NAME, ALIGN, CONTENT) typedef struct NAME CONTENT __attribute__ ((aligned (ALIGN))) NAME
#define ALIGNED_STRUCT_START(NAME, ALIGN) typedef struct NAME 
#define ALIGNED_STRUCT_END(NAME, ALIGN) __attribute__ ((aligned (ALIGN))) NAME
#define ALIGN_DECL(_align, _decl) _decl __attribute__( (aligned(_align) ) )
#define THREAD_LOCAL __thread
#else
#define ALIGNED_STRUCT(NAME, ALIGN, CONTENT) typedef struct __declspec(align(ALIGN)) NAME CONTENT NAME
#define ALIGNED_STRUCT_START(NAME, ALIGN) typedef struct __declspec(align(ALIGN)) NAME 
#define ALIGNED_STRUCT_END(NAME, ALIGN) NAME
#define ALIGN_DECL(_align, _decl) __declspec(align(_align) ) _decl
#define THREAD_LOCAL __declspec(thread)
#endif

#if __cplusplus >= 201103
#define DEFINE_ALIGNED(def, a) alignas(a) def
#else
#if defined(_WINDOWS) || defined(XBOX)
#define DEFINE_ALIGNED(def, a) __declspec(align(a)) def
#elif defined(__APPLE__)
#define DEFINE_ALIGNED(def, a) def __attribute__((aligned(a)))
#else
//If we haven't specified the platform here, we fallback on the C++11 and C11 keyword for aligning
//Best case -> No platform specific align defined -> use this one that does the same thing
//Worst case -> No platform specific align defined -> this one also doesn't work and fails to compile -> add a platform specific one :)
#define DEFINE_ALIGNED(def, a) alignas(a) def
#endif
#endif

#define COMPILER_ASSERT(exp)  typedef char __compilerAssert##__LINE__[(exp) ? 1 : -1]



#if COMPILER_CLANG && CPP_ENV
#define THREAD_ATTRIBUTE(x) __attribute__((x))
#else
#define THREAD_ATTRIBUTE(x)
#endif

#define THREAD_GUARDED(...)     THREAD_ATTRIBUTE(guarded_by(__VA_ARGS__))
#define THREAD_CAPABILITY(...)  THREAD_ATTRIBUTE(capability(__VA_ARGS__))
#define THREAD_ACQUIRES(...)    THREAD_ATTRIBUTE(acquire_capability(__VA_ARGS__))
#define THREAD_RELEASES(...)    THREAD_ATTRIBUTE(release_capability(__VA_ARGS__))
#define THREAD_TRY_ACQUIRE(...) THREAD_ATTRIBUTE(try_acquire_capability(__VA_ARGS__))
#define THREAD_EXCLUDES(...)    THREAD_ATTRIBUTE(locks_excluded(__VA_ARGS__))
#define THREAD_REQUIRES(...)    THREAD_ATTRIBUTE(requires_capability(__VA_ARGS__))
#define THREAD_INTERNAL         THREAD_ATTRIBUTE(no_thread_safety_analysis)

#pragma mark - API

#ifndef API
#define API extern
#endif

#ifndef LOCAL
#define LOCAL static
#endif


#ifndef DLL_API
#if OS_WIN
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif
#endif

#define CAST(TYPE, VALUE) ((TYPE)VALUE)


////////////////////////////////
// NOTE(pjako): Linked List
// dll == double linked list
// sll == single linked list

#define dll_pushBackNp(f,l,n, next, prev) ((f)==0?\
                                          ((f)=(l)=(n),(n)->next=(n)->prev=0):\
                                          ((n)->prev=(l),(l)->next=(n),(l)=(n),(n)->next = 0))
#define dll_removeNp(f,l,n, next, prev) (((f)==(n)?\
                                          ((f)=(f)->next,(f)->prev=0):\
                                          (l)==(n)?\
                                          ((l)=(n)->prev,(l)->next=0):\
                                          ((n)->next->prev=(n)->prev,\
                                           (n)->prev->next=(n)->next)))
#define dll_pushBack(f,l,n) dll_pushBackNp(f,l,n, next, prev)
#define dll_pushFront(f,l,n) dll_pushBackNp(f,l,n, prev, next)
#define dll_remove(f,l,n, next, prev) dll_removeNp(f,l,n, next, prev)

#define sll_queuePushN(f,l,n,next) ((f)==0?\
                                    (f)=(l)=(n):\
                                    ((l)->next=(n),(l)=(n)),\
                                    (n)->next=0)
#define sll_queuePush(f,l,n,next) sll_queuePushN(f,l,n,next)

#define sll_queuePushFrontN(f,l,n,next) ((f)==0?\
                                         ((f)=(l)=(n),(n)->next=0):\
                                         ((n)->next=(f),(f)=(n)))
#define sll_queuePushFront(f,l,n,next) sll_queuePushFrontN(f,l,n,next)

#define sll_queuePopN(f,l,next) ((f)=(l)?\
                                 (f)=(l)=0:\
                                 (f)=(f)->next)
#define sll_queuePop(f,l,next) sll_queuePopN(f,l,next)

#define sll_stackPushN(f,n,next) ((f)==0?\
                                 (f)=(l)=0:\
                                 ((n)->next=(f), (f)=(n)))
#define sll_stackPush(f,n,next) sll_stackPushN(f,n,next)

#define sll_stackPopN(f,next) ((f)==0?0:\
                               (f)=(f)->next)
#define sll_stackPop(f,next) sll_stackPopN(f,next)

#define unused(x) (void)(x)
// dont use this with VLA args, could have side effects
#define unusedVars(...) sizeof(__VA_ARGS__)

#define minVal(a, b) ((a) < (b) ? (a) : (b))
#define maxVal(a, b) ((a) > (b) ? (a) : (b))
#define clampVal(MINVAL, MAXVAL, VALUE) (maxVal(minVal(MAXVAL, VALUE), MINVAL))
#define absVal(A)    ((A) < 0 ? -(A) : (A))
#define squareVal(A) ((A) * (A))


#define alignDown(PTR, ALIGN)  ((umm_cast(PTR)) & ~(i32_cast(ALIGN) - 1))
#define alignUp(PTR, ALIGN)   (((umm_cast(PTR)) +  (i32_cast(ALIGN) - 1)) & ~(i32_cast(ALIGN) - 1))
#define isAligned(PTR, ALIGN)  ((umm_cast(PTR)  &  (i32_cast(ALIGN) - 1)) == 0)

#define OS_MEM_ALIGNMENT (8)
#define os_alignDown(PTR) alignDown((PTR), OS_MEM_ALIGNMENT)
#define os_alignUp(PTR)  alignUp((PTR), OS_MEM_ALIGNMENT)
#define os_isAligned(PTR)isAligned((PTR), OS_MEM_ALIGNMENT)

#define OS_DEFAULT_PAGE_SIZE (1024 * 1024 * 4)

#define clampTop(V, T) ((V > T) ? T : V)
#define ceilVal(a, b) (((a) / (b)) + (((a) % (b)) > 0 ? 1 : 0))
#define roundVal(a, b) (((a) / (b)) + (((a) % (b)) >= 5 ? 1 : 0))

#define isPowerOf2(x) (((x) != 0u) && ((x) & ((x) - 1)) == 0u)

#define u32_nextPowerOfTwo(VAL) ASSERT(VAL > 0); VAL--; VAL |= VAL >> 1; VAL |= VAL >> 2; VAL |= VAL >> 4; VAL |= VAL >> 8; VAL |= VAL >> 16; VAL += 1

#define sizeOf(S) ((i64)sizeof(S))
#define countOf(V) (sizeOf(V) / sizeOf((V)[0]))

#define ptr_dref(PTR) (*PTR)
#define ptr_ref(PTR) (&PTR)

#if defined(_Typeof)
// C23 typeof
#define typeOf(TYPE) _Typeof(TYPE)
#elif defined(typeof)
// non standard typeof in GCC/Clang
#define typeOf(TYPE) typeof(TYPE)
#elif defined(__typeof__)
#define typeOf(TYPE) __typeof__(TYPE)
#elif __cpp_decltype >= 200707L
// decltype from C++ could be a bit problematic since there are some subtle differences to typeof/_Typeof
#define typeOf(TYPE) decltype(TYPE)
#else
#define typeOf(TYPE) void*
//#error "No typeof variant exist in this enviroment"
#endif

#if CPP_ENV
#define CPLiteral(Type, ...) { __VA_ARGS__ }
#else
#define CPLiteral(Type, ...) (TYPE) { __VA_ARGS__ }
#endif

#if CPP_ENV
#define ZeroStruct() {}
#else
#define ZeroStruct() {0}
#endif

//
// Debug
//

#if COMPILER_MSVC
#include "assert.h"
#define ASSERT(C) assert(C)
//#define ASSERT(C) while(!(C)) __assume(0)
#else
#include "assert.h"
#define ASSERT(C) assert(C)
// while(!(C)) __builtin_unreachable()
#endif

#define BASE_STRING_JOIN_IMMEDIATE(ARG1, ARG2) ARG1 ## ARG2
#define BASE_STRING_JOIN_DELAY(ARG1, ARG2) BASE_STRING_JOIN_IMMEDIATE(ARG1, ARG2)
#define BASE_STRING_JOIN(ARG1, ARG2) BASE_STRING_JOIN_DELAY(ARG1, ARG2)

#ifdef COMPILER_MSVC
  #define BASE_UNIQUE_NAME(NAME) BASE_STRING_JOIN(NAME,__COUNTER__)
#else
  #define BASE_UNIQUE_NAME(NAME) BASE_STRING_JOIN(NAME,__LINE__)
#endif
#define BASE_LINE_UNIQUE_NAME(NAME) BASE_STRING_JOIN(NAME,__LINE__)

#ifndef STATIC_ASSERT
  #define STATIC_ASSERT(EXP) typedef u8 BASE_UNIQUE_NAME(__staticAssertDummyArr)[(EXP)?1:-1]
#endif

#if COMPILER_MSVC
#define debugBreak() __debugbreak()
#elif COMPILER_CLANG
#define debugBreak() __builtin_debugtrap()
#elif COMPILER_GCC
#define debugBreak() __builtin_trap()
#else // cross platform implementation
#define debugBreak() for (;;) {int* int3 = (int*)3L; *int3 = 3; break;}
//#define DEBUG_BREAK() (*((void*) 0) = 0)
#endif

///////////////////////////////////////////
// BASE TYPES /////////////////////////////
///////////////////////////////////////////

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uint64_t usize;

typedef uintptr_t umm;
typedef size_t mms;
#define umm_cast(V) ((umm) (V))

#define u8_cast(V)  ((u8)  (V))
#define u16_cast(V) ((u16) (V))
#define u32_cast(V) ((u32) (V))
#define u64_cast(V) ((u64) (V))

#define u8_val(val)  val
#define u16_val(val) val
#define u32_val(val) val##ul
#define u64_val(val) val##ull

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef int64_t  isize;

#define i8_cast(V)  ((i8)  (V))
#define i16_cast(V) ((i16) (V))
#define i32_cast(V) ((i32) (V))
#define i64_cast(V) ((i64) (V))

#define i8_val(val)  val
#define i16_val(val) val
#define i32_val(val) val##l
#define i64_val(val) val##ll


// bx should only used internally since its size is compiler dependent (per c spec)
#ifdef __cplusplus
typedef bool    bx;
#else
typedef _Bool    bx;
#endif
typedef u8       b8;
typedef u16      b16;
typedef u32      b32;

#define bx_cast(B)  ((b8) ((B) != 0))
#define b8_cast(B)  ((b8) ((B) != 0))
#define b16_cast(B) ((b16)((B) != 0))
#define b32_cast(B) ((b32)((B) != 0))

typedef u32      flags32;
typedef u64      flags64;

#define flag32(n) ((1u) << u32_cast(n))
#define flag64(n) ((1ull) << u64_cast(n))

typedef u32 volatile a32;
typedef u64 volatile a64;

typedef union f16 { u16 val; struct {i32 sign : 1; u32 exponent : 5 ; u32 mantissa : 10;}; } f16;
typedef float    f32;
typedef double   f64;

#define f32_cast(F) ((f32) (F))
#define f64_cast(F) ((f64) (F))

#ifdef SIMD_USE_SSE
#include <xmmintrin.h>

typedef union f16x4 {
    ALIGN_DECL(16, f16) values[4];
} f16x4;
typedef union f32x4 {
    __m128 simd;
    ALIGN_DECL(16, f32) values[4];
} f32x4;
typedef union i32x4 {
    __m128i simd;
    ALIGN_DECL(16, i32) values[4];
} i32x4;
typedef struct f32x16 {
    f32x4 _rows[4];
} f32x16;
typedef union f16x4 {
    ALIGN_DECL(16, f16) values[4];
} f16x4;


#elif defined(SIMD_USE_NEON)

typedef union f16x4 {
    float16x4_t simd;
    ALIGN_DECL(16, f16) values[4];
} f16x4;
typedef union f32x4 {
    float32x4_t simd;
    ALIGN_DECL(16, f32) values[4];
} f32x4;
typedef union i32x4 {
    int32x4_t simd;
    ALIGN_DECL(16, i32) values[4];
} i32x4;
typedef union f32x16 {
    f32x4 rows[4];
} f32x16;
typedef union f16x4 {
    float16x4_t simd;
    ALIGN_DECL(16, f16) values[4];
} f16x4;

#else

typedef union f32x4 {
    //ALIGN_DECL(16, f32) values[4];
    f32 values[4];
} f32x4;
typedef union i32x4 {
    //ALIGN_DECL(16, i32) values[4];
    i32 values[4];
} i32x4;
typedef union f32x16 {
    f32x4 rows[4];
} f32x16;
typedef union f16x4 {
    //ALIGN_DECL(16, f16) values[4];
    f16 values[4];
} f16x4;
#endif

typedef union Simd128 {
    f32x4 f32x4;
    i32x4 i32x4;
} Simd128;

typedef union Quat {
   f32 store[4];
   f32x4 simd;
} Quat;

typedef union Vec2 {
    struct {
        f32 x, y;
    };
    struct {
        f32 width, height;
    };
   f32 store[2];
#if CPP_ENV
    inline f32 &operator[](const int &index) {
        return store[maxVal(0, minVal(index, countOf(store)))];
    }
#endif
} Vec2;

typedef union Vec2i {
    struct {
        i32 x, y;
    };
   i32 store[2];
#if CPP_ENV
    inline i32 &operator[](const int &index) {
        return store[maxVal(0, minVal(index, countOf(store)))];
    }
#endif
} Vec2i;

typedef union Vec3 {
    struct {
        f32 x, y, z;
    };
    f32 store[3];
#if CPP_ENV
    inline f32 &operator[](const int &index) {
        return store[maxVal(0, minVal(index, countOf(store)))];
    }
#endif
} Vec3;

typedef union Vec4 {
    struct {
        f32 x, y, z, w;
    };
    struct {
        Vec2 origin;
        Vec2 size;
    };
    struct {
        Vec2 xy;
        Vec2 zw;
    };
    f32 store[4];
    f32x4 simd;
#if CPP_ENV
    inline f32 &operator[](const int &index) {
        return store[maxVal(0, minVal(index, countOf(store)))];
    }
#endif
} Vec4;

typedef union Mat4 {
    struct {
        f32 m00, m01, m02, m03;
        f32 m10, m11, m12, m13;
        f32 m20, m21, m22, m23;
        f32 m30, m31, m32, m33;
    };
    struct {
        Vec4 row0, row1, row2, row3;
    };
    f32 store[16];
    f32 store4x4[4][4];
    f32x16 simd;
#if CPP_ENV
    inline f32 &operator[](const int &index) {
        return store[maxVal(0, minVal(index, countOf(store)))];
    }
#endif
} Mat4;

typedef union Mat43 {
    struct {
        f32 m00, m01, m02;
        f32 m10, m11, m12;
        f32 m20, m21, m22;
        f32 m30, m31, m32;
    };
    struct {
        Vec3 row0, row1, row2, row3;
    };
   f32 store[16];
#if CPP_ENV
    inline f32 &operator[](const int &index) {
        return store[maxVal(0, minVal(index, countOf(store)))];
    }
#endif
} Mat43;

typedef union Rgb8 {
    struct {
        u8 red;
        u8 green;
        u8 blue;
    };
    u8 store[3];
#if CPP_ENV
    inline u8 &operator[](const int &index) {
        return store[index];
    }
#endif
} Rgb8;

typedef union Rgba8 {
    struct {
        u8 red;
        u8 green;
        u8 blue;
        u8 alpha;
    };
    struct {
        Rgb8 rgb;
        u8 ___unusedAlpha;
    };
    u32 uRgba;
    f32 fRgba;
    u8 store[4];
#if CPP_ENV
    inline u8 &operator[](const int &index) {
        return store[index];
    }
#endif
} Rgba8;

typedef union Rgba {
    struct {
        f32 red;
        f32 green;
        f32 blue;
        f32 alpha;
    };
    f32 store[4];
#if CPP_ENV
    inline f32 &operator[](const int &index) {
        return store[index];
    }
#endif
} Rgba;


#pragma mark - String

typedef struct S8 {
    u8* content;
    u64 size;
#if CPP_ENV
    inline u8 operator[](const int &index) {
        if (content == NULL || size == 0) {
            ASSERT(!"str is empty");
            return 0;
        }
        return content[maxVal(0, minVal(index, (size - 1)))];
    }
#endif
} S8;

#ifdef __cplusplus
#define str_lit(STR) {(u8*) STR, sizeof(STR) - 1}
#define str_dyn(NAME, SIZE) u8 __strContent##__COUNT__ S8 NAME = {&__strContent##__COUNT__[0], SIZE}
#else
#define str_lit(STR) (S8) {(u8*) STR, sizeof(STR) - 1}
#define str_dyn(NAME, SIZE) u8 __strContent##__COUNT__ S8 NAME = (S8) {&__strContent##__COUNT__[0], SIZE}
#endif

typedef struct S16 {
    u16* content;
    u64  size;
#if CPP_ENV
    inline u16 operator[](const int &index) {
        if (content == NULL || size == 0) {
            return 0;
        }
        return content[maxVal(0, minVal(index, (size - 1)))];
    }
#endif
} S16;

typedef struct S32 {
    u32* content;
    u64  size;
#if CPP_ENV
    inline u32 operator[](const int &index) {
        if (content == NULL || size == 0) {
            return 0;
        }
        return content[maxVal(0, minVal(index, (size - 1)))];
    }
#endif
} S32;

typedef struct str_handle {
    u32 id;
} str_handle;


#pragma mark - Time

typedef struct DenseTime {
    u64 value;
} DenseTime;

typedef struct DateTime {
    u16 milliSecond; // 0-999
    u8  second;  // 0-60
    u8  minute;  // 0-59
    u8  hour; // 0-23
    u8  day;  // 0-30
    u8  month;  // 0-11
    i16 year; // 1 = 1 CE, 2020 = 2020 CE, -100 = 101 VCE, ...
} DateTime;

typedef union tm_FrequencyInfo {
    u64 frequency;
    struct {
        u32 numer;
        u32 denom;
    };
} tm_FrequencyInfo;


#pragma mark - Memory

typedef void*(mem_allocFn)(u64 size, void* userPtr);
typedef void*(mem_reallocFn)(u64 size, void* oldPtr, u64 oldSize, void* userPtr);
typedef void (mem_freeFn)(void* ptr, void* userPtr);

typedef enum allocator_type {
    allocator_type_arena,
    // allocator_type_std, // malloc like allocator
    // allocator_type_bump, // fixed ring allocator
    // allocator_type_custom, // custom allocator
} allocator_type;

typedef struct Allocator {
    //allocator_type type;
    mem_allocFn* alloc;
    mem_reallocFn* realloc;
    mem_freeFn* free;
    void* allocator;
    flags32 flags;
} Allocator;

typedef struct AllocatorGroup {
    Allocator* temporary;
    Allocator* persistent;
} AllocatorGroup;

//#define allocator_alloc(ALLOCATOR, SIZE) (ALLOCATOR).alloc(SIZE, (ALLOCATOR).allocator)

typedef struct BaseMemory BaseMemory;
typedef struct Arena Arena;


#pragma mark - Profiler

typedef struct Profiler {
    void (*startProfile)(void* profiler, S8 file, u64 line, S8 funcName);
    void (*endProfile)(void* profiler, S8 file, u64 line, S8 funcName);
    void* profilerPtr;
} Profiler;

#define profiler_start(PROFILER) (PROFILER).startProfile((PROFILER).profilerPtr, s8(__FILE__), __LINE__, s8(__FUNCTION__))
#define profiler_end(PROFILER) (PROFILER).endProfile((PROFILER).profilerPtr)
#define profiler_scoped(PROFILER) profiler_start(PROFILER); for (i32 __i__ = 1; __i__ != 0; (__i__ = 0, profiler_end(PROFILER)))

#pragma mark - Context

/*ALIGN(16)*/
struct Context {
    Allocator* allocator;
	Profiler profiler;
    //jmp_Ctx jmp;
};
typedef struct Context Context;

#pragma mark - Containers

#define arrView(ARR) { .elements = (ARR).elements, .count = (ARR).count, .capacity = (ARR).capacity }
#define ct_def(TYPE) arrTypeDef(TYPE); mapTypeDef(TYPE)


#define mapTypeDef(TYPE) typedef struct TYPE##Map { ct_Map map; struct { TYPE* elements; u32 count; u32 capacity; } values; } TYPE##Map; typedef struct TYPE##MapIterator {u64 idx; u64 keyHash; TYPE* value;} TYPE##MapIterator
#define mapVarDef(TYPE) TYPE##Map
typedef struct ct_Map {
    Allocator allocator;
    u32 count;
} ct_Map;

// from C23 and one
#if __STDC_VERSION__ >= 202311L
// In C23 we can make generic data structures, without defining them anywhere globally
#define FixedArray(TYPE) struct FixedArray_##TYPE {TYPE* items; u64 count; u64 capacity;}
#define FixedArrayPtr(TYPE) struct FixedArrayPtr_##TYPE {TYPE** items; u64 count; u64 capacity;}
#define Array(TYPE) struct Array_##TYPE {FixedArray(TYPE) content; Allocator* allocator;}
#define ArrayPtr(TYPE) struct ArrayPtr_##TYPE {FixedArrayPtr(TYPE) content; Allocator* allocator}
#define Slice(TYPE) struct Slice_##TYPE {TYPE* items; u64 count;}
#else
// #define arrDef(TYPE) struct {TYPE* elements; u32 count; u32 capacity;}
#define arrFixedDef(TYPE) struct Array_##TYPE {TYPE* items; u32 count; u32 capacity;}
#define arrStructDef(TYPE) struct Array_##TYPE {TYPE* items; u32 count; u32 capacity; Allocator* allocator;}
#define arrVarDef(TYPE) TYPE##Array

#define FixedArray(TYPE) struct FixedArray_##TYPE
#define FixedArrayPtr(TYPE) struct FixedArrayPtr_##TYPE
#define Array(TYPE) struct Array_##TYPE
#define ArrayPtr(TYPE) struct ArrayPtr_##TYPE

#define sliceStructDef(TYPE) typedef struct Slice_##TYPE {TYPE* items; u64 count;}
#define Slice(TYPE) struct Slice_##TYPE

#endif

#define array_atIndex(ARRAY, IDX) (((IDX < 0) || ((ARRAY)->count <= (IDX))) ? NULL : &(ARRAY)->items[IDX])
#define array_init(ALLOCATOR, ARR, CAPACITY) (ARR)->items = (void*) allocator_alloc((ALLOCATOR), (sizeof((ARR)->items[0]) * CAPACITY)); (ARR)->capacity = CAPACITY; (ARR)->count = 0
#define array_pushPtr(ARRAY) (((ARRAY)->count < (ARRAY)->capacity) ? &(ARRAY)->items[(ARRAY)->count++] : NULL)

///////////////////////////////////////////
// BASE MEM ///////////////////////////////
///////////////////////////////////////////

#include <string.h>

#define mem_setZero(PTR, SIZE) memset((void*) (PTR), 0x0, (SIZE))
#define mem_structSetZero(PTR) mem_setZero((void*)(PTR), sizeof(*PTR))
#define mem_arrSetZero(PTR, COUNT) mem_setZero((PTR), sizeof((PTR)[0]) * (COUNT))

#define mem_copy(TO, FROM, SIZE) memcpy(TO, FROM, SIZE)


typedef void* (mem_reserveFunc)(void* ctx, u64 size);
typedef void  (mem_changeMemoryFunc)(void* ctx, void* ptr, u64 size);

typedef struct BaseMemory {
    void* ctx;
    u32 pageSize;
    mem_reserveFunc* reserve;
    mem_changeMemoryFunc* commit;
    mem_changeMemoryFunc* decommit;
    mem_changeMemoryFunc* release;
} BaseMemory;

// Abstract Allocator
#define allocator_alloc(ALLOCATOR, SIZE) (ALLOCATOR)->alloc(SIZE, ALLOCATOR->allocator)
#define allocator_realloc(ALLOCATOR, PTR, OLDSIZE, NEWSIZE) (ALLOCATOR)->realloc(PTR, OLDSIZE, NEWSIZE, ALLOCATOR->allocator)
#define allocator_free(ALLOCATOR, PTR) (ALLOCATOR)->free(PTR, ALLOCATOR->allocator)

// std malloc

#if 0
API Allocator* mem_makeStdMalloc(BaseMemory* baseMemory, usize size);
API void mem_destroyStdMalloc(Allocator* allocator);
#endif

// Arena Allocator

typedef struct Arena {
    Allocator allocator;
    BaseMemory base;
    i64 unsafeRecord;
    u64 alignment;
    u64 cap;
    u64 pos;
    u64 commitPos;
    u8  memory[0];
} Arena;

typedef struct MallocContext {
    Allocator allocator;
} MallocContext;

API BaseMemory mem_getMallocBaseMem(void);

API u64 mem_arenaStartUnsafeRecord(Arena* arena);
API void mem_arenaStopUnsafeRecord(Arena* arena);
API Arena* mem_makeArenaAligned(BaseMemory* baseMem, u64 size, u64 aligment);
API Arena* mem_makeArena(BaseMemory* baseMem, u64 size);
API Arena* mem_makeArenaPreAllocated(void* mem, u64 size);
API void mem_destroyArena(Arena* arena);
#define mem_defineMakeStackArena(ARENANAME, SIZE) u8 BASE_LINE_UNIQUE_NAME(ARENANAME##_mem)[sizeOf(Arena) + (SIZE)]; Arena* ARENANAME = mem_makeArenaPreAllocated((void*) &BASE_LINE_UNIQUE_NAME(ARENANAME##_mem)[0], sizeOf(Arena) + (SIZE))

API u64 mem_getArenaMemOffsetPos(Arena* arena);

API void* mem_arenaPush(Arena* arena, u64 size);

#define mem_arenaPushZero(ARENA, SIZE) mem_setZero(mem_arenaPush(ARENA, SIZE), SIZE)
#define mem_arenaPushStruct(ARENA, STRUCT) (STRUCT*) mem_arenaPush(ARENA, sizeof(STRUCT))
#define mem_arenaPushStructZero(ARENA, STRUCT) (STRUCT*) mem_setZero(mem_arenaPush(ARENA, sizeof(STRUCT)), sizeof(STRUCT))
#define mem_arenaPushArray(ARENA, STRUCT, COUNT) (STRUCT*) mem_arenaPush(ARENA, sizeof(STRUCT) * COUNT)
#define mem_arenaPushArrayZero(ARENA, STRUCT, COUNT) (STRUCT*) mem_setZero(mem_arenaPush(ARENA, sizeof(STRUCT) * COUNT), sizeof(STRUCT) * COUNT)
#define mem_areaPushData(ARENA, DATA) mem_copy(mem_arenaPush(ARENA, sizeof(DATA)), &DATA, sizeof(DATA))

API void  mem_arenaPopTo(Arena* arena, u64 amount);
API void  mem_arenaPopAmount(Arena* arena, u64 amount);

typedef struct mem_Scratch {
    Arena* arena;
    u64 start;
} mem_Scratch;

API mem_Scratch mem_scratchStart(Arena* arena);
API void mem_scratchEnd(mem_Scratch* scratch);
#define mem_scoped(NAME, ARENA) for (mem_Scratch NAME = mem_scratchStart(ARENA);(NAME).arena; (mem_scratchEnd(&NAME), (NAME).arena = NULL))


///////////////////////////////////////////
// BASE STR ///////////////////////////////
///////////////////////////////////////////

//#include <string.h>
#include <stdarg.h>

static S8 STR_TERMINATOR = {(u8*) "\0", 1};
static S8 STR_EMPTY = {(u8*) "", 0};
static S8 STR_NULL = {NULL, 0};

#if 0
// reference: https://github.com/mattiasgustavsson/libs/blob/main/strpool.h

typedef struct str_Pool {
    Arena* arena;
    str_handle* handles;
} str_Pool;

#define str_handleEqual(A, B) ((A).id == (B).id)

API void str_poolInit(Arena* arena, str_Pool* pool);
API str_handle str_poolInject(str_Pool* pool, S8 str);
API S8 str_poolGet(str_Pool* pool, str_handle handle);
#endif

typedef struct str__BuilderBlock {
    struct str__BuilderBlock* prev;
    struct str__BuilderBlock* next;
    S8 str;
} str__BuilderBlock;
typedef struct str_Builder {
    Arena* arena;
    u64 arenaLastOffset;
    u64 blockDefaultSize;
    u64 totalStringSize;
    str__BuilderBlock* firstBlock;
    str__BuilderBlock* lastBlock;
} str_Builder;

#define str8(STR) str_makeViewSized((u8*) (STR), sizeof(STR) - 1)
#define s8(STR) str_makeViewSized((u8*) (STR), sizeof(STR) - 1)
INLINE S8 str_makeViewSized(u8* c, u64 size) {
    S8 str;
    str.size = size;
    str.content = c;
    return str;
}
#define str_fromCppStd(STDSTRING) str_makeViewSized((u8*) (STDSTRING).c_str(), (STDSTRING).size())

#define STRINGIFY(...) #__VA_ARGS__
#define str_cpToStaticNullTerminated(STR, STATSTR) for (u32 i = 0; i < (STR).size) (STATSTR)[i] = (STR).content[i]; (STATSTR)[(STR).size] = '\0'
#define str_isEmpty(STR) ((STR).size == 0)

INLINE bx str_isNullTerminated(S8 str) {
    return str.content[str.size - 1] == '\0';
}

API S8 str_makeSized(Arena* arena, u8* arr, u32 size);
API S8 str_alloc(Arena* arena, mms size);
API S8 str_copy(Arena* arena, S8 sourceStr);

API S8 str_copyNullTerminated(Arena* arena, S8 str);


API S8 str_toLowerAscii(S8 str);
API S8 str_toUpperAscii(S8 str);
API S8 str_subStr(S8 str, u64 start, u64 size);
API S8 str_from(S8 str, u64 from);
API S8 str_to(S8 str, u64 to);

// returns the ut8 length of the string
API u64  str_length(S8 str);

API f32  str_parseF32 (S8 str);
API u64  str_parseF32N(S8 str, f32* f32);
API i64  str_parseS64 (S8 str);
API u64  str_parseS64N(S8 str, i64* i64);
API u32  str_parseU32 (S8 str);
API u64  str_parseU32N(S8 str, u32* u32);
API S8 str_parseQuotedStr(S8 str);


API i64  str_find(S8 str, S8 findExp);
API i64  str_findChar(S8 str, char c);
API i64  str_lastIndexOfChar(S8 str, char c);
API bx str_hasPrefix(S8 str, S8 prefix);
API bx   str_hasSuffix(S8 str, S8 endsWith);
API bool str_startsWithChar(S8 str, char startChar);
API bool str_isEqual(S8 left, S8 right);
//API bool str_isWhiteSpace(S8 str);
API i64  str_firstNonWhiteSpace(S8 str);
API S8 str_skipWhiteSpace(S8 str);

API bool str_isWhitespaceChar(char c);


API bool str_isSpacingChar(char c);
API bool str_isNumberChar(char c);
API bool str_isEndOfLineChar(char c);
API bool str_isAlphaChar(char c);
API bool str_isEndOfLineChar(char c);


#pragma mark - String manipulation

API u64 str_containsSubStringCount(S8 str, S8 findStr);
API u64 str_findFirst(S8 str, S8 findStr, u64 offset);
API u64 str_findLast(S8 str, S8 findStr, u64 offset);
API S8 str_replaceAll(Arena* arena, S8 str, S8 replaceStr, S8 replacement);

#pragma mark - UTF-8 functions

API u64 str_utf8Count(S8 str);

typedef struct str_StringDecode {
    u32 codepoint;
    u32 size;
} str_StringDecode;

API str_StringDecode str_decodeUtf8(u8* str, u64 cap);

// limited to 15 fraction digits
// Store value needs to b at least 15+fracDigits+2 in size
API S8 str_floatToStr(f64 value, S8 storeStr, i32* decimalPos, i32 fracDigits);
API S8 str_u32ToHex(Arena* arena, u32 value);

API S8  str_fromCharPtr(u8* str, u64 size);
API S8  str_fromNullTerminatedCharPtr(char* str);


#pragma mark - Utf8 to Utf16

API S16 str_toS16(Arena* arena, S8 str);
API S32 str_toS32(Arena *arena, S8 str);
API S8  str_fromS16(Arena* arena, S16 str);


#pragma mark - hash

API u32 str_hash32(S8 str);
API u64 str_hash64(S8 str);

////////////////////////////
// NOTE(pjako): fmt parser... do we need that?

typedef enum str_FmtToken {
    str_fmtToken_unknown = 0,
    str_fmtToken_string  = 1,
    str_fmtToken_fmt     = 2,
} str_FmtToken;

typedef struct str_FmtParser {
    S8         str;
    u32          offset;
    str_FmtToken currentToken;
    S8         key;
    S8         value;
    u32          blockIndex;
} str_FmtParser;

////////////////////////////
// NOTE(pjako): str8 format

typedef enum str_argType {
    str_argType_custom,
    str_argType_str,
    str_argType_char,
    str_argType_f32,
    str_argType_f64,
    str_argType_u32,
    str_argType_u64,
    str_argType_i32,
    str_argType_i64,
} str_argType;

typedef struct str_CustomVal {
    void* usrPtr;
    S8 (*buildStrFn)(Arena* recordArena, S8 fmtInfo, void* userPtr);
} str_CustomVal;

typedef struct str_Value {
    str_argType type;
    union {
        S8 strVal;
        char charVal;
        f32 f32Val;
        f64 f64Val;
        u32 u32Val;
        u64 u64Val;
        i32 i32Val;
        i64 i64Val;
        str_CustomVal customVal;
    };
} str_Value;

typedef struct str_KeyValue {
    S8 key;
    str_Value value;
} str_KeyValue;

#define str_keyValue(KEY, VALUE) str_kv(KEY, VALUE)
#define str_kv(KEY, VALUE) str__keyValue(str__convertToKey(KEY), str__convertToValue(VALUE))
INLINE str_KeyValue str__keyValue(S8 key, str_Value value) {
    str_KeyValue kv;
    kv.key = key;
    kv.value = value;
    return kv;
}

// Usage: str8_fmt("Foo {}", "Bar") => "Foo Bar"
//#define str_fmt(ARENA, TEMPLATE, ...) STR_ARG_OVER_UNDER_FLOW_CHECKER(str8_fmtRaw, STR_AT_LEAST_TWO_ARGS, __VA_ARGS__)(ARENA, str__convertToValue(TEMPLATE), STR_ARR_MACRO_CHOOSER(__VA_ARGS__)(str_Arg, str_convertToAny, __VA_ARGS__))

// Usage: str8_join("Foo", " ", "Bar") => "Foo Bar"

#define str_fmt(ARENA, FMTSTR, ... ) STR_ARG_OVER_UNDER_FLOW_CHECKER(str_fmtRaw, STR_AT_LEAST_TWO_ARGS, __VA_ARGS__)(ARENA, str__convertToKey(FMTSTR), STR_ARR_MACRO_CHOOSER(__VA_ARGS__)(str_KeyValue, str__convertToKeyValue, __VA_ARGS__))

#define str_join(ARENA, ...) STR_ARG_OVER_UNDER_FLOW_CHECKER(str_joinRaw, STR_AT_LEAST_TWO_ARGS, __VA_ARGS__)(ARENA, STR_ARR_MACRO_CHOOSER(__VA_ARGS__)(str_KeyVakue, str__convertToValue, __VA_ARGS__))

API S8 str_joinRaw(Arena* arena, u32 argCount, ...);
API S8 str_joinVargs(Arena* arena, u32 argCount, va_list list);
API S8 str_fmtRaw(Arena* arena, S8 fmt, u32 argCount, ...);
API S8 str_fmtVargs(Arena* arena, S8 fmt, u32 argCount, va_list list);

////////////////////////////
/// Buikder ////////////////
////////////////////////////

#define str_builderFmt(BUILDER, FMTSTR, ... ) STR_ARG_OVER_UNDER_FLOW_CHECKER(str_builderJoinRaw, STR_AT_LEAST_TWO_ARGS, __VA_ARGS__)(ARENA, str__convertToKey(FMTSTR), STR_ARR_MACRO_CHOOSER(__VA_ARGS__)(str_KeyValue, str__convertToKeyValue, __VA_ARGS__))

#define str_builderJoin(BUILDER, ...) STR_ARG_OVER_UNDER_FLOW_CHECKER(str_builderFmtRaw, STR_AT_LEAST_TWO_ARGS, __VA_ARGS__)(ARENA, STR_ARR_MACRO_CHOOSER(__VA_ARGS__)(str_KeyVakue, str__convertToValue, __VA_ARGS__))

API str_Builder str_builderInit(Arena* arena, u64 blockDefaultSize);
API S8 str_builderFinish(Arena* targetArena);

API void str_builderJoinRaw(str_Builder* builder, u32 argCount, ...);
// API void str_builderJoinVargs(str_Builder* builder, u32 argCount, va_list list);
API void str_builderFmtRaw(str_Builder* builder, S8 fmt, u32 argCount, ...);
// API void str_builderFmtVargs(str_Builder* builder, S8 fmt, u32 argCount, va_list list);

#define str_record(STR, ARENA) for (u64 startIdx = mem_arenaStartUnsafeRecord(ARENA) + 1;startIdx != 0; (( (startIdx - 1) < mem_getArenaMemOffsetPos(ARENA) ? (STR.content = &(ARENA)->memory[startIdx - 1], STR.size = (mem_getArenaMemOffsetPos(ARENA) - (startIdx - 1))) : (STR.content = NULL, STR.size = 0)  ), startIdx = 0, mem_arenaStopUnsafeRecord(ARENA)))

////////////////////////////
// NOTE(pjako): fmt/join implementation

#define STR(s)  #s

#ifdef __cplusplus
#define STR_TOFN(TYPE) str__toVal
#define STR_TOKEY(TYPE) str__toKey
#define STR_TOKV(TYPE) str__toKeyVal

#define str__convertToKeyValue(V) str__toKeyVal(V, sizeof(V), STR(V), sizeof(STR(V)))
#define str__convertToKey(V) str__toKey(V, sizeof(V), STR(V), sizeof(STR(V)))
// Never change V to anything more then a single letter since this cause bugs when this expression is passed: " "
#define str__convertToValue(V) str__toVal(V, sizeof(V), STR(V), sizeof(STR(V)))

#define STR_ARR_PREFIX(TYPE, COUNT) COUNT, 
#define STR_ARR_POSTFIX(COUNT) 
//#define STR_ARR_PREFIX(TYPE, COUNT) TYPE str__args##__COUNT__[COUNT] =
//#define STR_ARR_POSTFIX(COUNT) ;



#else

#define STR_TOKV(TYPE) str__##TYPE##ToKeyVal
#define str__convertToKeyValue(V) _Generic(V, \
    str_KeyValue: STR_TOKV(kv), \
    S8: STR_TOKV(str8), \
    char: STR_TOKV(char), \
    u8: STR_TOKV(uchar), \
    str_CustomVal: STR_TOKV(custom), \
    u32: STR_TOKV(u32), \
    u64: STR_TOKV(u64), \
    i32: STR_TOKV(i32), \
    i64: STR_TOKV(i64), \
    f32: STR_TOKV(f32), \
    f64: STR_TOKV(f64), \
    default: STR_TOKV(custom)  \
)((V), sizeof(V), STR(V), sizeof(STR(V)))

#define STR_TOKEY(TYPE) str__##TYPE##ToKey
#define str__convertToKey(V) _Generic(V, \
    S8: STR_TOKEY(str8), \
    default: STR_TOKEY(str8)  \
)((V), sizeof(V), STR(V), sizeof(STR(V)))

#define STR_TOFN(TYPE) str__##TYPE##ToVal
// Never change V to anything more then a single letter since this cause bugs when this expression is passed: " "

#define str___convertToValue(V) _Generic(V, \
    S8: STR_TOFN(str8), \
    char: STR_TOFN(char), \
    u8: STR_TOFN(uchar), \
    str_CustomVal: STR_TOFN(custom), \
    u32: STR_TOFN(u32), \
    u64: STR_TOFN(u64), \
    i32: STR_TOFN(i32), \
    i64: STR_TOFN(i64), \
    f32: STR_TOFN(f32), \
    f64: STR_TOFN(f64), \
    default: STR_TOFN(custom)  \
)

#define str__convertToValue(V) (str___convertToValue(V))((V), sizeof(V), STR(V), sizeof(STR(V)))
#define STR_ARR_PREFIX(TYPE, COUNT) COUNT, 
#define STR_ARR_POSTFIX(COUNT) 
//#define STR_ARR_PREFIX(TYPE, COUNT) ((TYPE*) (&(TYPE[COUNT]) 
//#define STR_ARR_POSTFIX(COUNT) )), (COUNT)
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif

INLINE bx str__readNumExpression(str_Value* out, const char* strExpression, u32 strExpressionSize) {
    if ((strExpression[0] < '0' ||  strExpression[0] > '9') && strExpression[0] != '-') return false;
    if (strExpression[1] == '\0') {
        // single digit
        S8 str;
        str.size = 1;
        str.content = (u8*) strExpression;
        out->type = str_argType_str;
        out->strVal = str;
        return true;
    }
    if (strExpression[1] < '0' ||  strExpression[1] > '9') return false;
    u32 idx = 1;
    for (; idx < (strExpressionSize - 1); idx++) {
        char c = strExpression[idx];
        if (c >= '0' && c <= '9') continue;
        if (c != 'u' && c != 'U' && c != 'l' && c != 'L' && c != 'z' && c != 'Z') return false;
        break;
    }

    // remove the ending of the number 23u  23uul ect.

    S8 str;
    str.size = idx;
    str.content = (u8*) strExpression;
    out->type = str_argType_str;
    out->strVal = str;
    return true;
}

////////////////////////////
// NOTE(pjako): Convert "anything" to str_Value

INLINE str_Value str_valToVal(str_Value val, u32 size, const char* strExpression, u32 strExpressionSize) {
    return val;
}

INLINE str_Value STR_TOFN(custom)(str_CustomVal custom, u32 size, const char* strExpression, u32 strExpressionSize) {
    str_Value value;
    value.type = str_argType_custom;
    value.customVal = custom;
    return value;
}

INLINE str_Value STR_TOFN(char)(char c, u32 size, const char* strExpression, u32 strExpressionSize) {
    str_Value value;
    value.type = str_argType_char;
    value.charVal = c;
    return value;
}

INLINE str_Value STR_TOFN(uchar)(unsigned char c, u32 size, const char* strExpression, u32 strExpressionSize) {
    str_Value value;
    value.type = str_argType_char;
    value.charVal = c;
    return value;
}

INLINE str_Value STR_TOFN(str8)(S8 str, u32 size, const char* strExpression, u32 strExpressionSize) {
    str_Value value;
    value.type = str_argType_str;
    value.strVal = str;
    return value;
}

INLINE str_Value STR_TOFN(f32)(f32 floatVal, u32 size, const char* strExpression, u32 strExpressionSize) {
    str_Value value;
    value.type = str_argType_f32;
    value.f32Val = floatVal;
    return value;
}

INLINE str_Value STR_TOFN(f64)(f64 floatVal, u32 size, const char* strExpression, u32 strExpressionSize) {
    str_Value value;
    if ((strExpression[0] >= '0' && strExpression[0] <= '9') || strExpression[0] == '-' || strExpression[0] == '.') {
        bool simpleFloat = true;
        u32 idx = (strExpression[0] == '-' || strExpression[0] == '.') ? 1 : 0;
        for (; idx < (strExpressionSize - 1); idx++) {
            if ( (strExpression[idx] >= '0' && strExpression[idx] <= '9') || strExpression[idx] == '.') continue;
            simpleFloat = false;
            break;
        }
        if (simpleFloat) {
            // pass simple float expression as string
            S8 str;
            str.size = strExpressionSize - 1;
            str.content = (u8*) strExpression;
            value.type = str_argType_str;
            value.strVal = str;
            return value;
        }
    }
    value.type = str_argType_f64;
    value.f64Val = floatVal;
    return value;
}

INLINE str_Value STR_TOFN(u32)(u32 u32Val, u32 size, const char* strExpression, u32 strExpressionSize) {
    str_Value value;
    if (str__readNumExpression(&value, strExpression, strExpressionSize)) return value;
    value.type = str_argType_u32;
    value.u32Val = u32Val;
    return value;
}

INLINE str_Value STR_TOFN(u64)(u64 u64Val, u32 size, const char* strExpression, u32 strExpressionSize) {
    str_Value value;
    if (str__readNumExpression(&value, strExpression, strExpressionSize)) return value;
    value.type = str_argType_u64;
    value.u64Val = u64Val;
    return value;
}

INLINE str_Value STR_TOFN(i32)(i32 i32Val, u32 size, const char* strExpression, u32 strExpressionSize) {
    str_Value value;
    if (strExpression[0] == '\'') {
        // handle char literals
        value.type = str_argType_char;
        value.charVal = (char) (maxVal(minVal(255, i32Val), 0));
        return value;
    }
    if (str__readNumExpression(&value, strExpression, strExpressionSize)) return value;
    value.type = str_argType_i32;
    value.i32Val = i32Val;
    return value;
}

INLINE str_Value STR_TOFN(i64)(i64 i64Val, u32 size, const char* strExpression, u32 strExpressionSize) {
    str_Value value;
    if (str__readNumExpression(&value, strExpression, strExpressionSize)) return value;
    value.type = str_argType_i64;
    value.i64Val = i64Val;
    return value;
}


////////////////////////////
// NOTE(pjako): Convert char* (and S8) to S8 to

INLINE S8 STR_TOKEY(str8)(S8 str, u32 size, const char* strExpression, u32 strExpressionSize) {
    return str;
}

INLINE S8 STR_TOKEY(charConst)(const char* val, u32 size, const char* strExpression, u32 strExpressionSize) {
    S8 str;
    if (strExpression[0] == '"') { // if expression starts with '"' it is a string literal
        // its a string literal
        str.size = size - 1;
        str.content = (u8*) val;
    } else if (strExpression[0] != '\0' && strExpression[1] != '\0' && strExpression[2] != '\0' && strExpression[0] != 'u' && strExpression[1] != '8' && strExpression[2] != '"') {
        // its a string literal starting with u8"
        str.size = size - 1;
        str.content = (u8*) val;
    } else {
        str.size = strlen(val);
        str.content = (u8*) val;
    }

    return str;
}

////////////////////////////
// NOTE(pjako): Convert to str_KeyValue

INLINE str_KeyValue STR_TOKV(value)(str_Value value) {
    str_KeyValue keyValue;
    keyValue.key.content = NULL;
    keyValue.key.size = 0;
    keyValue.value = value;

    return keyValue;
}

INLINE str_KeyValue STR_TOKV(kv)(str_KeyValue kv, u32 size, const char* strExpression, u32 strExpressionSize) {
    return kv;
}

INLINE str_KeyValue STR_TOKV(charConst)(const char* val, u32 size, const char* strExpression, u32 strExpressionSize) {
    S8 str;
    if (strExpression[0] == '"') { // if expression starts with '"' it is a string literal
        // its a string literal
        str.size = size - 1;
        str.content = (u8*) val;
    } else if (strExpression[0] != '\0' && strExpression[1] != '\0' && strExpression[2] != '\0' && strExpression[0] != 'u' && strExpression[1] != '8' && strExpression[2] != '"') {
        // its a string literal starting with u8"
        str.size = size - 1;
        str.content = (u8*) val;
    } else {
        str.size = strlen(val);
        str.content = (u8*) val;
    }

    str_Value value;
    value.type = str_argType_str;
    value.strVal = str;
    return STR_TOKV(value)(value);
}

INLINE str_KeyValue STR_TOKV(custom)(str_CustomVal custom, u32 size, const char* strExpression, u32 strExpressionSize) {
    str_Value value;
    value.type = str_argType_custom;
    value.customVal = custom;
    return STR_TOKV(value)(value);
}

INLINE str_KeyValue STR_TOKV(char)(char c, u32 size, const char* strExpression, u32 strExpressionSize) {
    str_Value value;
    value.type = str_argType_char;
    value.charVal = c;
    return STR_TOKV(value)(value);
}

INLINE str_KeyValue STR_TOKV(uchar)(unsigned char c, u32 size, const char* strExpression, u32 strExpressionSize) {
    str_Value value;
    value.type = str_argType_char;
    value.charVal = c;
    return STR_TOKV(value)(value);
}

INLINE str_KeyValue STR_TOKV(str8)(S8 str, u32 size, const char* strExpression, u32 strExpressionSize) {
    str_Value value;
    value.type = str_argType_str;
    value.strVal = str;
    return STR_TOKV(value)(value);
}

INLINE str_KeyValue STR_TOKV(f32)(f32 floatVal, u32 size, const char* strExpression, u32 strExpressionSize) {
    str_Value value;
    value.type = str_argType_f32;
    value.f32Val = floatVal;
    return STR_TOKV(value)(value);
}

INLINE str_KeyValue STR_TOKV(f64)(f64 floatVal, u32 size, const char* strExpression, u32 strExpressionSize) {
    str_Value value;
    if ((strExpression[0] >= '0' && strExpression[0] <= '9') || strExpression[0] == '-' || strExpression[0] == '.') {
        bool simpleFloat = true;
        u32 idx = (strExpression[0] == '-' || strExpression[0] == '.') ? 1 : 0;
        for (; idx < (strExpressionSize - 1); idx++) {
            if ( (strExpression[idx] >= '0' && strExpression[idx] <= '9') || strExpression[idx] == '.') continue;
            simpleFloat = false;
            break;
        }
        if (simpleFloat) {
            // pass simple float expression as string
            S8 str;
            str.size = strExpressionSize - 1;
            str.content = (u8*) strExpression;
            value.type = str_argType_str;
            value.strVal = str;
            return STR_TOKV(value)(value);
        }
    }
    value.type = str_argType_f64;
    value.f64Val = floatVal;
    return STR_TOKV(value)(value);
}

INLINE str_KeyValue STR_TOKV(u32)(u32 u32Val, u32 size, const char* strExpression, u32 strExpressionSize) {
    str_Value value;
    if (str__readNumExpression(&value, strExpression, strExpressionSize)) return STR_TOKV(value)(value);
    value.type = str_argType_u32;
    value.u32Val = u32Val;
    return STR_TOKV(value)(value);
}

INLINE str_KeyValue STR_TOKV(u64)(u64 u64Val, u32 size, const char* strExpression, u32 strExpressionSize) {
    str_Value value;
    if (str__readNumExpression(&value, strExpression, strExpressionSize)) return STR_TOKV(value)(value);
    value.type = str_argType_u64;
    value.u64Val = u64Val;
    return STR_TOKV(value)(value);
}

INLINE str_KeyValue STR_TOKV(i32)(i32 i32Val, u32 size, const char* strExpression, u32 strExpressionSize) {
    str_Value value;
    if (strExpression[0] == '\'') {
        // handle char literals
        value.type = str_argType_char;
        value.charVal = (char) (maxVal(minVal(255, i32Val), 0));
        return STR_TOKV(value)(value);
    }
    if (str__readNumExpression(&value, strExpression, strExpressionSize)) return STR_TOKV(value)(value);
    value.type = str_argType_i32;
    value.i32Val = i32Val;
    return STR_TOKV(value)(value);
}

INLINE str_KeyValue STR_TOKV(i64)(i64 i64Val, u32 size, const char* strExpression, u32 strExpressionSize) {
    str_Value value;
    if (str__readNumExpression(&value, strExpression, strExpressionSize)) return STR_TOKV(value)(value);
    value.type = str_argType_i64;
    value.i64Val = i64Val;
    return STR_TOKV(value)(value);
}


////////////////////////////
// NOTE(pjako): Macros to handle variable length arguments and conversion

#define STR_AT_LEAST_TWO_ARGS(...) STATIC_ASSERT("Needs at least two arguments!");
#define STR_AT_LEAST_ONE_ARG(...) STATIC_ASSERT("Needs at least one argument!");

#define STR__TOO_MANY(...) STATIC_ASSERT("A maximum of 12 dynamic arguments are currently supported")
#define STR__ARR12(TYPE, CONV, a, b, c, d, e, f, g, h, i, j, k, l, ...) STR_ARR_PREFIX(TYPE, 12) CONV(a), CONV(b), CONV(c), CONV(d), CONV(e), CONV(f), CONV(g), CONV(h), CONV(i), CONV(j), CONV(k), CONV(l) STR_ARR_POSTFIX(12)
#define STR__ARR11(TYPE, CONV, a, b, c, d, e, f, g, h, i, j, k) STR_ARR_PREFIX(TYPE, 11) CONV(a), CONV(b), CONV(c), CONV(d), CONV(e), CONV(f), CONV(g), CONV(h), CONV(i), CONV(j), CONV(k) STR_ARR_POSTFIX(11)
#define STR__ARR10(TYPE, CONV, a, b, c, d, e, f, g, h, i, j) STR_ARR_PREFIX(TYPE, 10) CONV(a), CONV(b), CONV(c), CONV(d), CONV(e), CONV(f), CONV(g), CONV(h), CONV(i), CONV(j) STR_ARR_POSTFIX(10)
#define STR__ARR9(TYPE, CONV, a, b, c, d, e, f, g, h, i) STR_ARR_PREFIX(TYPE, 9) CONV(a), CONV(b), CONV(c), CONV(d), CONV(e), CONV(f), CONV(g), CONV(h), CONV(i) STR_ARR_POSTFIX(9)
#define STR__ARR8(TYPE, CONV, a, b, c, d, e, f, g, h) STR_ARR_PREFIX(TYPE, 8) CONV(a), CONV(b), CONV(c), CONV(d), CONV(e), CONV(f), CONV(g), CONV(h) STR_ARR_POSTFIX(8)
#define STR__ARR7(TYPE, CONV, a, b, c, d, e, f, g) STR_ARR_PREFIX(TYPE, 7) CONV(a), CONV(b), CONV(c), CONV(d), CONV(e), CONV(f), CONV(g) STR_ARR_POSTFIX(7)
#define STR__ARR6(TYPE, CONV, a, b, c, d, e, f) STR_ARR_PREFIX(TYPE, 6) CONV(a), CONV(b), CONV(c), CONV(d), CONV(e), CONV(f) STR_ARR_POSTFIX(6)
#define STR__ARR5(TYPE, CONV, a, b, c, d, e) STR_ARR_PREFIX(TYPE, 5) CONV(a), CONV(b), CONV(c), CONV(d), CONV(e) STR_ARR_POSTFIX(5)
#define STR__ARR4(TYPE, CONV, a, b, c, d) STR_ARR_PREFIX(TYPE, 4) CONV(a), CONV(b), CONV(c), CONV(d) STR_ARR_POSTFIX(4)
#define STR__ARR3(TYPE, CONV, a, b, c) STR_ARR_PREFIX(TYPE, 3) CONV(a), CONV(b), CONV(c) STR_ARR_POSTFIX(3)
#define STR__ARR2(TYPE, CONV, a, b) STR_ARR_PREFIX(TYPE, 2)  CONV(a), CONV(b) STR_ARR_POSTFIX(2)
#define STR__ARR1(TYPE, CONV, a) STR_ARR_PREFIX(TYPE, 1) CONV(a) STR_ARR_POSTFIX(1)
#define STR__ARR0(...) 0
#define STR__ARG0(_0, ...) _0

#define STR__FUNC_CHOOSER(_f1, _f2, _f3, _f4, _f5, _f6, _f7, _f8, _f9, _f10, _f11, _f12, _f13, _f14, _f15, _f16, ...) _f16
#define STR__FUNC_RECOMPOSER(argsWithParentheses) STR__FUNC_CHOOSER argsWithParentheses
#define STR__CHOOSE_FROM_ARG_COUNT(...) STR__FUNC_RECOMPOSER((__VA_ARGS__, STR__TOO_MANY, STR__TOO_MANY, STR__TOO_MANY, STR__ARR12, STR__ARR11, STR__ARR10, STR__ARR9, STR__ARR8, STR__ARR7, STR__ARR6, STR__ARR5, STR__ARR4, STR__ARR3, STR__ARR2, STR__ARR1))

#define STR__NO_ARG_EXPANDER(...) ,,,,,,,,,,,,,,, STR__ARG0(__VA_ARGS__)

#define STR__CHOOSE_FROM_ARG_COUNT_CHECK(FN, ...) STR__FUNC_RECOMPOSER((__VA_ARGS__, STR__TOO_MANY, STR__TOO_MANY, STR__TOO_MANY, FN, FN, FN, FN,  FN, FN, FN, FN, FN, FN, FN, FN))

/* collects all passed arguments chooses the STR__ARRN that can fit all of them in */
#define STR_ARR_MACRO_CHOOSER(...) STR__CHOOSE_FROM_ARG_COUNT(STR__NO_ARG_EXPANDER __VA_ARGS__ (STR__ARR0))

/* returns a static assert if there are too many args, calls FNNOARGS if there are too little */
#define STR_ARG_OVER_UNDER_FLOW_CHECKER(FN, FNNOARGS, ...) STR__CHOOSE_FROM_ARG_COUNT_CHECK(FN, STR__NO_ARG_EXPANDER __VA_ARGS__ (FNNOARGS))

///////////////////////////////////////////
// BASE MATH //////////////////////////////
///////////////////////////////////////////

static const i16 i16_max        = 0xFFFF;
static const u16 u16_max        = 0xFFFF;
static const i32 i32_max        = 0x7FFFFFFF;
static const u32 u32_max        = 0x7FFFFFFF;
static const i64 i64_max        = 0xFFFFFFFFFFFFFFFF;
static const u64 u64_max        = 0xFFFFFFFFFFFFFFFF;

static const f32 f32_pi         = 3.1415926535897932384626433832795f;
static const f32 f32_pi2        = 6.2831853071795864769252867665590f;
static const f32 f32_invPi      = 0.3183098733425140380859375f; // 1.0f / f32_pi;
static const f32 f32_piHalf     = 1.5707963267948966192313216916398f;
static const f32 f32_piQuarter  = 0.7853981633974483096156608458199f;
static const f32 f32_sqrt2      = 1.4142135623730950488016887242097f;
static const f32 f32_logNat10   = 2.3025850929940456840179914546844f;
static const f32 f32_invLogNat2 = 1.4426950408889634073599246810019f;
static const f32 f32_logNat2Hi  = 0.6931471805599453094172321214582f;
static const f32 f32_logNat2Lo  = 1.90821492927058770002e-10f;
static const f32 f32_e          = 2.7182818284590452353602874713527f;
static const f32 f32_nearZero   = 0.0000000037252902984619140625f; // 1.0f / f32_cast(1 << 28);
static const f32 f32_min        = 1.175494e-38f;
static const f32 f32_max        = 3.402823e+38f;

static const f16 f16_infinity = {0x7C00};

static const u8  f32_signNumBits      = 1;
static const u8  f32_signBitShift     = 31;
static const u32 f32_signMask         = u32_val(0x80000000);
static const u8  f32_exponentNumBits  = 8;
static const u8  f32_exponentBitShift = 23;
static const u32 f32_exponentMask     = u32_val(0x7f800000);
static const u32 f32_exponentBias     = 127;
static const u8  f32_mantissaNumBits  = 23;
static const u8  f32_mantissaBitShift = 0;
static const u32 f32_mantissaMask     = u32_val(0x007fffff);

// Smallest normalized positive floating-point number.
static const f32 f32_smallestValue  = 1.175494351e-38f;

// Maximum representable floating-point number.
static const f32 f32_largestValue   = 3.402823466e+38f;

static const union { u32 __valU32; f32 value; } f32__infinity = { 255 << 23 };//{0x7f800000};
#define f32_infinity (f32__infinity.value)
#define f32_infinityNegative (-f32__infinity.value)

static const union { u32 __valU32; f32 value; } f32__nan = { 0x7f800000 | 0x007fffff }; // { f32_exponentMask | f32_mantissaMask };
#define f32_nan (f32__nan.value)

FORCE_INLINE f16 f16_fromF32(f32 input) {
    f16 o = { 0 };

    union {
        u32 u;
        f32 f;
        struct {
            u32 mantissa : 23;
            u32 exponent : 8;
            u32 sign : 1;
        };
    } f;
    f.f = input;

    // Based on ISPC reference code (with minor modifications)
    if (f.exponent == 255) {// Inf or NaN (all exponent bits set)
        o.exponent = 31;
        o.mantissa = f.mantissa ? 0x200 : 0; // NaN->qNaN and Inf->Inf
    } else { // Normalized number
        // Exponent unbias the single, then bias the halfp
        int newexp = f.exponent - 127 + 15;
        if (newexp >= 31) { // Overflow, return signed infinity
            o.exponent = 31;
        } else if (newexp <= 0) {// Underflow
            if ((14 - newexp) <= 24) {// Mantissa might be non-zero
                u32 mant = f.mantissa | 0x800000; // Hidden 1 bit
                o.mantissa = mant >> (14 - newexp);
                if ((mant >> (13 - newexp)) & 1) {// Check for rounding
                    o.val++; // Round, might overflow into exp bit, but this is OK
                }
            }
        } else {
            o.exponent = newexp;
            o.mantissa = f.mantissa >> 13;
            if (f.mantissa & 0x1000) {// Check for rounding
                o.val++; // Round, might overflow to inf, this is OK
            }
        }
    }

    o.sign = f.sign;
    return o;
}

FORCE_INLINE bx f16_isInfinity(f16 input) {
    return input.exponent == f16_infinity.exponent && input.mantissa == f16_infinity.mantissa;
}


#ifdef SIMD_USE_SSE

#ifndef USE_SIMD
#define USE_SIMD 1
#endif


// SSE SIMD (x86)

FORCE_INLINE f32x4 f32x4_make(f32 x, f32 y, f32 z, f32 w) {
    f32x4 result;
    result.simd = _mm_setr_ps(x, y, z, w);
    return result;
}

FORCE_INLINE f32x4 f32x4_add(f32x4 left, f32x4 right) {
    f32x4 result;
    result.simd = _mm_add_ps(left, right);
    return result;
}

FORCE_INLINE f32x4 f32x4_sub(f32x4 left, f32x4 right) {
    f32x4 result;
    result.simd = _mm_sub_ps(left, right);
    return result;
}

FORCE_INLINE f32x4 f32x4_cos(f32x4 in) {
    float32x4_t result;
    result.simd = _mm_cos_ps(in.simd);
    return result;
}

FORCE_INLINE f16x4 f16x4_fromF32x4(f32x4 from) {
    __m128i mask_fabs  = _mm_set1_epi32(0x7fffffff);
    __m128i c_f32infty = _mm_set1_epi32((255 << 23));
    __m128i c_expinf   = _mm_set1_epi32((255 ^ 31) << 23);
    __m128i c_f16max   = _mm_set1_epi32((127 + 16) << 23);
    __m128i c_magic    = _mm_set1_epi32(15 << 23);

    __m128  mabs        = _mm_castsi128_ps(mask_fabs);
    __m128  fabs        = _mm_and_ps(mabs, from.simd);
    __m128  justsign    = _mm_xor_ps(from.simd, fabs);

    __m128  f16max      = _mm_castsi128_ps(c_f16max);
    __m128  expinf      = _mm_castsi128_ps(c_expinf);
    __m128  infnancase  = _mm_xor_ps(expinf, fabs);
    __m128  clamped     = _mm_min_ps(f16max, fabs);
    __m128  b_notnormal = _mm_cmpnlt_ps(fabs, _mm_castsi128_ps(c_f32infty));
    __m128  scaled      = _mm_mul_ps(clamped, _mm_castsi128_ps(c_magic));

    __m128  merge1      = _mm_and_ps(infnancase, b_notnormal);
    __m128  merge2      = _mm_andnot_ps(b_notnormal, scaled);
    __m128  merged      = _mm_or_ps(merge1, merge2);

    __m128i shifted     = _mm_srli_epi32(_mm_castps_si128(merged), 13);
    __m128i signshifted = _mm_srli_epi32(_mm_castps_si128(justsign), 16);

    union {
        struct {
            f16x4 f16x4;
            f16x4 __f16x4;
        };
        i32x4 i32x4;

    } result;
    result.i32x4.simd = _mm_or_si128(shifted, signshifted);
    return result.f16x4;
}

#elif defined(SIMD_USE_NEON)

#ifndef USE_SIMD
#define USE_SIMD 1
#endif

// Neon SIMD (Arm)

FORCE_INLINE f32x4 f32x4_make(f32 x, f32 y, f32 z, f32 w) {
    ALIGN_DECL(16, f32) data[4] = {w, z, y, x};
    f32x4 result;
    result.simd = vld1q_f32(data);
    return result;
}

FORCE_INLINE f32x4 f32x4_add(f32x4 left, f32x4 right) {
    f32x4 result;
    result.simd = vaddq_f32(left.simd, right.simd);
    return result;
}

FORCE_INLINE f32x4 f32x4_sub(f32x4 left, f32x4 right) {
    f32x4 result;
    result.simd = vsubq_f32(left.simd, right.simd);
    return result;
}

FORCE_INLINE f16x4 f16x4_fromF32x4(f32x4 from) {
    f16x4 result;
    result.simd = vcvt_f16_f32(from.simd);
    return result;
}

FORCE_INLINE f32x4 f32x4_cos(f32x4 in) {
    float32x4_t result;
    result.simd = vcosq_f32(in.simd);
    return result;
}

#else

#ifndef USE_SIMD
#define USE_SIMD 0
#endif

// Fallback non simd version

FORCE_INLINE f32x4 f32x4_make(f32 x, f32 y, f32 z, f32 w) {
    f32x4 result;
    result.values[0] = x;
    result.values[1] = y;
    result.values[2] = z;
    result.values[3] = w;
    return result;
}

FORCE_INLINE f32x4 f32x4_add(f32x4 left, f32x4 right) {
    f32x4 result;
    result.values[0] = left.values[0] + right.values[0];
    result.values[1] = left.values[1] + right.values[1];
    result.values[2] = left.values[2] + right.values[2];
    result.values[3] = left.values[3] + right.values[3];
    return result;
}

FORCE_INLINE f32x4 f32x4_sub(f32x4 left, f32x4 right) {
    f32x4 result;
    result.values[0] = left.values[0] - right.values[0];
    result.values[1] = left.values[1] - right.values[1];
    result.values[2] = left.values[2] - right.values[2];
    result.values[3] = left.values[3] - right.values[3];
    return result;
}

FORCE_INLINE f16x4 f16x4_fromF32x4(f32x4 from) {
    f16x4 result;
    result.values[0] = f16_fromF32(from.values[0]);
    result.values[1] = f16_fromF32(from.values[1]);
    result.values[2] = f16_fromF32(from.values[2]);
    result.values[3] = f16_fromF32(from.values[3]);
    return result;
}

#if 0
FORCE_INLINE f32x4 f32x4_cos(f32x4 in) {
    float32x4_t result;
    result.values[0] = f32_cos(in.values[0]);
    result.values[1] = f32_cos(in.values[1]);
    result.values[2] = f32_cos(in.values[2]);
    result.values[3] = f32_cos(in.values[3]);
    return result;
}
#endif

#endif

INLINE f32 f32_equal(f32 val, f32 cmp, f32 delta) {
    return ((((val)-(cmp))> -(delta))&&(((val)-(cmp))<(delta)));
}

INLINE f32 f32_fromBits(u32 bits) {
    union { u32 u32; f32 f32;} result;
    result.u32 = bits;
    return result.f32;
}

INLINE u32 f32_toBits(f32 val) {
    union { u32 u32; f32 f32;} result;
    result.f32 = val;
    return result.u32;
}

INLINE f32 f32_trunc(f32 a) {
    return f32_cast(i32_cast(a));
}

INLINE f32 f32_frac(f32 a) {
    return a - f32_trunc(a);
}

INLINE bx f32_isNaN(f32 a) {
    const u32 tmp = f32_toBits(a) & i32_max;
    return tmp > u32_cast(0x7f800000);
}

INLINE bx f32_isInfite(f32 a) {
    const u32 tmp = f32_toBits(a) & i32_max;
    return tmp == u32_cast(0x7f800000);
}

INLINE bx f32_isFinite(f32 a) {
    const u32 tmp = f32_toBits(a) & i32_max;
    return tmp < u32_cast(0x7f800000);
}

INLINE f32 f32_toRad(f32 deg) {
    return deg * f32_pi / 180.0f;
}

INLINE f32 f32_toDeg(f32 rad) {
    return rad * 180.0f / f32_pi;
}

INLINE f32 f32_floor(f32 a) {
    if (a < 0.0f) {
        const f32 fr = f32_frac(-a);
        const f32 result = - a - fr;

        return -(0.0f != fr ? result + 1.0f : result);
    }

    return a - f32_frac(a);
}

INLINE f32 f32_ceil(f32 a) {
    return -f32_floor(-a);
}

INLINE f32 f32_round(f32 a) {
    return f32_floor(a + 0.5f);
}

INLINE f32 f32_sign(f32 a) {
    return a < 0.0f ? -1.0f : 1.0f;
}

INLINE f32 f32_abs(f32 a) {
    return a < 0.0f ? -a : a;
}

INLINE f32 f32_square(f32 a) {
    return a * a;
}

INLINE f32 f32_mad(f32 a, f32 b, f32 c) {
    return (a * b) + c;
}


INLINE i32 i32_random(i32 min, i32 max, u32* seed) {
    ASSERT(seed);
    if (min > max) {
        i32 temp = min;
        min = max;
        max = temp;
    }

    u32 range = max - min + 1;
    *seed ^= (*seed << 13);
    *seed ^= (*seed >> 17);
    *seed ^= (*seed << 5);
    return min + (i32)(i32_cast(*seed) % range);
}

INLINE f32 f32_random(f32 min, f32 max, u32* seed) {
    ASSERT(seed);
    if (min > max) {
        f32 temp = min;
        min = max;
        max = temp;
    }

    *seed ^= (*seed << 13);
    *seed ^= (*seed >> 17);
    *seed ^= (*seed << 5);
    f32 randomValue = (f32) (*seed);
    f32 normalizedValue = randomValue / f32_cast(u32_max);

    return min + normalizedValue * (max - min);
}

#define u32_and(A, B)    u32_cast(u32_cast(A) & u32_cast(B))
#define u32_or(A, B)     u32_cast(u32_cast(A) | u32_cast(B))
#define u32_sra(A, STR)  u32_cast(i32_cast(A) >> (STR))
#define u32_i32Add(A, B) u32_cast(i32_cast(A) + i32_cast(B))
#define u32_sll(A, SA)   u32_cast(u32_cast(A) << (SA))
#define u32_srl(A, SA)   u32_cast(u32_cast(A) >> (SA))

INLINE f32 f32_ldexp(f32 num, i32 _b) {
    const u32 ftob        = f32_toBits(num);
    const u32 masked      = u32_and(ftob, f32_signMask | f32_exponentMask);
    const u32 expsign0    = u32_sra(masked, 23);
    const u32 tmp         = u32_i32Add(expsign0, _b);
    const u32 expsign1    = u32_sll(tmp, 23);
    const u32 mantissa    = u32_and(ftob, f32_mantissaMask);
    const u32 bits        = u32_or(mantissa, expsign1);
    const f32    result   = f32_fromBits(bits);

    return result;
}

INLINE f32 f32_exp(f32 num) {
    static const f32 f32__expC0  =  1.66666666666666019037e-01f;
    static const f32 f32__expC1  = -2.77777777770155933842e-03f;
    static const f32 f32__expC2  =  6.61375632143793436117e-05f;
    static const f32 f32__expC3  = -1.65339022054652515390e-06f;
    static const f32 f32__expC4  =  4.13813679705723846039e-08f;

    if (f32_abs(num) <= f32_nearZero) {
        return num + 1.0f;
    }

    const f32 kk     = f32_round(num*f32_invLogNat2);
    const f32 hi     = num - kk*f32_logNat2Hi;
    const f32 lo     =       kk*f32_logNat2Lo;
    const f32 hml    = hi - lo;
    const f32 hmlsq  = f32_square(hml);
    const f32 tmp0   = f32_mad(f32__expC4, hmlsq, f32__expC3);
    const f32 tmp1   = f32_mad(tmp0,   hmlsq, f32__expC2);
    const f32 tmp2   = f32_mad(tmp1,   hmlsq, f32__expC1);
    const f32 tmp3   = f32_mad(tmp2,   hmlsq, f32__expC0);
    const f32 tmp4   = hml - hmlsq * tmp3;
    const f32 tmp5   = hml*tmp4/(2.0f-tmp4);
    const f32 tmp6   = 1.0f - ( (lo - tmp5) - hi);
    const f32 result = f32_ldexp(tmp6, i32_cast(kk) );

    return result;
}

INLINE f32 f32_frexp(f32 num, i32* _outExp) {
    const u32 ftob     = f32_toBits(num);
    const u32 masked0  = u32_and(ftob, u32_cast(0x7f800000) );
    const u32 exp0     = u32_srl(masked0, 23);
    const u32 masked1  = u32_and(ftob,   u32_cast(0x807fffff) );
    const u32 bits     = u32_or(masked1, u32_cast(0x3f000000) );
    const f32 result   = f32_fromBits(bits);

    *_outExp = i32_cast(exp0 - 0x7e);

    return result;
}

INLINE f32 f32_log(f32 num) {
    static const f32 f32__logC0 = 6.666666666666735130e-01f;
    static const f32 f32__logC1 = 3.999999999940941908e-01f;
    static const f32 f32__logC2 = 2.857142874366239149e-01f;
    static const f32 f32__logC3 = 2.222219843214978396e-01f;
    static const f32 f32__logC4 = 1.818357216161805012e-01f;
    static const f32 f32__logC5 = 1.531383769920937332e-01f;
    static const f32 f32__logC6 = 1.479819860511658591e-01f;

    int32_t exp;
    f32 ff = f32_frexp(num, &exp);
    if (ff < f32_sqrt2*0.5f) {
        ff *= 2.0f;
        --exp;
    }

    ff -= 1.0f;
    const f32 kk     = f32_cast(exp);
    const f32 hi     = kk*f32_logNat2Hi;
    const f32 lo     = kk*f32_logNat2Lo;
    const f32 ss     = ff / (2.0f + ff);
    const f32 s2     = f32_square(ss);
    const f32 s4     = f32_square(s2);

    const f32 tmp0   = f32_mad(f32__logC6, s4, f32__logC4);
    const f32 tmp1   = f32_mad(tmp0,   s4, f32__logC2);
    const f32 tmp2   = f32_mad(tmp1,   s4, f32__logC0);
    const f32 t1     = s2*tmp2;

    const f32 tmp3   = f32_mad(f32__logC5, s4, f32__logC3);
    const f32 tmp4   = f32_mad(tmp3,   s4, f32__logC1);
    const f32 t2     = s4*tmp4;

    const f32 t12    = t1 + t2;
    const f32 hfsq   = 0.5f * f32_square(ff);
    const f32 result = hi - ( (hfsq - (ss*(hfsq+t12) + lo) ) - ff);

    return result;
}


INLINE f32 f32_pow(f32 num, f32 _b) {
    return f32_exp(_b * f32_log(num));
}

INLINE f32 f32_sqrt(f32 num) {
    if (num < 0.0f) {
        ASSERT(!"num is negative, you probably never want this...");
        return f32_nan;
    } else if (num < f32_nearZero) {
        return 0;
    }
#ifdef SIMD_USE_SSE
    __m128 In = _mm_set_ss(num);
    __m128 Out = _mm_sqrt_ss(In);
    return _mm_cvtss_f32(Out);
#else
    return num * f32_pow(num, -0.5f);
#endif
}

INLINE f32 f32_mod(f32 a, f32 b) {
    return a - b * f32_floor(a / b); 
}

INLINE f32 f32_cos(f32 a) {
#if USE_SIMD
    f32x4 inx4 = f32x4_make(a, 0, 0, 0);
    return f32x4_cos(inx4).x;
#else
    static const f32 f32__sinC2  = -0.16666667163372039794921875f;
    static const f32 f32__sinC4  =  8.333347737789154052734375e-3f;
    static const f32 f32__sinC6  = -1.9842604524455964565277099609375e-4f;
    static const f32 f32__sinC8  =  2.760012648650445044040679931640625e-6f;
    static const f32 f32__sinC10 = -2.50293279435709337121807038784027099609375e-8f;

    static const f32 f32__cosC2  = -0.5f;
    static const f32 f32__cosC4  =  4.166664183139801025390625e-2f;
    static const f32 f32__cosC6  = -1.388833043165504932403564453125e-3f;
    static const f32 f32__cosC8  =  2.47562347794882953166961669921875e-5f;
    static const f32 f32__cosC10 = -2.59630184018533327616751194000244140625e-7f;

    const f32 scaled = a * 2.0f * f32_invPi;
    const f32 real   = f32_floor(scaled);
    const f32 xx     = a - real * f32_piHalf;
    const i32 reali  = i32_cast(real);
    const i32 bits   = reali & 3;

    f32 c0, c2, c4, c6, c8, c10;

    if (bits == 0 || bits == 2) {
        c0  = 1.0f;
        c2  = f32__cosC2;
        c4  = f32__cosC4;
        c6  = f32__cosC6;
        c8  = f32__cosC8;
        c10 = f32__cosC10;
    } else {
        c0  = xx;
        c2  = f32__sinC2;
        c4  = f32__sinC4;
        c6  = f32__sinC6;
        c8  = f32__sinC8;
        c10 = f32__sinC10;
    }

    const f32 xsq    = f32_square(xx);
    const f32 tmp0   = f32_mad(c10,  xsq, c8 );
    const f32 tmp1   = f32_mad(tmp0, xsq, c6 );
    const f32 tmp2   = f32_mad(tmp1, xsq, c4 );
    const f32 tmp3   = f32_mad(tmp2, xsq, c2 );
    const f32 tmp4   = f32_mad(tmp3, xsq, 1.0);
    const f32 result = tmp4 * c0;

    return (bits == 1 || bits == 2) ? -result : result;
#endif
}

INLINE f32 f32_sin(f32 a) {
    return f32_cos(a - f32_piHalf);
}

INLINE f32 f32_acos(f32 num) {
    static const f32 f32__aCosC0 =  1.5707288f;
    static const f32 f32__aCosC1 = -0.2121144f;
    static const f32 f32__aCosC2 =  0.0742610f;
    static const f32 f32__aCosC3 = -0.0187293f;

    const f32 absa   = absVal(num);
    const f32 tmp0   = f32_mad(f32__aCosC3, absa, f32__aCosC2);
    const f32 tmp1   = f32_mad(tmp0,    absa, f32__aCosC1);
    const f32 tmp2   = f32_mad(tmp1,    absa, f32__aCosC0);
    const f32 tmp3   = tmp2 * f32_sqrt(1.0f - absa);
    const f32 negate = f32_cast(num < 0.0f);
    const f32 tmp4   = tmp3 - 2.0f*negate*tmp3;
    const f32 result = negate * f32_pi + tmp4;

    return result;
}

INLINE f32 f32_tan(f32 a) {
    return f32_sin(a) / f32_cos(a);
}

INLINE f32 f32_atan2(f32 _y, f32 _x) {
    static const f32 f32__aTtan2C0 = -0.013480470f;
    static const f32 f32__aTtan2C1 =  0.057477314f;
    static const f32 f32__aTtan2C2 = -0.121239071f;
    static const f32 f32__aTtan2C3 =  0.195635925f;
    static const f32 f32__aTtan2C4 = -0.332994597f;
    static const f32 f32__aTtan2C5 =  0.999995630f;

    const f32 ax     = absVal(_x);
    const f32 ay     = absVal(_y);
    const f32 maxaxy = maxVal(ax, ay);
    const f32 minaxy = minVal(ax, ay);

    if (maxaxy == 0.0f) {
        return 0.0f * f32_sign(_y);
    }

    const f32 mxy    = minaxy / maxaxy;
    const f32 mxysq  = f32_square(mxy);
    const f32 tmp0   = f32_mad(f32__aTtan2C0, mxysq, f32__aTtan2C1);
    const f32 tmp1   = f32_mad(tmp0,     mxysq, f32__aTtan2C2);
    const f32 tmp2   = f32_mad(tmp1,     mxysq, f32__aTtan2C3);
    const f32 tmp3   = f32_mad(tmp2,     mxysq, f32__aTtan2C4);
    const f32 tmp4   = f32_mad(tmp3,     mxysq, f32__aTtan2C5);
    const f32 tmp5   = tmp4 * mxy;
    const f32 tmp6   = ay > ax   ? f32_piHalf - tmp5 : tmp5;
    const f32 tmp7   = _x < 0.0f ? f32_pi     - tmp6 : tmp6;
    const f32 result = f32_sign(_y) * tmp7;

    return result;
}

INLINE f32 f32_toRadians(f32 degrees) {
    return degrees * (f32_pi / 180.0f);
}

#define f32_invSqrt(VAL) (1.0f / f32_sqrt(f32Val))

INLINE f32 f32_lerp(f32 a, f32 time, f32 b) {
    return (1.0f - time) * a + time * b;
}

INLINE f32 f32_clamp(f32 min, f32 value, f32 max) {
    f32 result = value;

    if (result < min) {
        result = min;
    }

    if (result > max) {
        result = max;
    }

    return result;
}


#pragma mark - Vec2

INLINE Vec2 v2_make(f32 x, f32 y) {
    Vec2 result;
    result.x = x;
    result.y = y;

    return result;
}

INLINE Vec2 v2_makeZero(void) {
    Vec2 result;
    result.x = 0;
    result.y = 0;

    return result;
}

INLINE Vec2 v2_makeF32(f32 val) {
    Vec2 result;
    result.x = val;
    result.y = val;

    return result;
}

INLINE Vec2 v2_max(Vec2 left, Vec2 right) {
    Vec2 result;
    result.x = maxVal(left.x, right.x);
    result.y = maxVal(left.y, right.y);

    return result;
}

INLINE Vec2 v2_min(Vec2 left, Vec2 right) {
    Vec2 result;
    result.x = minVal(left.x, right.x);
    result.y = minVal(left.y, right.y);

    return result;
}

INLINE Vec2 v2_zero(void) {
    Vec2 vec;
    vec.x = 0;
    vec.y = 0;

    return vec;
}

INLINE f32 v2_leng(Vec2 left) {
    return f32_sqrt(left.x * left.x + left.y * left.y);
}

INLINE Vec2 v2_add(Vec2 left, Vec2 right) {
    left.x += right.x;
    left.y += right.y;

    return left;
}

INLINE Vec2 v2_sub(Vec2 left, Vec2 right) {
    left.x -= right.x;
    left.y -= right.y;

    return left;

}

INLINE Vec2 v2_neg(Vec2 left) {
    left.x = -left.x;
    left.y = -left.y;

    return left;
}

INLINE Vec2 v2_mult(Vec2 left, Vec2 right) {
    left.x *= right.x;
    left.y *= right.y;

    return left;
}

INLINE Vec2 v2_multF32(Vec2 left, f32 scale) {
    left.x *= scale;
    left.y *= scale;

    return left;
}
INLINE Vec2 v2_div(Vec2 left, Vec2 right) {
    left.x /= right.x;
    left.y /= right.y;

    return left;
}


#pragma mark - Vec3

INLINE Vec3 vec3_make(f32 x, f32 y, f32 z) {
    Vec3 result;
    result.x = x;
    result.y = y;
    result.z = z;

    return result;
}

INLINE Vec3 vec3_zero(void) {
    Vec3 result;
    result.x = 0;
    result.y = 0;
    result.z = 0;

    return result;
}

INLINE Vec3 vec3_splat(f32 val) {
    Vec3 result;
    result.x = val;
    result.y = val;
    result.z = val;

    return result;
}

INLINE f32 vec3_length(Vec3 left) {
    return f32_sqrt(left.x * left.x + left.y * left.y + left.z * left.z);
}

INLINE Vec3 vec3_add(Vec3  left, Vec3 right) {
    left.x += right.x;
    left.y += right.y;
    left.z += right.z;

    return left;
}

INLINE Vec3 vec3_subtract(Vec3 left, Vec3 right) {
    left.x -= right.x;
    left.y -= right.y;
    left.z -= right.z;

    return left;
}

INLINE Vec3 vec3_negative(Vec3 left) {
    left.x = -left.x;
    left.y = -left.y;
    left.z = -left.z;

    return left;
}

INLINE Vec3 vec3_multiply(Vec3 left, Vec3 right) {
    left.x *= right.x;
    left.y *= right.y;
    left.z *= right.z;

    return left;
}

INLINE Vec3 vec3_normalize(Vec3 A) {
    Vec3 result = vec3_zero();

    f32 vectorLength = vec3_length(A);

    if (vectorLength != 0.0f) {
        result.x = A.x * (1.0f / vectorLength);
        result.y = A.y * (1.0f / vectorLength);
        result.z = A.z * (1.0f / vectorLength);
    }
    
    return result;
}

INLINE Vec3 vec3_cross(Vec3 vecOne, Vec3 vecTwo) {
    Vec3 result = vec3_zero();

    result.x = (vecOne.y * vecTwo.z) - (vecOne.z * vecTwo.y);
    result.y = (vecOne.z * vecTwo.x) - (vecOne.x * vecTwo.z);
    result.z = (vecOne.x * vecTwo.y) - (vecOne.y * vecTwo.x);

    return result;
}

INLINE f32 vec3_dotVec3(Vec3 vecOne, Vec3 vecTwo) {
    f32 result = 0.0f;

    result = (vecOne.x * vecTwo.x) + (vecOne.y * vecTwo.y) + (vecOne.z * vecTwo.z);
    
    return result;
}


#pragma mark - Vec4

INLINE Vec4 v4_make(f32 x, f32 y, f32 z, f32 w) {
    Vec4 result;
    result.simd = f32x4_make(x, y, z, w);

    return result;
}

INLINE Vec4 v4_makeF32(f32 x) {
    Vec4 result;
    result.simd = f32x4_make(x, x, x, x);

    return result;
}

INLINE Vec4 v4_zero(void) {
    Vec4 result;
    result.simd = f32x4_make(0, 0, 0, 0);

    return result;
}

INLINE Vec4 v4_add(Vec4 left, Vec4 right) {
    Vec4 result;
    result.simd = f32x4_add(left.simd, right.simd);

    return result;
}

INLINE Vec4 v4_sub(Vec4 left, Vec4 right) {
    Vec4 result;
    result.simd = f32x4_sub(left.simd, right.simd);

    return result;
}


#pragma mark - Mat4

INLINE Mat4 mat4_zero(void) {
    Mat4 result = {{0}};

    return result;
}

INLINE Mat4 mat4_diag(f32 diagonal) {
    Mat4 result = mat4_zero();

    result.store4x4[0][0] = diagonal;
    result.store4x4[1][1] = diagonal;
    result.store4x4[2][2] = diagonal;
    result.store4x4[3][3] = diagonal;

    return result;
}

INLINE Mat4 mat4_multiply(Mat4 left, Mat4 right) {
    Mat4 result = mat4_zero();

#ifdef SIMD_USE_SSE
#error "Not implemented"
    for (int i = 0; i < 4; i++) {
        // Load a row of matrix A into a SIMD register
        __m128 rowA = _mm_loadu_ps(&A[i][0]);

        for (int j = 0; j < 4; j++) {
            // Load a column of matrix B into a SIMD register
            __m128 colB = _mm_set_ps(B[0][j], B[1][j], B[2][j], B[3][j]);

            // Multiply corresponding elements and accumulate
            __m128 prod = _mm_mul_ps(rowA, colB);

            // Horizontal addition of the four values in the SIMD register
            // This results in the dot product of the row and column
            __m128 sum = _mm_hadd_ps(prod, prod);
            sum = _mm_hadd_ps(sum, sum);

            // Store the result in the corresponding position in the result matrix
            result.store4x4[i][j] = _mm_cvtss_f32(sum);
        }
    }
#elif defined(SIMD_USE_NEON)
#error "Not implemented"
    for (int i = 0; i < 4; i++) {
        float4 rowA = A->row[i];

        for (int j = 0; j < 4; j++) {
            float4 colB = {B->row[0][j], B->row[1][j], B->row[2][j], B->row[3][j]};

            // Perform element-wise multiplication
            float4 prod = vmulq_f32(rowA, colB);

            // Perform horizontal addition
            float32x2_t sum2 = vadd_f32(vget_low_f32(prod), vget_high_f32(prod));
            float32x2_t sum = vpadd_f32(sum2, sum2);

            // Store the result in the corresponding position in the result matrix
            result->row[i][j] = vget_lane_f32(sum, 0);
        }
    }
#else
    int columns;
    for (columns = 0; columns < 4; ++columns) {
        int rows;
        for (rows = 0; rows < 4; ++rows) {
            f32 sum = 0;
            int currentMatrice;
            for (currentMatrice = 0; currentMatrice < 4; ++currentMatrice) {
                sum += left.store4x4[currentMatrice][rows] * right.store4x4[columns][currentMatrice];
            }
            result.store4x4[columns][rows] = sum;
        }
    }
#endif 
    return result;
}

INLINE Mat4 mat4_perspective(f32 fov, f32 aspectRatio, f32 nearX, f32 farX) {
    Mat4 result = mat4_diag(1.0f);

    f32 tanThetaOver2 = f32_tan(fov * (f32_pi / 360.0f));
    
    result.store4x4[0][0] = 1.0f / tanThetaOver2;
    result.store4x4[1][1] = aspectRatio / tanThetaOver2;
    result.store4x4[2][3] = -1.0f;
    result.store4x4[2][2] = (nearX + farX) / (nearX - farX);
    result.store4x4[3][2] = (2.0f * nearX * farX) / (nearX - farX);
    result.store4x4[3][3] = 0.0f;

    return result;
}

INLINE Mat4 mat4_lookAt(Vec3 eye, Vec3 center, Vec3 up) {
    Mat4 result = mat4_zero();

    Vec3 f = vec3_normalize(vec3_subtract(center, eye));
    Vec3 s = vec3_normalize(vec3_cross(f, up));
    Vec3 u = vec3_cross(s, f);

    result.store4x4[0][0] =  s.x;
    result.store4x4[0][1] =  u.x;
    result.store4x4[0][2] = -f.x;

    result.store4x4[1][0] =  s.y;
    result.store4x4[1][1] =  u.y;
    result.store4x4[1][2] = -f.y;

    result.store4x4[2][0] =  s.z;
    result.store4x4[2][1] =  u.z;
    result.store4x4[2][2] = -f.z;

    result.store4x4[3][0] = -vec3_dotVec3(s, eye);
    result.store4x4[3][1] = -vec3_dotVec3(u, eye);
    result.store4x4[3][2] =  vec3_dotVec3(f, eye);
    result.store4x4[3][3] = 1.0f;

    return result;
}

INLINE Mat4 mat4_rotate(f32 Angle, Vec3 axis) {
    Mat4 result = mat4_diag(1.0f);
    
    axis = vec3_normalize(axis);
    
    f32 sinTheta = f32_sin(f32_toRadians(Angle));
    f32 cosTheta = f32_cos(f32_toRadians(Angle));
    f32 cosValue = 1.0f - cosTheta;
    
    result.store4x4[0][0] = (axis.x * axis.x * cosValue) + cosTheta;
    result.store4x4[0][1] = (axis.x * axis.y * cosValue) + (axis.z * sinTheta);
    result.store4x4[0][2] = (axis.x * axis.z * cosValue) - (axis.y * sinTheta);
    
    result.store4x4[1][0] = (axis.y * axis.x * cosValue) - (axis.z * sinTheta);
    result.store4x4[1][1] = (axis.y * axis.y * cosValue) + cosTheta;
    result.store4x4[1][2] = (axis.y * axis.z * cosValue) + (axis.x * sinTheta);
    
    result.store4x4[2][0] = (axis.z * axis.x * cosValue) + (axis.y * sinTheta);
    result.store4x4[2][1] = (axis.z * axis.y * cosValue) - (axis.x * sinTheta);
    result.store4x4[2][2] = (axis.z * axis.z * cosValue) + cosTheta;
    
    return result;
}

///////////////////////////////////////////
// BASE ARGS //////////////////////////////
///////////////////////////////////////////

typedef enum arg_errors {
    arg_error_duplicate    = '&',
    arg_error_missingArg   = '=',
    arg_error_missingValue = '+',
    arg_error_invalidValue = '!',
} arg_errors;

static const i32 arg_end = -1;

typedef enum arg_flags {
    arg_flag_none               = 0,
    arg_flag_required           = 1 << 0,
    arg_flag_requiredValue      = 1 << 1,
    arg_flag_duplicatesAllowed  = 1 << 2,
} arg_flags;

typedef enum arg_optType {
    arg_optType_none,
    arg_optType_string,
    arg_optType_f32,
    arg_optType_i32,
    arg_optType_flags,
} arg_optType;

typedef struct arg_Opt {
    S8 name;
    i32 shortName;
    arg_optType type;
    u32 flags;
    S8 defaultValue;
    S8 desc;
    S8 helpText;
} arg_Opt;

typedef struct arg_Ctx {
    arg_Opt* opts;
    u32 optCount;
    char** inputOpts;
    u32 inputOptsCount;
    u64 redArgsBitField;
    u32 nextIdx;
    arg_Opt* currentOpt;
    S8 foundKey;
    S8 foundValue;
    i32 foundi32Value;
    f32 foundF32Value;
    u32 foundFlagsValue;
} arg_Ctx;

API arg_Ctx arg_makeCtx(arg_Opt* opts, u32 optsCount, char** inputOpts, u32 inputOptsCount);

API S8 arg_createHelpText(Arena* arena, arg_Ctx* ctx);

API i32 arg_nextOpt(arg_Ctx* ctx);


///////////////////////////////////////////
// BASE TIME //////////////////////////////
///////////////////////////////////////////

#pragma mark - Date time

API DenseTime tm_toDenseTime(DateTime in);
API DateTime tm_fromDenseTime(DenseTime in);
API DateTime tm_nowGm(void);
API DateTime tm_nowLocal(void);

#pragma mark - High resolution time measurement

API tm_FrequencyInfo tm_getPerformanceFrequency(void);
API u64 tm_currentCount(void);
API u64 tm_countToNanoseconds(tm_FrequencyInfo info, i64 count);
API u64 tm_roundToCommonRefreshRate(u64 nanoSeconds);

#define tm_countToSeconds(COUNT) (f64_cast(COUNT) / 1000000000.0f)
#define tm_countToMiliseconds(COUNT) (f64_cast(COUNT) / 1000000.0f)
#define tm_countToMicroSeconds(COUNT) (f64_cast(COUNT) / 1000.0f)
#define tm_countToNanoSeconds(COUNT) (f64_cast(COUNT))

///////////////////////////////////////////
// BASE ATOMOIC ///////////////////////////
///////////////////////////////////////////

#ifdef _MSC_VER
// #include <winnt.h>
#include <windows.h>
#include <intrin.h>
//typedef u32 volatile A32;
//typedef u64 volatile A64;
#define a8_compareAndSwap(dst, expected, desired)  ((u8)  _InterlockedCompareExchange8((volatile char*)dst, (char)desired, (char)expected))
#define a16_compareAndSwap(dst, expected, desired) ((u16) _InterlockedCompareExchange16((volatile short*)dst, (short)desired, (short)expected))
#define a32_compareAndSwap(dst, expected, desired) ((u32) _InterlockedCompareExchange32((volatile long*)dst, (long)desired, (long)expected))
#define a64_compareAndSwap(dst, expected, desired) ((u64) _InterlockedCompareExchange64((volatile long long*)dst, (long long)desired, (long long)expected))
#define a32_loadVALPTR) InterlockedOr(VALPTR, 0)
#define a64_load(VALPTR) InterlockedOr64(VALPTR, 0)
#define a32_store(dst, val) _InterlockedExchange((volatile long*)dst, (u32)val)
#define a64_store(dst, val) _InterlockedExchange64((volatile long long*)dst, (u64)val)

#define a32_add(dst, val) _InterlockedExchangeAdd((a32*) dst, (u32) val)
#define a64_add(dst, val) _InterlockedExchangeAdd64((a64*) dst, (u32) val)

// these intrinsics are x86 only
#define u32_bitScanNonZero(BITFIELD) u32_cast(_lzcnt_u32(BITFIELD))
#define u64_bitScanNonZero(BITFIELD) u64_cast(_lzcnt_u64(BITFIELD))
#define u32_bitScanReverseNonZero(BITFIELD) u32_cast(_tzcnt_u32(BITFIELD))
#define u64_bitScanReverseNonZero(BITFIELD) u64_cast(_tzcnt_u64(BITFIELD))
#define u32_popCount(BITFIELD) u32_cast(__popcnt(BITFIELD))
#define u64_popCount(BITFIELD) u64_cast(_mm_popcnt_u64(BITFIELD))

#else
#include <stdatomic.h>
//typedef _Atomic(u32) A32;
//typedef _Atomic(u64) A64;
// these are clang/GCC specific
#define a8_compareAndSwap(dst, expected, desired)       ((u8 ) __sync_val_compare_and_swap(dst, expected, desired))
#define a16_compareAndSwap(dst, expected, desired)      ((u16) __sync_val_compare_and_swap(dst, expected, desired))
#define a32_compareAndSwap(dst, expected, desired)      ((u32) __sync_val_compare_and_swap(dst, expected, desired))
#define a64_compareAndSwap(dst, expected, desired)      ((u64) __sync_val_compare_and_swap(dst, expected, desired))
#define a32_load(VALPTR)  __atomic_load_n(VALPTR, __ATOMIC_SEQ_CST)
#define a64_load(VALPTR)  __atomic_load_n(VALPTR, __ATOMIC_SEQ_CST)
#define a32_store(dst, val) __sync_lock_test_and_set((u32*)dst, (u32)val)
#define a64_store(dst, val) __sync_lock_test_and_set((u64*)dst, (u64)val)
#define a32_add(dst, val) __atomic_fetch_add((u32*) dst, (u32) val, __ATOMIC_SEQ_CST)
#define a64_add(dst, val) __atomic_fetch_add((u64*) dst, (u64) val, __ATOMIC_SEQ_CST)

#define u32_bitScanNonZero(BITFIELD) u32_cast(__builtin_clz(BITFIELD))
#define u64_bitScanNonZero(BITFIELD) u64_cast(__builtin_clzll(BITFIELD))
#define u32_bitScanReverseNonZero(BITFIELD) u32_cast(__builtin_ctz(BITFIELD))
#define u64_bitScanReverseNonZero(BITFIELD) u64_cast(__builtin_ctzll(BITFIELD))
#define u32_popCount(BITFIELD) u32_cast(__builtin_popcount(BITFIELD))
#define u64_popCount(BITFIELD) u64_cast(__builtin_popcountll(BITFIELD))
#endif

static inline bool a32_compareAndSwapFull(a32* dst, a32* expected, a32 desired) {
    u32 old = a32_compareAndSwap(dst, *expected, desired);
    if (old != *expected) {
        *expected = old;
        return false;
    }
    return true;
}
static inline bool a64_compareAndSwapFull(a64* dst, a64* expected, a64 desired) {
    a64 old = a64_compareAndSwap(dst, *expected, desired);
    if (old != *expected) {
        *expected = old;
        return false;
    }
    return true;
}

///////////////////////////////////////////
// OS /////////////////////////////////////
///////////////////////////////////////////

/////////////////////////
// Memory Fuctions
#if OS_EMSCRIPTEN
#define OS_VIRTUAL_MEMORY 0
#else
#define OS_VIRTUAL_MEMORY 1
#endif // OS_EMSCRIPTEN

API u32 os_coreCount(void);

API umm os_getProcessMemoryUsed(void);
API u32 os_memoryPageSize(void);

API void* os_memoryReserve(u64 size);
API void  os_memoryCommit(void* ptr, u64 size);
API void  os_memorydecommit(void* ptr, u64 size);
API void  os_memoryRelease(void* ptr, u64 size);

API BaseMemory os_getBaseMemory(void);

/////////////////////////
// time related functionality

API DateTime os_timeUniversalNow(void);
API DateTime os_timeLocalFromUniversal(DateTime* date_time);
API DateTime os_timeUniversalFromLocal(DateTime* date_time);
API u64 os_timeMicrosecondsNow(void);


/////////////////////////
// File handling

API S8 os_fileRead(Arena* arena, S8 fileName);
API bx os_fileWrite(S8 fileName, S8 data);
API bx os_fileDelete(S8 fileName);
API bx os_fileExists(S8 fileName);
typedef enum os_systemPath {
	os_systemPath_currentDir,
	os_systemPath_binary,
	os_systemPath_userData,
	os_systemPath_tempData,
} os_systemPath;

API S8 os_filepath(Arena* arena, os_systemPath path);
API S8 os_workingPath(Arena* arena);

INLINE S8 os_getDirectoryFromFilepath(S8 filepath) {
    i64 lastSlash = str_lastIndexOfChar(filepath, '/');
    if (lastSlash == -1) {
        lastSlash = str_lastIndexOfChar(filepath, '\\');
    }
    S8 result = filepath;
    if (lastSlash != -1) {
        result.size = lastSlash;
    }
    return result;
}
API S8* os_pathList(Arena* arena, S8 pathName, u64* pathCount);
API bx os_pathMakeDirectory(S8 pathName);
API bx os_pathDirectoryExist(S8 pathName);

/////////////////////////
// File Helper

INLINE S8 os_fixFilepath(Arena* arena, S8 path) {
    S8 resultPath = STR_NULL;
    mem_scoped(scratch, arena) {
        resultPath = str_alloc(arena, path.size);

        S8 fixedPath = path;
        fixedPath = str_replaceAll(scratch.arena, fixedPath, s8("\\"), s8("/"));
        fixedPath = str_replaceAll(scratch.arena, fixedPath, s8("/./"), s8("/"));
        while (true) {
            u64 dotdot = str_findFirst(fixedPath, s8(".."), 0);
            if (dotdot == fixedPath.size) break;
            
            u64 lastSlash = str_findLast(fixedPath, s8("/"), dotdot - 1);
            
            u64 range = (dotdot + 3) - lastSlash;
            S8 old = fixedPath;
            fixedPath = str_alloc(scratch.arena, fixedPath.size - range);
            memcpy(fixedPath.content, old.content, lastSlash);
            memcpy(fixedPath.content + lastSlash, old.content + dotdot + 3, old.size - range - lastSlash + 1);
        }
        ASSERT(fixedPath.size >= path.size);
        mem_copy(resultPath.content, fixedPath.content, fixedPath.size);
        resultPath.size = fixedPath.size;
        // manipulate scratch to not free our result path and only free the temporary strings
        scratch.start += resultPath.size;
    }
    return resultPath;
}

/////////////////////////
// File properties

typedef enum os_DataAccessFlags {
    os_dataAccessFlag_read    = (1 << 0),
    os_dataAccessFlag_write   = (1 << 1),
    os_dataAccessFlag_execute = (1 << 2),
} os_DataAccessFlags;

typedef enum os_FilePropertyFlags {
    os_filePropertyFlag_isFolder = (1 << 0),
} os_FilePropertyFlags;

typedef struct os_FileProperties {
    os_FilePropertyFlags flags;
    os_DataAccessFlags access;
    u64 size;
    DenseTime creationTime;
    DenseTime lastChangeTime;
} os_FileProperties;

API os_FileProperties os_fileProperties(S8 fileName);


/////////////////////////
// Filesystem watching

typedef struct os_fsPathWatchId {
    u32 id;
} os_fsPathWatchId;

typedef enum os_fsAction {
    os_fsAction_create = 1,
    os_fsAction_delete,
    os_fsAction_modifiy,
    os_fsAction_move
} os_fsAction;

typedef enum os_fsPathWatchFlag {os_fsPathTrackFlag_none = 0x0} os_fsPathWatchFlag;
typedef flags32 os_fsPathWatchFlags;

typedef void(os_fsPathWatchCallback)(os_fsPathWatchId id, S8 path, os_fsAction fsAction, void* custom);

typedef struct os_fsPathWatchCtx os_fsPathWatchCtx;
API os_fsPathWatchId os_fsWatchPathStart(os_fsPathWatchCtx* ctx, S8 folder, os_fsPathWatchFlags trackFlags, os_fsPathWatchCallback* callback, void* custom);
API void os_fsWatchPathStop(os_fsPathWatchCtx* ctx, os_fsPathWatchId handle);

API os_fsPathWatchCtx* os_fsWatchPathCreatCtx(Arena* arena);
API void os_fsWatchPathDestroyCtx(os_fsPathWatchCtx* ctx);
API void os_fsWatchPathTick(os_fsPathWatchCtx* ctx);


/////////////////////////
// Execute

API void* os_execute(Arena* tmpArena, S8 execPath, S8* args, u32 argCount);

/////////////////////////
// Dynamic library

typedef struct os_Dl os_Dl;

API os_Dl* os_dlOpen(S8 filePath);
API void   os_dlClose(os_Dl* handle);
API void*  os_dlSym(os_Dl* handle, const char* symbol);

/////////////////////////
// Threading Related

API void os_sleep(u32 ms);
API void os_yield(void);

/////////////////////////
// Logging
void os_log(S8 msg);

/////////////////////////
// Mutex
typedef struct os_Mutex {
    ALIGN_DECL(16, u8) internal[64];
} os_Mutex;

API void os_mutexInit(os_Mutex* mutex);
API void os_mutexDestroy(os_Mutex* mutex);
API void os_mutexLock(os_Mutex* mutex);
API bx   os_mutexTryLock(os_Mutex* mutex);
API void os_mutexUnlock(os_Mutex* mutex);

API u32 os__mutexLockRet(os_Mutex* mutex);
#define os_mutexScoped(mutex) for (u32 iii = (0, os__mutexLockRet(mutex), 0); iii == 0; (iii++, os_mutexUnlock(mutex)))

/////////////////////////
// Semaphores
typedef struct os_Semaphore {
    ALIGN_DECL(16, uint8_t) internal[128];
} os_Semaphore;

API void os_semaphoreInit(os_Semaphore* sem);
API void os_semaphorePost(os_Semaphore* sem, u32 count);
API bool os_semaphoreWait(os_Semaphore* sem, i32 count);
API void os_semaphoreDestroy(os_Semaphore* sem);

/////////////////////////
// Threads
struct os_Thread;
typedef i32 (os_threadFunc)(struct os_Thread* thread, void* userData);
typedef struct os_Thread {
    ALIGN_DECL(16, u8) internal[64];
    os_threadFunc*     entryFunc;
    void*              userData;
    os_Semaphore       sem;
    u32                stackSize;
    i32                exitCode;
    bool               running;
} os_Thread;

API bool os_threadCreate(os_Thread* thread, os_threadFunc threadFunc, void* userData, u32 stackSize, S8 name);
API void os_threadShutdown(os_Thread* thread);
API bool os_threadIsRunning(os_Thread* thread);
API i32  os_threadGetExitCode(os_Thread* thread);
API void os_threadSetName(os_Thread* thread, S8 str);


///////////////////////////////////////////
// URL PARSER /////////////////////////////
///////////////////////////////////////////

typedef enum url_errorCode {
	url_errorCode_ok = 0,
	url_errorCode_scheme,
	url_errorCode_userOrPass,
	url_errorCode_hostOrPort,
	url_errorCode_path,
	url_errorCode_query,
	url_errorCode_fragment,
} url_errorCode;

/**
 * Struct describing a parsed url.
 *
 * @example <scheme>://<user>:<pass>@<host>:<port>/<path>?<query>#<fragment>
 */
typedef struct Url {
	url_errorCode error;
	S8  		  scheme;
	S8  		  user;
	S8  		  pass;
	S8  		  host;
	i32   		  port;
	S8  		  path;
	S8  		  query;
	S8  		  fragment;
} Url;

/**
 * Parse an url specified by RFC1738 into its parts.
 *
 * @param urlStr url to parse.
 *
 * @return parsed url. If mem is NULL this value will need to be free:ed with free().
 */
API Url url_fromStr(S8 urlStr);
#pragma warning(pop)
#endif // _BASE_H_