//
// Created by Alone on 2022-4-3.
//

#ifndef CPP_TORRENT_BENCODE_H
#define CPP_TORRENT_BENCODE_H

#include <variant>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <map>
namespace bencode {
    enum class Error {
        ErrNum,
        ErrCol,
        ErrEpI,
        ErrEpE,
        ErrTyp,
        ErrIvd,
        NoError
    };
    enum class BType {
        BSTR,
        BINT,
        BLIST,
        BDICT
    };


    void perror(Error error_code);


    template<class T>
    class BEntity {

    };

    class BObject {
    public:
        using LIST = std::vector<std::shared_ptr<BObject>>;
        using DICT = std::unordered_map<std::string, std::shared_ptr<BObject>>;
        using BValue = std::variant<int, std::string, LIST, DICT>;

        friend class BEntity<LIST>;

        friend class BEntity<int>;

        friend class BEntity<std::string>;

        friend class BEntity<DICT>;

        BObject() = default;

        BObject(std::string);

        BObject(const char *str);

        BObject(int v);

        BObject(LIST list);

        BObject(DICT dict);

        BObject &operator=(int);

        BObject &operator=(std::string);

        BObject& operator=(LIST);

        BObject& operator=(DICT);

        std::string *Str(Error *error_code = nullptr);

        int *Int(Error *error_code = nullptr);

        LIST *List(Error *error_code = nullptr);

        DICT *Dict(Error *error_code = nullptr);


        int Bencode(std::ostream &os);

        static std::shared_ptr<BObject> Parse(std::istream& in,Error* error);

        static int EncodeString(std::ostream &os, std::string_view val);

        static std::string DecodeString(std::istream &in, Error *error);

        static int EncodeInt(std::ostream &os, int val);

        static int DecodeInt(std::istream &in, Error *error);

    private:
        BType type;
        BValue value;

        static int getIntLen(int val);
    };


    using LIST = BObject::LIST;
    using DICT = BObject::DICT;//方便外面少写一个区域名

    template<>
    class BEntity<LIST> {
        BObject object;
        BObject::LIST *list;
    public:
        BEntity() : object(BObject::LIST()) {
            list = object.List();
            if (list == nullptr)
                throw std::bad_alloc();
        }

        BEntity &add(BObject src) {
            list->push_back(std::make_shared<BObject>(std::move(src)));
            return *this;
        }

        int bencode(std::ostream &os) {
            return object.Bencode(os);
        }

        friend std::ostream &operator<<(std::ostream &os, BEntity entity) {
            entity.object.Bencode(os);
            return os;
        }
    };

    template<>
    class BEntity<DICT> {
        BObject object;
        BObject::DICT *dict;
    public:
        BEntity() : object(BObject::DICT()) {
            dict = object.Dict();
            if (dict == nullptr)
                throw std::bad_alloc();
        }

        BEntity &put(const std::string &key, BObject value) {
            dict->emplace(key, std::make_shared<BObject>(std::move(value)));
            return *this;
        }

        int bencode(std::ostream &os) {
            return object.Bencode(os);
        }

        friend std::ostream &operator<<(std::ostream &os, BEntity entity) {
            entity.object.Bencode(os);
            return os;
        }
    };

    template<>
    class BEntity<int> {
        BObject object;
        int *val;
    public:
        BEntity() : object(0) {
            val = object.Int();
            if (val == nullptr)
                throw std::bad_alloc();
        }

        void set(int v) {
            *val = v;
        }

        int *data() {
            return val;
        }

        int bencode(std::ostream &os) {
            return object.Bencode(os);
        }

        friend std::ostream &operator<<(std::ostream &os, BEntity entity) {
            entity.object.Bencode(os);
            return os;
        }
    };

    template<>
    class BEntity<std::string> {
        BObject object;
        std::string *val;
    public:
        BEntity() : object("") {
            val = object.Str();
            if (val == nullptr)
                throw std::bad_alloc();
        }

        void set(std::string v) {
            *val = std::move(v);
        }

        const char *data() {
            return val->c_str();
        }

        int bencode(std::ostream &os) {
            return object.Bencode(os);
        }

        friend std::ostream &operator<<(std::ostream &os, BEntity entity) {
            entity.object.Bencode(os);
            return os;
        }
    };
}


#endif //CPP_TORRENT_BENCODE_H
