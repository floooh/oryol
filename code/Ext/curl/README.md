### CURL headers 

libcurl is used in Oryol as HTTP client on 64-bit Linux platforms. To prevent any compatibility problems with
different Linux configs it is linked statically into the executable, and stripped from all protocols 
except HTTP (no HTTPS support either).

libcurl.a has been created like this:

1. git clone https://github.com/bagder/curl.git
2. cd curl
3. ./buildconfig (+ install any missing packages it complains about)
4. copy the oryol_curl_config.sh script to the curl directory
5. run oryol_curl_config.sh instead of ./configure
6. make
7. the resulting lib is curl/lib/.libs/libcurl.a, copy this to oryol/libs/linux

