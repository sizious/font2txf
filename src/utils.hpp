#ifndef __UTILS_HPP__
#define __UTILS_HPP__

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
    enum class Severity { Info, Warning, Error, Fatal };

    private:
        // This probably could be better in C++17 (but we want to keep compatibility with older compilers...)
        std::string get_severity_name(Severity severity)
        {
            std::string result = std::string();

            switch(severity) {
                case Severity::Info:
                    break;                  
                case Severity::Warning:
                    result = "warning";
                    break;
                case Severity::Error:
                    result = "error";
                    break;
                case Severity::Fatal:
                    result = "fatal";
                    break;
            }

            return result;
        }

    protected:
        template <typename T>
        void log_argument(T t)
        {
            std::cout << t << " ";
        }

        template <typename... Args>
        void log_trigger(Severity severity, Args&&... args)
        {
            std::string severity_name = get_severity_name(severity);
            std::cout << program_name_get() << ": " << severity_name << ( !severity_name.empty() ? ": " : std::string() );

            using expander = int[];
            (void) expander { 0, ( (void) log_argument( std::forward<Args>(args) ), 0 )... };
            
            std::cout << std::endl;
        }

    public:
        template <typename... Args>
        void log(Args&&... args)
        {
            log_trigger(Severity::Info, args ...);
        }

        template <typename... Args>
        void warn(Args&&... args)
        {
            log_trigger(Severity::Warning, args ...);
        }

        template <typename... Args>
        void error(Args&&... args)
        {
            log_trigger(Severity::Error, args ...);
        }

        template <typename... Args>
        void fatal(Args&&... args)
        {
            log_trigger(Severity::Fatal, args ...);
        }        
};

#endif /* __UTILS_HPP__ */
