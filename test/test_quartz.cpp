#include "catch.hpp"
#include "croncpp.h"

#define ARE_EQUAL(x, y)          REQUIRE(x == y)
#define CRON_EXPR(x)             make_cron<cron::cron_quartz_traits>(x)
#define CRON_NEXT(x, y)             cron_next<cron::cron_quartz_traits>(x, y)
#define CRON_QUARTZ_EQUAL(x, y)     ARE_EQUAL(CRON_EXPR(x), CRON_EXPR(y))
#define CRON_EXPECT_EXCEPT(x)    REQUIRE_THROWS_AS(CRON_EXPR(x), bad_cronexpr)
#define CRON_EXPECT_MSG(x, msg)  REQUIRE_THROWS_WITH(CRON_EXPR(x), msg)

using namespace cron;

constexpr bool operator==(std::tm const & tm1, std::tm const & tm2) noexcept
{
   return
      tm1.tm_sec == tm2.tm_sec &&
      tm1.tm_min == tm2.tm_min &&
      tm1.tm_hour == tm2.tm_hour &&
      tm1.tm_mday == tm2.tm_mday &&
      tm1.tm_mon == tm2.tm_mon &&
      tm1.tm_year == tm2.tm_year &&
      tm1.tm_wday == tm2.tm_wday &&
      tm1.tm_yday == tm2.tm_yday &&
      tm1.tm_isdst == tm2.tm_isdst;
}

void check_next_quartz(std::string_view expr, std::string_view time, std::string_view expected)
{
   auto cex = CRON_EXPR(expr);

   auto initial_time = utils::to_tm(time);

   auto result1 = CRON_NEXT(cex, utils::tm_to_time(initial_time));
   auto result2 = CRON_NEXT(cex, initial_time);

   std::tm result1_tm;
   utils::time_to_tm(&result1, &result1_tm);

   REQUIRE(result1_tm == result2);

   auto value = utils::to_string(result1_tm);

   REQUIRE(value == expected);
}

TEST_CASE("Test simple quartz", "")
{
   auto cex = CRON_EXPR("* * * * * *");
   auto cex_with_years = CRON_EXPR("* * * * * * *");
   REQUIRE(to_string(cex) == "111111111111111111111111111111111111111111111111111111111111 111111111111111111111111111111111111111111111111111111111111 111111111111111111111111 1111111111111111111111111111111 111111111111 1111111 1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111");
   REQUIRE(to_string(cex_with_years) == "111111111111111111111111111111111111111111111111111111111111 111111111111111111111111111111111111111111111111111111111111 111111111111111111111111 1111111111111111111111111111111 111111111111 1111111 1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111");
}

TEST_CASE("quartz: check seconds", "[quartz]")
{
   CRON_QUARTZ_EQUAL("*/5 * * * * *", "0,5,10,15,20,25,30,35,40,45,50,55 * * * * *");
   CRON_QUARTZ_EQUAL("1/6 * * * * *", "1,7,13,19,25,31,37,43,49,55 * * * * *");
   CRON_QUARTZ_EQUAL("0/30 * * * * *", "0,30 * * * * *");
   CRON_QUARTZ_EQUAL("0-5 * * * * *", "0,1,2,3,4,5 * * * * *");
   CRON_QUARTZ_EQUAL("55/1 * * * * *", "55,56,57,58,59 * * * * *");
   CRON_QUARTZ_EQUAL("57,59 * * * * *", "57/2 * * * * *");
   CRON_QUARTZ_EQUAL("1,3,5 * * * * *", "1-6/2 * * * * *");
   CRON_QUARTZ_EQUAL("0,5,10,15 * * * * *", "0-15/5 * * * * *");
}

