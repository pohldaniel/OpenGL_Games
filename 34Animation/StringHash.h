#pragma once

#include <string>

class StringHash {

public:

    StringHash() :value(0){
    }
    

    StringHash(const StringHash& hash) : value(hash.value) {
    }
    
    explicit StringHash(unsigned value_) : value(value_) {
    }
    
    explicit StringHash(const std::string& str) : value(Calculate(str.c_str())){
    }
    
    explicit StringHash(const char* str) : value(Calculate(str)) {
    }
    
    explicit StringHash(char* str) : value(Calculate(str)){
    }
    
    StringHash& operator = (const StringHash& rhs){
        value = rhs.value;
        return *this;
    }
    
    StringHash& operator = (const std::string& rhs){
        value = Calculate(rhs.c_str());
        return *this;
    }

    StringHash& operator = (const char* rhs){
        value = Calculate(rhs);
        return *this;
    }
    
    StringHash& operator = (char* rhs){
        value = Calculate(rhs);
        return *this;
    }
    
    StringHash operator + (const StringHash& rhs) const{
        StringHash ret;
        ret.value = value + rhs.value;
        return ret;
    }
    
    StringHash& operator += (const StringHash& rhs) {
        value += rhs.value;
        return *this;
    }
    
    bool operator == (const StringHash& rhs) const { return value == rhs.value; }
    bool operator != (const StringHash& rhs) const { return value != rhs.value; }
    bool operator < (const StringHash& rhs) const { return value < rhs.value; }
    bool operator > (const StringHash& rhs) const { return value > rhs.value; }
    operator bool () const { return value != 0; }
    unsigned Value() const { return value; }
    std::string ToString() const;
    unsigned ToHash() const { return value; }
    static unsigned Calculate(const char* str);
   
    static const StringHash ZERO;
    
private:

    unsigned value;
};
