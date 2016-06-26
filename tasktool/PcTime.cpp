#include "StdAfx.h"
#include "PcTime.h"
#include "CommonHelper.h"
#pragma comment(lib, "Winmm.lib")

namespace pcutil
{

namespace
{
typedef enum { PR_FAILURE = -1, PR_SUCCESS = 0 } PRStatus;
#define PR_TRUE 1
#define PR_FALSE 0
}

const __int64 CPcTime::kTimeTToMicrosecondsOffset = 11644473600000000I64;

CPcTimeDelta CPcTimeDelta::FromDays( __int64 days )
{
    return CPcTimeDelta( days * CPcTime::kMicrosecondsPerDay );
}

CPcTimeDelta CPcTimeDelta::FromHours( __int64 hours )
{
    return CPcTimeDelta( hours * CPcTime::kMicrosecondsPerHour );
}

CPcTimeDelta CPcTimeDelta::FromMinutes( __int64 minutes )
{
    return CPcTimeDelta( minutes * CPcTime::kMicrosecondsPerMinute );
}

CPcTimeDelta CPcTimeDelta::FromSeconds( __int64 secs )
{
    return CPcTimeDelta( secs * CPcTime::kMicrosecondsPerSecond );
}

CPcTimeDelta CPcTimeDelta::FromMilliseconds( __int64 ms )
{
    return CPcTimeDelta( ms * CPcTime::kMicrosecondsPerMillisecond );
}

CPcTimeDelta CPcTimeDelta::FromMicroseconds( __int64 us )
{
    return CPcTimeDelta( us );
}

int CPcTimeDelta::InDays() const
{
    return static_cast<int>( delta_ / CPcTime::kMicrosecondsPerDay );
}

int CPcTimeDelta::InHours() const
{
    return static_cast<int>( delta_ / CPcTime::kMicrosecondsPerHour );
}

int CPcTimeDelta::InMinutes() const
{
    return static_cast<int>( delta_ / CPcTime::kMicrosecondsPerMinute );
}

double CPcTimeDelta::InSecondsF() const
{
    return static_cast<double>( delta_ ) / CPcTime::kMicrosecondsPerSecond;
}

__int64 CPcTimeDelta::InSeconds() const
{
    return delta_ / CPcTime::kMicrosecondsPerSecond;
}

double CPcTimeDelta::InMillisecondsF() const
{
    return static_cast<double>( delta_ ) / CPcTime::kMicrosecondsPerMillisecond;
}

__int64 CPcTimeDelta::InMilliseconds() const
{
    return delta_ / CPcTime::kMicrosecondsPerMillisecond;
}

__int64 CPcTimeDelta::InMicroseconds() const
{
    return delta_;
}

__int64 CPcTime::initial_time_ = 0;
CPcTimeTicks CPcTime::initial_ticks_;


CPcTime::CPcTime() : us_( 0 )
{

}

CPcTime::CPcTime( __int64 us ) : us_( us )
{

}

CPcTime::CPcTime( const CPcTime &other )
{
    *this = other;
}

bool CPcTime::IsNull() const
{
    return us_ == 0;
}

CPcTime CPcTime::FromUTCExploded( const TDateTime& exploded )
{
    return FromExploded( false, exploded );
}

CPcTime CPcTime::FromLocalExploded( const TDateTime& exploded )
{
    return FromExploded( true, exploded );
}

CPcTime CPcTime::FromInt64Value( __int64 us )
{
    return CPcTime( us );
}

bool CPcTime::FromString( const CString &time_string , CPcTime &parsed_time )
{
    return FromString( time_string , &parsed_time );
}

__int64 CPcTime::ToInt64Value() const
{
    return us_;
}

TDateTime CPcTime::ToUTCDateTime() const
{
    TDateTime e;
    Explode( false, &e );
    return e;
}

TDateTime CPcTime::ToLocalDateTime() const
{
    TDateTime e;
    Explode( true, &e );
    return e;
}

void CPcTime::UTCExplode( TDateTime* exploded ) const
{
    return Explode( false, exploded );
}
void CPcTime::LocalExplode( TDateTime* exploded ) const
{
    return Explode( true, exploded );
}








void CPcTime::InitializeClock()
{
    initial_ticks_ = CPcTimeTicks::Now();
    initial_time_ = CurrentWallclockMicroseconds();
}

CPcTime CPcTime::Now()
{
    if ( initial_time_ == 0 )
        InitializeClock();

    while ( true )
    {
        CPcTimeTicks ticks = CPcTimeTicks::Now();
        CPcTimeDelta elapsed = ticks - initial_ticks_;
        if ( elapsed.InMilliseconds() > 1/*kMaxMillisecondsToAvoidDrift*/ )
        {
            InitializeClock();
            continue;
        }
        return elapsed + CPcTime( initial_time_ );
    }
}

CPcTime CPcTime::FromTimeT( time_t tt )
{
    if ( tt == 0 )
    {
        return CPcTime();  // Preserve 0 so we can tell it doesn't exist.
    }
    return CPcTime( ( tt * kMicrosecondsPerSecond ) + kTimeTToMicrosecondsOffset );
}

time_t CPcTime::ToTimeT() const
{
    if ( us_ == 0 )
    {
        return 0;  // Preserve 0 so we can tell it doesn't exist.
    }
    return ( us_ - kTimeTToMicrosecondsOffset ) / kMicrosecondsPerSecond;
}

double CPcTime::ToDoubleT() const
{
    if ( us_ == 0 )
    {
        return 0;  // Preserve 0 so we can tell it doesn't exist.
    }
    return ( static_cast<double>( us_ - kTimeTToMicrosecondsOffset ) /
             static_cast<double>( kMicrosecondsPerSecond ) );
}

CPcTime CPcTime::LocalMidnight() const
{
    TDateTime exploded;
    LocalExplode( &exploded );
    exploded.hour = 0;
    exploded.minute = 0;
    exploded.second = 0;
    exploded.millisecond = 0;
    return FromLocalExploded( exploded );
}

static void localtime_r( const time_t* secs, struct tm* time )
{
    ( void ) localtime_s( time, secs );
}

#define LL_I2L(l, i)    ((l) = (__int64)(i))
#define LL_MUL(r, a, b) ((r) = (a) * (b))

#define PR_MSEC_PER_SEC		1000UL
#define PR_USEC_PER_SEC		1000000UL
#define PR_NSEC_PER_SEC		1000000000UL
#define PR_USEC_PER_MSEC	1000UL
#define PR_NSEC_PER_MSEC	1000000UL

typedef struct PRTimeParameters
{
    int tp_gmt_offset;
    int tp_dst_offset;
} PRTimeParameters;

typedef struct PRExplodedTime
{
    int tm_usec;		    /* microseconds past tm_sec (0-99999)  */
    int tm_sec;             /* seconds past tm_min (0-61, accomodating
                                   up to two leap seconds) */
    int tm_min;             /* minutes past tm_hour (0-59) */
    int tm_hour;            /* hours past tm_day (0-23) */
    int tm_mday;            /* days past tm_mon (1-31, note that it
				                starts from 1) */
    int tm_month;           /* months past tm_year (0-11, Jan = 0) */
    short tm_year;            /* absolute year, AD (note that we do not
				                count from 1900) */
    char tm_wday;		        /* calculated day of the week
				                (0-6, Sun = 0) */
    short tm_yday;            /* calculated day of the year
				                (0-365, Jan 1 = 0) */
    PRTimeParameters tm_params;  /* time parameters used by conversion */
} PRExplodedTime;


static __int64 PR_ImplodeTime( const PRExplodedTime *exploded )
{
    static const __int64 kSecondsToMicroseconds = static_cast<__int64>( 1000000 );
    SYSTEMTIME st = {0};
    FILETIME ft = {0};
    ULARGE_INTEGER uli = {0};
    st.wYear = exploded->tm_year;
    st.wMonth = exploded->tm_month + 1;
    st.wDayOfWeek = exploded->tm_wday;
    st.wDay = exploded->tm_mday;
    st.wHour = exploded->tm_hour;
    st.wMinute = exploded->tm_min;
    st.wSecond = exploded->tm_sec;
    st.wMilliseconds = exploded->tm_usec / 1000;
    if ( !SystemTimeToFileTime( &st, &ft ) )
    {
        //ATLASSERT( FALSE );
        return 0;
    }
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    __int64 result = static_cast<__int64>( ( uli.QuadPart / 10 ) - 11644473600000000i64 );
    result -= ( exploded->tm_params.tp_gmt_offset +
                exploded->tm_params.tp_dst_offset ) * kSecondsToMicroseconds;
    return result;
}

typedef enum
{
    TT_UNKNOWN,

    TT_SUN, TT_MON, TT_TUE, TT_WED, TT_THU, TT_FRI, TT_SAT,

    TT_JAN, TT_FEB, TT_MAR, TT_APR, TT_MAY, TT_JUN,
    TT_JUL, TT_AUG, TT_SEP, TT_OCT, TT_NOV, TT_DEC,

    TT_PST, TT_PDT, TT_MST, TT_MDT, TT_CST, TT_CDT, TT_EST, TT_EDT,
    TT_AST, TT_NST, TT_GMT, TT_BST, TT_MET, TT_EET, TT_JST
} TIME_TOKEN;

static PRStatus PR_ParseTimeString(
    const char *string,
    bool default_to_gmt,
    __int64 *result )
{
    PRExplodedTime tm;
    TIME_TOKEN dotw = TT_UNKNOWN;
    TIME_TOKEN month = TT_UNKNOWN;
    TIME_TOKEN zone = TT_UNKNOWN;
    int zone_offset = -1;
    int date = -1;
    int year = -1;
    int hour = -1;
    int min = -1;
    int sec = -1;
    const char *rest = string;
#ifdef DEBUG
    int iterations = 0;
#endif
    ATLASSERT( string && result );
    if ( !string || !result ) return PR_FAILURE;
    while ( *rest )
    {
#ifdef DEBUG
        if ( iterations++ > 1000 )
        {
            ATLASSERT( 0 );
            return PR_FAILURE;
        }
#endif
        switch ( *rest )
        {
            case 'a':
            case 'A':
                if ( month == TT_UNKNOWN &&
                        ( rest[1] == 'p' || rest[1] == 'P' ) &&
                        ( rest[2] == 'r' || rest[2] == 'R' ) )
                    month = TT_APR;
                else if ( zone == TT_UNKNOWN &&
                          ( rest[1] == 's' || rest[1] == 's' ) &&
                          ( rest[2] == 't' || rest[2] == 'T' ) )
                    zone = TT_AST;
                else if ( month == TT_UNKNOWN &&
                          ( rest[1] == 'u' || rest[1] == 'U' ) &&
                          ( rest[2] == 'g' || rest[2] == 'G' ) )
                    month = TT_AUG;
                break;
            case 'b':
            case 'B':
                if ( zone == TT_UNKNOWN &&
                        ( rest[1] == 's' || rest[1] == 'S' ) &&
                        ( rest[2] == 't' || rest[2] == 'T' ) )
                    zone = TT_BST;
                break;
            case 'c':
            case 'C':
                if ( zone == TT_UNKNOWN &&
                        ( rest[1] == 'd' || rest[1] == 'D' ) &&
                        ( rest[2] == 't' || rest[2] == 'T' ) )
                    zone = TT_CDT;
                else if ( zone == TT_UNKNOWN &&
                          ( rest[1] == 's' || rest[1] == 'S' ) &&
                          ( rest[2] == 't' || rest[2] == 'T' ) )
                    zone = TT_CST;
                break;
            case 'd':
            case 'D':
                if ( month == TT_UNKNOWN &&
                        ( rest[1] == 'e' || rest[1] == 'E' ) &&
                        ( rest[2] == 'c' || rest[2] == 'C' ) )
                    month = TT_DEC;
                break;
            case 'e':
            case 'E':
                if ( zone == TT_UNKNOWN &&
                        ( rest[1] == 'd' || rest[1] == 'D' ) &&
                        ( rest[2] == 't' || rest[2] == 'T' ) )
                    zone = TT_EDT;
                else if ( zone == TT_UNKNOWN &&
                          ( rest[1] == 'e' || rest[1] == 'E' ) &&
                          ( rest[2] == 't' || rest[2] == 'T' ) )
                    zone = TT_EET;
                else if ( zone == TT_UNKNOWN &&
                          ( rest[1] == 's' || rest[1] == 'S' ) &&
                          ( rest[2] == 't' || rest[2] == 'T' ) )
                    zone = TT_EST;
                break;
            case 'f':
            case 'F':
                if ( month == TT_UNKNOWN &&
                        ( rest[1] == 'e' || rest[1] == 'E' ) &&
                        ( rest[2] == 'b' || rest[2] == 'B' ) )
                    month = TT_FEB;
                else if ( dotw == TT_UNKNOWN &&
                          ( rest[1] == 'r' || rest[1] == 'R' ) &&
                          ( rest[2] == 'i' || rest[2] == 'I' ) )
                    dotw = TT_FRI;
                break;
            case 'g':
            case 'G':
                if ( zone == TT_UNKNOWN &&
                        ( rest[1] == 'm' || rest[1] == 'M' ) &&
                        ( rest[2] == 't' || rest[2] == 'T' ) )
                    zone = TT_GMT;
                break;
            case 'j':
            case 'J':
                if ( month == TT_UNKNOWN &&
                        ( rest[1] == 'a' || rest[1] == 'A' ) &&
                        ( rest[2] == 'n' || rest[2] == 'N' ) )
                    month = TT_JAN;
                else if ( zone == TT_UNKNOWN &&
                          ( rest[1] == 's' || rest[1] == 'S' ) &&
                          ( rest[2] == 't' || rest[2] == 'T' ) )
                    zone = TT_JST;
                else if ( month == TT_UNKNOWN &&
                          ( rest[1] == 'u' || rest[1] == 'U' ) &&
                          ( rest[2] == 'l' || rest[2] == 'L' ) )
                    month = TT_JUL;
                else if ( month == TT_UNKNOWN &&
                          ( rest[1] == 'u' || rest[1] == 'U' ) &&
                          ( rest[2] == 'n' || rest[2] == 'N' ) )
                    month = TT_JUN;
                break;
            case 'm':
            case 'M':
                if ( month == TT_UNKNOWN &&
                        ( rest[1] == 'a' || rest[1] == 'A' ) &&
                        ( rest[2] == 'r' || rest[2] == 'R' ) )
                    month = TT_MAR;
                else if ( month == TT_UNKNOWN &&
                          ( rest[1] == 'a' || rest[1] == 'A' ) &&
                          ( rest[2] == 'y' || rest[2] == 'Y' ) )
                    month = TT_MAY;
                else if ( zone == TT_UNKNOWN &&
                          ( rest[1] == 'd' || rest[1] == 'D' ) &&
                          ( rest[2] == 't' || rest[2] == 'T' ) )
                    zone = TT_MDT;
                else if ( zone == TT_UNKNOWN &&
                          ( rest[1] == 'e' || rest[1] == 'E' ) &&
                          ( rest[2] == 't' || rest[2] == 'T' ) )
                    zone = TT_MET;
                else if ( dotw == TT_UNKNOWN &&
                          ( rest[1] == 'o' || rest[1] == 'O' ) &&
                          ( rest[2] == 'n' || rest[2] == 'N' ) )
                    dotw = TT_MON;
                else if ( zone == TT_UNKNOWN &&
                          ( rest[1] == 's' || rest[1] == 'S' ) &&
                          ( rest[2] == 't' || rest[2] == 'T' ) )
                    zone = TT_MST;
                break;
            case 'n':
            case 'N':
                if ( month == TT_UNKNOWN &&
                        ( rest[1] == 'o' || rest[1] == 'O' ) &&
                        ( rest[2] == 'v' || rest[2] == 'V' ) )
                    month = TT_NOV;
                else if ( zone == TT_UNKNOWN &&
                          ( rest[1] == 's' || rest[1] == 'S' ) &&
                          ( rest[2] == 't' || rest[2] == 'T' ) )
                    zone = TT_NST;
                break;
            case 'o':
            case 'O':
                if ( month == TT_UNKNOWN &&
                        ( rest[1] == 'c' || rest[1] == 'C' ) &&
                        ( rest[2] == 't' || rest[2] == 'T' ) )
                    month = TT_OCT;
                break;
            case 'p':
            case 'P':
                if ( zone == TT_UNKNOWN &&
                        ( rest[1] == 'd' || rest[1] == 'D' ) &&
                        ( rest[2] == 't' || rest[2] == 'T' ) )
                    zone = TT_PDT;
                else if ( zone == TT_UNKNOWN &&
                          ( rest[1] == 's' || rest[1] == 'S' ) &&
                          ( rest[2] == 't' || rest[2] == 'T' ) )
                    zone = TT_PST;
                break;
            case 's':
            case 'S':
                if ( dotw == TT_UNKNOWN &&
                        ( rest[1] == 'a' || rest[1] == 'A' ) &&
                        ( rest[2] == 't' || rest[2] == 'T' ) )
                    dotw = TT_SAT;
                else if ( month == TT_UNKNOWN &&
                          ( rest[1] == 'e' || rest[1] == 'E' ) &&
                          ( rest[2] == 'p' || rest[2] == 'P' ) )
                    month = TT_SEP;
                else if ( dotw == TT_UNKNOWN &&
                          ( rest[1] == 'u' || rest[1] == 'U' ) &&
                          ( rest[2] == 'n' || rest[2] == 'N' ) )
                    dotw = TT_SUN;
                break;
            case 't':
            case 'T':
                if ( dotw == TT_UNKNOWN &&
                        ( rest[1] == 'h' || rest[1] == 'H' ) &&
                        ( rest[2] == 'u' || rest[2] == 'U' ) )
                    dotw = TT_THU;
                else if ( dotw == TT_UNKNOWN &&
                          ( rest[1] == 'u' || rest[1] == 'U' ) &&
                          ( rest[2] == 'e' || rest[2] == 'E' ) )
                    dotw = TT_TUE;
                break;
            case 'u':
            case 'U':
                if ( zone == TT_UNKNOWN &&
                        ( rest[1] == 't' || rest[1] == 'T' ) &&
                        !( rest[2] >= 'A' && rest[2] <= 'Z' ) &&
                        !( rest[2] >= 'a' && rest[2] <= 'z' ) )
                    /* UT is the same as GMT but UTx is not. */
                    zone = TT_GMT;
                break;
            case 'w':
            case 'W':
                if ( dotw == TT_UNKNOWN &&
                        ( rest[1] == 'e' || rest[1] == 'E' ) &&
                        ( rest[2] == 'd' || rest[2] == 'D' ) )
                    dotw = TT_WED;
                break;

            case '+':
            case '-':
            {
                const char *end;
                int sign;
                if ( zone_offset != -1 )
                {
                    /* already got one... */
                    rest++;
                    break;
                }
                if ( zone != TT_UNKNOWN && zone != TT_GMT )
                {
                    /* GMT+0300 is legal, but PST+0300 is not. */
                    rest++;
                    break;
                }

                sign = ( ( *rest == '+' ) ? 1 : -1 );
                rest++; /* move over sign */
                end = rest;
                while ( *end >= '0' && *end <= '9' )
                    end++;
                if ( rest == end ) /* no digits here */
                    break;

                if ( ( end - rest ) == 4 )
                    /* offset in HHMM */
                    zone_offset = ( ( ( ( ( rest[0] - '0' ) * 10 ) + ( rest[1] - '0' ) ) * 60 ) +
                                    ( ( ( rest[2] - '0' ) * 10 ) + ( rest[3] - '0' ) ) );
                else if ( ( end - rest ) == 2 )
                    /* offset in hours */
                    zone_offset = ( ( ( rest[0] - '0' ) * 10 ) + ( rest[1] - '0' ) ) * 60;
                else if ( ( end - rest ) == 1 )
                    /* offset in hours */
                    zone_offset = ( rest[0] - '0' ) * 60;
                else
                    /* 3 or >4 */
                    break;

                zone_offset *= sign;
                zone = TT_GMT;
                break;
            }

            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                int tmp_hour = -1;
                int tmp_min = -1;
                int tmp_sec = -1;
                const char *end = rest + 1;
                while ( *end >= '0' && *end <= '9' )
                    end++;

                /* end is now the first character after a range of digits. */

                if ( *end == ':' )
                {
                    if ( hour >= 0 && min >= 0 ) /* already got it */
                        break;

                    /* We have seen "[0-9]+:", so this is probably HH:MM[:SS] */
                    if ( ( end - rest ) > 2 )
                        /* it is [0-9][0-9][0-9]+: */
                        break;
                    else if ( ( end - rest ) == 2 )
                        tmp_hour = ( ( rest[0] - '0' ) * 10 +
                                     ( rest[1] - '0' ) );
                    else
                        tmp_hour = ( rest[0] - '0' );

                    /* move over the colon, and parse minutes */

                    rest = ++end;
                    while ( *end >= '0' && *end <= '9' )
                        end++;

                    if ( end == rest )
                        /* no digits after first colon? */
                        break;
                    else if ( ( end - rest ) > 2 )
                        /* it is [0-9][0-9][0-9]+: */
                        break;
                    else if ( ( end - rest ) == 2 )
                        tmp_min = ( ( rest[0] - '0' ) * 10 +
                                    ( rest[1] - '0' ) );
                    else
                        tmp_min = ( rest[0] - '0' );

                    /* now go for seconds */
                    rest = end;
                    if ( *rest == ':' )
                        rest++;
                    end = rest;
                    while ( *end >= '0' && *end <= '9' )
                        end++;

                    if ( end == rest )
                        /* no digits after second colon - that's ok. */
                        ;
                    else if ( ( end - rest ) > 2 )
                        /* it is [0-9][0-9][0-9]+: */
                        break;
                    else if ( ( end - rest ) == 2 )
                        tmp_sec = ( ( rest[0] - '0' ) * 10 +
                                    ( rest[1] - '0' ) );
                    else
                        tmp_sec = ( rest[0] - '0' );

                    /* If we made it here, we've parsed hour and min,
                       and possibly sec, so it worked as a unit. */

                    /* skip over whitespace and see if there's an AM or PM
                       directly following the time.
                     */
                    if ( tmp_hour <= 12 )
                    {
                        const char *s = end;
                        while ( *s && ( *s == ' ' || *s == '\t' ) )
                            s++;
                        if ( ( s[0] == 'p' || s[0] == 'P' ) &&
                                ( s[1] == 'm' || s[1] == 'M' ) )
                            /* 10:05pm == 22:05, and 12:05pm == 12:05 */
                            tmp_hour = ( tmp_hour == 12 ? 12 : tmp_hour + 12 );
                        else if ( tmp_hour == 12 &&
                                  ( s[0] == 'a' || s[0] == 'A' ) &&
                                  ( s[1] == 'm' || s[1] == 'M' ) )
                            /* 12:05am == 00:05 */
                            tmp_hour = 0;
                    }

                    hour = tmp_hour;
                    min = tmp_min;
                    sec = tmp_sec;
                    rest = end;
                    break;
                }
                else if ( ( *end == '/' || *end == '-' ) &&
                          end[1] >= '0' && end[1] <= '9' )
                {
                    /* Perhaps this is 6/16/95, 16/6/95, 6-16-95, or 16-6-95
                       or even 95-06-05...
                       #### But it doesn't handle 1995-06-22.
                     */
                    int n1, n2, n3;
                    const char *s;

                    if ( month != TT_UNKNOWN )
                        /* if we saw a month name, this can't be. */
                        break;

                    s = rest;

                    n1 = ( *s++ - '0' );                              /* first 1 or 2 digits */
                    if ( *s >= '0' && *s <= '9' )
                        n1 = n1 * 10 + ( *s++ - '0' );

                    if ( *s != '/' && *s != '-' )              /* slash */
					{
						if ( *s >= '0' && *s <= '9' )
						{
							n1 = n1 * 100;
							n1 += ( *s++ - '0' ) * 10;
							if ( *s >= '0' && *s <= '9' )
							{
								n1 += ( *s++ - '0' );
							}
							else
							{
								break;
							}
							
						}
						else
						{
							break;
						}
					}
                    s++;

                    if ( *s < '0' || *s > '9' )              /* second 1 or 2 digits */
                        break;
                    n2 = ( *s++ - '0' );
                    if ( *s >= '0' && *s <= '9' )
                        n2 = n2 * 10 + ( *s++ - '0' );

                    if ( *s != '/' && *s != '-' )              /* slash */
                        break;
                    s++;

                    if ( *s < '0' || *s > '9' )              /* third 1, 2, or 4 digits */
                        break;
                    n3 = ( *s++ - '0' );
                    if ( *s >= '0' && *s <= '9' )
                        n3 = n3 * 10 + ( *s++ - '0' );

                    if ( *s >= '0' && *s <= '9' )              /* optional digits 3 and 4 */
                    {
                        n3 = n3 * 10 + ( *s++ - '0' );
                        if ( *s < '0' || *s > '9' )
                            break;
                        n3 = n3 * 10 + ( *s++ - '0' );
                    }

                    if ( ( *s >= '0' && *s <= '9' ) ||     /* followed by non-alphanum */
                            ( *s >= 'A' && *s <= 'Z' ) ||
                            ( *s >= 'a' && *s <= 'z' ) )
                        break;

                    /* Ok, we parsed three 1-2 digit numbers, with / or -
                       between them.  Now decide what the hell they are
                       (DD/MM/YY or MM/DD/YY or YY/MM/DD.)
                     */

                    if ( n1 > 31 || n1 == 0 )  /* must be YY/MM/DD */
                    {
                        if ( n2 > 12 ) break;
                        if ( n3 > 31 ) break;
                        year = n1;
                        if ( year < 70 )
                            year += 2000;
                        else if ( year < 100 )
                            year += 1900;
                        month = ( TIME_TOKEN )( n2 + ( ( int )TT_JAN ) - 1 );
                        date = n3;
                        rest = s;
                        break;
                    }

                    if ( n1 > 12 && n2 > 12 )  /* illegal */
                    {
                        rest = s;
                        break;
                    }

                    if ( n3 < 70 )
                        n3 += 2000;
                    else if ( n3 < 100 )
                        n3 += 1900;

                    if ( n1 > 12 )  /* must be DD/MM/YY */
                    {
                        date = n1;
                        month = ( TIME_TOKEN )( n2 + ( ( int )TT_JAN ) - 1 );
                        year = n3;
                    }
                    else                  /* assume MM/DD/YY */
                    {
                        /* #### In the ambiguous case, should we consult the
                           locale to find out the local default? */
                        month = ( TIME_TOKEN )( n1 + ( ( int )TT_JAN ) - 1 );
                        date = n2;
                        year = n3;
                    }
                    rest = s;
                }
                else if ( ( *end >= 'A' && *end <= 'Z' ) ||
                          ( *end >= 'a' && *end <= 'z' ) )
                    /* Digits followed by non-punctuation - what's that? */
                    ;
                else if ( ( end - rest ) == 4 )            /* four digits is a year */
                    year = ( year < 0
                             ? ( ( rest[0] - '0' ) * 1000L +
                                 ( rest[1] - '0' ) * 100L +
                                 ( rest[2] - '0' ) * 10L +
                                 ( rest[3] - '0' ) )
                             : year );
                else if ( ( end - rest ) == 2 )            /* two digits - date or year */
                {
                    int n = ( ( rest[0] - '0' ) * 10 +
                              ( rest[1] - '0' ) );
                    /* If we don't have a date (day of the month) and we see a number
                         less than 32, then assume that is the date.

                             Otherwise, if we have a date and not a year, assume this is the
                             year.  If it is less than 70, then assume it refers to the 21st
                             century.  If it is two digits (>= 70), assume it refers to this
                             century.  Otherwise, assume it refers to an unambiguous year.

                             The world will surely end soon.
                       */
                    if ( date < 0 && n < 32 )
                        date = n;
                    else if ( year < 0 )
                    {
                        if ( n < 70 )
                            year = 2000 + n;
                        else if ( n < 100 )
                            year = 1900 + n;
                        else
                            year = n;
                    }
                    /* else what the hell is this. */
                }
                else if ( ( end - rest ) == 1 )            /* one digit - date */
                    date = ( date < 0 ? ( rest[0] - '0' ) : date );
                /* else, three or more than four digits - what's that? */

                break;
            }
        }

        /* Skip to the end of this token, whether we parsed it or not.
               Tokens are delimited by whitespace, or ,;-/
               But explicitly not :+-.
         */
        while ( *rest &&
                *rest != ' ' && *rest != '\t' &&
                *rest != ',' && *rest != ';' &&
                *rest != '-' && *rest != '+' &&
                *rest != '/' &&
                *rest != '(' && *rest != ')' && *rest != '[' && *rest != ']' )
            rest++;
        /* skip over uninteresting chars. */
SKIP_MORE:
        while ( *rest &&
                ( *rest == ' ' || *rest == '\t' ||
                  *rest == ',' || *rest == ';' || *rest == '/' ||
                  *rest == '(' || *rest == ')' || *rest == '[' || *rest == ']' ) )
            rest++;

        /* "-" is ignored at the beginning of a token if we have not yet
               parsed a year (e.g., the second "-" in "30-AUG-1966"), or if
               the character after the dash is not a digit. */
        if ( *rest == '-' && ( ( rest > string && isalpha( rest[-1] ) && year < 0 )
                               || rest[1] < '0' || rest[1] > '9' ) )
        {
            rest++;
            goto SKIP_MORE;
        }

    }