TEST_CASE("quartz: check minutes", "[quartz]")
{
   CRON_QUARTZ_EQUAL("* */5 * * * *", "* 0,5,10,15,20,25,30,35,40,45,50,55 * * * *");
   CRON_QUARTZ_EQUAL("* 1/6 * * * *", "* 1,7,13,19,25,31,37,43,49,55 * * * *");
   CRON_QUARTZ_EQUAL("* 0/30 * * * *", "* 0,30 * * * *");
   CRON_QUARTZ_EQUAL("* 0-5 * * * *", "* 0,1,2,3,4,5 * * * *");
   CRON_QUARTZ_EQUAL("* 55/1 * * * *", "* 55,56,57,58,59 * * * *");
   CRON_QUARTZ_EQUAL("* 57,59 * * * *", "* 57/2 * * * *");
   CRON_QUARTZ_EQUAL("* 1,3,5 * * * *", "* 1-6/2 * * * *");
   CRON_QUARTZ_EQUAL("* 0,5,10,15 * * * *", "* 0-15/5 * * * *");
}

TEST_CASE("quartz: check hours", "[quartz]")
{
   CRON_QUARTZ_EQUAL("* * */5 * * *", "* * 0,5,10,15,20 * * *");
   CRON_QUARTZ_EQUAL("* * 1/6 * * *", "* * 1,7,13,19 * * *");
   CRON_QUARTZ_EQUAL("* * 0/12 * * *", "* * 0,12 * * *");
   CRON_QUARTZ_EQUAL("* * 0-5 * * *", "* * 0,1,2,3,4,5 * * *");
   CRON_QUARTZ_EQUAL("* * 15/1 * * *", "* * 15,16,17,18,19,20,21,22,23 * * *");
   CRON_QUARTZ_EQUAL("* * 17,19,21,23 * * *", "* * 17/2 * * *");
   CRON_QUARTZ_EQUAL("* * 1,3,5 * * *", "* * 1-6/2 * * *");
   CRON_QUARTZ_EQUAL("* * 0,5,10,15 * * *", "* * 0-15/5 * * *");
}

TEST_CASE("quartz: check days of month", "[quartz]")
{
   CRON_QUARTZ_EQUAL("* * * 1-31 * *", "* * * 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31 * *");
   CRON_QUARTZ_EQUAL("* * * 1/5 * *", "* * * 1,6,11,16,21,26,31 * *");
   CRON_QUARTZ_EQUAL("* * * 1,11,21,31 * *", "* * * 1-31/10 * *");
   CRON_QUARTZ_EQUAL("* * * */5 * *", "* * * 1,6,11,16,21,26,31 * *");
}

TEST_CASE("quartz: check months", "[quartz]")
{
   CRON_QUARTZ_EQUAL("* * * * 1,6,11 *", "* * * * 1/5 *");
   CRON_QUARTZ_EQUAL("* * * * 1-12 *", "* * * * 1,2,3,4,5,6,7,8,9,10,11,12 *");
   CRON_QUARTZ_EQUAL("* * * * 1-12/3 *", "* * * * 1,4,7,10 *");
   CRON_QUARTZ_EQUAL("* * * * */2 *", "* * * * 1,3,5,7,9,11 *");
   CRON_QUARTZ_EQUAL("* * * * 2/2 *", "* * * * 2,4,6,8,10,12 *");
   CRON_QUARTZ_EQUAL("* * * * 1 *", "* * * * JAN *");
   CRON_QUARTZ_EQUAL("* * * * 2 *", "* * * * FEB *");
   CRON_QUARTZ_EQUAL("* * * * 3 *", "* * * * mar *");
   CRON_QUARTZ_EQUAL("* * * * 4 *", "* * * * apr *");
   CRON_QUARTZ_EQUAL("* * * * 5 *", "* * * * may *");
   CRON_QUARTZ_EQUAL("* * * * 6 *", "* * * * Jun *");
   CRON_QUARTZ_EQUAL("* * * * 7 *", "* * * * Jul *");
   CRON_QUARTZ_EQUAL("* * * * 8 *", "* * * * auG *");
   CRON_QUARTZ_EQUAL("* * * * 9 *", "* * * * sEp *");
   CRON_QUARTZ_EQUAL("* * * * 10 *", "* * * * oCT *");
   CRON_QUARTZ_EQUAL("* * * * 11 *", "* * * * nOV *");
   CRON_QUARTZ_EQUAL("* * * * 12 *", "* * * * DEC *");
   CRON_QUARTZ_EQUAL("* * * * 1,FEB *", "* * * * JAN,2 *");
   CRON_QUARTZ_EQUAL("* * * * 1,6,11 *", "* * * * NOV,JUN,jan *");
   CRON_QUARTZ_EQUAL("* * * * 1,6,11 *", "* * * * jan,jun,nov *");
   CRON_QUARTZ_EQUAL("* * * * 1,6,11 *", "* * * * jun,jan,nov *");
   CRON_QUARTZ_EQUAL("* * * * JAN,FEB,MAR,APR,MAY,JUN,JUL,AUG,SEP,OCT,NOV,DEC *", "* * * * 1,2,3,4,5,6,7,8,9,10,11,12 *");
   CRON_QUARTZ_EQUAL("* * * * JUL,AUG,SEP,OCT,NOV,DEC,JAN,FEB,MAR,APR,MAY,JUN *", "* * * * 1,2,3,4,5,6,7,8,9,10,11,12 *");
}

