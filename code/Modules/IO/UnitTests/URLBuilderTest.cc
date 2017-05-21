//------------------------------------------------------------------------------
//  URLBuilderTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "IO/IOTypes.h"

using namespace Oryol;

TEST(URLBuilderTest) {
    
    URLBuilder urlBuilder;
    CHECK(urlBuilder.Scheme.Empty());
    CHECK(urlBuilder.User.Empty());
    CHECK(urlBuilder.Password.Empty());
    CHECK(urlBuilder.Host.Empty());
    CHECK(urlBuilder.Port.Empty());
    CHECK(urlBuilder.Path.Empty());
    CHECK(urlBuilder.Fragment.Empty());
    CHECK(urlBuilder.Query.Empty());
    
    const URL srcUrl("http://www.flohofwoe.net/demos.html");
    urlBuilder.ParseURL(srcUrl);
    CHECK(urlBuilder.Scheme == "http");
    CHECK(urlBuilder.User.Empty());
    CHECK(urlBuilder.Password.Empty());
    CHECK(urlBuilder.Host == "www.flohofwoe.net");
    CHECK(urlBuilder.Port.Empty());
    CHECK(urlBuilder.Path == "demos.html");
    CHECK(urlBuilder.Fragment.Empty());
    CHECK(urlBuilder.Query.Empty());
    URL url(urlBuilder.BuildURL());
    CHECK(url == srcUrl);
    CHECK(url.Get() == srcUrl.Get());
    CHECK(url.Get() == "http://www.flohofwoe.net/demos.html");
    
    // add a user-name
    urlBuilder.User = "floh";
    url = urlBuilder.BuildURL();
    CHECK(url.Get() == "http://floh@www.flohofwoe.net/demos.html");
    
    // add a password
    urlBuilder.Password = "pwd";
    url = urlBuilder.BuildURL();
    CHECK(url.Get() == "http://floh:pwd@www.flohofwoe.net/demos.html");
    
    // add a port-number
    urlBuilder.Port = "8080";
    url = urlBuilder.BuildURL();
    CHECK(url.Get() == "http://floh:pwd@www.flohofwoe.net:8080/demos.html");
    
    // add a fragment
    urlBuilder.Fragment = "bla";
    url = urlBuilder.BuildURL();
    CHECK(url.Get() == "http://floh:pwd@www.flohofwoe.net:8080/demos.html#bla");
    
    // add a query
    urlBuilder.Query.Add("key0", "value0");
    urlBuilder.Query.Add("key1", "value1");
    url = urlBuilder.BuildURL();
    CHECK(url.Get() == "http://floh:pwd@www.flohofwoe.net:8080/demos.html?key0=value0&key1=value1#bla");
    
    // construct from URL
    URLBuilder urlBuilder1(url);
    CHECK(urlBuilder1.Scheme == "http");
    CHECK(urlBuilder1.User == "floh");
    CHECK(urlBuilder1.Password == "pwd");
    CHECK(urlBuilder1.Host == "www.flohofwoe.net");
    CHECK(urlBuilder1.Port == "8080");
    CHECK(urlBuilder1.Path == "demos.html");
    CHECK(urlBuilder1.Fragment == "bla");
    CHECK(urlBuilder.Query.Size() == 2);
    CHECK(urlBuilder.Query["key0"] == "value0");
    CHECK(urlBuilder.Query["key1"] == "value1");
    URL url1(urlBuilder1.BuildURL());
    CHECK(url1 == url);
}
