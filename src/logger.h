#ifndef _LOGGER_H_
#define _LOGGER_H_


#include <iostream>
#include <sys/time.h>
#define ENABLE_DEBUG

#ifdef ENABLE_ERROR
# undef ENABLE_DEBUG
# undef ENABLE_INFO
# undef ENABLE_WARN
#endif

#ifdef ENABLE_WARN
# undef ENABLE_DEBUG
# undef ENABLE_INFO
# define ENABLE_ERROR
#endif

#ifdef ENABLE_INFO
# undef ENABLE_DEBUG
# define ENABLE_WARN
# define ENABLE_ERROR
#endif

#ifdef ENABLE_DEBUG
# define ENABLE_INFO
# define ENABLE_WARN
# define ENABLE_ERROR
#endif

template <bool B>
struct LogStream {

	template <typename T>
	LogStream<B>& operator<<(T t) {
		std::clog << t;
		return *this;
	}

	LogStream<B>& operator<<(std::ostream& (*t)(std::ostream&)) {
		std::clog << t;
		return *this;
	}
};

extern LogStream<true> logger;


#define LOCATION " (" << __FILE__ << ':' << __LINE__ << ')'


struct TimeStamp {
	char stamp[30];
	TimeStamp() {
		struct timeval tv;
		struct timezone tz;
		struct tm *tm;
		gettimeofday(&tv, &tz);
		tm=localtime(&tv.tv_sec);
		snprintf(stamp,29,"%04d-%02d-%02d %02d:%02d:%02d.%03d",
				tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
				tm->tm_hour, tm->tm_min,
		    tm->tm_sec, (int)(tv.tv_usec / 1000));
	}
};

#ifdef ENABLE_DEBUG
#define DEBUG(X) logger << TimeStamp().stamp << LOCATION << " DEBUG: " << X << std::endl;
#else
#define DEBUG(X)
#endif

#ifdef ENABLE_INFO
#define INFO(X) logger << TimeStamp().stamp << LOCATION  << " INFO: " << X << std::endl;
#else
#define INFO(X)
#endif

#ifdef ENABLE_WARN
#define WARN(X) logger << TimeStamp().stamp << LOCATION << " WARN: " << X << std::endl;
#else
#define WARN(X)
#endif

#ifdef ENABLE_ERROR
#define ERROR(X) logger << TimeStamp().stamp << LOCATION << " ERROR: " << X << std::endl;
#else
#define ERROR(X)
#endif

#endif