TEST_CASE("quartz: check days of week", "[quartz]")
{
   CRON_QUARTZ_EQUAL("* * * * * 1-7", "* * * * * 1,2,3,4,5,6,7");
   CRON_QUARTZ_EQUAL("* * * * * 1-7/2", "* * * * * 1,3,5,7");
   CRON_QUARTZ_EQUAL("* * * * * 1-7/3", "* * * * * 1,4,7");
   CRON_QUARTZ_EQUAL("* * * * * */3", "* * * * * 1,4,7");
   CRON_QUARTZ_EQUAL("* * * * * 2/3", "* * * * * 2,5");
   CRON_QUARTZ_EQUAL("* * * * * 1", "* * * * * SUN");
   CRON_QUARTZ_EQUAL("* * * * * 2", "* * * * * MON");
   CRON_QUARTZ_EQUAL("* * * * * 3", "* * * * * TUE");
   CRON_QUARTZ_EQUAL("* * * * * 4", "* * * * * WED");
   CRON_QUARTZ_EQUAL("* * * * * 5", "* * * * * THU");
   CRON_QUARTZ_EQUAL("* * * * * 6", "* * * * * FRI");
   CRON_QUARTZ_EQUAL("* * * * * 7", "* * * * * SAT");
   CRON_QUARTZ_EQUAL("* * * * * 1-7", "* * * * * SUN,MON,TUE,WED,THU,FRI,SAT");
   CRON_QUARTZ_EQUAL("* * * * * 1-7/2", "* * * * * SUN,TUE,THU,SAT");
   CRON_QUARTZ_EQUAL("* * * * * 1-7/3", "* * * * * SUN,WED,SAT");
   CRON_QUARTZ_EQUAL("* * * * * */3", "* * * * * SUN,WED,SAT");
   CRON_QUARTZ_EQUAL("* * * * * 2/3", "* * * * * MON,THU");
}

TEST_CASE("quartz: check year", "[quartz]")
{
	CRON_QUARTZ_EQUAL("* * * * * ? 2018-2020", "* * * * * ? 2018,2019,2020");
	CRON_QUARTZ_EQUAL("* * * * * ? 2018-2028/2", "* * * * * ? 2018,2020,2022,2024,2026,2028");
}


TEST_CASE("quartz: invalid seconds", "[quartz]")
{
   CRON_EXPECT_EXCEPT("TEN * * * * *");
   CRON_EXPECT_EXCEPT("60 * * * * *");
   CRON_EXPECT_EXCEPT("-1 * * * * *");
   CRON_EXPECT_EXCEPT("0-60 * * * * *");
   CRON_EXPECT_EXCEPT("0-10-20 * * * * *");
   CRON_EXPECT_EXCEPT(", * * * * *");
   CRON_EXPECT_EXCEPT("0,,1 * * * * *");
   CRON_EXPECT_EXCEPT("0,1, * * * * *");
   CRON_EXPECT_EXCEPT(",0,1 * * * * *");
   CRON_EXPECT_EXCEPT("0/10/2 * * * * *");
   CRON_EXPECT_EXCEPT("/ * * * * *");
   CRON_EXPECT_EXCEPT("/2 * * * * *");
   CRON_EXPECT_EXCEPT("2/ * * * * *");
   CRON_EXPECT_EXCEPT("*/ * * * * *");
   CRON_EXPECT_EXCEPT("/* * * * * *");
   CRON_EXPECT_EXCEPT("-/ * * * * *");
   CRON_EXPECT_EXCEPT("/- * * * * *");
   CRON_EXPECT_EXCEPT("*-/ * * * * *");
   CRON_EXPECT_EXCEPT("-*/ * * * * *");
   CRON_EXPECT_EXCEPT("/-* * * * * *");
   CRON_EXPECT_EXCEPT("/*- * * * * *");
   CRON_EXPECT_EXCEPT("*2/ * * * * *");
   CRON_EXPECT_EXCEPT("/2* * * * * *");
   CRON_EXPECT_EXCEPT("-2/ * * * * *");
   CRON_EXPECT_EXCEPT("/2- * * * * *");
   CRON_EXPECT_EXCEPT("*2-/ * * * * *");
   CRON_EXPECT_EXCEPT("-2*/ * * * * *");
   CRON_EXPECT_EXCEPT("/2-* * * * * *");
   CRON_EXPECT_EXCEPT("/2*- * * * * *");
}

