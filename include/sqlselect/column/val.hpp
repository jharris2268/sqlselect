#ifndef COLUMN_VAL_HPP
#define COLUMN_VAL_HPP

#include "column.hpp"

namespace sqlselect {


class Val : public Column {
    public:
        typedef std::shared_ptr<Val> Ptr;
        Val(Value val_) : val(val_) {}
        
        virtual ~Val() {}
        static Ptr Create(Value val) {
            return std::make_shared<Val>(val);
        }
        
        
        virtual Value Call(Feature::Ptr feat, int64 zoom) const { return val; }
        virtual std::string Str() const;
        
        Value StoredValue() const { return val; }
    private:
        
        Value val;
};

}

#endif
