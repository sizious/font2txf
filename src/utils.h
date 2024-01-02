#ifndef __UTILS_H__
#define __UTILS_H__

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <libgen.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

#include <cstdio>
#include <iostream>
#include <utility>


class Console {
    protected:
        template <typename T>
        void log_argument(T t) {
            std::cout << t << " ";
        }
    public:
        template <typename... Args>
        void log(Args&&... args) {
            int dummy[] = { 0, ((void) log_argument(std::forward<Args>(args)),0)... };
            std::cout << std::endl;
        }

        template <typename... Args>
        void warn(Args&&... args) {
            std::cout << "WARNING: ";
            int dummy[] = { 0, ((void) log_argument(std::forward<Args>(args)),0)... };
            std::cout << std::endl;
        }

        template <typename... Args>
        void error(Args&&... args) {
            std::cout << "ERROR: ";
            int dummy[] = { 0, ((void) log_argument(std::forward<Args>(args)),0)... };
            std::cout << std::endl;
        }
};

void program_name_initialize( char* argv0 );
char* program_name_get();
void program_name_finalize();

const char* bool_to_str( bool b );

void halt();

#endif /* __UTILS_H__ */