TEST_CASE("quartz: invalid minutes", "[quartz]")
{
   CRON_EXPECT_EXCEPT("* TEN * * * *");
   CRON_EXPECT_EXCEPT("* 60 * * * *");
   CRON_EXPECT_EXCEPT("* -1 * * * *");
   CRON_EXPECT_EXCEPT("* 0-60 * * * *");
   CRON_EXPECT_EXCEPT("* 0-10-20 * * * *");
   CRON_EXPECT_EXCEPT("* , * * * *");
   CRON_EXPECT_EXCEPT("* 0,,1 * * * *");
   CRON_EXPECT_EXCEPT("* 0,1, * * * *");
   CRON_EXPECT_EXCEPT("* ,0,1 * * * *");
   CRON_EXPECT_EXCEPT("* 0/10/2 * * * *");
   CRON_EXPECT_EXCEPT("* / * * * *");
   CRON_EXPECT_EXCEPT("* /2 * * * *");
   CRON_EXPECT_EXCEPT("* 2/ * * * *");
   CRON_EXPECT_EXCEPT("* */ * * * *");
   CRON_EXPECT_EXCEPT("* /* * * * *");
   CRON_EXPECT_EXCEPT("* -/ * * * *");
   CRON_EXPECT_EXCEPT("* /- * * * *");
   CRON_EXPECT_EXCEPT("* *-/ * * * *");
   CRON_EXPECT_EXCEPT("* -*/ * * * *");
   CRON_EXPECT_EXCEPT("* /-* * * * *");
   CRON_EXPECT_EXCEPT("* /*- * * * *");
   CRON_EXPECT_EXCEPT("* *2/ * * * *");
   CRON_EXPECT_EXCEPT("* /2* * * * *");
   CRON_EXPECT_EXCEPT("* -2/ * * * *");
   CRON_EXPECT_EXCEPT("* /2- * * * *");
   CRON_EXPECT_EXCEPT("* *2-/ * * * *");
   CRON_EXPECT_EXCEPT("* -2*/ * * * *");
   CRON_EXPECT_EXCEPT("* /2-* * * * *");
   CRON_EXPECT_EXCEPT("* /2*- * * * *");
}

