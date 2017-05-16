#ifndef KIRIN_COMMON_LOG_H
#define KIRIN_COMMON_LOG_H

#include <alog/Logger.h>
#include <alog/Configurator.h>
#include <iostream>

#define KIRIN_ROOT_LOG_CONFIG() alog::Configurator::configureRootLogger()
#define KIRIN_LOG_CONFIG(filename) do {                          \
        try {                                                   \
            alog::Configurator::configureLogger(filename);      \
        } catch(std::exception &e) {                            \
            std::cerr << "Error!!! Failed to configure logger!" \
                      << e.what() << std::endl;                 \
            exit(-1);                                           \
        }                                                       \
    }while(0)

#define KIRIN_ROOT_LOG_SETLEVEL(level)                                  \
    alog::Logger::getRootLogger()->setLevel(alog::LOG_LEVEL_##level)

#define KIRIN_LOG_DECLARE() static alog::Logger *_logger

#define KIRIN_LOG_SETUP(n, c) alog::Logger *c::_logger  \
    = alog::Logger::getLogger("kirin." #n "." #c)

#define KIRIN_LOG_SETUP_TEMPLATE(n, c, T) template <typename T> \
    alog::Logger *c<T>::_logger                                 \
    = alog::Logger::getLogger("kirin." #n "." #c)

#define KIRIN_LOG_SETUP_TEMPLATE3(n, c, T1, T2, T3) template <typename T1, typename T2, typename T3> \
    alog::Logger *c<T1, T2, T3>::_logger                                \
    = alog::Logger::getLogger("kirin." #n "." #c)

#define KIRIN_LOG(level, format, args...) ALOG_LOG(_logger, alog::LOG_LEVEL_##level, format, ##args)

#define KIRIN_LOG_COMMON(level, format, args...) ALOG_LOG(CommonLog::_logger, alog::LOG_LEVEL_##level, format, ##args)

#define KIRIN_DECLARE_AND_SETUP_LOGGER(n, c) static alog::Logger *_logger \
    = alog::Logger::getLogger("kirin." #n "." #c)
#define KIRIN_LOG_SHUTDOWN() alog::Logger::shutdown()
#define KIRIN_LOG_FLUSH() alog::Logger::flushAll()

#endif //KIRIN_COMMON_LOG_H
