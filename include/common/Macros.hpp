#pragma once

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
#define URSQL_DISABLE_COPY_CTOR(Class) Class(const Class&) = delete
#define URSQL_DISABLE_COPY_ASSIGN(Class) Class& operator=(const Class&) = delete
#define URSQL_DISABLE_COPY(Class) \
    URSQL_DISABLE_COPY_CTOR(Class); \
    URSQL_DISABLE_COPY_ASSIGN(Class)

#define URSQL_DEFAULT_COPY_CTOR(Class) Class(const Class&) = default
#define URSQL_DEFAULT_COPY_ASSIGN(Class) Class& operator=(const Class&) = default
#define URSQL_DEFAULT_COPY(Class) \
	URSQL_DEFAULT_COPY_CTOR(Class); \
	URSQL_DEFAULT_COPY_ASSIGN(Class)

#define URSQL_DISABLE_MOVE_CTOR(Class) Class(Class&&) noexcept = delete
#define URSQL_DISABLE_MOVE_ASSIGN(Class) Class& operator=(const Class&) noexcept = delete
#define URSQL_DISABLE_MOVE(Class) \
    URSQL_DISABLE_MOVE_CTOR(Class); \
    URSQL_DISABLE_MOVE_ASSIGN(Class)

#define URSQL_DEFAULT_MOVE_CTOR(Class) Class(Class&&) noexcept = default
#define URSQL_DEFAULT_MOVE_ASSIGN(Class) Class& operator=(Class&&) noexcept = default
#define URSQL_DEFAULT_MOVE(Class) \
	URSQL_DEFAULT_MOVE_CTOR(Class); \
	URSQL_DEFAULT_MOVE_ASSIGN(Class)

// Debug macros
#ifndef NDEBUG
#define URSQL_DEBUG
#endif

#ifdef URSQL_DEBUG

#define URSQL_TRUTH(TRUTH, MESSAGE) \
    do { \
        if (!(TRUTH)) { \
			std::cerr << "At line " << __LINE__ << ", in file '" << __FILE__ << "'\n"; \
			std::cerr << "Truth is a lie! " << (MESSAGE) << '\n'; \
			std::cerr << "Aborting...\n"; \
			std::terminate(); \
		} \
    } while(false)

#else

#define URSQL_TRUTH(TRUTH, MESSAGE) ((void)0)

#endif
