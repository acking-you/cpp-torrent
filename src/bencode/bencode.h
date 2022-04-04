//
// Created by Alone on 2022-4-3.
//

#ifndef CPP_TORRENT_BENCODE_H
#define CPP_TORRENT_BENCODE_H

#include <utility>
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


    void perror(Error error_code, const char *info = nullptr);

    template<class T>
    class BEntity;

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

        //TODO 隐式转化五件套
        BObject(std::string);

        BObject(const char *str);

        BObject(int v);

        BObject(LIST list);

        BObject(DICT dict);

        operator std::string();

        operator int();

        BObject &operator=(int);

        BObject &operator=(std::string);

        BObject &operator=(LIST);

        BObject &operator=(DICT);

        std::string *Str(Error *error_code = nullptr);

        int *Int(Error *error_code = nullptr);

        LIST *List(Error *error_code = nullptr);

        DICT *Dict(Error *error_code = nullptr);


        int Bencode(std::ostream &os);

        static std::shared_ptr<BObject> Parse(std::istream &in, Error *error);

        static int EncodeString(std::ostream &os, std::string_view val);

        static std::string DecodeString(std::istream &in, Error *error);

        static int EncodeInt(std::ostream &os, int val);

        static int DecodeInt(std::istream &in, Error *error);

    private:
        BType type;
        BValue value;

        static int getIntLen(int val);
    };

    //TODO 用于直接序列化的工具模板类

    using LIST = BObject::LIST;
    using DICT = BObject::DICT;//方便外面少写一个区域名
    class Bencode;

    template<>
    class BEntity<LIST> {
        std::shared_ptr<BObject> object;
        BObject::LIST *list;
    public:
        BEntity() : object(std::make_shared<BObject>(LIST())) {
            list = object->List();
            if (list == nullptr)
                throw std::bad_alloc();
        }

        BEntity &add(BObject src) {
            list->push_back(std::make_shared<BObject>(std::move(src)));
            return *this;
        }

        int bencode(std::ostream &os) {
            return object->Bencode(os);
        }

        friend std::ostream &operator<<(std::ostream &os, const BEntity &entity) {
            entity.object->Bencode(os);
            return os;
        }

        friend std::istream &operator>>(std::istream &is, BEntity &entity) {
            Error error;
            entity.object = std::move(BObject::Parse(is, &error));
            if (error != Error::NoError) {
                perror(error);
                exit(-1);
            }
            return is;
        }
    };

    template<>
    class BEntity<DICT> {
    public:
        std::shared_ptr<BObject> object;
        BObject::DICT *dict;

        friend class Bencode;

        BEntity() : object(std::make_shared<BObject>(DICT())) {
            dict = object->Dict();
            if (dict == nullptr)
                throw std::bad_alloc();
        }

        BEntity &put(const std::string &key, BObject value) {
            dict->emplace(key, std::make_shared<BObject>(std::move(value)));
            return *this;
        }

        int bencode(std::ostream &os) {
            return object->Bencode(os);
        }

        friend std::ostream &operator<<(std::ostream &os, const BEntity &entity) {
            entity.object->Bencode(os);
            return os;
        }

        friend std::istream &operator>>(std::istream &is, BEntity &entity) {
            Error error;
            entity.object = std::move(BObject::Parse(is, &error));
            if (error != Error::NoError) {
                perror(error);
                exit(-1);
            }
            return is;
        }
    };

    template<>
    class BEntity<int> {
        std::shared_ptr<BObject> object;
        int *val;
    public:
        BEntity() : object(std::make_shared<BObject>(0)) {
            val = object->Int();
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
            return object->Bencode(os);
        }

        friend std::ostream &operator<<(std::ostream &os, const BEntity &entity) {
            entity.object->Bencode(os);
            return os;
        }

        friend std::istream &operator>>(std::istream &is, BEntity &entity) {
            Error error;
            entity.object = std::move(BObject::Parse(is, &error));
            if (error != Error::NoError) {
                perror(error);
                exit(-1);
            }
            return is;
        }
    };

    template<>
    class BEntity<std::string> {
        std::shared_ptr<BObject> object;
        std::string *val;
    public:
        BEntity() : object(std::make_shared<BObject>("")) {
            val = object->Str();
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
            return object->Bencode(os);
        }

        friend std::ostream &operator<<(std::ostream &os, const BEntity &entity) {
            entity.object->Bencode(os);
            return os;
        }

        friend std::istream &operator>>(std::istream &is, BEntity &entity) {
            Error error;
            entity.object = std::move(BObject::Parse(is, &error));
            if (error != Error::NoError) {
                perror(error);
                exit(-1);
            }
            return is;
        }
    };


    template<class T>
    extern void to_bencode(Bencode &bEntity, T &src);

    template<class T>
    extern void from_bencode(BEntity<T> &bEntity, T &src);

    class Bencode {
        BEntity<DICT> m_dict;
        std::string cur_key;
    public:
        Bencode() = default;

        Bencode &operator[](const std::string &key) {
            cur_key = key;
            return *this;
        }

        Bencode &operator=(BObject object) {
            if (cur_key.empty()) {
                perror(Error::ErrIvd, "operator= valid because of string empty line 277");
                exit(-1);
            }
            m_dict.put(cur_key, std::move(object));
            return *this;
        }

        template<class T>
        Bencode &operator=(const std::vector<T> &src) {
            if (cur_key.empty()) {
                perror(Error::ErrIvd, "operator= valid because of string empty line 287");
                exit(-1);
            }

            BObject listObj((LIST()));
            auto list = listObj.List();
            if (!list) {
                perror(Error::ErrIvd, "list get error in operator= at line293");
                exit(-1);
            }

            for (const auto &item: src) {
                list->emplace_back(std::make_shared<BObject>(item));
            }
            m_dict.put(cur_key, std::move(listObj));
            return *this;
        }

        template<class T>
        Bencode &operator=(const std::unordered_map<std::string, T> &src) {
            if (cur_key.empty()) {
                perror(Error::ErrIvd, "operator= valid because of string empty line 308");
                exit(-1);
            }

            BObject dictObj((DICT()));
            auto dict = dictObj.Dict();
            if (!dict) {
                perror(Error::ErrIvd, "list get error in operator= at line 315");
                exit(-1);
            }

            for (auto&&[k, v]: src) {
                dict->emplace(k, std::make_shared<BObject>(v));
            }
            m_dict.put(cur_key, std::move(dictObj));
            return *this;
        }


        //TODO type trait
        template<class T>
        static constexpr bool isVector(const std::vector<T> &v) {
            return true;
        }

        template<class T>
        static constexpr bool isVector(const T &v) {
            return false;
        }

        template<class T>
        static constexpr bool isMap(const std::unordered_map<std::string, T> &v) {
            return true;
        }

        template<class T>
        static constexpr bool isMap(const std::map<std::string, T> &v) {
            return true;
        }

        template<class T>
        static constexpr bool isMap(const T &v) {
            return false;
        }


        template<class T>
        static void getMap(std::unordered_map<std::string, T> &obj, BObject &src) {
            Error error;
            auto dict = src.Dict(&error);
            if (dict == nullptr) {
                perror(error, "In getMap line 310");
                exit(-1);
            }
            for (auto&&[k, v]: *dict) {
                obj.emplace(k, T(*v));
            }
        }

        template<class T>
        static void getVector(std::vector<T> &obj, BObject &src) {
            Error error;
            auto list = src.List(&error);
            if (list == nullptr) {
                perror(error, "In getVector line 322");
                exit(-1);
            }
            for (auto &&v: *list) {
                obj.emplace_back(T(*v));
            }
        }

        template<class T>
        T get() {
            auto it = m_dict.dict->find(cur_key);
            T ret;
            if (it != m_dict.dict->end()) {
                BObject &object = *it->second;
                if constexpr(isMap(ret)) {
                    getMap(ret, object);
                } else if constexpr(isVector(ret)) {
                    getVector(ret, object);
                } else {
                    ret = T(object);
                }
            }
            return ret;
        }


        template<class T>
        friend Bencode &operator<<(Bencode &bencode, T &src) {
            to_bencode(bencode, src);
            return bencode;
        }

        template<class T>
        friend Bencode &operator>>(Bencode &bencode, T &src) {
            from_bencode(bencode, src);
            return bencode;
        }

        friend std::ostream &operator<<(std::ostream &os, Bencode &bencode) {
            bencode.m_dict.bencode(os);
            return os;
        }

        friend std::istream &operator>>(std::istream &is, Bencode &bencode) {
            Error error;
            bencode.m_dict.object = std::move(BObject::Parse(is, &error));
            if (error != Error::NoError) {
                perror(error);
                exit(-1);
            }
            return is;
        }

    };
}


#endif //CPP_TORRENT_BENCODE_H
