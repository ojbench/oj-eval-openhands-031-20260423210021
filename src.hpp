#ifndef PYLIST_H
#define PYLIST_H

#include <iostream>
#include <vector>
#include <memory>
#include <set>
#include <unordered_set>

class pylist {
private:
    // Forward declare the internal implementation
    struct Impl;
    std::shared_ptr<Impl> data;
    
    // Global registry for tracking all Impl instances
    static std::unordered_set<Impl*>& get_registry() {
        static std::unordered_set<Impl*> registry;
        return registry;
    }

public:
    // Forward declare the proxy classes
    class ValueProxy;
    class ElementProxy;
    
    pylist();
    ~pylist();
    
    void append(int x);
    void append(const pylist& x);
    
    ValueProxy pop();
    ElementProxy operator[](size_t i);
    
    friend std::ostream& operator<<(std::ostream& os, const pylist& ls);
};

// Now define the implementation struct with variant
#include <variant>
using Element = std::variant<int, pylist>;

struct pylist::Impl {
    std::vector<Element> elements;
    
    ~Impl() {
        // Try to break cycles by clearing
        elements.clear();
    }
};

// ValueProxy holds a value (not a reference) and can act as both int and pylist
class pylist::ValueProxy {
private:
    Element value;
    
public:
    ValueProxy(const Element& e) : value(e) {}
    ValueProxy(int val) : value(val) {}
    ValueProxy(const pylist& lst) : value(lst) {}
    
    // Conversion to int
    operator int() const {
        if (std::holds_alternative<int>(value)) {
            return std::get<int>(value);
        }
        return 0;
    }
    
    // Conversion to pylist
    operator pylist() const {
        if (std::holds_alternative<pylist>(value)) {
            return std::get<pylist>(value);
        }
        return pylist();
    }
    
    // Allow printing
    friend std::ostream& operator<<(std::ostream& os, const ValueProxy& proxy) {
        if (std::holds_alternative<int>(proxy.value)) {
            os << std::get<int>(proxy.value);
        } else {
            os << std::get<pylist>(proxy.value);
        }
        return os;
    }
    
    // Arithmetic operators
    friend int operator+(int a, const ValueProxy& b) { return a + (int)b; }
    friend int operator-(int a, const ValueProxy& b) { return a - (int)b; }
    friend int operator*(int a, const ValueProxy& b) { return a * (int)b; }
    friend int operator/(int a, const ValueProxy& b) { return a / (int)b; }
    friend int operator%(int a, const ValueProxy& b) { return a % (int)b; }
    friend int operator&(int a, const ValueProxy& b) { return a & (int)b; }
    friend int operator|(int a, const ValueProxy& b) { return a | (int)b; }
    friend int operator^(int a, const ValueProxy& b) { return a ^ (int)b; }
    friend int operator<<(int a, const ValueProxy& b) { return a << (int)b; }
    friend int operator>>(int a, const ValueProxy& b) { return a >> (int)b; }
    
    friend int operator+(const ValueProxy& a, int b) { return (int)a + b; }
    friend int operator-(const ValueProxy& a, int b) { return (int)a - b; }
    friend int operator*(const ValueProxy& a, int b) { return (int)a * b; }
    friend int operator/(const ValueProxy& a, int b) { return (int)a / b; }
    friend int operator%(const ValueProxy& a, int b) { return (int)a % b; }
    friend int operator&(const ValueProxy& a, int b) { return (int)a & b; }
    friend int operator|(const ValueProxy& a, int b) { return (int)a | b; }
    friend int operator^(const ValueProxy& a, int b) { return (int)a ^ b; }
    
    friend int operator+(const ValueProxy& a, const ValueProxy& b) { return (int)a + (int)b; }
    friend int operator-(const ValueProxy& a, const ValueProxy& b) { return (int)a - (int)b; }
    friend int operator*(const ValueProxy& a, const ValueProxy& b) { return (int)a * (int)b; }
    friend int operator/(const ValueProxy& a, const ValueProxy& b) { return (int)a / (int)b; }
    friend int operator%(const ValueProxy& a, const ValueProxy& b) { return (int)a % (int)b; }
    friend int operator&(const ValueProxy& a, const ValueProxy& b) { return (int)a & (int)b; }
    friend int operator|(const ValueProxy& a, const ValueProxy& b) { return (int)a | (int)b; }
    friend int operator^(const ValueProxy& a, const ValueProxy& b) { return (int)a ^ (int)b; }
    
