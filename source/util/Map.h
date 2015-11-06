#ifndef BLE_CLIAPP_DYNAMIC_OBJECT_H_
#define BLE_CLIAPP_DYNAMIC_OBJECT_H_

#include <memory>
#include <utility>
#include "util/Vector.h"

namespace container {

template<typename K, typename V>
class Map {

    typedef std::pair<K, V>  Pair_t;

public:

    typedef typename container::Vector<Pair_t>::iterator iterator;
    typedef typename container::Vector<Pair_t>::const_iterator const_iterator;

    Map() : _array() {}

    V& operator[](const K& key) { 
        for(size_t i = 0; i < _array.size(); ++i) {
            if(_array[i].first == key) {
                return _array[i].second;
            }
        }
        _array.push_back(Pair_t(key, V()));
        return _array[_array.size() - 1].second;
    }

    V& operator[](K&& key) { 
        for(size_t i = 0; i < _array.size(); ++i) {
            if(_array[i].first == key) {
                return _array[i].second;
            }
        }
        _array.push_back(Pair_t(std::move(key), V()));
        return _array[_array.size() - 1].second;
    }

    iterator begin() { 
        return _array.begin();
    }

    const_iterator cbegin() const { 
        return _array.cbegin();
    }

    iterator end() { 
        return _array.end();
    }

    const_iterator cend() const { 
        return _array.cend();
    }

    iterator find(const K& key) { 
        return std::find(begin(), end(), [&](const Pair_t& p) {
            return p.first == key;
        });
    }

    const_iterator find(const K& key) const { 
        return std::find(cbegin(), cend(), [&](const Pair_t& p) {
            return p.first == key;
        });
    }

    friend bool operator==(const Map& rhs, const Map& lhs) {
        return lhs._array == rhs._array;
    }

    friend bool operator!=(const Map& lhs, const Map& rhs) {
        return !(lhs == rhs);
    }

    bool empty() const {
        return _array.size() == 0;
    }

private:
    // The storage for our map
    container::Vector<Pair_t> _array;
};

} // namespace container

#endif //BLE_CLIAPP_DYNAMIC_OBJECT_H_
