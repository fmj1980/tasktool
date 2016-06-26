#pragma once
#include "OperatingSystem.h"

/*

*
*支持以下时间字符串格式的解析:
*   14 Apr 89 03:20:12
*   14 Apr 89 03:20 GMT
*   Fri, 17 Mar 89 4:01:33
*   Fri, 17 Mar 89 4:01 GMT
*   Mon Jan 16 16:12 PDT 1989
*   Mon Jan 16 16:12 +0130 1989
*   6 May 1992 16:41-JST (Wednesday)
*   22-AUG-1993 10:59:12.82
*   22-AUG-1993 10:59pm
*   22-AUG-1993 12:59am
*   22-AUG-1993 12:59 PM
*   Friday, August 04, 1995 3:54 PM
*   06/21/95 04:24:34 PM
*   20/06/95 21:07
*   95-06-08 19:32:48 EDT
*
*
*/

namespace pcutil
{

	enum EDateFormat
	{
		//YYYYMMDDHHmm, //like: "2008-9-27 14:04"
		YYYYMMDD, // like: "2008-9-27"
		MMDD,     // like: "9-27"
		HHmm,	  // like: "14:04"
		YYYYMMDDHHmmss, // like: "2008-9-27 14:04:03"
		YYYYMMDDHHmmssfff,	//like :2008-9-27 14:04:03 332
		PYYYYMMDDHHmmss	//Plain like 20080927140403
	};

	struct TDateTime
	{
		int year;          // Four digit year "2007"
		int month;         // 1-based month (values 1 = January, etc.)
		int day_of_week;   // 0-based day of week (0 = Sunday, etc.)
		int day_of_month;  // 1-based day of month (1-31)
		int hour;          // Hour within the current day (0-23)
		int minute;        // Minute within the current hour (0-59)
		int second;        // Second within the current minute (0-59 plus leap
		// seconds which may take it up to 60).
		int millisecond;   // Milliseconds within the current second (0-999)

		CString ToString( EDateFormat format)
		{
			CString ret;
			switch ( format )
			{
			case YYYYMMDD:
				ret.Format( _T("%04d-%02d-%02d"), year , month , day_of_month  );
				break;
			case MMDD:
				ret.Format( _T("%02d-%02d"),   month ,  day_of_month  );
				break;
			case HHmm:
				ret.Format( _T("%02d:%02d"),   hour ,  minute  );
				break;
			case YYYYMMDDHHmmss:
				ret.Format( _T("%04d-%02d-%02d %02d:%02d:%02d"),  year ,  month , day_of_month ,
					hour ,  minute ,  second  );
				break;
			case YYYYMMDDHHmmssfff:
				ret.Format( _T("%04d-%02d-%02d %02d:%02d:%02d %03d"),  year ,  month , day_of_month ,
					hour ,  minute ,  second, millisecond );
				break;
			case PYYYYMMDDHHmmss:
				ret.Format( _T("%04d%02d%02d%02d%02d%02d"),  year ,  month , day_of_month ,
					hour ,  minute ,  second  );
				break;
			}
			return ret;
		}
	};



	class CPcTime;
	class CPcTimeTicks;

	class WEAVERLIB_API_UTILS CPcTimeDelta
	{
	public:
		CPcTimeDelta() : delta_( 0 )
		{
		}

		static CPcTimeDelta FromDays( __int64 days );
		static CPcTimeDelta FromHours( __int64 hours );
		static CPcTimeDelta FromMinutes( __int64 minutes );
		static CPcTimeDelta FromSeconds( __int64 secs );
		static CPcTimeDelta FromMilliseconds( __int64 ms );
		static CPcTimeDelta FromMicroseconds( __int64 us );

		__int64 ToInternalValue() const
		{
			return delta_;
		}

		int InDays() const;
		int InHours() const;
		int InMinutes() const;
		double InSecondsF() const;
		__int64 InSeconds() const;
		double InMillisecondsF() const;
		__int64 InMilliseconds() const;
		__int64 InMicroseconds() const;

		CPcTimeDelta& operator=( CPcTimeDelta other )
		{
			delta_ = other.delta_;
			return *this;
		}

		CPcTimeDelta operator+( CPcTimeDelta other ) const
		{
			return CPcTimeDelta( delta_ + other.delta_ );
		}
		CPcTimeDelta operator-( CPcTimeDelta other ) const
		{
			return CPcTimeDelta( delta_ - other.delta_ );
		}

