// Author: Derek Barnett

#ifndef PBCOPPER_ALIGN_SEEDS_INL_H
#define PBCOPPER_ALIGN_SEEDS_INL_H

#include <pbcopper/align/Seeds.h>

#include <cassert>
#include <type_traits>

namespace PacBio {
namespace Align {
namespace internal {

inline bool CanMergeSeeds(const Seed& lhs, const Seed& rhs)
{
    // rhs must be to right of lhs AND seeds must overlap
    const auto ok = rhs.BeginPositionH() >= lhs.BeginPositionH() &&
                    rhs.BeginPositionV() >= lhs.BeginPositionV() &&
                    rhs.BeginPositionH() <= lhs.EndPositionH() &&
                    rhs.BeginPositionV() <= lhs.EndPositionV();
    return ok;
}

}  // namespace internal

static_assert(std::is_copy_constructible<Seeds>::value, "Seeds(const Seeds&) is not = default");
static_assert(std::is_copy_assignable<Seeds>::value,
              "Seeds& operator=(const Seeds&) is not = default");

static_assert(std::is_nothrow_move_constructible<Seeds>::value, "Seeds(Seeds&&) is not = noexcept");
static_assert(std::is_nothrow_move_assignable<Seeds>::value,
              "Seeds& operator=(Seeds&&) is not = noexcept");

inline bool Seeds::AddSeed(const Seed& s)
{
    data_.insert(s);
    return true;
}

inline bool Seeds::TryMergeSeed(const Seed& s)
{
    Seed left;
    for (auto it = data_.begin(); it != data_.end(); ++it) {
        if (internal::CanMergeSeeds(*it, s)) {
            left = *it;
            left += s;
            data_.erase(it);
            data_.insert(left);
            return true;
        } else if (internal::CanMergeSeeds(s, *it)) {
            left = s;
            left += *it;
            data_.erase(it);
            data_.insert(left);
            return true;
        }
    }
    return false;
}

}  // namespace Align
}  // namespace PacBio

#endif  // PBCOPPER_ALIGN_SEEDS_INL_H
