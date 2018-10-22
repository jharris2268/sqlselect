#ifndef RESULT_HPP
#define RESULT_HPP

#include "feature.hpp"
#include <vector>
namespace sqlselect {



class Result {
    public:

    
        typedef std::shared_ptr<Result> Ptr;
        
        virtual void reset()=0;
        virtual Feature::Ptr next() =0;
        virtual ~Result() {}
};

class EmptyResult : public Result {
    public:
        EmptyResult() {}
        typedef std::shared_ptr<EmptyResult> Ptr;
        static Ptr Create() { return std::make_shared<EmptyResult>(); }
        
        void reset() {}
        virtual Feature::Ptr next() { return nullptr; }
};

Feature_vec read_result(Result::Ptr result);

class MoveResult : public Result {
    public:
        MoveResult(Feature_vec&& feats_) : feats(feats_), ii(0) {}
        virtual ~MoveResult() {}
        typedef std::shared_ptr<MoveResult> Ptr;
        static Ptr Create(Feature_vec&& feats) { return std::make_shared<MoveResult>(std::move(feats)); }
        
        void reset() { ii=0; }
        
        Feature::Ptr next() {
            if (ii >= feats.size()) { return nullptr; }
            auto f= feats.at(ii);
            ii++;
            return f;
        }
    private:
        Feature_vec feats;
        size_t ii;
};



class RefResult : public Result {
    public:
        RefResult(const Feature_vec& feats_, int64 zoom_) : feats(feats_), ii(0), zoom(zoom_) {}
        virtual ~RefResult() {}
        
        typedef std::shared_ptr<RefResult> Ptr;
        static Ptr Create(const Feature_vec& feats, int64 minzoom) { return std::make_shared<RefResult>(feats,minzoom); }
        
        void reset() { ii=0; }
        Feature::Ptr next() {
            while (ii < feats.size()) {
                auto f= feats.at(ii);
                ii++;
                if ((zoom==-1) || (f->MinZoom() <= zoom)) {
                    return f;
                }
            }
            return nullptr;
        
        }
    private:
        const Feature_vec& feats;
        size_t ii;
        int64 zoom;
};

class UnionResult : public Result {
    public:
        UnionResult(Result::Ptr left_, Result::Ptr right_) : left(left_), right(right_), finished_left(false), finished_right(false) {
            if (!left) { finished_left=true; }
            if (!right) { finished_right=true; }
        }
        
        virtual ~UnionResult() {}
        
        typedef std::shared_ptr<UnionResult> Ptr;
        static Ptr Create(Result::Ptr left, Result::Ptr right) { return std::make_shared<UnionResult>(left,right); }
        
        void reset() {
            if (left) {
                left->reset();
                finished_left=false;
            }
            if (right) {
                right->reset();
                finished_right=false;
            }
        }
        
        Feature::Ptr next() {
            if (!finished_left) {
                auto f = left->next();
                if (!f) {
                    finished_left=true;
                    return next();
                }
                return f;
            }
            if (!finished_right) {
                auto f = right->next();
                if (!f) {
                    finished_right=true;
                    return next();
                }
                return f;
            }
            return nullptr;
        }
    private:
        Result::Ptr left;
        Result::Ptr right;
        bool finished_left;
        bool finished_right;
};
        
        
         



}

#endif
