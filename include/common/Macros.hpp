#pragma once

#include <cstdlib>
#include <boost/stacktrace.hpp>

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

#ifdef __GNUC__
#include <execinfo.h>
#define BACKTRACE_MAX_DEPTH (1 << 10)
#define URSQL_PRINT_BACKTRACE std::cerr << boost::stacktrace::stacktrace()
#else
#define URSQL_PRINT_BACKTRACE ((void)0)
#endif

#define URSQL_PRINT_LOCATION \
    std::cerr << __FILE__ << ':' << __LINE__ << '\n'

#define URSQL_FAIL(e, message) \
    do {                       \
        URSQL_PRINT_LOCATION;  \
        URSQL_PRINT_BACKTRACE; \
        throw e(message);      \
    } while (false)

#define URSQL_EXPECT(predicate, e, message) \
    do {                                    \
        if (!(predicate)) {                 \
            URSQL_FAIL(e, message);          \
        }                                   \
    } while (false)

#define URSQL_ASSERT(predicate, message) \
    URSQL_EXPECT(predicate, AssertFailure, message)

// unreachable
#define URSQL_UNREACHABLE(message) URSQL_FAIL(UnreachableReached, message)
