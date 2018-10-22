#ifndef COLUMN_COALESCE_HPP
#define COLUMN_COALESCE_HPP

#include "column.hpp"

namespace sqlselect {
    

class Coalesce : public Column {
    public:
        Coalesce(const std::vector<Column::Ptr>& cols_) : cols(cols_) {}
        virtual ~Coalesce() {}
        
        typedef std::shared_ptr<Coalesce> Ptr;
        static Ptr Create(const std::vector<Column::Ptr>& cols) {
            return std::make_shared<Coalesce>(cols);
        }
        
        virtual Value Call(Feature::Ptr feat, int64 zoom) const {
            for (const auto& cc : cols) {
                auto v = cc->Call(feat,zoom);
                if (!v.is_null()) {
                    return v;
                }
            }
            return null_value();
        }
        
        virtual std::string Str() const {
            return "Coalesce({"+all_str(cols)+"})";
        }
        
        const std::vector<Column::Ptr>& GetColumns() const { return cols; }
    private:
        std::vector<Column::Ptr> cols;
};


}
#endif