TEST_CASE("quartz: invalid hours", "[quartz]")
{
   CRON_EXPECT_EXCEPT("* * TEN * * *");
   CRON_EXPECT_EXCEPT("* * 24 * * *");
   CRON_EXPECT_EXCEPT("* * -1 * * *");
   CRON_EXPECT_EXCEPT("* * 0-24 * * *");
   CRON_EXPECT_EXCEPT("* * 0-10-20 * * *");
   CRON_EXPECT_EXCEPT("* * , * * *");
   CRON_EXPECT_EXCEPT("* * 0,,1 * * *");
   CRON_EXPECT_EXCEPT("* * 0,1, * * *");
   CRON_EXPECT_EXCEPT("* * ,0,1 * * *");
   CRON_EXPECT_EXCEPT("* * 0/10/2 * * *");
   CRON_EXPECT_EXCEPT("* * / * * *");
   CRON_EXPECT_EXCEPT("* * /2 * * *");
   CRON_EXPECT_EXCEPT("* * 2/ * * *");
   CRON_EXPECT_EXCEPT("* * */ * * *");
   CRON_EXPECT_EXCEPT("* * /* * * *");
   CRON_EXPECT_EXCEPT("* * -/ * * *");
   CRON_EXPECT_EXCEPT("* * /- * * *");
   CRON_EXPECT_EXCEPT("* * *-/ * * *");
   CRON_EXPECT_EXCEPT("* * -*/ * * *");
   CRON_EXPECT_EXCEPT("* * /-* * * *");
   CRON_EXPECT_EXCEPT("* * /*- * * *");
   CRON_EXPECT_EXCEPT("* * *2/ * * *");
   CRON_EXPECT_EXCEPT("* * /2* * * *");
   CRON_EXPECT_EXCEPT("* * -2/ * * *");
   CRON_EXPECT_EXCEPT("* * /2- * * *");
   CRON_EXPECT_EXCEPT("* * *2-/ * * *");
   CRON_EXPECT_EXCEPT("* * -2*/ * * *");
   CRON_EXPECT_EXCEPT("* * /2-* * * *");
   CRON_EXPECT_EXCEPT("* * /2*- * * *");
}

TEST_CASE("quartz: invalid days of month", "[quartz]")
{
   CRON_EXPECT_EXCEPT("* * * TEN * *");
   CRON_EXPECT_EXCEPT("* * * 32 * *");
   CRON_EXPECT_EXCEPT("* * * 0 * *");
   CRON_EXPECT_EXCEPT("* * * 0-32 * *");
   CRON_EXPECT_EXCEPT("* * * 0-10-20 * *");
   CRON_EXPECT_EXCEPT("* * * , * *");
   CRON_EXPECT_EXCEPT("* * * 0,,1 * *");
   CRON_EXPECT_EXCEPT("* * * 0,1, * *");
   CRON_EXPECT_EXCEPT("* * * ,0,1 * *");
   CRON_EXPECT_EXCEPT("* * * 0/10/2 * * *");
   CRON_EXPECT_EXCEPT("* * * / * *");
   CRON_EXPECT_EXCEPT("* * * /2 * *");
   CRON_EXPECT_EXCEPT("* * * 2/ * *");
   CRON_EXPECT_EXCEPT("* * * */ * *");
   CRON_EXPECT_EXCEPT("* * * /* * *");
   CRON_EXPECT_EXCEPT("* * * -/ * *");
   CRON_EXPECT_EXCEPT("* * * /- * *");
   CRON_EXPECT_EXCEPT("* * * *-/ * *");
   CRON_EXPECT_EXCEPT("* * * -*/ * *");
   CRON_EXPECT_EXCEPT("* * * /-* * *");
   CRON_EXPECT_EXCEPT("* * * /*- * *");
   CRON_EXPECT_EXCEPT("* * * *2/ * *");
   CRON_EXPECT_EXCEPT("* * * /2* * *");
   CRON_EXPECT_EXCEPT("* * * -2/ * *");
   CRON_EXPECT_EXCEPT("* * * /2- * *");
   CRON_EXPECT_EXCEPT("* * * *2-/ * *");
   CRON_EXPECT_EXCEPT("* * * -2*/ * *");
   CRON_EXPECT_EXCEPT("* * * /2-* * *");
   CRON_EXPECT_EXCEPT("* * * /2*- * *");
}

