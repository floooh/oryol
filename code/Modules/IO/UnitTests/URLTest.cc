//------------------------------------------------------------------------------
//  URLTest.cc
//  Test URLs
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/unittest++.h"
#include "IO/URL.h"

using namespace Oryol;
using namespace IO;

TEST(URLTest) {
    
    // a simple URL
    URL url0("http://www.flohofwoe.net/index.html");
    URL url1("http://www.flohofwoe.org/index.html");
    CHECK(url0.Get() == "http://www.flohofwoe.net/index.html");
    CHECK(url0 != url1);
    
    // split into parts
    URLParts parts = url0.Split();
    CHECK(parts.Valid);
    CHECK(parts.Scheme == "http");
    CHECK(parts.User.empty());
    CHECK(parts.Password.empty());
    CHECK(parts.Host == "www.flohofwoe.net");
    CHECK(parts.Port.empty());
    CHECK(parts.Path == "index.html");
    CHECK(parts.Query.empty());
    CHECK(parts.Fragment.empty());
    
    // test some invalid URL
    URL brokenUrl;
    brokenUrl = "www.bla.org";
    parts = brokenUrl.Split();
    CHECK(!parts.Valid);
    
    // test with username, no password, port number, deep path and fragment
    URL url2("http://user@www.flohofwoe.net:8000/bla/blub/blob.txt#frag");
    parts = url2.Split();
    CHECK(parts.Valid);
    CHECK(parts.Scheme == "http");
    CHECK(parts.User == "user");
    CHECK(parts.Password.empty());
    CHECK(parts.Host == "www.flohofwoe.net");
    CHECK(parts.Port == "8000");
    CHECK(parts.Path == "bla/blub/blob.txt");
    CHECK(parts.Query.empty());
    CHECK(parts.Fragment == "frag");
    
    // a complex URL with user+password and a query
    URL url3("http://user:pwd@www.flohofwoe.net/bla.txt?key0=val0&key1=val1#frag");
    parts = url3.Split();
    CHECK(parts.Valid);
    CHECK(parts.Scheme == "http");
    CHECK(parts.User == "user");
    CHECK(parts.Password == "pwd");
    CHECK(parts.Host == "www.flohofwoe.net");
    CHECK(parts.Path == "bla.txt");
    CHECK(parts.Query.size() == 2);
    CHECK(parts.Query["key0"] == "val0");
    CHECK(parts.Query["key1"] == "val1");
    CHECK(parts.Fragment == "frag");
}
