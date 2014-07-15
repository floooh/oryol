//------------------------------------------------------------------------------
//  URLBuilderTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "IO/Core/URLBuilder.h"

using namespace Oryol::Core;
using namespace Oryol::IO;

TEST(URLBuilderTest) {
    
    URLBuilder urlBuilder;
    CHECK(urlBuilder.GetScheme().Empty());
    CHECK(urlBuilder.GetUser().Empty());
    CHECK(urlBuilder.GetPassword().Empty());
    CHECK(urlBuilder.GetHost().Empty());
    CHECK(urlBuilder.GetPort().Empty());
    CHECK(urlBuilder.GetPath().Empty());
    CHECK(urlBuilder.GetFragment().Empty());
    CHECK(urlBuilder.GetQuery().Empty());
    
    const URL srcUrl("http://www.flohofwoe.net/demos.html");
    urlBuilder.SetURL(srcUrl);
    CHECK(urlBuilder.GetScheme() == "http");
    CHECK(urlBuilder.GetUser().Empty());
    CHECK(urlBuilder.GetPassword().Empty());
    CHECK(urlBuilder.GetHost() == "www.flohofwoe.net");
    CHECK(urlBuilder.GetPort().Empty());
    CHECK(urlBuilder.GetPath() == "demos.html");
    CHECK(urlBuilder.GetFragment().Empty());
    CHECK(urlBuilder.GetQuery().Empty());
    URL url(urlBuilder.BuildURL());
    CHECK(url == srcUrl);
    CHECK(url.Get() == srcUrl.Get());
    CHECK(url.Get() == "http://www.flohofwoe.net/demos.html");
    
    // add a user-name
    urlBuilder.SetUser("floh");
    url = urlBuilder.BuildURL();
    CHECK(url.Get() == "http://floh@www.flohofwoe.net/demos.html");
    
    // add a password
    urlBuilder.SetPassword("pwd");
    url = urlBuilder.BuildURL();
    CHECK(url.Get() == "http://floh:pwd@www.flohofwoe.net/demos.html");
    
    // add a port-number
    urlBuilder.SetPort("8080");
    url = urlBuilder.BuildURL();
    CHECK(url.Get() == "http://floh:pwd@www.flohofwoe.net:8080/demos.html");
    
    // add a fragment
    urlBuilder.SetFragment("bla");
    url = urlBuilder.BuildURL();
    CHECK(url.Get() == "http://floh:pwd@www.flohofwoe.net:8080/demos.html#bla");
    
    // add a query
    Map<String, String> query;
    query.Insert("key0", "value0");
    query.Insert("key1", "value1");
    urlBuilder.SetQuery(query);
    url = urlBuilder.BuildURL();
    CHECK(url.Get() == "http://floh:pwd@www.flohofwoe.net:8080/demos.html?key0=value0&key1=value1#bla");
    
    // construct from URL
    URLBuilder urlBuilder1(url);
    CHECK(urlBuilder1.GetScheme() == "http");
    CHECK(urlBuilder1.GetUser() == "floh");
    CHECK(urlBuilder1.GetPassword() == "pwd");
    CHECK(urlBuilder1.GetHost() == "www.flohofwoe.net");
    CHECK(urlBuilder1.GetPort() == "8080");
    CHECK(urlBuilder1.GetPath() == "demos.html");
    CHECK(urlBuilder1.GetFragment() == "bla");
    const Map<String, String>& query1 = urlBuilder.GetQuery();
    CHECK(query1.Size() == 2);
    CHECK(query1["key0"] == "value0");
    CHECK(query1["key1"] == "value1");
    URL url1(urlBuilder1.BuildURL());
    CHECK(url1 == url);
}
