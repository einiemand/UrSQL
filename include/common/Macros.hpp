#pragma once

#include <cstdlib>

// Helper macros
#define URSQL_DISABLE_COPY_CTOR(Class) Class(const Class&) = delete
#define URSQL_DISABLE_COPY_ASSIGN(Class) Class& operator=(const Class&) = delete
#define URSQL_DISABLE_COPY(Class)   \
    URSQL_DISABLE_COPY_CTOR(Class); \
    URSQL_DISABLE_COPY_ASSIGN(Class)

#define URSQL_DEFAULT_COPY_CTOR(Class) Class(const Class&) = default
#define URSQL_DEFAULT_COPY_ASSIGN(Class) \
    Class& operator=(const Class&) = default
#define URSQL_DEFAULT_COPY(Class)   \
    URSQL_DEFAULT_COPY_CTOR(Class); \
    URSQL_DEFAULT_COPY_ASSIGN(Class)

#define URSQL_DISABLE_MOVE_CTOR(Class) Class(Class&&) noexcept = delete
#define URSQL_DISABLE_MOVE_ASSIGN(Class) \
    Class& operator=(const Class&) noexcept = delete
#define URSQL_DISABLE_MOVE(Class)   \
    URSQL_DISABLE_MOVE_CTOR(Class); \
    URSQL_DISABLE_MOVE_ASSIGN(Class)

#define URSQL_DEFAULT_MOVE_CTOR(Class) Class(Class&&) noexcept = default
#define URSQL_DEFAULT_MOVE_ASSIGN(Class) \
    Class& operator=(Class&&) noexcept = default
#define URSQL_DEFAULT_MOVE(Class)   \
    URSQL_DEFAULT_MOVE_CTOR(Class); \
    URSQL_DEFAULT_MOVE_ASSIGN(Class)

// std::unreachable()
#ifdef __GNUC__
#define URSQL_UNREACHABLE __builtin_unreachable()
#elif defined(_MSC_VER)
#define URSQL_UNREACHABLE __assume(false)
#else
#define URSQL_UNREACHABLE ((void)0)
#endif

// Debug macros
#ifndef NDEBUG
#define URSQL_DEBUG
#endif

#ifdef URSQL_DEBUG

#ifdef __GNUC__
#include <execinfo.h>
#define BACKTRACE_MAX_DEPTH (1 << 10)
#define URSQL_PRINT_BACKTRACE                               \
    do {                                                    \
        std::cerr << "Printing backtrace...\n"              \
                     "=====================\n";             \
        void* btAddr[BACKTRACE_MAX_DEPTH];                  \
        int depth = backtrace(btAddr, BACKTRACE_MAX_DEPTH); \
        char** btNames = backtrace_symbols(btAddr, depth);  \
        for (int i = 0; i < depth; ++i) {                   \
            std::cerr << btNames[i] << '\n';                \
        }                                                   \
        free(btNames);                                      \
        std::cerr << "=====================\n";             \
    } while (false)
#else
#define URSQL_PRINT_BACKTRACE ((void)0)
#endif

#define URSQL_TRUTH(TRUTH, MESSAGE)                                          \
    do {                                                                     \
        if (!(TRUTH)) {                                                      \
            std::cerr << "At line " << __LINE__ << ", in file '" << __FILE__ \
                      << "'\n";                                              \
            std::cerr << "Truth is a lie! " << (MESSAGE) << '\n';            \
            URSQL_PRINT_BACKTRACE;                                           \
            std::cerr << "Terminating...\n";                                 \
            std::terminate();                                                \
        }                                                                    \
    } while (false)

#else

#define URSQL_TRUTH(TRUTH, MESSAGE) ((void)0)

#endif
