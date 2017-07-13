#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-rare");
    BOOST_CHECK(GetBoolArg("-rare"));
    BOOST_CHECK(GetBoolArg("-rare", false));
    BOOST_CHECK(GetBoolArg("-rare", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-rareo"));
    BOOST_CHECK(!GetBoolArg("-rareo", false));
    BOOST_CHECK(GetBoolArg("-rareo", true));

    ResetArgs("-rare=0");
    BOOST_CHECK(!GetBoolArg("-rare"));
    BOOST_CHECK(!GetBoolArg("-rare", false));
    BOOST_CHECK(!GetBoolArg("-rare", true));

    ResetArgs("-rare=1");
    BOOST_CHECK(GetBoolArg("-rare"));
    BOOST_CHECK(GetBoolArg("-rare", false));
    BOOST_CHECK(GetBoolArg("-rare", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-norare");
    BOOST_CHECK(!GetBoolArg("-rare"));
    BOOST_CHECK(!GetBoolArg("-rare", false));
    BOOST_CHECK(!GetBoolArg("-rare", true));

    ResetArgs("-norare=1");
    BOOST_CHECK(!GetBoolArg("-rare"));
    BOOST_CHECK(!GetBoolArg("-rare", false));
    BOOST_CHECK(!GetBoolArg("-rare", true));

    ResetArgs("-rare -norare");  // -rare should win
    BOOST_CHECK(GetBoolArg("-rare"));
    BOOST_CHECK(GetBoolArg("-rare", false));
    BOOST_CHECK(GetBoolArg("-rare", true));

    ResetArgs("-rare=1 -norare=1");  // -rare should win
    BOOST_CHECK(GetBoolArg("-rare"));
    BOOST_CHECK(GetBoolArg("-rare", false));
    BOOST_CHECK(GetBoolArg("-rare", true));

    ResetArgs("-rare=0 -norare=0");  // -rare should win
    BOOST_CHECK(!GetBoolArg("-rare"));
    BOOST_CHECK(!GetBoolArg("-rare", false));
    BOOST_CHECK(!GetBoolArg("-rare", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--rare=1");
    BOOST_CHECK(GetBoolArg("-rare"));
    BOOST_CHECK(GetBoolArg("-rare", false));
    BOOST_CHECK(GetBoolArg("-rare", true));

    ResetArgs("--norare=1");
    BOOST_CHECK(!GetBoolArg("-rare"));
    BOOST_CHECK(!GetBoolArg("-rare", false));
    BOOST_CHECK(!GetBoolArg("-rare", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-rare", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-rare", "eleven"), "eleven");

    ResetArgs("-rare -bar");
    BOOST_CHECK_EQUAL(GetArg("-rare", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-rare", "eleven"), "");

    ResetArgs("-rare=");
    BOOST_CHECK_EQUAL(GetArg("-rare", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-rare", "eleven"), "");

    ResetArgs("-rare=11");
    BOOST_CHECK_EQUAL(GetArg("-rare", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-rare", "eleven"), "11");

    ResetArgs("-rare=eleven");
    BOOST_CHECK_EQUAL(GetArg("-rare", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-rare", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-rare", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-rare", 0), 0);

    ResetArgs("-rare -bar");
    BOOST_CHECK_EQUAL(GetArg("-rare", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-rare=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-rare", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-rare=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-rare", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--rare");
    BOOST_CHECK_EQUAL(GetBoolArg("-rare"), true);

    ResetArgs("--rare=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-rare", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-norare");
    BOOST_CHECK(!GetBoolArg("-rare"));
    BOOST_CHECK(!GetBoolArg("-rare", true));
    BOOST_CHECK(!GetBoolArg("-rare", false));

    ResetArgs("-norare=1");
    BOOST_CHECK(!GetBoolArg("-rare"));
    BOOST_CHECK(!GetBoolArg("-rare", true));
    BOOST_CHECK(!GetBoolArg("-rare", false));

    ResetArgs("-norare=0");
    BOOST_CHECK(GetBoolArg("-rare"));
    BOOST_CHECK(GetBoolArg("-rare", true));
    BOOST_CHECK(GetBoolArg("-rare", false));

    ResetArgs("-rare --norare");
    BOOST_CHECK(GetBoolArg("-rare"));

    ResetArgs("-norare -rare"); // rare always wins:
    BOOST_CHECK(GetBoolArg("-rare"));
}

BOOST_AUTO_TEST_SUITE_END()
