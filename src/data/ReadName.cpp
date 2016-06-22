#include "pbcopper/data/ReadName.h"
using namespace PacBio;
using namespace PacBio::Data;
using namespace std;

bool ReadName::operator==(const ReadName& other) const
{
    // simple int check first
    if (zmw_ != other.zmw_)
        return false;

    // not equal if one is CCS and one is not
    if (IsCCS() != other.IsCCS())
        return false;
    else {
        // reads share same CCS status, check intervals if both non-CCS
        if (!IsCCS()) {
            assert(queryInterval_);
            assert(other.queryInterval_);
            if (*queryInterval_ != *other.queryInterval_)
                return false;
        }
    }

    // else compare on movie
    return movieName_ == other.movieName_;
}

bool ReadName::operator<(const ReadName& other) const
{
    // sort by:
    //   1 - movie name
    //   2 - zmw hole number
    //   3 - query_interval (CCS to end)

    if (movieName_ != other.movieName_)
        return movieName_ < other.movieName_;
    if (zmw_ != other.zmw_)
        return zmw_ < other.zmw_;

    // if this read name is CCS, then we're either never less-than a subread
    // with query interval... or the other read is the same read name, so not
    // "less than" either
    if (IsCCS())
        return false;

    // this read is non-CCS. so if other is CCS, then we are always "less-than"
    else if (other.IsCCS())
        return true;

    // if here, both non-CCS, compare on query interval
    assert(!IsCCS());
    assert(!other.IsCCS());
    const auto thisInterval  = *queryInterval_;
    const auto otherInterval = *other.queryInterval_;
    if (thisInterval.Start() == otherInterval.Start())
        return thisInterval.End() < otherInterval.End();
    return thisInterval.End() < otherInterval.End();
}

void ReadName::Check(void) const
{
    if (movieName_.ToStdString().empty())
        throw std::runtime_error("ReadName: movie name must not be empty");

    if (zmw_ < 0)
        throw std::runtime_error("ReadName: ZMW hole number must be a positive integer");

    if (!IsCCS()) {
        if (QueryStart() < 0)
            throw std::runtime_error("ReadName: query start must be a positive integer");
        if (QueryEnd() < 0)
            throw std::runtime_error("ReadName: query end must be a positive integer");
    }
}

void ReadName::FromString(std::string&& name)
{
    // ensure clean slate
    movieName_ = PacBio::Data::MovieName{ };
    zmw_ = -1;
    queryInterval_.reset(nullptr);

    // parse name parts
    if (name.empty())
        return;
    auto parts = PacBio::Utility::Split(name, '/');
    if (parts.size() != 3)
        return;

    // rip out & store name parts
    movieName_ = PacBio::Data::MovieName{ std::move(parts.at(0)) };
    zmw_       = std::stoi(parts.at(1));
    if (parts.at(2) != "ccs") {
        auto queryParts = PacBio::Utility::Split(parts.at(2), '_');
        if (queryParts.size() != 2)
            return;
        const Position queryStart = std::stoi(queryParts.at(0));
        const Position queryEnd   = std::stoi(queryParts.at(1));
        queryInterval_.reset(new Interval<Position>{ queryStart, queryEnd });
    }
}

string ReadName::ToString(void) const
{
    // ensure we're OK
    Check();

    // construct name from parts
    auto result = string{ };
    result.reserve(128);
    result += movieName_.ToStdString();
    result += "/";
    result += std::to_string(zmw_);
    result += "/";

    if (IsCCS()) {
        result += "ccs";
    } else {
        result += std::to_string(queryInterval_->Start());
        result += "_";
        result += std::to_string(queryInterval_->End());
    }

    return result;
}

