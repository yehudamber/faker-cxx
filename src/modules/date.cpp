#include "faker-cxx/date.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>

#include "common/format_helper.h"
#include "date_data.h"
#include "faker-cxx/generator.h"
#include "faker-cxx/helper.h"
#include "faker-cxx/number.h"

namespace faker::date
{
std::string serializeTimePoint(const auto& timePoint, DateFormat dateFormat)
{
    time_t timePointTimeT = std::chrono::system_clock::to_time_t(timePoint);

    std::tm utcTime = *std::gmtime(&timePointTimeT);

    std::stringstream ss;

    if (dateFormat == DateFormat::Timestamp)
    {
        ss << std::chrono::duration_cast<std::chrono::seconds>(timePoint.time_since_epoch()).count();
    }
    else
    {
        ss << std::put_time(&utcTime, "%Y-%m-%dT%H:%M:%SZ");
    }

    return ss.str();
}

std::string betweenDate(const auto& from, const auto& to, DateFormat dateFormat)
{
    const auto size = std::chrono::duration_cast<std::chrono::seconds>(to - from).count();

    const auto randomDateWithinRange = from + std::chrono::seconds{number::integer(size - 1)};

    return serializeTimePoint(randomDateWithinRange, dateFormat);
}

std::string between(const std::string& from, const std::string& to, DateFormat dateFormat)
{
    std::istringstream fromStream(from);
    std::istringstream toStream(to);

    std::tm fromTm{}, toTm{};

    fromStream >> std::get_time(&fromTm, "%Y-%m-%dT%H:%M:%SZ");
    toStream >> std::get_time(&toTm, "%Y-%m-%dT%H:%M:%SZ");

    auto fromTimePoint = std::chrono::system_clock::from_time_t(std::mktime(&fromTm));
    auto toTimePoint = std::chrono::system_clock::from_time_t(std::mktime(&toTm));

    return betweenDate(fromTimePoint, toTimePoint, dateFormat);
}

std::string between(int64_t from, int64_t to, DateFormat dateFormat)
{
    const auto fromTimePoint = std::chrono::system_clock::from_time_t(from);
    const auto toTimePoint = std::chrono::system_clock::from_time_t(to);

    return betweenDate(fromTimePoint, toTimePoint, dateFormat);
}

const auto numberOfHoursInDay = 24;
const auto numberOfDaysInYear = 365;

std::string anytime(DateFormat dateFormat)
{
    constexpr int64_t total_seconds = 3600LL * 24LL * 365LL * 200LL; // sec/hr * hr/d * d/yr * years

    int64_t now_seconds =
        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    int64_t max_seconds = now_seconds + total_seconds;

    std::mt19937_64& gen = getGenerator();
    std::uniform_int_distribution<int64_t> dis(0, max_seconds);

    int64_t random_seconds = dis(gen);

    auto timePoint = std::chrono::system_clock::time_point{std::chrono::seconds{random_seconds}};
    std::string result;

    if (dateFormat == DateFormat::Timestamp)
    {
        result = std::to_string(random_seconds);
    }
    else
    {
        time_t timePointTimeT = std::chrono::system_clock::to_time_t(timePoint);

        std::tm utcTime = *std::gmtime(&timePointTimeT);

        std::stringstream ss;
        ss << std::put_time(&utcTime, "%Y-%m-%dT%H:%M:%SZ");
        result = ss.str();
    }

    return result;
}

std::string futureDate(unsigned int years, DateFormat dateFormat)
{
    const auto startDate = std::chrono::system_clock::now() + std::chrono::hours{1};

    const auto endDate = startDate + std::chrono::hours{numberOfHoursInDay * numberOfDaysInYear * years};

    return betweenDate(startDate, endDate, dateFormat);
}

std::string pastDate(unsigned int years, DateFormat dateFormat)
{
    const auto startDate =
        std::chrono::system_clock::now() - std::chrono::hours{numberOfHoursInDay * numberOfDaysInYear * years};

    const auto endDate = std::chrono::system_clock::now() - std::chrono::hours{1};

    return betweenDate(startDate, endDate, dateFormat);
}

std::string soonDate(unsigned int days, DateFormat dateFormat)
{
    const auto startDate = std::chrono::system_clock::now() + std::chrono::hours{1};

    const auto endDate = startDate + std::chrono::hours{numberOfHoursInDay * days};

    return betweenDate(startDate, endDate, dateFormat);
}

std::string recentDate(unsigned int days, DateFormat dateFormat)
{
    const auto startDate = std::chrono::system_clock::now() - std::chrono::hours{numberOfHoursInDay * days};

    const auto endDate = std::chrono::system_clock::now() - std::chrono::hours{1};

    return betweenDate(startDate, endDate, dateFormat);
}

std::string birthdateByAge(unsigned int minAge, unsigned int maxAge, DateFormat dateFormat)
{
    const auto startDate =
        std::chrono::system_clock::now() - std::chrono::hours{numberOfHoursInDay * numberOfDaysInYear * maxAge};

    const auto endDate =
        std::chrono::system_clock::now() - std::chrono::hours{numberOfHoursInDay * numberOfDaysInYear * minAge};

    return betweenDate(startDate, endDate, dateFormat);
}

std::string birthdateByYear(unsigned int minYear, unsigned int maxYear, DateFormat dateFormat)
{
    tm startDateTime{};
    startDateTime.tm_year = static_cast<int>(minYear - 1900);
    startDateTime.tm_mon = 0;
    startDateTime.tm_mday = 1;
    startDateTime.tm_hour = 0;
    startDateTime.tm_min = 0;
    startDateTime.tm_sec = 0;
    startDateTime.tm_isdst = -1;

    const auto startDate = std::chrono::system_clock::from_time_t(mktime(&startDateTime));

    tm endDateTime{};
    endDateTime.tm_year = static_cast<int>(maxYear - 1900);
    endDateTime.tm_mon = 11;
    endDateTime.tm_mday = 31;
    endDateTime.tm_hour = 23;
    endDateTime.tm_min = 59;
    endDateTime.tm_sec = 59;
    endDateTime.tm_isdst = -1;

    const auto endDate = std::chrono::system_clock::from_time_t(mktime(&endDateTime));

    return betweenDate(startDate, endDate, dateFormat);
}

std::string_view weekdayName()
{
    return helper::randomElement(weekdayNames);
}

std::string_view weekdayAbbreviatedName()
{
    return helper::randomElement(weekdayAbbreviatedNames);
}

std::string_view monthName()
{
    return helper::randomElement(monthNames);
}

std::string_view monthAbbreviatedName()
{
    return helper::randomElement(monthAbbreviatedNames);
}

unsigned int year()
{
    unsigned minYear = 1950;
    unsigned maxYear = 2050;

    return number::integer<unsigned>(minYear, maxYear);
}

unsigned int month()
{
    return number::integer<unsigned>(1, 12);
}

unsigned int hour()
{
    return number::integer<unsigned>(0, 23);
}

unsigned int minute()
{
    return number::integer<unsigned>(0, 59);
}

unsigned int second()
{
    return number::integer<unsigned>(0, 59);
}

std::string time()
{
    return common::format("{:02}:{:02}", hour(), minute());
}

unsigned int dayOfMonth()
{
    return number::integer<unsigned>(1, 31);
}

unsigned dayOfWeek()
{
    return number::integer<unsigned>(1, 7);
}

std::string_view timezoneRandom()
{
    return helper::randomElement(timezonesAbbreviatedNames);
}

}
