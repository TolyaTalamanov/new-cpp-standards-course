#include <exception>

#include "string_fragmentator.hpp"

void StringFragmentator::AddFragments(std::vector<Fragment>&& fragments)
{
    for (auto&& fragment : fragments) {
        AddFragment(std::move(fragment));
    }
}

void StringFragmentator::AddFragments(const std::vector<Fragment>& fragments)
{
    for (const auto& fragment : fragments) {
        AddFragment(fragment);
    }
}

std::vector<Fragment> StringFragmentator::Split(const std::vector<std::string>& strs, size_t num_fragments) {
    if (num_fragments == 0) {
        throw std::logic_error("The number of fragments must be greater than zero");
    }

    std::vector<Fragment> fragments;
    fragments.reserve(num_fragments * strs.size());

    for (const auto& str : strs) {
        if (num_fragments > str.size()) {
            throw std::logic_error("Number of fragments larger than buffer size");
        }

        int f_size    = str.size() / num_fragments;
        int remainder = str.size() % num_fragments;

        const char* str_data = str.c_str();

        for (int fid = 0; fid < (num_fragments - 1); ++fid)
        {
            fragments.emplace_back(_id_counter, fid, num_fragments, std::string(str_data, f_size));
            str_data += f_size;
        }

        size_t size_last_fragment = f_size + remainder;
        fragments.emplace_back(_id_counter, num_fragments - 1, num_fragments,
                               std::string(str_data, size_last_fragment));

        ++_id_counter;
    }

    return fragments;
}