    if ( zone != TT_UNKNOWN && zone_offset == -1 )
    {
        switch ( zone )
        {
            case TT_PST:
                zone_offset = -8 * 60;
                break;
            case TT_PDT:
                zone_offset = -7 * 60;
                break;
            case TT_MST:
                zone_offset = -7 * 60;
                break;
            case TT_MDT:
                zone_offset = -6 * 60;
                break;
            case TT_CST:
                zone_offset = -6 * 60;
                break;
            case TT_CDT:
                zone_offset = -5 * 60;
                break;
            case TT_EST:
                zone_offset = -5 * 60;
                break;
            case TT_EDT:
                zone_offset = -4 * 60;
                break;
            case TT_AST:
                zone_offset = -4 * 60;
                break;
            case TT_NST:
                zone_offset = -3 * 60 - 30;
                break;
            case TT_GMT:
                zone_offset =  0 * 60;
                break;
            case TT_BST:
                zone_offset =  1 * 60;
                break;
            case TT_MET:
                zone_offset =  1 * 60;
                break;
            case TT_EET:
                zone_offset =  2 * 60;
                break;
            case TT_JST:
                zone_offset =  9 * 60;
                break;
            default:
                ATLASSERT ( 0 );
                break;
        }
    }

    /* If we didn't find a year, month, or day-of-the-month, we can't
           possibly parse this, and in fact, mktime() will do something random
           (I'm seeing it return "Tue Feb  5 06:28:16 2036", which is no doubt
           a numerologically significant date... */
    if ( month == TT_UNKNOWN || date == -1 || year == -1 )
        return PR_FAILURE;