TEST_CASE("quartz: invalid months", "[quartz]")
{
   CRON_EXPECT_EXCEPT("* * * * TEN *");
   CRON_EXPECT_EXCEPT("* * * * 13 *");
   CRON_EXPECT_EXCEPT("* * * * 0 *");
   CRON_EXPECT_EXCEPT("* * * * 0-13 *");
   CRON_EXPECT_EXCEPT("* * * * 0-10-11 *");
   CRON_EXPECT_EXCEPT("* * * * , *");
   CRON_EXPECT_EXCEPT("* * * * 0,,1 *");
   CRON_EXPECT_EXCEPT("* * * * 0,1, *");
   CRON_EXPECT_EXCEPT("* * * * ,0,1 *");
   CRON_EXPECT_EXCEPT("* * * * 0/10/2 *");
   CRON_EXPECT_EXCEPT("* * * * / *");
   CRON_EXPECT_EXCEPT("* * * * /2 *");
   CRON_EXPECT_EXCEPT("* * * * 2/ *");
   CRON_EXPECT_EXCEPT("* * * * */ *");
   CRON_EXPECT_EXCEPT("* * * * /* *");
   CRON_EXPECT_EXCEPT("* * * * -/ *");
   CRON_EXPECT_EXCEPT("* * * * /- *");
   CRON_EXPECT_EXCEPT("* * * * *-/ *");
   CRON_EXPECT_EXCEPT("* * * * -*/ *");
   CRON_EXPECT_EXCEPT("* * * * /-* *");
   CRON_EXPECT_EXCEPT("* * * * /*- *");
   CRON_EXPECT_EXCEPT("* * * * *2/ *");
   CRON_EXPECT_EXCEPT("* * * * /2* *");
   CRON_EXPECT_EXCEPT("* * * * -2/ *");
   CRON_EXPECT_EXCEPT("* * * * /2- *");
   CRON_EXPECT_EXCEPT("* * * * *2-/ *");
   CRON_EXPECT_EXCEPT("* * * * -2*/ *");
   CRON_EXPECT_EXCEPT("* * * * /2-* *");
   CRON_EXPECT_EXCEPT("* * * * /2*- *");
}

TEST_CASE("quartz: invalid days of week", "[quartz]")
{
   CRON_EXPECT_EXCEPT("* * * * * TEN");
   CRON_EXPECT_EXCEPT("* * * * * 8");
   CRON_EXPECT_EXCEPT("* * * * * -1");
   CRON_EXPECT_EXCEPT("* * * * * -1-7");
   CRON_EXPECT_EXCEPT("* * * * * 1-5-6");
   CRON_EXPECT_EXCEPT("* * * * * ,");
   CRON_EXPECT_EXCEPT("* * * * * 1,,2");
   CRON_EXPECT_EXCEPT("* * * * * 1,2,");
   CRON_EXPECT_EXCEPT("* * * * * ,1,2");
   CRON_EXPECT_EXCEPT("* * * * * 1/2/2");
   CRON_EXPECT_EXCEPT("* * * * * /");
   CRON_EXPECT_EXCEPT("* * * * * /2");
   CRON_EXPECT_EXCEPT("* * * * * 2/");
   CRON_EXPECT_EXCEPT("* * * * * */");
   CRON_EXPECT_EXCEPT("* * * * * /*");
   CRON_EXPECT_EXCEPT("* * * * * -/");
   CRON_EXPECT_EXCEPT("* * * * * /-");
   CRON_EXPECT_EXCEPT("* * * * * *-/");
   CRON_EXPECT_EXCEPT("* * * * * -*/");
   CRON_EXPECT_EXCEPT("* * * * * /-*");
   CRON_EXPECT_EXCEPT("* * * * * /*-");
   CRON_EXPECT_EXCEPT("* * * * * *2/");
   CRON_EXPECT_EXCEPT("* * * * * /2*");
   CRON_EXPECT_EXCEPT("* * * * * -2/");
   CRON_EXPECT_EXCEPT("* * * * * /2-");
   CRON_EXPECT_EXCEPT("* * * * * *2-/");
   CRON_EXPECT_EXCEPT("* * * * * -2*/");
   CRON_EXPECT_EXCEPT("* * * * * /2-*");
   CRON_EXPECT_EXCEPT("* * * * * /2*-");
}