    // Make ElementProxy a friend
    friend class ElementProxy;
};

// Proxy class to handle operator[] return value
class pylist::ElementProxy {
private:
    Element& elem;
    
public:
    ElementProxy(Element& e) : elem(e) {}
    
    // Conversion to int
    operator int() const {
        if (std::holds_alternative<int>(elem)) {
            return std::get<int>(elem);
        }
        return 0;
    }
    
    // Conversion to pylist
    operator pylist() const {
        if (std::holds_alternative<pylist>(elem)) {
            return std::get<pylist>(elem);
        }
        return pylist();
    }
    
    // Conversion to pylist& for address-taking
    operator pylist&() {
        if (std::holds_alternative<pylist>(elem)) {
            return std::get<pylist>(elem);
        }
        // This shouldn't happen, but we need to return something
        static pylist dummy;
        return dummy;
    }
    
    // Address-of operator to allow taking address of the underlying pylist
    pylist* operator&() {
        if (std::holds_alternative<pylist>(elem)) {
            return &std::get<pylist>(elem);
        }
        return nullptr;
    }
    
    // Assignment from int
    ElementProxy& operator=(int val) {
        elem = val;
        return *this;
    }
    
    // Assignment from pylist
    ElementProxy& operator=(const pylist& lst) {
        elem = lst;
        return *this;
    }
    
    // Assignment from ValueProxy
    ElementProxy& operator=(const ValueProxy& vp) {
        // Convert ValueProxy to pylist or int
        if (std::holds_alternative<pylist>(vp.value)) {
            elem = std::get<pylist>(vp.value);
        } else {
            elem = std::get<int>(vp.value);
        }
        return *this;
    }
    
    // Assignment from another ElementProxy
    ElementProxy& operator=(const ElementProxy& other) {
        elem = other.elem;
        return *this;
    }
    
    // Subscript operator for nested access
    ElementProxy operator[](size_t i) {
        if (std::holds_alternative<pylist>(elem)) {
            return std::get<pylist>(elem)[i];
        }
        // Should not happen, but return self for safety
        return *this;
    }
    
    // Allow chaining operations
    friend std::ostream& operator<<(std::ostream& os, const ElementProxy& proxy) {
        if (std::holds_alternative<int>(proxy.elem)) {
            os << std::get<int>(proxy.elem);
        } else {
            os << std::get<pylist>(proxy.elem);
        }
        return os;
    }
    
    // Arithmetic operators
    friend int operator+(int a, const ElementProxy& b) { return a + (int)b; }
    friend int operator-(int a, const ElementProxy& b) { return a - (int)b; }
    friend int operator*(int a, const ElementProxy& b) { return a * (int)b; }
    friend int operator/(int a, const ElementProxy& b) { return a / (int)b; }
    friend int operator%(int a, const ElementProxy& b) { return a % (int)b; }
    friend int operator&(int a, const ElementProxy& b) { return a & (int)b; }
    friend int operator|(int a, const ElementProxy& b) { return a | (int)b; }
    friend int operator^(int a, const ElementProxy& b) { return a ^ (int)b; }
    friend int operator<<(int a, const ElementProxy& b) { return a << (int)b; }
    friend int operator>>(int a, const ElementProxy& b) { return a >> (int)b; }
    
    friend int operator+(const ElementProxy& a, int b) { return (int)a + b; }
    friend int operator-(const ElementProxy& a, int b) { return (int)a - b; }
    friend int operator*(const ElementProxy& a, int b) { return (int)a * b; }
    friend int operator/(const ElementProxy& a, int b) { return (int)a / b; }
    friend int operator%(const ElementProxy& a, int b) { return (int)a % b; }
    friend int operator&(const ElementProxy& a, int b) { return (int)a & b; }
    friend int operator|(const ElementProxy& a, int b) { return (int)a | b; }
    friend int operator^(const ElementProxy& a, int b) { return (int)a ^ b; }
    
