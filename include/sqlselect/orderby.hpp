#ifndef ORDERBY_HPP
#define ORDERBY_HPP

#include "feature.hpp"

namespace sqlselect {
    
    
struct OrderItem {
    OrderItem(Column::Ptr column_, bool asc_) : column(column_), asc(asc_) {}
    
    std::string Str() const;
    
    Column::Ptr column;
    bool asc;
};
typedef std::vector<OrderItem> OrderItemVec;
    
class OrderBy {
    public:
        typedef std::shared_ptr<OrderBy> Ptr;
        typedef std::function<bool(Feature::Ptr,Feature::Ptr)> cmp_func;
        
        OrderBy(const OrderItemVec& spec_) : spec(spec_) {}
        
        static Ptr Create(const OrderItemVec& spec) {
            return std::make_shared<OrderBy>(spec);
        }
        
        virtual cmp_func Call(int64 zoom) const;
        virtual std::string Str() const;
        
        const OrderItemVec& GetSpec() const { return spec; }
        
    private:
        OrderItemVec spec;
        
};



}

#endif
