#ifndef COLUMN_CASE_HPP
#define COLUMN_CASE_HPP

#include "column.hpp"
#include "where.hpp"
namespace sqlselect {


struct Clause {
    Where::Ptr where;
    Column::Ptr column;
    Clause(Where::Ptr where_, Column::Ptr column_) : where(where_), column(column_) {}
    
    std::string Str() const {
        std::string res = "Clause(";
        if (where) { res += where->Str(); }
        else { res += "None"; }
        res += ", "+column->Str()+")";
        return res;
    }
};

typedef std::vector<Clause> Clause_Vec;



class Case : public Column {
    public:
        Case(const Clause_Vec& clauses_) : clauses(clauses_) {}
        
        typedef std::shared_ptr<Case> Ptr;
        static Ptr Create(const Clause_Vec& clauses) {
            return std::make_shared<Case>(clauses);
        }
        
        virtual Value Call(Feature::Ptr feat, int64 zoom) const {
            for (const auto& clause : clauses) {
                if (!clause.where || (clause.where->Call(feat,zoom))) {
                    return clause.column->Call(feat,zoom);
                }
            }
            return null_value();
        }
        
        virtual std::string Str() const {
            return "Case("+all_str_obj(clauses)+")";
        }
        
        const Clause_Vec& GetClauses() const { return clauses; }
    private:
        Clause_Vec clauses;
};



}
#endif
