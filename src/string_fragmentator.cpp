#include <exception>

#include "string_fragmentator.hpp"

/************************** Fragment **************************/

Fragment::Fragment(uint16_t id, uint16_t fid, uint16_t total, uint16_t size, const char* data)
    : _id(id), _fid(fid), _total(total), _size(size) {
    AllocateAndFillData(data, _size);
}

Fragment::Fragment(const Fragment& other) {
    MakeCopy(other);
}

Fragment::Fragment(Fragment&& other) noexcept {
    MakeMove(std::move(other));
}

Fragment& Fragment::operator=(const Fragment& other) {
    if (this != &other) {
        Destroy();
        MakeCopy(other);
    }

    return *this;
}

Fragment& Fragment::operator=(Fragment&& other) noexcept {
    if (this != &other) {
        Destroy();
        MakeMove(std::move(other));
    }

    return *this;
}

void Fragment::MakeCopy(const Fragment& other) {
    CopySimpleFields(other);
    AllocateAndFillData(other._data, other._size);
}

void Fragment::MakeMove(Fragment&& other) noexcept {
    _data = other._data;
    other._data = nullptr;

    CopySimpleFields(other);
}

void Fragment::AllocateAndFillData(const char* data, uint16_t size)
{
    _data = new char[size + 1];
    std::copy(data, data + size, _data);
    _data[size] = '\0';
}

void Fragment::CopySimpleFields(const Fragment& other) noexcept{
    _id    = other._id;
    _fid   = other._fid;
    _total = other._total;
    _size  = other._size;
}

void Fragment::Destroy() noexcept {
    if (_data)
        delete[] _data;
}

Fragment::~Fragment() {
    Destroy();
}

/************************** String Fragmentator **************************/


void StringFragmentator::AddFragments(std::vector<Fragment>&& fragments)
{
    for (auto&& fragment : fragments)
        AddFragment(std::move(fragment));
}

void StringFragmentator::AddFragments(const std::vector<Fragment>& fragments)
{
    for (const auto& fragment : fragments)
        AddFragment(fragment);
}

const std::unordered_map<int, std::string>& StringFragmentator::GetStringMap() const {
    return _id_to_strs;
}

std::vector<Fragment> StringFragmentator::Split(const std::vector<std::string>& strs, size_t num_fragments) {
    if (strs.empty()) {
        throw std::logic_error("String vector to split is empty");
    }

    std::vector<Fragment> fragments;
    for (const auto& str : strs) {
        if (num_fragments > str.size()) {
            throw std::logic_error("Number of fragments larger than buffer size");
        }

        if (num_fragments == 0) {
            throw std::logic_error("The number of fragments must be greater than zero");
        }

        int f_size = str.size() / num_fragments;
        int remainder = str.size() % num_fragments;
        const char* str_data = str.data();

        for (int fid = 0; fid < num_fragments - 1; ++fid)
        {
            fragments.emplace_back(_id_counter, fid, num_fragments, f_size, str_data);
            str_data += f_size;
        }

        fragments.emplace_back(_id_counter, num_fragments - 1, num_fragments, f_size + remainder, str_data);

        ++_id_counter;
    }

    return fragments;
}