    friend int operator+(const ElementProxy& a, const ElementProxy& b) { return (int)a + (int)b; }
    friend int operator-(const ElementProxy& a, const ElementProxy& b) { return (int)a - (int)b; }
    friend int operator*(const ElementProxy& a, const ElementProxy& b) { return (int)a * (int)b; }
    friend int operator/(const ElementProxy& a, const ElementProxy& b) { return (int)a / (int)b; }
    friend int operator%(const ElementProxy& a, const ElementProxy& b) { return (int)a % (int)b; }
    friend int operator&(const ElementProxy& a, const ElementProxy& b) { return (int)a & (int)b; }
    friend int operator|(const ElementProxy& a, const ElementProxy& b) { return (int)a | (int)b; }
    friend int operator^(const ElementProxy& a, const ElementProxy& b) { return (int)a ^ (int)b; }
    
    // Comparison operators
    friend bool operator<(const ElementProxy& a, const ElementProxy& b) { return (int)a < (int)b; }
    friend bool operator>(const ElementProxy& a, const ElementProxy& b) { return (int)a > (int)b; }
    friend bool operator<=(const ElementProxy& a, const ElementProxy& b) { return (int)a <= (int)b; }
    friend bool operator>=(const ElementProxy& a, const ElementProxy& b) { return (int)a >= (int)b; }
    friend bool operator==(const ElementProxy& a, const ElementProxy& b) { return (int)a == (int)b; }
    friend bool operator!=(const ElementProxy& a, const ElementProxy& b) { return (int)a != (int)b; }
    
    // Unary operators
    int operator+() const { return +(int)*this; }
    int operator-() const { return -(int)*this; }
    
    // Append method for nested lists
    void append(int val) {
        if (std::holds_alternative<pylist>(elem)) {
            std::get<pylist>(elem).append(val);
        }
    }
    
    void append(const pylist& lst) {
        if (std::holds_alternative<pylist>(elem)) {
            std::get<pylist>(elem).append(lst);
        }
    }
    
    void append(const ElementProxy& ep) {
        if (std::holds_alternative<pylist>(elem)) {
            // Convert ElementProxy to int or pylist and append
            if (std::holds_alternative<int>(ep.elem)) {
                std::get<pylist>(elem).append(std::get<int>(ep.elem));
            } else {
                std::get<pylist>(elem).append(std::get<pylist>(ep.elem));
            }
        }
    }
    
    // Pop method for nested lists
    ValueProxy pop() {
        if (std::holds_alternative<pylist>(elem)) {
            return std::get<pylist>(elem).pop();
        }
        // Should not happen
        return ValueProxy(0);
    }
    
    // Make ValueProxy a friend so it can access elem
    friend class ValueProxy;
};

// Implementation of pylist methods
inline pylist::pylist() {
    // Custom deleter that clears the vector before deleting
    data = std::shared_ptr<Impl>(new Impl(), [](Impl* p) {
        if (p) {
            p->elements.clear();  // Break cycles
            delete p;
        }
    });
    get_registry().insert(data.get());
}

inline pylist::~pylist() {
    // Cleanup is handled by the custom deleter
}

inline void pylist::append(int x) {
    data->elements.push_back(x);
}

inline void pylist::append(const pylist& x) {
    data->elements.push_back(x);
}

inline pylist::ValueProxy pylist::pop() {
    Element temp = data->elements.back();
    data->elements.pop_back();
    return ValueProxy(temp);
}

inline pylist::ElementProxy pylist::operator[](size_t i) {
    return ElementProxy(data->elements[i]);
}

inline std::ostream& operator<<(std::ostream& os, const pylist& ls) {
    static thread_local std::set<const void*> visited;
    
    // Check if we're already printing this list (cycle detection)
    if (visited.count(ls.data.get())) {
        os << "[...]";
        return os;
    }
    
    visited.insert(ls.data.get());
    
    os << "[";
    for (size_t i = 0; i < ls.data->elements.size(); ++i) {
        if (i > 0) os << ", ";
        const Element& elem = ls.data->elements[i];
        if (std::holds_alternative<int>(elem)) {
            os << std::get<int>(elem);
        } else {
            os << std::get<pylist>(elem);
        }
    }
    os << "]";
    
    visited.erase(ls.data.get());
    
    return os;
}

#endif //PYLIST_H
