#ifndef TABLE_HPP
#define TABLE_HPP

#include "result.hpp"
#include "data.hpp"



namespace sqlselect {

class Table {
    public:
        typedef std::shared_ptr<Table> Ptr;
        virtual ~Table() {}
        virtual Result::Ptr Call(Data::Ptr data, int64 zoom) const =0;
        virtual std::string Str() const=0;
};


}

#endif
