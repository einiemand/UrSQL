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

#define URSQL_THROW(t, e, message) t(e(message))
#define URSQL_THROW_NORMAL(e, message) URSQL_THROW(throw, e, message)
#define URSQL_THROW_TRACED(e, message) URSQL_THROW(throw_traced, e, message)

#define URSQL_CHECK(predicate, t, e, message) \
    do {                                    \
        if (!(predicate)) {                 \
            t(e, message);         \
        }                                   \
    } while (false)

#define URSQL_EXPECT(predicate, e, message) URSQL_CHECK(predicate, URSQL_THROW_NORMAL, e, message)
#define URSQL_ASSERT(predicate, message) URSQL_CHECK(predicate, URSQL_THROW_TRACED, AssertFailure, message)

// unreachable
#define URSQL_UNREACHABLE(message) URSQL_THROW_TRACED(UnreachableReached, message)
