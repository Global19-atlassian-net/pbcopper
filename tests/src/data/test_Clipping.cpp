// Author: Derek Barnett

#include <algorithm>

#include <gtest/gtest.h>

#include <pbcopper/data/Clipping.h>
#include <pbcopper/data/MappedRead.h>
#include <pbcopper/data/Read.h>
#include <pbcopper/data/internal/ClippingImpl.h>

using namespace PacBio::Data;

TEST(Data_Read, ClipRead)
{
    const std::string seq = "AACCGTTAGC";
    const QualityValues quals = QualityValues::FromFastq("0123456789");
    const SNR snr{0.9, 0.9, 0.9, 0.9};
    const Position qStart = 500;
    const Position qEnd = 510;
    const std::vector<uint16_t> pw{10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    const std::vector<uint16_t> ipd{10, 20, 30, 40, 50, 60, 70, 80, 90, 100};

    // ClipToQuery(read, 502, 509);
    const size_t clipStart = 502;
    const size_t clipEnd = 509;
    const ClipResult clipResult{2, 502, 509};

    Read read{"name", seq, quals, snr, qStart, qEnd, pw, ipd};
    internal::ClipRead(read, clipResult, clipStart, clipEnd);

    const std::string expectedSeq = "CCGTTAG";
    const QualityValues expectedQuals = QualityValues::FromFastq("2345678");
    const SNR expectedSnr{0.9, 0.9, 0.9, 0.9};
    const Position expectedQStart = 502;
    const Position expectedQEnd = 509;
    const std::vector<uint8_t> expectedPw{30, 40, 50, 60, 67, 72, 77};
    const std::vector<uint16_t> expectedIpd{30, 40, 50, 60, 70, 80, 90};

    EXPECT_EQ("name", read.Id.MovieName);
    EXPECT_EQ(expectedSeq, read.Seq);
    EXPECT_EQ(expectedQuals, read.Qualities);
    EXPECT_EQ(expectedQStart, read.QueryStart);
    EXPECT_EQ(expectedQEnd, read.QueryEnd);
    EXPECT_EQ(expectedSnr, read.SignalToNoise);
    EXPECT_TRUE(read.PulseWidth.size());
    EXPECT_TRUE(std::equal(expectedPw.cbegin(), expectedPw.cend(), read.PulseWidth.begin()));
    EXPECT_TRUE(read.IPD);
    EXPECT_TRUE(std::equal(expectedIpd.cbegin(), expectedIpd.cend(), read.IPD->begin()));
}

TEST(Data_Read, ClipMappedRead)
{
    const std::string seq = "AACCGTTAGC";
    const QualityValues quals = QualityValues::FromFastq("0123456789");
    const SNR snr{0.9, 0.9, 0.9, 0.9};
    const Position qStart = 500;
    const Position qEnd = 510;
    const std::vector<uint16_t> pw{10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    const std::vector<uint16_t> ipd{10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    const Strand strand = Strand::FORWARD;
    const Position tStart = 100;
    const Position tEnd = 111;
    const Cigar cigar{"4=1D2I2D4="};
    const uint8_t mapQV = 80;

    // ClipToReference(read, 102, 107);
    const ClipResult clipResult{2, 502, 507, 102, Cigar{"2=1D2I2D"}};

    MappedRead read{
        Read{"name", seq, quals, snr, qStart, qEnd, pw, ipd}, strand, tStart, tEnd, cigar, mapQV};
    internal::ClipMappedRead(read, clipResult);

    const std::string expectedSeq = "CCGTT";
    const QualityValues expectedQuals = QualityValues::FromFastq("23456");
    const SNR expectedSnr{0.9, 0.9, 0.9, 0.9};
    const Position expectedQStart = 502;
    const Position expectedQEnd = 507;
    const std::vector<uint8_t> expectedPw{30, 40, 50, 60, 67};
    const std::vector<uint16_t> expectedIpd{30, 40, 50, 60, 70};
    const Strand expectedStrand = Strand::FORWARD;
    const Position expectedTStart = 102;
    const Position expectedTEnd = 107;
    const Cigar expectedCigar{"2=1D2I2D"};
    const uint8_t expectedMapQV = 80;

    EXPECT_EQ("name", read.Id.MovieName);
    EXPECT_EQ(expectedSeq, read.Seq);
    EXPECT_EQ(expectedQuals, read.Qualities);
    EXPECT_EQ(expectedQStart, read.QueryStart);
    EXPECT_EQ(expectedQEnd, read.QueryEnd);
    EXPECT_EQ(expectedSnr, read.SignalToNoise);
    EXPECT_TRUE(std::equal(expectedPw.cbegin(), expectedPw.cend(), read.PulseWidth.begin()));
    EXPECT_TRUE(std::equal(expectedIpd.cbegin(), expectedIpd.cend(), read.IPD->begin()));
    EXPECT_EQ(expectedStrand, read.Strand);
    EXPECT_EQ(expectedTStart, read.TemplateStart);
    EXPECT_EQ(expectedTEnd, read.TemplateEnd);
    EXPECT_EQ(expectedCigar, read.Cigar);
    EXPECT_EQ(expectedMapQV, read.MapQuality);
}

TEST(Data_Read, ClipToReferenceOutsideAlignedRegion)
{
    const std::string seq{"GATTACA"};
    const QualityValues quals{"ZZZZZZZ"};
    const SNR snr{0.9, 0.9, 0.9, 0.9};
    const Position qStart = 500;
    const Position qEnd = 507;
    const std::vector<uint16_t> pw{10, 20, 30, 40, 50, 60, 70};
    const std::vector<uint16_t> ipd{10, 20, 30, 40, 50, 60, 70};
    const Strand strand = Strand::FORWARD;
    const Position tStart = 200;
    const Position tEnd = 207;
    const Cigar cigar{"7="};
    const uint8_t mapQV = 99;

    const std::string expectedSeq{""};
    const QualityValues expectedQuals{""};
    const SNR expectedSnr{0.9, 0.9, 0.9, 0.9};
    const Position expectedQStart = -1;
    const Position expectedQEnd = -1;
    const std::vector<uint8_t> expectedPw{};
    const std::vector<uint16_t> expectedIpd{};
    const Strand expectedStrand = Strand::FORWARD;
    const Position expectedTStart = -1;
    const Position expectedTEnd = -1;
    const Cigar expectedCigar{""};
    const uint8_t expectedMapQV = 255;

    auto shouldClipToEmptyRead = [&](Position start, Position end) {
        MappedRead read{Read{"name", seq, quals, snr, qStart, qEnd, pw, ipd},
                        strand,
                        tStart,
                        tEnd,
                        cigar,
                        mapQV};

        ClipToReference(read, start, end, true);

        EXPECT_EQ("name", read.Id.MovieName);
        EXPECT_EQ(expectedSeq, read.Seq);
        EXPECT_EQ(expectedQuals, read.Qualities);
        EXPECT_EQ(expectedQStart, read.QueryStart);
        EXPECT_EQ(expectedQEnd, read.QueryEnd);
        EXPECT_EQ(expectedSnr, read.SignalToNoise);
        EXPECT_TRUE(std::equal(expectedPw.cbegin(), expectedPw.cend(), read.PulseWidth.begin()));
        EXPECT_TRUE(std::equal(expectedIpd.cbegin(), expectedIpd.cend(), read.IPD->begin()));
        EXPECT_EQ(expectedStrand, read.Strand);
        EXPECT_EQ(expectedTStart, read.TemplateStart);
        EXPECT_EQ(expectedTEnd, read.TemplateEnd);
        EXPECT_EQ(expectedCigar, read.Cigar);
        EXPECT_EQ(expectedMapQV, read.MapQuality);
    };

    {
        SCOPED_TRACE("clip region is well before alignment start");
        shouldClipToEmptyRead(0, 50);
    }
    {
        SCOPED_TRACE("clip region ends at alignment start");
        shouldClipToEmptyRead(150, 200);
    }
    {
        SCOPED_TRACE("clip region starts at alignment end");
        shouldClipToEmptyRead(207, 250);
    }
    {
        SCOPED_TRACE("clip region starts well after alignment end");
        shouldClipToEmptyRead(500, 600);
    }
}

TEST(Data_Read, MultipleClipsToReference)
{
    const std::string seq(1200, 'A');
    const QualityValues quals{std::string(1200, 'Z')};
    const SNR snr{0.9, 0.9, 0.9, 0.9};
    const Position qStart = 0;
    const Position qEnd = 1200;
    const std::vector<uint16_t> pw(1200, 20);
    const std::vector<uint16_t> ipd(1200, 20);
    const Strand strand = Strand::FORWARD;
    const Position tStart = 0;
    const Position tEnd = 1200;
    const Cigar cigar{"1200="};
    const uint8_t mapQV = 99;

    // intial read, aligned to reference: [0, 1200)
    MappedRead read{
        Read{"name", seq, quals, snr, qStart, qEnd, pw, ipd}, strand, tStart, tEnd, cigar, mapQV};

    // clip to reference: [0, 1000) - shrinking from right
    ClipToReference(read, 0, 1000, true);

    std::string expectedSeq(1000, 'A');
    QualityValues expectedQuals{std::string(1000, 'Z')};
    const SNR expectedSnr{0.9, 0.9, 0.9, 0.9};
    Position expectedQStart = 0;
    Position expectedQEnd = 1000;
    std::vector<uint16_t> expectedPw(1000, 20);
    std::vector<uint16_t> expectedIpd(1000, 20);
    const Strand expectedStrand = Strand::FORWARD;
    Position expectedTStart = 0;
    Position expectedTEnd = 1000;
    Cigar expectedCigar{"1000="};
    const uint8_t expectedMapQV = 99;

    EXPECT_EQ("name", read.Id.MovieName);
    EXPECT_EQ(expectedSeq, read.Seq);
    EXPECT_EQ(expectedQuals, read.Qualities);
    EXPECT_EQ(expectedQStart, read.QueryStart);
    EXPECT_EQ(expectedQEnd, read.QueryEnd);
    EXPECT_EQ(expectedSnr, read.SignalToNoise);
    EXPECT_TRUE(std::equal(expectedPw.cbegin(), expectedPw.cend(), read.PulseWidth.begin()));
    EXPECT_TRUE(std::equal(expectedIpd.cbegin(), expectedIpd.cend(), read.IPD->begin()));
    EXPECT_EQ(expectedStrand, read.Strand);
    EXPECT_EQ(expectedTStart, read.TemplateStart);
    EXPECT_EQ(expectedTEnd, read.TemplateEnd);
    EXPECT_EQ(expectedCigar, read.Cigar);
    EXPECT_EQ(expectedMapQV, read.MapQuality);

    // clip again, to reference: [100, 1000) - shrinking from left
    ClipToReference(read, 100, 1000, true);

    expectedSeq = std::string(900, 'A');
    expectedQuals = QualityValues{std::string(900, 'Z')};
    expectedQStart = 100;
    expectedQEnd = 1000;
    expectedPw = std::vector<uint16_t>(900, 20);
    expectedIpd = std::vector<uint16_t>(900, 20);
    expectedTStart = 100;
    expectedTEnd = 1000;
    expectedCigar = Cigar{"900="};

    EXPECT_EQ("name", read.Id.MovieName);
    EXPECT_EQ(expectedSeq, read.Seq);
    EXPECT_EQ(expectedQuals, read.Qualities);
    EXPECT_EQ(expectedQStart, read.QueryStart);
    EXPECT_EQ(expectedQEnd, read.QueryEnd);
    EXPECT_EQ(expectedSnr, read.SignalToNoise);
    EXPECT_TRUE(std::equal(expectedPw.cbegin(), expectedPw.cend(), read.PulseWidth.begin()));
    EXPECT_TRUE(std::equal(expectedIpd.cbegin(), expectedIpd.cend(), read.IPD->begin()));
    EXPECT_EQ(expectedStrand, read.Strand);
    EXPECT_EQ(expectedTStart, read.TemplateStart);
    EXPECT_EQ(expectedTEnd, read.TemplateEnd);
    EXPECT_EQ(expectedCigar, read.Cigar);
    EXPECT_EQ(expectedMapQV, read.MapQuality);

    // clip again, to reference: [200, 800) - shrinking both sides
    ClipToReference(read, 200, 800, true);

    expectedSeq = std::string(600, 'A');
    expectedQuals = QualityValues{std::string(600, 'Z')};
    expectedQStart = 200;
    expectedQEnd = 800;
    expectedPw = std::vector<uint16_t>(600, 20);
    expectedIpd = std::vector<uint16_t>(600, 20);
    expectedTStart = 200;
    expectedTEnd = 800;
    expectedCigar = Cigar{"600="};

    EXPECT_EQ("name", read.Id.MovieName);
    EXPECT_EQ(expectedSeq, read.Seq);
    EXPECT_EQ(expectedQuals, read.Qualities);
    EXPECT_EQ(expectedQStart, read.QueryStart);
    EXPECT_EQ(expectedQEnd, read.QueryEnd);
    EXPECT_EQ(expectedSnr, read.SignalToNoise);
    EXPECT_TRUE(std::equal(expectedPw.cbegin(), expectedPw.cend(), read.PulseWidth.begin()));
    EXPECT_TRUE(std::equal(expectedIpd.cbegin(), expectedIpd.cend(), read.IPD->begin()));
    EXPECT_EQ(expectedStrand, read.Strand);
    EXPECT_EQ(expectedTStart, read.TemplateStart);
    EXPECT_EQ(expectedTEnd, read.TemplateEnd);
    EXPECT_EQ(expectedCigar, read.Cigar);
    EXPECT_EQ(expectedMapQV, read.MapQuality);
}

TEST(Data_Read, MultipleClipsToReference_WithLargeDeletion)
{
    const std::string seq(1000, 'A');
    const QualityValues quals{std::string(1000, 'Z')};
    const SNR snr{0.9, 0.9, 0.9, 0.9};
    const Position qStart = 0;
    const Position qEnd = 1000;
    const std::vector<uint16_t> pw(1200, 20);
    const std::vector<uint16_t> ipd(1200, 20);
    const Strand strand = Strand::FORWARD;
    const Position tStart = 0;
    const Position tEnd = 1200;
    const Cigar cigar{"400=200D600="};
    const uint8_t mapQV = 99;

    // intial read, aligned to reference: [0, 1200)
    MappedRead read{
        Read{"name", seq, quals, snr, qStart, qEnd, pw, ipd}, strand, tStart, tEnd, cigar, mapQV};

    // clip to reference: [0, 1000) - shrinking from right
    ClipToReference(read, 0, 1000, true);

    std::string expectedSeq(800, 'A');
    QualityValues expectedQuals{std::string(800, 'Z')};
    const SNR expectedSnr{0.9, 0.9, 0.9, 0.9};
    Position expectedQStart = 0;
    Position expectedQEnd = 800;
    std::vector<uint16_t> expectedPw(800, 20);
    std::vector<uint16_t> expectedIpd(800, 20);
    const Strand expectedStrand = Strand::FORWARD;
    Position expectedTStart = 0;
    Position expectedTEnd = 1000;
    Cigar expectedCigar{"400=200D400="};
    const uint8_t expectedMapQV = 99;

    EXPECT_EQ("name", read.Id.MovieName);
    EXPECT_EQ(expectedSeq, read.Seq);
    EXPECT_EQ(expectedQuals, read.Qualities);
    EXPECT_EQ(expectedQStart, read.QueryStart);
    EXPECT_EQ(expectedQEnd, read.QueryEnd);
    EXPECT_EQ(expectedSnr, read.SignalToNoise);
    EXPECT_TRUE(std::equal(expectedPw.cbegin(), expectedPw.cend(), read.PulseWidth.begin()));
    EXPECT_TRUE(std::equal(expectedIpd.cbegin(), expectedIpd.cend(), read.IPD->begin()));
    EXPECT_EQ(expectedStrand, read.Strand);
    EXPECT_EQ(expectedTStart, read.TemplateStart);
    EXPECT_EQ(expectedTEnd, read.TemplateEnd);
    EXPECT_EQ(expectedCigar, read.Cigar);
    EXPECT_EQ(expectedMapQV, read.MapQuality);

    // clip again, to reference: [100, 1000) - shrinking from left
    ClipToReference(read, 100, 1000, true);

    expectedSeq = std::string(700, 'A');
    expectedQuals = QualityValues{std::string(700, 'Z')};
    expectedQStart = 100;
    expectedQEnd = 800;
    expectedPw = std::vector<uint16_t>(700, 20);
    expectedIpd = std::vector<uint16_t>(700, 20);
    expectedTStart = 100;
    expectedTEnd = 1000;
    expectedCigar = Cigar{"300=200D400="};

    EXPECT_EQ("name", read.Id.MovieName);
    EXPECT_EQ(expectedSeq, read.Seq);
    EXPECT_EQ(expectedQuals, read.Qualities);
    EXPECT_EQ(expectedQStart, read.QueryStart);
    EXPECT_EQ(expectedQEnd, read.QueryEnd);
    EXPECT_EQ(expectedSnr, read.SignalToNoise);
    EXPECT_TRUE(std::equal(expectedPw.cbegin(), expectedPw.cend(), read.PulseWidth.begin()));
    EXPECT_TRUE(std::equal(expectedIpd.cbegin(), expectedIpd.cend(), read.IPD->begin()));
    EXPECT_EQ(expectedStrand, read.Strand);
    EXPECT_EQ(expectedTStart, read.TemplateStart);
    EXPECT_EQ(expectedTEnd, read.TemplateEnd);
    EXPECT_EQ(expectedCigar, read.Cigar);
    EXPECT_EQ(expectedMapQV, read.MapQuality);

    // clip again, to reference: [200, 800) - shrinking both sides
    ClipToReference(read, 200, 800, true);

    expectedSeq = std::string(400, 'A');
    expectedQuals = QualityValues{std::string(400, 'Z')};
    expectedQStart = 200;
    expectedQEnd = 600;
    expectedPw = std::vector<uint16_t>(400, 20);
    expectedIpd = std::vector<uint16_t>(400, 20);
    expectedTStart = 200;
    expectedTEnd = 800;
    expectedCigar = Cigar{"200=200D200="};

    EXPECT_EQ("name", read.Id.MovieName);
    EXPECT_EQ(expectedSeq, read.Seq);
    EXPECT_EQ(expectedQuals, read.Qualities);
    EXPECT_EQ(expectedQStart, read.QueryStart);
    EXPECT_EQ(expectedQEnd, read.QueryEnd);
    EXPECT_EQ(expectedSnr, read.SignalToNoise);
    EXPECT_TRUE(std::equal(expectedPw.cbegin(), expectedPw.cend(), read.PulseWidth.begin()));
    EXPECT_TRUE(std::equal(expectedIpd.cbegin(), expectedIpd.cend(), read.IPD->begin()));
    EXPECT_EQ(expectedStrand, read.Strand);
    EXPECT_EQ(expectedTStart, read.TemplateStart);
    EXPECT_EQ(expectedTEnd, read.TemplateEnd);
    EXPECT_EQ(expectedCigar, read.Cigar);
    EXPECT_EQ(expectedMapQV, read.MapQuality);
}
