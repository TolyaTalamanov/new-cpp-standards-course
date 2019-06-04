#pragma once

#include <vector>
#include <string>
#include <unordered_map>

struct Fragment final
{
    Fragment() = default;
    Fragment(uint16_t, uint16_t, uint16_t, uint16_t, const char*);

    Fragment(const Fragment& other);
    Fragment(Fragment&&) noexcept;

    Fragment& operator=(const Fragment&);
    Fragment& operator=(Fragment&&) noexcept;

    uint16_t    id()    const { return _id;    }
    uint16_t    fid()   const { return _fid;   }
    uint16_t    total() const { return _total; }
    uint16_t    size()  const { return _size;  }
    const char* data()  const { return _data;  }

    ~Fragment();

private:
    void MakeCopy(const Fragment&);
    void MakeMove(Fragment&&) noexcept;

    void AllocateAndFillData(const char*, uint16_t);
    void CopySimpleFields(const Fragment&) noexcept;

    void Destroy() noexcept;

    uint16_t _id;
    uint16_t _fid;
    uint16_t _total;
    uint16_t _size;
    char*    _data;
};


class StringFragmentator final
{
public:
    void AddFragments(std::vector<Fragment>&&);
    void AddFragments(const std::vector<Fragment>&);

    template<typename T>
    void AddFragment(T&&);

    std::vector<Fragment> Split(const std::vector<std::string>& strs, size_t num_fragments);

    const std::unordered_map<int, std::string>& GetStringMap() const;
private:
    std::unordered_map<int, std::string>           _id_to_strs;
    std::unordered_map<int, std::vector<Fragment>> _id_to_fragments;

    // FIXME Make Singletone or static
    int _id_counter = 0;
};

template<typename T>
void StringFragmentator::AddFragment(T&& f) {
    // NB: Create and reserve if needed
    _id_to_fragments[f.id()].reserve(f.total());

    // NB: If string is already collected, remove it and start to collect again
    if (_id_to_fragments[f.id()].size() == f.total()) {
        _id_to_fragments[f.id()].clear();
        _id_to_strs.erase(f.id());
    }

    _id_to_fragments[f.id()].push_back(std::forward<T>(f));

    if (_id_to_fragments[f.id()].size() == f.total()) {
        auto& str = _id_to_strs[f.id()];
        str.reserve(f.total() * f.size() + f.size());
        const auto& all_fragments = _id_to_fragments[f.id()];

        for (const auto& fragment : all_fragments) {
            str.append(fragment.data(), fragment.size());
        }
    }
}
