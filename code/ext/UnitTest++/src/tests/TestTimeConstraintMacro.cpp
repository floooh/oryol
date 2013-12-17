#include "../UnitTest++.h"
#include "../TimeHelpers.h"

#include "RecordingReporter.h"
#include "ScopedCurrentTest.h"

namespace {

TEST (TimeConstraintMacroQualifiesNamespace)
{
    // If this compiles without a "using namespace UnitTest;", all is well.
    UNITTEST_TIME_CONSTRAINT(1);
}

TEST (TimeConstraintMacroUsesCorrectInfo)
{
    int testLine = 0;
    RecordingReporter reporter;
    {
        UnitTest::TestResults testResults(&reporter);
		ScopedCurrentTest scopedResults(testResults);

        UNITTEST_TIME_CONSTRAINT(10); testLine = __LINE__;
        UnitTest::TimeHelpers::SleepMs(20);
    }

	using namespace std;

    CHECK_EQUAL(1, reporter.testFailedCount);
    CHECK(strstr(reporter.lastFailedFile, __FILE__));
    CHECK_EQUAL(testLine, reporter.lastFailedLine);
    CHECK(strstr(reporter.lastFailedTest, "TimeConstraintMacroUsesCorrectInfo"));
}

}
