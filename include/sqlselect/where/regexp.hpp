#ifndef WHERE_REGEXP_HPP
#define WHERE_REGEXP_HPP

#include "where.hpp"
#include "column.hpp"
#include <regex>
namespace sqlselect {

    
class RegEx : public Where {
    public:
        typedef std::shared_ptr<RegEx> Ptr;
        RegEx(Column::Ptr col_, const std::string& regex_str_) : col(col_), regex_str(regex_str_) { regex = std::regex(regex_str); }
        
        virtual ~RegEx() {}
        static Ptr Create(Column::Ptr col, const std::string& regex_str) { return std::make_shared<RegEx>(col,regex_str); }
        
        virtual bool Call(Feature::Ptr feat, int64 zoom) const {
            auto s = col->Call(feat,zoom).to_string();
            return std::regex_match(s, regex);
            
        }
            
        virtual std::string Str() const {
            return "RegEx("+col->Str()+", "+quote_str(regex_str)+")";
        }
        
        Column::Ptr GetColumn() const { return col; }
        std::string GetMatch() const { return regex_str; }
        
        
    private:
        Column::Ptr col;
        std::string regex_str;
        std::regex regex;
        
};



}
#endif