TEST_CASE("quartz: invalid year", "[quartz]")
{
	CRON_EXPECT_EXCEPT("* * * * * ? 1969");
	CRON_EXPECT_EXCEPT("* * * * * ? 2100");
	CRON_EXPECT_EXCEPT("* * * * * ? 200O");
	CRON_EXPECT_EXCEPT("* * * * * ? /");
	CRON_EXPECT_EXCEPT("* * * * * ? /2");
	CRON_EXPECT_EXCEPT("* * * * * ? 2/");
	CRON_EXPECT_EXCEPT("* * * * * ? */");
	CRON_EXPECT_EXCEPT("* * * * * ? /*");
	CRON_EXPECT_EXCEPT("* * * * * ? -/");
	CRON_EXPECT_EXCEPT("* * * * * ? /-");
	CRON_EXPECT_EXCEPT("* * * * * ? *-/");
	CRON_EXPECT_EXCEPT("* * * * * ? -*/");
	CRON_EXPECT_EXCEPT("* * * * * ? /-*");
	CRON_EXPECT_EXCEPT("* * * * * ? /*-");
	CRON_EXPECT_EXCEPT("* * * * * ? *2/");
	CRON_EXPECT_EXCEPT("* * * * * ? /2*");
	CRON_EXPECT_EXCEPT("* * * * * ? -2/");
	CRON_EXPECT_EXCEPT("* * * * * ? /2-");
	CRON_EXPECT_EXCEPT("* * * * * ? *2-/");
	CRON_EXPECT_EXCEPT("* * * * * ? -2*/");
	CRON_EXPECT_EXCEPT("* * * * * ? /2-*");
	CRON_EXPECT_EXCEPT("* * * * * ? /2*-");
}

