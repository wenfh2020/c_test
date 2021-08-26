/*
 * brew install cryptopp / yum install cryptopp
 * g++ -std='c++11' test_cryptopp.cpp -lcryptopp -o encry && ./encry
 */

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

#include <cryptopp/aes.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/filters.h>
#include <cryptopp/gzip.h>
#include <cryptopp/hex.h>
#include <cryptopp/md5.h>
#include <cryptopp/modes.h>
#include <cryptopp/secblock.h>

#include <fstream>
#include <iostream>
#include <sstream>

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

std::string md5(const std::string& text) {
    /* https://www.cryptopp.com/wiki/MD5 */
    /* https://blog.csdn.net/ecrisraul/article/details/88042012 */
    std::string digest;
    CryptoPP::Weak1::MD5 md5;
    CryptoPP::HashFilter hashfilter(md5);
    hashfilter.Attach(new CryptoPP::HexEncoder(new CryptoPP::StringSink(digest), false));
    hashfilter.Put(reinterpret_cast<const unsigned char*>(text.c_str()), text.length());
    hashfilter.MessageEnd();
    return digest;
}

void test_gzip() {
    std::cout << "test gzip:" << std::endl;
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
}

void test_md5() {
    std::cout << "test md5:" << std::endl;
    std::cout << md5("hello world!").c_str() << std::endl;
}

bool read_data_from_file(const std::string path, std::string& data) {
    std::ifstream is(path);
    if (!is.good()) {
        return false;
    }

    std::stringstream content;
    content << is.rdbuf();
    data = content.str();
    std::cout << "file path: " << path << std::endl
              << "file data cout: " << data.length() << std::endl
              << "--------" << std::endl;
    //   << "file data: " << data << std::endl;
    is.close();
    return true;
}

/* 1. hex to string. key and iv.
 * 2. read data from file. 
 * 3. test enrypt and decrypt. */

std::string char2hex(char c) {
    std::string sValue;
    static char MAPX[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    sValue += MAPX[(c >> 4) & 0x0F];
    sValue += MAPX[c & 0x0F];
    return sValue;
}

std::string encode_string_to_hex(const std::string& src) {
    std::string result;

    std::string::const_iterator pos;
    for (pos = src.begin(); pos != src.end(); pos++) {
        result.append(char2hex(*pos));
    }
    return result;
}

char hex2char(const std::string& hex) {
    unsigned char c = 0;
    for (unsigned int i = 0; i < std::min<unsigned int>(hex.size(), (unsigned int)2); ++i) {
        unsigned char c1 = std::toupper(hex[i]);
        unsigned char c2 = (c1 >= 'A') ? (c1 - ('A' - 10)) : (c1 - '0');
        (c <<= 4) += c2;
    }
    return c;
}

std::string encode_hex_to_string(const std::string& sSrc) {
    std::string result;
    std::string::const_iterator pos;
    for (pos = sSrc.begin(); pos != sSrc.end();) {
        result.append(1, hex2char(std::string(pos, pos + 2)));
        pos += 2;
    }

    return result;
}

std::string aes_crt_encrypt(const std::string& k1, const std::string& iv1, const std::string& message) {
    using namespace CryptoPP;

    std::string encrypted;

    CryptoPP::SecByteBlock key(16), iv(16);
    std::memcpy(key, k1.c_str(), k1.size());
    std::memcpy(iv, iv1.c_str(), iv1.size());

    std::cout << "k1: " << k1 << " len: " << iv1.length() << std::endl
              << "iv1: " << iv1 << " len: " << iv1.length() << std::endl;

    AES::Encryption aesEncryption(key, key.size());
    CTR_Mode_ExternalCipher::Encryption ctrEncryption(aesEncryption, iv);

    StreamTransformationFilter stfEncryptor(
        ctrEncryption, new StringSink(encrypted));

    stfEncryptor.Put((const byte*)&message[0], message.size());
    stfEncryptor.MessageEnd();
    return encrypted;
}

std::string aes_crt_decrypt(const std::string& k1, const std::string& iv1, std::string encrypted) {
    using namespace CryptoPP;

    std::string decrypted;

    CryptoPP::SecByteBlock key(16), iv(16);
    std::memcpy(key, k1.c_str(), k1.size());
    std::memcpy(iv, iv1.c_str(), iv1.size());

    std::cout << "encrypted data cnt: " << encrypted.length() << std::endl
              << "-----" << std::endl;

    AES::Encryption aesDecryption(key, key.size());
    CTR_Mode_ExternalCipher::Decryption ctrDecryption(aesDecryption, iv);

    StreamTransformationFilter stfDecryptor(
        ctrDecryption, new StringSink(decrypted));

    std::cout << "encrypted size: " << encrypted.size() << std::endl;

    stfDecryptor.Put((const byte*)&encrypted[0], encrypted.size());
    stfDecryptor.MessageEnd();

    std::cout << "recovered data len: " << decrypted.length() << std::endl;
    return decrypted;
}

void test_aes_crt_decrypt() {
    std::string path, encrypted, decrypted, key_hex, iv_hex, iv, key;

    key_hex = "1f9090460b972e5fd343cb16d0505a7b";
    iv_hex = "7f3d6d25eabf97c953f7705470cb31b8";

    key = encode_hex_to_string(key_hex);
    iv = encode_hex_to_string(iv_hex);

    std::cout << "key hex: " << key_hex << " len: " << key_hex.length() << std::endl
              << "key str: " << key << " len: " << key.length() << std::endl
              << "iv hex:  " << iv_hex << " len: " << iv_hex.length() << std::endl
              << "iv str:  " << iv << " len: " << iv.length() << std::endl;

    std::cout << "encode hex to string: " << std::endl
              << "key hex: " << encode_string_to_hex(key) << std::endl
              << "iv hex:  " << encode_string_to_hex(iv) << std::endl;

    /* read data from local file. */
    path = "/Users/wenfh2020/Downloads/test_aes/encrypt.data";
    if (read_data_from_file(path, encrypted)) {
        char md5str[32] = {0};
        std::cout << "read data from file done!" << std::endl
                  << "encrypt data len: " << encrypted.length() << std::endl;
        decrypted = aes_crt_decrypt(key, iv, encrypted);
        std::cout << "decrypt data's md5: " << md5(decrypted) << std::endl;
        char num[4] = {0};
        num[0] = decrypted[0];
        num[1] = decrypted[1];
        num[2] = decrypted[2];
        num[3] = decrypted[3];
        int result = *(int*)num;
        std::cout << "result head 4: " << result << std::endl;
    } else {
        std::cout << "bad!!!!" << std::endl;
    }
}

void test_aes_crt() {
    std::string encrypted, decrypted, message;
    std::string key = "1234567890123456";
    std::string iv = "1234567890123456";
    message =
        "Now is the time for all good men "
        "to come to the aide of their country.";
    encrypted = aes_crt_encrypt(key, iv, message);
    decrypted = aes_crt_decrypt(key, iv, encrypted);
    std::cout << "decrypt data: " << decrypted << std::endl;
}

int main(int argc, char** argv) {
    // test_gzip();
    // test_md5();
    // test_aes_crt();
    // test_aes_crt_decrypt();
    return 0;
}
