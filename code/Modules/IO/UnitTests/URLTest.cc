//------------------------------------------------------------------------------
//  URLTest.cc
//  Test URLs
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "IO/URL.h"

using namespace Oryol;
using namespace Core;
using namespace IO;

TEST(URLTest) {
    
    // empty URL
    URL url;
    CHECK(url.Empty());
    CHECK(!url.IsValid());
    CHECK(url.Get() == "");
    
    // a simple URL
    URL url0("http://www.flohofwoe.net/index.html");
    URL url1("http://www.flohofwoe.org/index.html");
    CHECK(url0.Get() == "http://www.flohofwoe.net/index.html");
    CHECK(url0 != url1);

    // check parts
    CHECK(url0.IsValid());
    CHECK(!url0.Empty());
    CHECK(url0.Scheme() == "http");
    CHECK(url0.User().Empty());
    CHECK(url0.Password().Empty());
    CHECK(url0.Host() == "www.flohofwoe.net");
    CHECK(url0.Port().Empty());
    CHECK(url0.Path() == "index.html");
    CHECK(url0.Query().Empty());
    CHECK(url0.Fragment().Empty());
    
    // test some invalid URL
    URL brokenUrl;
    brokenUrl = "www.bla.org";
    CHECK(!brokenUrl.IsValid());
    
    // test with username, no password, port number, deep path and fragment
    URL url2("http://user@www.flohofwoe.net:8000/bla/blub/blob.txt#frag");
    CHECK(url2.IsValid());
    CHECK(url2.Scheme() == "http");
    CHECK(url2.User() == "user");
    CHECK(url2.Password().Empty());
    CHECK(url2.Host() == "www.flohofwoe.net");
    CHECK(url2.Port() == "8000");
    CHECK(url2.Path() == "bla/blub/blob.txt");
    CHECK(url2.Query().Empty());
    CHECK(url2.Fragment() == "frag");
    
    // a complex URL with user+password and a query
    URL url3("http://user:pwd@www.flohofwoe.net/bla.txt?key0=val0&key1=val1#frag");
    CHECK(url3.IsValid());
    CHECK(url3.Scheme() == "http");
    CHECK(url3.User() == "user");
    CHECK(url3.Password() == "pwd");
    CHECK(url3.Host() == "www.flohofwoe.net");
    CHECK(url3.Path() == "bla.txt");
    Map<String, String> query = url3.Query();
    CHECK(query.Size() == 2);
    CHECK(query["key0"] == "val0");
    CHECK(query["key1"] == "val1");
    CHECK(url3.Fragment() == "frag");
    
}
