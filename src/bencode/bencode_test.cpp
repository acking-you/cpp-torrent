//#include <catch2/catch.hpp>
#include "bencode.h"
#include "../../../BenchMark/Timer.h"
#include <fstream>
#include <iostream>
#include <thread>
#include <sstream>
#include <nlohmann/json.hpp>

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


struct TorrentInfo{
    int length;
    std::string name;
    int piece_length;
    std::string pieces;
};
//test custom type
struct Torrent {
    std::string announce;
    std::string created_by;
    int creation_data;
    TorrentInfo info;
};

using namespace bencode;

    void to_bencode(Bencode &obj,const Torrent &student) {
        obj["announce"] =  student.announce;
        obj["created by"]= student.created_by ;
        obj["creation date"]= student.creation_data;
        obj["info"]= student.info;
    }
    void to_bencode(Bencode& obj,const TorrentInfo& info){
        obj["name"] = info.name;
        obj["length"] = info.length;
        obj["piece length"] = info.piece_length;
        obj["pieces"] = info.pieces;
    }



void from_bencode(Bencode &obj, Torrent &student) {
    student.announce = obj["announce"].get<std::string>();
    student.created_by = obj["created by"].get<std::string>();
    student.creation_data = obj["creation date"].get<int>();
    student.info = obj["info"].get<TorrentInfo>();
}

void from_bencode(Bencode& obj,TorrentInfo& info){
    info.name = obj["name"].get<std::string>();
    info.length = obj["length"].get<int>();
    info.piece_length = obj["piece length"].get<int>();
    info.pieces = obj["pieces"].get<std::string >();
}




int main() {
    //std::ifstream ifs("../test.tt",ifs.binary);
    //if(!ifs){
    //    std::cout<<"no file";
    //    return 0;
    //}

}