TEST_CASE("quartz: next","[quartz]")
{
    check_next_quartz("*/15 * 1-4 * * *",  "2012-07-01 09:53:50", "2012-07-02 01:00:00");
    check_next_quartz("*/15 * 1-4 * * *",  "2012-07-01 09:53:00", "2012-07-02 01:00:00");
    check_next_quartz("0 */2 1-4 * * *",   "2012-07-01 09:00:00", "2012-07-02 01:00:00");
    check_next_quartz("* * * * * *",       "2012-07-01 09:00:00", "2012-07-01 09:00:01");
   check_next_quartz("* * * * * *",       "2012-12-01 09:00:58", "2012-12-01 09:00:59");
   check_next_quartz("10 * * * * *",      "2012-12-01 09:42:09", "2012-12-01 09:42:10");
   check_next_quartz("11 * * * * *",      "2012-12-01 09:42:10", "2012-12-01 09:42:11");
   check_next_quartz("10 * * * * *",      "2012-12-01 09:42:10", "2012-12-01 09:43:10");
   check_next_quartz("10-15 * * * * *",   "2012-12-01 09:42:09", "2012-12-01 09:42:10");
   check_next_quartz("10-15 * * * * *",   "2012-12-01 21:42:14", "2012-12-01 21:42:15");
   check_next_quartz("0 * * * * *",       "2012-12-01 21:10:42", "2012-12-01 21:11:00");
   check_next_quartz("0 * * * * *",       "2012-12-01 21:11:00", "2012-12-01 21:12:00");
   check_next_quartz("0 11 * * * *",      "2012-12-01 21:10:42", "2012-12-01 21:11:00");
   check_next_quartz("0 10 * * * *",      "2012-12-01 21:11:00", "2012-12-01 22:10:00");
   check_next_quartz("0 0 * * * *",       "2012-09-30 11:01:00", "2012-09-30 12:00:00");
   check_next_quartz("0 0 * * * *",       "2012-09-30 12:00:00", "2012-09-30 13:00:00");
   check_next_quartz("0 0 * * * *",       "2012-09-10 23:01:00", "2012-09-11 00:00:00");
   check_next_quartz("0 0 * * * *",       "2012-09-11 00:00:00", "2012-09-11 01:00:00");
   check_next_quartz("0 0 0 * * *",       "2012-09-01 14:42:43", "2012-09-02 00:00:00");
   check_next_quartz("0 0 0 * * *",       "2012-09-02 00:00:00", "2012-09-03 00:00:00");
   check_next_quartz("* * * 10 * *",      "2012-10-09 15:12:42", "2012-10-10 00:00:00");
   check_next_quartz("* * * 10 * *",      "2012-10-11 15:12:42", "2012-11-10 00:00:00");
   check_next_quartz("0 0 0 * * *",       "2012-09-30 15:12:42", "2012-10-01 00:00:00");
   check_next_quartz("0 0 0 * * *",       "2012-10-01 00:00:00", "2012-10-02 00:00:00");
   check_next_quartz("0 0 0 * * *",       "2012-08-30 15:12:42", "2012-08-31 00:00:00");
   check_next_quartz("0 0 0 * * *",       "2012-08-31 00:00:00", "2012-09-01 00:00:00");
   check_next_quartz("0 0 0 * * *",       "2012-10-30 15:12:42", "2012-10-31 00:00:00");
   check_next_quartz("0 0 0 * * *",       "2012-10-31 00:00:00", "2012-11-01 00:00:00");
   check_next_quartz("0 0 0 1 * *",       "2012-10-30 15:12:42", "2012-11-01 00:00:00");
   check_next_quartz("0 0 0 1 * *",       "2012-11-01 00:00:00", "2012-12-01 00:00:00");
   check_next_quartz("0 0 0 1 * *",       "2010-12-31 15:12:42", "2011-01-01 00:00:00");
   check_next_quartz("0 0 0 1 * *",       "2011-01-01 00:00:00", "2011-02-01 00:00:00");
   check_next_quartz("0 0 0 31 * *",      "2011-10-30 15:12:42", "2011-10-31 00:00:00");
   check_next_quartz("0 0 0 1 * *",       "2011-10-30 15:12:42", "2011-11-01 00:00:00");
   check_next_quartz("* * * * * 3",       "2010-10-25 15:12:42", "2010-10-26 00:00:00");
   check_next_quartz("* * * * * 3",       "2010-10-20 15:12:42", "2010-10-26 00:00:00");
   check_next_quartz("* * * * * 3",       "2010-10-27 15:12:42", "2010-11-02 00:00:00");
   check_next_quartz("55 5 * * * *",      "2010-10-27 15:04:54", "2010-10-27 15:05:55");
   check_next_quartz("55 5 * * * *",      "2010-10-27 15:05:55", "2010-10-27 16:05:55");
   check_next_quartz("55 * 10 * * *",     "2010-10-27 09:04:54", "2010-10-27 10:00:55");
   check_next_quartz("55 * 10 * * *",     "2010-10-27 10:00:55", "2010-10-27 10:01:55");
   check_next_quartz("* 5 10 * * *",      "2010-10-27 09:04:55", "2010-10-27 10:05:00");
   check_next_quartz("* 5 10 * * *",      "2010-10-27 10:05:00", "2010-10-27 10:05:01");
   check_next_quartz("55 * * 3 * *",      "2010-10-02 10:05:54", "2010-10-03 00:00:55");
   check_next_quartz("55 * * 3 * *",      "2010-10-03 00:00:55", "2010-10-03 00:01:55");
   check_next_quartz("* * * 3 11 *",      "2010-10-02 14:42:55", "2010-11-03 00:00:00");
   check_next_quartz("* * * 3 11 *",      "2010-11-03 00:00:00", "2010-11-03 00:00:01");
   check_next_quartz("0 0 0 29 2 *",      "2007-02-10 14:42:55", "2008-02-29 00:00:00");
   check_next_quartz("0 0 0 29 2 *",      "2008-02-29 00:00:00", "2012-02-29 00:00:00");
   check_next_quartz("0 0 7 ? * MON-FRI", "2009-09-26 00:42:55", "2009-09-28 07:00:00");
   check_next_quartz("0 0 7 ? * MON-FRI", "2009-09-28 07:00:00", "2009-09-29 07:00:00");
   check_next_quartz("0 30 23 30 1/3 ?",  "2010-12-30 00:00:00", "2011-01-30 23:30:00");
   check_next_quartz("0 30 23 30 1/3 ?",  "2011-01-30 23:30:00", "2011-04-30 23:30:00");
   check_next_quartz("0 30 23 30 1/3 ?",  "2011-04-30 23:30:00", "2011-07-30 23:30:00");    
   check_next_quartz("0 0 0 1 1 ? 2015",  "2011-04-30 23:30:00", "2015-01-01 00:00:00");
   check_next_quartz("0 0 0 1/20 * ? 2010/3",  "2010-12-25 23:30:00", "2013-01-01 00:00:00");
   check_next_quartz("0 0 0 13 * FRI 2021", "2020-08-21 10:00:00", "2021-08-13 00:00:00");
   check_next_quartz("0 0 11 13 * FRI", "2020-08-13 10:00:00", "2020-11-13 11:00:00");
}