    memset( &tm, 0, sizeof( tm ) );
    if ( sec != -1 )
        tm.tm_sec = sec;
    if ( min != -1 )
        tm.tm_min = min;
    if ( hour != -1 )
        tm.tm_hour = hour;
    if ( date != -1 )
        tm.tm_mday = date;
    if ( month != TT_UNKNOWN )
        tm.tm_month = ( ( ( int )month ) - ( ( int )TT_JAN ) );
    if ( year != -1 )
        tm.tm_year = year;
    if ( dotw != TT_UNKNOWN )
        tm.tm_wday = ( ( ( int )dotw ) - ( ( int )TT_SUN ) );

    if ( zone == TT_UNKNOWN && default_to_gmt )
    {
        /* No zone was specified, so pretend the zone was GMT. */
        zone = TT_GMT;
        zone_offset = 0;
    }

    if ( zone_offset == -1 )
    {
        /* no zone was specified, and we're to assume that everything
          is local. */
        struct tm localTime;
        time_t secs;

        ATLASSERT( tm.tm_month > -1
                   && tm.tm_mday > 0
                   && tm.tm_hour > -1
                   && tm.tm_min > -1
                   && tm.tm_sec > -1 );

        /*
         * To obtain time_t from a tm structure representing the local
         * time, we call mktime().  However, we need to see if we are
         * on 1-Jan-1970 or before.  If we are, we can't call mktime()
         * because mktime() will crash on win16. In that case, we
         * calculate zone_offset based on the zone offset at
         * 00:00:00, 2 Jan 1970 GMT, and subtract zone_offset from the
         * date we are parsing to transform the date to GMT.  We also
         * do so if mktime() returns (time_t) -1 (time out of range).
        */

        /* month, day, hours, mins and secs are always non-negative
           so we dont need to worry about them. */
        if ( tm.tm_year >= 1970 )
        {
            __int64 usec_per_sec;

            localTime.tm_sec = tm.tm_sec;
            localTime.tm_min = tm.tm_min;
            localTime.tm_hour = tm.tm_hour;
            localTime.tm_mday = tm.tm_mday;
            localTime.tm_mon = tm.tm_month;
            localTime.tm_year = tm.tm_year - 1900;
            /* Set this to -1 to tell mktime "I don't care".  If you set
               it to 0 or 1, you are making assertions about whether the
               date you are handing it is in daylight savings mode or not;
               and if you're wrong, it will "fix" it for you. */
            localTime.tm_isdst = -1;
            secs = mktime( &localTime );
            if ( secs != ( time_t ) - 1 )
            {
#if defined(XP_MAC) && (__MSL__ < 0x6000)
                /*
                 * The mktime() routine in MetroWerks MSL C
                 * Runtime library returns seconds since midnight,
                 * 1 Jan. 1900, not 1970 - in versions of MSL (Metrowerks Standard
                 * Library) prior to version 6.  Only for older versions of
                 * MSL do we adjust the value of secs to the NSPR epoch
                 */
                secs -= ( ( 365 * 70UL ) + 17 ) * 24 * 60 * 60;
#endif
                LL_I2L( *result, secs );
                LL_I2L( usec_per_sec, PR_USEC_PER_SEC );
                LL_MUL( *result, *result, usec_per_sec );
                return PR_SUCCESS;
            }
        }

        /* So mktime() can't handle this case.  We assume the
           zone_offset for the date we are parsing is the same as
           the zone offset on 00:00:00 2 Jan 1970 GMT. */
        secs = 86400;
        localtime_r( &secs, &localTime );
        zone_offset = localTime.tm_min
                      + 60 * localTime.tm_hour
                      + 1440 * ( localTime.tm_mday - 2 );
    }

