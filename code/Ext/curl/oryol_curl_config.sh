#!/bin/bash
# configure libcurl as statically linked, minimalistics HTTP-only lib
#
# 1) get libcurl from https://github.com/bagder/curl
# 2) copy this file to the curl directory
# 3) in the curl directory:
#    - run ./buildconf, then oryol_curl_config.sh (instead of ./configure), then make
#    - the compiled static lib is curl/lib/.libs/libcurl.a, copy this to oryol/libs/linux
#
./configure --disable-shared --enable-http --disable-ftp --disable-file --disable-ldap --disable-rtsp --disable-dict --disable-telnet --disable-tftp --disable-pop3 --disable-imap --disable-smtp --disable-gopher --disable-manual --disable-sspi --disable-crypto-auth --disable-ntlm-wb --disable-tls-srp --disable-cookies --without-ssl --without-gnutls --without-polarssl --without-cyassl --without-nss --without-axtls --without-ca-path --without-libmetalink --without-libssh2 --without-librtmp --without-winidn --without-libidn --without-nghttp2
