#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <iostream>

struct Fragment final
{
    Fragment(uint16_t id, uint16_t fid, uint16_t total, uint16_t size, std::string data)
        : _id(id), _fid(fid), _total(total), _size(size), _data(std::move(data)) {
    }

    uint16_t id()    const { return _id;    }
    uint16_t fid()   const { return _fid;   }
    uint16_t total() const { return _total; }
    uint16_t size()  const { return _size;  }

    // Only read access
    const std::string&  data() const &  { return _data; }
    const std::string&  data() &        { return _data; }

    const std::string&& data() const && = delete;
          std::string&& data() &&       { return std::move(_data); }

private:
    uint16_t    _id;
    uint16_t    _fid;
    uint16_t    _total;
    uint16_t    _size;
    std::string _data;
};

class StringFragmentator final
{
public:
    void AddFragments(std::vector<Fragment>&&);
    void AddFragments(const std::vector<Fragment>&);

    template<typename T>
    void AddFragment(T&&);

    std::vector<Fragment> Split(const std::vector<std::string>& strs, size_t num_fragments);

    const std::unordered_map<int, std::string>&  GetStringMap() &        { return _id_to_strs;            }
    const std::unordered_map<int, std::string>&  GetStringMap() const &  { return _id_to_strs;            }

    const std::unordered_map<int, std::string>&& GetStringMap() const && = delete;
          std::unordered_map<int, std::string>&& GetStringMap() &&       { return std::move(_id_to_strs); }


private:
    /* FIXME Struct has not trivial logic with increment num_received_fragments and clearing state,
     * maybe better use class ? But now it internal only stuff and used in one place */
    struct FragmentsInfo {
        FragmentsInfo() = default;
        explicit FragmentsInfo(int num_fragments) : fragments(num_fragments) {};
        size_t num_received_fragments = 0;
        std::vector<std::string> fragments;
    };

    std::unordered_map<int, std::string  > _id_to_strs;
    std::unordered_map<int, FragmentsInfo> _id_to_finfo;

    int _id_counter = 0;
};

template<typename T>
void StringFragmentator::AddFragment(T&& f) {
    if (auto it = _id_to_finfo.find(f.id()); it == _id_to_finfo.end()) {
        _id_to_finfo.emplace(f.id(), f.total());
    }

    auto& all_finfos = _id_to_finfo[f.id()];

    // NB: If string is already collected, remove it and start to collect again
    if (all_finfos.num_received_fragments == f.total()) {
        all_finfos.num_received_fragments = 0;
        _id_to_strs.erase(f.id());
    }

    all_finfos.fragments[f.fid()] = std::forward<T>(f).data();
    all_finfos.num_received_fragments++;

    if (all_finfos.num_received_fragments == f.total()) {
        std::string total_str;
        total_str.reserve(f.total() * f.size() + f.size());
        for (const auto& s : all_finfos.fragments) {
            total_str.append(s);
        }

        _id_to_strs.emplace(f.id(), std::move(total_str));
    }
}