    tm.tm_params.tp_gmt_offset = zone_offset * 60;

    *result = PR_ImplodeTime( &tm );

    return PR_SUCCESS;
}

// static
bool CPcTime::FromString( const wchar_t* time_string, CPcTime* parsed_time )
{
    std::string ascii_time_string = CommonHelper::ConvertLocalToUtf8( time_string );
    if ( ascii_time_string.length() == 0 )
    {
        return false;
    }
    __int64 result_time = 0;
    PRStatus result = PR_ParseTimeString( ascii_time_string.c_str(), PR_FALSE,
                                          &result_time );
    if ( PR_SUCCESS != result )
    {
        return false;
    }
    result_time += kTimeTToMicrosecondsOffset;
    *parsed_time = CPcTime( result_time );
    return true;
}


template <class Dest, class Source>
inline Dest bit_cast( const Source& source )
{
    // Compile time assertion: sizeof(Dest) == sizeof(Source)
    // A compile error here means your Dest and Source have different sizes.
    typedef char VerifySizesAreEqual [sizeof( Dest ) == sizeof( Source ) ? 1 : -1];

    Dest dest;
    memcpy( &dest, &source, sizeof( dest ) );
    return dest;
}

static void MicrosecondsToFileTime( __int64 us, FILETIME* ft )
{
    *ft = bit_cast<FILETIME, __int64>( us * 10 );
}