		CPcTimeDelta& operator+=( CPcTimeDelta other )
		{
			delta_ += other.delta_;
			return *this;
		}
		CPcTimeDelta& operator-=( CPcTimeDelta other )
		{
			delta_ -= other.delta_;
			return *this;
		}
		CPcTimeDelta operator-() const
		{
			return CPcTimeDelta( -delta_ );
		}

		CPcTimeDelta operator*( __int64 a ) const
		{
			return CPcTimeDelta( delta_ * a );
		}
		CPcTimeDelta operator/( __int64 a ) const
		{
			return CPcTimeDelta( delta_ / a );
		}
		CPcTimeDelta& operator*=( __int64 a )
		{
			delta_ *= a;
			return *this;
		}
		CPcTimeDelta& operator/=( __int64 a )
		{
			delta_ /= a;
			return *this;
		}
		__int64 operator/( CPcTimeDelta a ) const
		{
			return delta_ / a.delta_;
		}

		CPcTime operator+( CPcTime t ) const;
		CPcTimeTicks operator+( CPcTimeTicks t ) const;

		bool operator==( CPcTimeDelta other ) const
		{
			return delta_ == other.delta_;
		}
		bool operator!=( CPcTimeDelta other ) const
		{
			return delta_ != other.delta_;
		}
		bool operator<( CPcTimeDelta other ) const
		{
			return delta_ < other.delta_;
		}
		bool operator<=( CPcTimeDelta other ) const
		{
			return delta_ <= other.delta_;
		}
		bool operator>( CPcTimeDelta other ) const
		{
			return delta_ > other.delta_;
		}
		bool operator>=( CPcTimeDelta other ) const
		{
			return delta_ >= other.delta_;
		}

	private:
		friend class CPcTime;
		friend class CPcTimeTicks;
		friend CPcTimeDelta operator*( __int64 a, CPcTimeDelta td );
		explicit CPcTimeDelta( __int64 delta_us ) : delta_( delta_us )
		{
		}
		__int64 delta_;
	};

	inline CPcTimeDelta operator*( __int64 a, CPcTimeDelta td )
	{
		return CPcTimeDelta( a * td.delta_ );
	}

	class WEAVERLIB_API_UTILS CPcTime
	{
	public:
		CPcTime();
		explicit CPcTime( __int64 us );
		CPcTime( const CPcTime &other );

	public:
		bool IsNull() const;
		time_t ToTimeT() const;
		double ToDoubleT() const;
		__int64 ToInt64Value() const;
		TDateTime ToUTCDateTime() const;
		TDateTime ToLocalDateTime() const;
		CPcTime LocalMidnight() const;

	public:
		static CPcTime FromUTCExploded( const TDateTime& exploded );
		static CPcTime FromLocalExploded( const TDateTime& exploded );
		static CPcTime Now();
		static CPcTime FromTimeT( time_t tt );
		static CPcTime FromInt64Value( __int64 us );
		static bool FromString( const CString &time_string , CPcTime &parsed_time );
		static bool FromString( const wchar_t* time_string, CPcTime* parsed_time );

	public:
		CPcTime& operator=( const CPcTime &other )
		{
			if( this != &other )
			{
				us_ = other.us_;
			}
			return *this;
		}

		CPcTimeDelta operator-( const CPcTime &other ) const
		{
			return CPcTimeDelta( us_ - other.us_ );
		}

		CPcTime& operator+=( const CPcTimeDelta &delta )
		{
			us_ += delta.delta_;
			return *this;
		}
		CPcTime& operator-=( const CPcTimeDelta &delta )
		{
			us_ -= delta.delta_;
			return *this;
		}

		CPcTime operator+( const CPcTimeDelta &delta ) const
		{
			return CPcTime( us_ + delta.delta_ );
		}
		CPcTime operator-( const CPcTimeDelta &delta ) const
		{
			return CPcTime( us_ - delta.delta_ );
		}

