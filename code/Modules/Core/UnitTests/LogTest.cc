//------------------------------------------------------------------------------
//  LogTest.cc
//  Test Log class.
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Log.h"
#include "Core/Logger.h"

using namespace Oryol;

class MyLogger : public Logger {
    OryolClassDecl(MyLogger);

    /// generic vprint-style method
    virtual void VPrint(Log::Level l, const char* msg, va_list args) override {
        printf("In MyLogger::VPrint(): ");
        vprintf(msg, args);
    };
};

void test_vinfo(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    Log::VInfo(msg, args);
    va_end(args);
}

void test_log() {
    Log::Dbg("Dbg log msg %d '%s'\n", 2, "Bla");
    Log::Info("Info log msg %f %d\n", 0.1, 4);
    Log::Warn("Warning log msg %d %d\n", 2, 3);
    Log::Error("Error log msg %d\n", 10);
}

TEST(LogTest) {

    test_log();
    Log::SetLogLevel(Log::Level::Dbg);
    test_log();
    Log::SetLogLevel(Log::Level::Info);
    test_log();
    Log::SetLogLevel(Log::Level::Warn);
    test_log();
    Log::SetLogLevel(Log::Level::Error);
    test_log();
    Log::SetLogLevel(Log::Level::None);
    test_log();
    Log::SetLogLevel(Log::Level::Dbg);

    test_vinfo("Log::VInfo %d %d %d...\n", 1, 2, 3);

    Log::AddLogger(MyLogger::Create());
    test_log();
}


