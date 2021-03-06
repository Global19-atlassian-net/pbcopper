// Author: Brett Bowman

#ifndef PBCOPPER_ALIGN_CHAINSEEDSCONFIG_H
#define PBCOPPER_ALIGN_CHAINSEEDSCONFIG_H

#include <cstddef>

namespace PacBio {
namespace Align {

/// A simple struct for represting a complete specialization for an
///  implementation of the Baker-Giancarlo SDP algorithm.  Templating
///  around this struct allows us to greatly reduce the number of
///  parameters that need to be thrown around.
///
struct ChainSeedsConfig
{
    explicit ChainSeedsConfig(const size_t numCandidatesArg);

    ChainSeedsConfig(const size_t numCandidatesArg, const long minScoreArg, const int matchScoreArg,
                     const int nonMatchPenaltyArg, const int insertionPenaltyArg,
                     const int deletionPenaltyArg, const int maxSeedGapArg);

    ChainSeedsConfig() = default;

    size_t numCandidates = 10;
    long minScore = 18;
    int matchScore = 5;
    int nonMatchPenalty = 0;
    int insertionPenalty = -4;
    int deletionPenalty = -8;
    int maxSeedGap = 200;
};
}  // namespace Align
}  // namespace PacBio

#endif  // PBCOPPER_ALIGN_CHAINSEEDSCONFIG_H
