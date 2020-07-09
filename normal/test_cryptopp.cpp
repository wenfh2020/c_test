/*
 * brew install cryptopp / yum install cryptopp
 * g++ -std='c++11' test_encryption.cpp -lcryptopp -o test_encryption && ./test_encryption
 */

#include <cryptopp/gzip.h>

#include <iostream>

#if defined(CRYPTOPP_NO_GLOBAL_BYTE)
using CryptoPP::byte;
#endif

bool gzip(const std::string& src, std::string& dst) {
    try {
        CryptoPP::Gzip zip;
        zip.Put((byte*)src.c_str(), src.size());
        zip.MessageEnd();

        CryptoPP::word64 avail = zip.MaxRetrievable();
        if (avail) {
            dst.resize(avail);
            zip.Get((byte*)&dst[0], dst.size());
        }
    } catch (CryptoPP::InvalidDataFormat& e) {
        std::cout << e.GetWhat() << std::endl;
        return false;
    }
    return true;
}

bool ungzip(const std::string& src, std::string& dst) {
    try {
        CryptoPP::Gunzip zip;
        zip.Put((byte*)src.c_str(), src.size());
        zip.MessageEnd();
        CryptoPP::word64 avail = zip.MaxRetrievable();
        if (avail) {
            dst.resize(avail);
            zip.Get((byte*)&dst[0], dst.size());
        }
    } catch (CryptoPP::InvalidDataFormat& e) {
        std::cout << e.GetWhat() << std::endl;
        return false;
    }
    return (true);
}

int main(int argc, char** argv) {
    // gzip
    std::string src("hello world!"), dst;
    if (gzip(src, dst)) {
        std::cout << "gzip: " << dst << std::endl;
    } else {
        std::cout << "gzip failed!" << std::endl;
    }

    // ungizp
    if (ungzip(dst, src)) {
        std::cout << "ungzip: " << src << std::endl;
    } else {
        std::cout << "ungzip failed!" << std::endl;
    }

    return 0;
}
