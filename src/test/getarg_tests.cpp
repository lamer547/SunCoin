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
    ResetArgs("-snc");
    BOOST_CHECK(GetBoolArg("-snc"));
    BOOST_CHECK(GetBoolArg("-snc", false));
    BOOST_CHECK(GetBoolArg("-snc", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-fooo"));
    BOOST_CHECK(!GetBoolArg("-fooo", false));
    BOOST_CHECK(GetBoolArg("-fooo", true));

    ResetArgs("-snc=0");
    BOOST_CHECK(!GetBoolArg("-snc"));
    BOOST_CHECK(!GetBoolArg("-snc", false));
    BOOST_CHECK(!GetBoolArg("-snc", true));

    ResetArgs("-snc=1");
    BOOST_CHECK(GetBoolArg("-snc"));
    BOOST_CHECK(GetBoolArg("-snc", false));
    BOOST_CHECK(GetBoolArg("-snc", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-nofoo");
    BOOST_CHECK(!GetBoolArg("-snc"));
    BOOST_CHECK(!GetBoolArg("-snc", false));
    BOOST_CHECK(!GetBoolArg("-snc", true));

    ResetArgs("-nofoo=1");
    BOOST_CHECK(!GetBoolArg("-snc"));
    BOOST_CHECK(!GetBoolArg("-snc", false));
    BOOST_CHECK(!GetBoolArg("-snc", true));

    ResetArgs("-snc -nofoo");  // -snc should win
    BOOST_CHECK(GetBoolArg("-snc"));
    BOOST_CHECK(GetBoolArg("-snc", false));
    BOOST_CHECK(GetBoolArg("-snc", true));

    ResetArgs("-snc=1 -nofoo=1");  // -snc should win
    BOOST_CHECK(GetBoolArg("-snc"));
    BOOST_CHECK(GetBoolArg("-snc", false));
    BOOST_CHECK(GetBoolArg("-snc", true));

    ResetArgs("-snc=0 -nofoo=0");  // -snc should win
    BOOST_CHECK(!GetBoolArg("-snc"));
    BOOST_CHECK(!GetBoolArg("-snc", false));
    BOOST_CHECK(!GetBoolArg("-snc", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--snc=1");
    BOOST_CHECK(GetBoolArg("-snc"));
    BOOST_CHECK(GetBoolArg("-snc", false));
    BOOST_CHECK(GetBoolArg("-snc", true));

    ResetArgs("--nofoo=1");
    BOOST_CHECK(!GetBoolArg("-snc"));
    BOOST_CHECK(!GetBoolArg("-snc", false));
    BOOST_CHECK(!GetBoolArg("-snc", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-snc", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-snc", "eleven"), "eleven");

    ResetArgs("-snc -bar");
    BOOST_CHECK_EQUAL(GetArg("-snc", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-snc", "eleven"), "");

    ResetArgs("-snc=");
    BOOST_CHECK_EQUAL(GetArg("-snc", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-snc", "eleven"), "");

    ResetArgs("-snc=11");
    BOOST_CHECK_EQUAL(GetArg("-snc", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-snc", "eleven"), "11");

    ResetArgs("-snc=eleven");
    BOOST_CHECK_EQUAL(GetArg("-snc", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-snc", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-snc", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-snc", 0), 0);

    ResetArgs("-snc -bar");
    BOOST_CHECK_EQUAL(GetArg("-snc", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-snc=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-snc", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-snc=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-snc", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--snc");
    BOOST_CHECK_EQUAL(GetBoolArg("-snc"), true);

    ResetArgs("--snc=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-snc", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-nofoo");
    BOOST_CHECK(!GetBoolArg("-snc"));
    BOOST_CHECK(!GetBoolArg("-snc", true));
    BOOST_CHECK(!GetBoolArg("-snc", false));

    ResetArgs("-nofoo=1");
    BOOST_CHECK(!GetBoolArg("-snc"));
    BOOST_CHECK(!GetBoolArg("-snc", true));
    BOOST_CHECK(!GetBoolArg("-snc", false));

    ResetArgs("-nofoo=0");
    BOOST_CHECK(GetBoolArg("-snc"));
    BOOST_CHECK(GetBoolArg("-snc", true));
    BOOST_CHECK(GetBoolArg("-snc", false));

    ResetArgs("-snc --nofoo");
    BOOST_CHECK(GetBoolArg("-snc"));

    ResetArgs("-nofoo -snc"); // snc always wins:
    BOOST_CHECK(GetBoolArg("-snc"));
}

BOOST_AUTO_TEST_SUITE_END()