__int64 FileTimeToMicroseconds( const FILETIME& ft )
{
    return bit_cast<__int64, FILETIME>( ft ) / 10;
}

__int64 CPcTime::CurrentWallclockMicroseconds()
{
    FILETIME ft;
    ::GetSystemTimeAsFileTime( &ft );
    return FileTimeToMicroseconds( ft );
}

CPcTime CPcTime::FromExploded( bool is_local, const TDateTime& exploded )
{
    // Create the system struct representing our exploded time. It will either be
    // in local time or UTC.
    SYSTEMTIME st;
    st.wYear = exploded.year;
    st.wMonth = exploded.month;
    st.wDayOfWeek = exploded.day_of_week;
    st.wDay = exploded.day_of_month;
    st.wHour = exploded.hour;
    st.wMinute = exploded.minute;
    st.wSecond = exploded.second;
    st.wMilliseconds = exploded.millisecond;

    // Convert to FILETIME.
    FILETIME ft;
    if ( !SystemTimeToFileTime( &st, &ft ) )
    {
        return CPcTime( 0 );
    }

    // Ensure that it's in UTC.
    if ( is_local )
    {
        FILETIME utc_ft;
        LocalFileTimeToFileTime( &ft, &utc_ft );
        return CPcTime( FileTimeToMicroseconds( utc_ft ) );
    }
    return CPcTime( FileTimeToMicroseconds( ft ) );
}


