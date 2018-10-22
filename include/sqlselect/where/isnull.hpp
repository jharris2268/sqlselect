#ifndef WHERE_ISNULL_HPP
#define WHERE_ISNULL_HPP

#include "where.hpp"
#include "column.hpp"
namespace sqlselect {

class IsNull : public Where {
    public:
        IsNull(Column::Ptr val_) : val(val_) {}
        virtual ~IsNull() {}
        
        typedef std::shared_ptr<IsNull> Ptr;
        static Ptr Create(Column::Ptr val) { return std::make_shared<IsNull>(val); }
        
        bool Call(Feature::Ptr feat, int64 zoom) const;
        std::string Str() const;
        
        Column::Ptr GetVal() { return val; }
        
    private:
        Column::Ptr val;
};

    
}
#endif
