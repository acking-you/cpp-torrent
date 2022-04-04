//#include <catch2/catch.hpp>
#include "bencode.h"
#include "../../../BenchMark/Timer.h"
#include <fstream>
#include <iostream>
#include <thread>
#include <sstream>

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



//test custom type
struct Student {
    int id;
    std::string name;
    std::vector<std::string> ip;
    std::unordered_map<std::string, int> mapping;
};

using namespace bencode;

void to_bencode(Bencode &obj, Student &student) {
    obj["id"] = student.id;
    obj["name"] = student.name;
    obj["ip"] = student.ip;
    obj["mapping"] = student.mapping;
}

void from_bencode(Bencode &obj, Student &student) {
    student.id = obj["id"].get<int>();
    student.name = obj["name"].get<std::string>();
    student.ip = obj["ip"].get<std::vector<std::string>>();
    student.mapping = obj["mapping"].get<std::unordered_map<std::string, int>>();
}

void printStudent(Student const &student) {
    std::cout << student.name << '\n';
    std::cout << student.id << '\n';
    for (const auto &item: student.ip) {
        std::cout << item << "  ";
    }
    std::cout << '\n';
    for (auto &&[k, v]: student.mapping) {
        std::cout << "k:" << k << "  " << "v:" << v << '\n';
    }
}

int main() {
    std::stringstream ss;
    Student student{
        .id = 2002,
        .name = "刘xx",
        .ip = {
          "2329323",
          "3432424",
          "23434322"
        },
        .mapping = {
                {"test",283},
                {"testname",3232}
        }
    };
    Bencode b;
    b<<student;
    ss<<b;
    std::cout<<ss.str()<<std::endl;
    Student tmp;
    b>>tmp;
    printStudent(tmp);
}