void CPcTime::Explode( bool is_local, TDateTime* exploded ) const
{
    // FILETIME in UTC.
    FILETIME utc_ft;
    MicrosecondsToFileTime( us_, &utc_ft );

    // FILETIME in local time if necessary.
    BOOL success = TRUE;
    FILETIME ft;
    if ( is_local )
        success = FileTimeToLocalFileTime( &utc_ft, &ft );
    else
        ft = utc_ft;

    // FILETIME in SYSTEMTIME (exploded).
    SYSTEMTIME st;
    if ( !success || !FileTimeToSystemTime( &ft, &st ) )
    {
        //NOTREACHED() << "Unable to convert time, don't know why";
        ZeroMemory( exploded, sizeof( exploded ) );
        return;
    }

    exploded->year = st.wYear;
    exploded->month = st.wMonth;
    exploded->day_of_week = st.wDayOfWeek;
    exploded->day_of_month = st.wDay;
    exploded->hour = st.wHour;
    exploded->minute = st.wMinute;
    exploded->second = st.wSecond;
    exploded->millisecond = st.wMilliseconds;

}

//CPcTimeTicks::TickFunction CPcTimeTicks::tick_function_= reinterpret_cast<CPcTimeTicks::TickFunction>(&timeGetTime);
typedef DWORD ( __stdcall *TickFunction )( void );
static TickFunction tick_function_;

