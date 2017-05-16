#ifndef KIRIN_COMMON_STRING_UTIL_H
#define KIRIN_COMMON_STRING_UTIL_H

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <vector>

#include "kirin/common/non_copyable.h"

BEGIN_KIRIN_NS(common);
class string_util: public non_copyable {
public:
    static inline void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                std::not1(std::ptr_fun<int, int>(std::isspace))));
    }   

    static inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(),
                std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    }   

    static inline void trim(std::string &s) {
        ltrim(s);
        rtrim(s);
    }

    static std::vector<std::string> split(std::string& str, 
                        const std::string& splitter = " ") {
        std::vector<std::string> result;
        if (str.empty()) return result;

        std::string::size_type begin_pos = str.find_first_not_of(splitter);
        std::string::size_type splitter_pos = 0;
        const std::size_t splitter_size = splitter.size();

        std::string tmp_str("");
        while (begin_pos != std::string::npos) {
            splitter_pos = str.find_first_of(splitter, begin_pos);
            if (splitter_pos != std::string::npos) {
                tmp_str = str.substr(begin_pos, splitter_pos - begin_pos);
                begin_pos = splitter_pos + splitter_size;
            } else {
                tmp_str = str.substr(begin_pos);
                begin_pos = std::string::npos;
            }

            if (!tmp_str.empty()) {
                result.push_back(tmp_str);
                tmp_str.clear();
            }
        }

        return result;
    }
};
END_KIRIN_NS(common);

#endif
