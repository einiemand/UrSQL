#pragma once
#ifndef MACROS_HPP
#define MACROS_HPP

#include <cstdlib>

// Platform macros
#ifdef __linux__
#define OSPLATFORM_LINUX
#elif defined(__APPLE__)
#define OSPLATFORM_MACOS
#elif defined(_WIN32)
#define OSPLATFORM_WINDOWS
#else
// unknown os
#endif

// Compiler macros
#ifdef __GNUC__
#define COMPILER_GNUC
#elif defined(__MINGW64__)
#define COMPILER_MINGW64
#elif defined(_MSC_VER)
#define COMPILER_MSVC
#else
// other compilers not supported
#endif

// Helper macros
#define DISABLE_COPY_CTOR(Class) Class(const Class&) = delete
#define DISABLE_COPY_ASSIGN(Class) Class& operator=(const Class&) = delete
#define DISABLE_COPY(Class) \
    DISABLE_COPY_CTOR(Class); \
    DISABLE_COPY_ASSIGN(Class)

#ifdef URSQL_DEBUG

#define URSQL_TRUTH(truth) \
    do { \
        if (!(truth)) std::abort(); \
    } while(false)

#define URSQL_MUST_INLINE inline

#else

#define URSQL_TRUTH()

#if defined(COMPILER_GNUC) || defined(COMPILER_MINGW64)
#define URSQL_MUST_INLINE __attribute__((always_inline))
#elif defined(COMPILER_MSVC)
#define URSQL_MUST_INLINE __forceinline
#endif

#endif

#endif /* Macros.hpp */
