#ifndef DATA_HPP
#define DATA_HPP

#include "result.hpp"

namespace sqlselect {


class Data {
    public:
        Data(Data_map&& contents_) : contents(contents_) {}
        
        typedef std::shared_ptr<Data> Ptr;
        static Ptr Create(Data_map&& feats) { return std::make_shared<Data>(std::move(feats)); }
        
        virtual Result::Ptr Call(const std::string& table, int64 zoom) const {
            auto it = contents.find(table);
            if (it==contents.end()) { return EmptyResult::Create(); }
            return RefResult::Create(it->second, zoom);
        }

        void AddResult(const std::string& key, Result::Ptr result) {
            contents[key] = read_result(result);
        }

        const Data_map& Contents() const { return contents; }
    private:
        Data_map contents;
        
};



}

#endif
