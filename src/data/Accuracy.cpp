// Author: Derek Barnett

#include <pbcopper/data/Accuracy.h>

#include <cassert>
#include <type_traits>

#include <boost/algorithm/clamp.hpp>

namespace PacBio {
namespace Data {

const float Accuracy::MIN = 0.0f;
const float Accuracy::MAX = 1.0f;

static_assert(std::is_copy_constructible<Accuracy>::value,
              "Accuracy(const Accuracy&) is not = default");
static_assert(std::is_copy_assignable<Accuracy>::value,
              "Accuracy& operator=(const Accuracy&) is not = default");

static_assert(std::is_nothrow_move_constructible<Accuracy>::value,
              "Accuracy(Accuracy&&) is not = noexcept");
static_assert(std::is_nothrow_move_assignable<Accuracy>::value,
              "Accuracy& operator=(Accuracy&&) is not = noexcept");

Accuracy::Accuracy(float accuracy)
    : accuracy_{boost::algorithm::clamp(accuracy, Accuracy::MIN, Accuracy::MAX)}
{
}

Accuracy::operator float() const noexcept { return accuracy_; }

}  // namespace Data
}  // namespace PacBio
