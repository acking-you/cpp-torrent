//#include <catch2/catch.hpp>
#include "bencode.h"
#include "../../../BenchMark/Timer.h"
#include <fstream>
#include <iostream>
#ifdef TEST
TEST_CASE("test-string","[string-test]"){
    std::stringstream ss;
    SECTION("string_test1"){
        std::string v = "abcdefghij";
        int wlen = bencode::BObject::EncodeString(ss,v);
        REQUIRE(v.size()+3==wlen);
        auto str = bencode::BObject::DecodeString(ss, nullptr);
        REQUIRE(str==v);
    }

    SECTION("string_test2"){
        std::string val;
        for(int i = 0; i < 20; i++) {
            val += char ('a' + i);
        }
        ss.clear();
        int wlen = bencode::BObject::EncodeString(ss, val);
        REQUIRE(wlen==23);
        auto str = bencode::BObject::DecodeString(ss, nullptr);
        REQUIRE( val== str);
    }

}

TEST_CASE("test-int","[test-int]"){
    std::stringstream ss;
    SECTION("int_test1"){
        int val = 999;
        int wlen = bencode::BObject::EncodeInt(ss,val);
        REQUIRE(5==wlen);
        auto num = bencode::BObject::DecodeInt(ss, nullptr);
        REQUIRE(num==val);
    }

    SECTION("int_test2"){
        int val = 0;
        ss.clear();
        int wlen = bencode::BObject::EncodeInt(ss, val);
        REQUIRE(wlen==3);
        auto num = bencode::BObject::DecodeInt(ss, nullptr);
        REQUIRE( val== num);
    }

    SECTION("int_test3"){
        int val = -99;
        ss.clear();
        int wlen = bencode::BObject::EncodeInt(ss, val);
        REQUIRE(wlen==5);
        auto num = bencode::BObject::DecodeInt(ss, nullptr);
        REQUIRE( val== num);
    }
}
#endif
int main(){
    std::ios::sync_with_stdio(false);
    for(int i=0;i<50;i++){
        std::ifstream ifs("../debian-iso.tt",std::ifstream::binary);
        std::ofstream ofs("../debian-iso(1).tt",std::ofstream::binary);
        Timer timer;//这是我本地写的一个计时类，利用的RAII特性计时
        if(ifs){//TODO 得到解码又编码后的数据
            auto obj = bencode::BObject::Parse(ifs, nullptr);
            if(obj)
                obj->Bencode(ofs);
        }
    }
}