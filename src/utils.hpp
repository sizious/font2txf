#ifndef __UTILS_H__
#define __UTILS_H__

#include <cstdio>
#include <string>
#include <iostream>
#include <utility>

void program_name_initialize( char* argv0 );
std::string program_name_get();

const char* bool_to_str( bool b );

void halt();

/* 
    Thanks to kungfooman and Nikos Athanasiou
    See: https://stackoverflow.com/a/52970404
    See: https://stackoverflow.com/a/33869493/3726096
*/

class Console {
    protected:
        template <typename T>
        void log_argument(T t) {
            std::cout << t << " ";
        }
    public:
        template <typename... Args>
        void log(Args&&... args) {
            std::cout << program_name_get() << ": ";
            using expander = int[];
            (void) expander { 0, ( (void) log_argument( std::forward<Args>(args) ), 0 )... };
            std::cout << std::endl;
        }

        template <typename... Args>
        void warn(Args&&... args) {
            std::cout << program_name_get() << ": warning: ";
            using expander = int[];
            (void) expander { 0, ((void) log_argument(std::forward<Args>(args)),0)... };
            std::cout << std::endl;
        }

        template <typename... Args>
        void error(Args&&... args) {
            std::cout << program_name_get() << ": fatal: ";
            using expander = int[];
            (void) expander { 0, ((void) log_argument(std::forward<Args>(args)),0)... };
            std::cout << std::endl;
        }
};

#endif /* __UTILS_H__ */
