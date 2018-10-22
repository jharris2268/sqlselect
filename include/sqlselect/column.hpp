#ifndef COLUMN_HPP
#define COLUMN_HPP

#include "feature.hpp"

namespace sqlselect {
class Column {
    public:
        typedef std::shared_ptr<Column> Ptr;
        
        virtual ~Column() {}
        
        virtual const std::string& Key() const { return empty; };
        virtual Value Call(Feature::Ptr feat, int64 zoom) const=0;
        virtual std::string Str() const=0;
    private:
        std::string empty;
};

}
#endif