		bool operator==( const CPcTime &other ) const
		{
			return us_ == other.us_;
		}
		bool operator!=( const CPcTime &other ) const
		{
			return us_ != other.us_;
		}
		bool operator<( const CPcTime &other ) const
		{
			return us_ < other.us_;
		}
		bool operator<=( const CPcTime &other ) const
		{
			return us_ <= other.us_;
		}
		bool operator>( const CPcTime &other ) const
		{
			return us_ > other.us_;
		}
		bool operator>=( const CPcTime &other ) const
		{
			return us_ >= other.us_;
		}

	private:
		void UTCExplode( TDateTime* exploded ) const;
		void LocalExplode( TDateTime* exploded ) const;
		friend class CPcTimeDelta;
		static __int64 CurrentWallclockMicroseconds();
		static void InitializeClock();
		void Explode( bool is_local, TDateTime* exploded ) const;
		static CPcTime FromExploded( bool is_local, const TDateTime& exploded );
		__int64 us_;
		static __int64 initial_time_;
		static CPcTimeTicks initial_ticks_;

	public:
		static const __int64 kMillisecondsPerSecond = 1000;
		static const __int64 kMicrosecondsPerMillisecond = 1000;
		static const __int64 kMicrosecondsPerSecond = kMicrosecondsPerMillisecond * kMillisecondsPerSecond;
		static const __int64 kMicrosecondsPerMinute = kMicrosecondsPerSecond * 60;
		static const __int64 kMicrosecondsPerHour = kMicrosecondsPerMinute * 60;
		static const __int64 kMicrosecondsPerDay = kMicrosecondsPerHour * 24;
		static const __int64 kMicrosecondsPerWeek = kMicrosecondsPerDay * 7;
		static const __int64 kNanosecondsPerMicrosecond = 1000;
		static const __int64 kNanosecondsPerSecond = kNanosecondsPerMicrosecond * kMicrosecondsPerSecond;
		static const int kMaxMillisecondsToAvoidDrift = 60 * CPcTime::kMillisecondsPerSecond;
		static const __int64 kTimeTToMicrosecondsOffset;

	};

	inline CPcTime CPcTimeDelta::operator+( CPcTime t ) const
	{
		return CPcTime( t.us_ + delta_ );
	}

	class WEAVERLIB_API_UTILS CPcTimeTicks
	{
	public:
		CPcTimeTicks() : ticks_( 0 )
		{
		}
		static CPcTimeTicks Now();
		static CPcTimeTicks UnreliableHighResNow();

		bool is_null() const
		{
			return ticks_ == 0;
		}

		CPcTimeTicks& operator=( const CPcTimeTicks &other )
		{
			ticks_ = other.ticks_;
			return *this;
		}

		CPcTimeDelta operator-( const CPcTimeTicks &other ) const
		{
			return CPcTimeDelta( ticks_ - other.ticks_ );
		}

		CPcTimeTicks& operator+=( const CPcTimeDelta &delta )
		{
			ticks_ += delta.delta_;
			return *this;
		}

		CPcTimeTicks& operator-=( const CPcTimeDelta &delta )
		{
			ticks_ -= delta.delta_;
			return *this;
		}

		CPcTimeTicks operator+( const CPcTimeDelta &delta ) const
		{
			return CPcTimeTicks( ticks_ + delta.delta_ );
		}

		CPcTimeTicks operator-( const CPcTimeDelta &delta ) const
		{
			return CPcTimeTicks( ticks_ - delta.delta_ );
		}

		bool operator==( const CPcTimeTicks &other ) const
		{
			return ticks_ == other.ticks_;
		}

		bool operator!=( const CPcTimeTicks &other ) const
		{
			return ticks_ != other.ticks_;
		}

		bool operator<( const CPcTimeTicks &other ) const
		{
			return ticks_ < other.ticks_;
		}

		bool operator<=( const CPcTimeTicks &other ) const
		{
			return ticks_ <= other.ticks_;
		}

		bool operator>( const CPcTimeTicks &other ) const
		{
			return ticks_ > other.ticks_;
		}

		bool operator>=( const CPcTimeTicks &other ) const
		{
			return ticks_ >= other.ticks_;
		}
	protected:
		friend class CPcTimeDelta;
		explicit CPcTimeTicks( __int64 ticks ) : ticks_( ticks )
		{
		}
		__int64 ticks_;
	protected:
		

	};

	inline CPcTimeTicks CPcTimeDelta::operator+( CPcTimeTicks t ) const
	{
		return CPcTimeTicks( t.ticks_ + delta_ );
	}

}