// this module will help with timing stuff and write in logfiles ...

#ifndef HELPER_H
#define HELPER_H

#include <ctime>
#include <string>
#include <sstream>
#include <fstream>


namespace Helper
{
	template <class T> 
	std::string ToString(const T &);
	struct DateTime
	{
		DateTime()
		{
			time_t ms;
			time(&ms);
			struct tm *info = localtime(&ms);
			d = info->tm_mday;
			m = info->tm_mon + 1;
			y = info->tm_year + 1900;
			mn = info->tm_min;
			h = info->tm_hour;
			s = info ->tm_sec; 
		}
		DateTime(int _d, int _m, int _y, int _mn, int _h, int _s) : d(_d),m(_m),y(_y),mn(_mn),h(_h),s(_s){}
		DateTime(int _d, int _m, int _y) : d(_d),m(_m),y(_y),mn(0),h(0),s(0){}
		DateTime Now() const
		{
			return DateTime();
		}

		int d,m,y,mn,h,s;
		std::string GetDateString() const{
			return std::string(d < 10 ? "0":"") + ToString(d) + std::string( m < 10 ? "_0" : "_") + ToString(m)
			+ "_" +ToString(y);

		}
		std::string GetTimeString(const std::string &sep = "_") const{
			return std::string(h < 10 ? "0":"") + ToString(h) + sep +std::string( mn < 10 ? "0" : "") + ToString(mn)
			 + sep +std::string( s < 10 ? "0" : "") + ToString(s);
			
		}
		std::string GetDateTimeString(const std::string &sep = "_") const{
			return GetDateString() + "_" + GetTimeString(sep);
		}


		
	};

	template <class T> 
	std::string ToString(const T &e){
		std::ostringstream s;
		s << e;
		return s.str();
	}

	void WriteAppLog( const std::string &s){
		std::ofstream file("Applog.txt",std::ios::app);
		file << "[" << Helper::DateTime().GetDateTimeString() << "]" << "\n" << s << std::endl << "\n";
		file.close();
	}
}


#endif