CComAutoCriticalSection g_csPcTime;
CPcTimeTicks CPcTimeTicks::Now()
{
    // Uses the multimedia timers on Windows to get a higher resolution clock.
    // timeGetTime() provides a resolution which is variable depending on
    // hardware and system configuration.  It can also be changed by other
    // apps.  This class does not attempt to change the resolution of the
    // timer, because we don't want to affect other applications.

    // timeGetTime() should at least be accurate to ~5ms on all systems.
    // timeGetTime() returns a 32-bit millisecond counter which has rollovers
    // every ~49 days.
    static DWORD last_tick_count = 0;
    static __int64 tick_rollover_accum = 0;
    //static Lock* tick_lock = NULL;  // To protect during rollover periods.

 
    // Atomically protect the low and high 32bit values for time.
    // In the future we may be able to optimize with
    // InterlockedCompareExchange64, but that doesn't work on XP.
    DWORD tick_count;
    __int64 rollover_count;
    {

		CComCritSecLock<CComAutoCriticalSection> guard( g_csPcTime );

		tick_function_= reinterpret_cast<TickFunction>(&timeGetTime);
        tick_count = timeGetTime();
        if ( tick_count < last_tick_count )
            tick_rollover_accum += 0x100000000I64;

        last_tick_count = tick_count;
        rollover_count = tick_rollover_accum;
    }

    // GetTickCount returns milliseconds, we want microseconds.
    return CPcTimeTicks( ( tick_count + rollover_count ) *
                         CPcTime::kMicrosecondsPerMillisecond );
}



}