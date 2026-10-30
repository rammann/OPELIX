#include <gtest/gtest.h>

#include <memory>
#include <sstream>
#include <string>

#include "AbsBeamline/Drift.h"
#include "AbsBeamline/Marker.h"
#include "AbsBeamline/SBend.h"
#include "Beamlines/Beamline.h"
#include "Lines/Ring.h"
#include "OpalParser/SimpleStatement.h"
#include "OpalParser/Token.h"
#include "Utilities/ParseError.h"

namespace {
    constexpr char testFile[] = "TestRing.in";

    Token delimiter(char value) { return Token(testFile, 1, Token::IS_DELIMITER, value); }

    Token word(const char* value) { return Token(testFile, 1, Token::IS_WORD, value); }

    SimpleStatement statement(Statement::TokenList& tokens) {
        return SimpleStatement(testFile, tokens);
    }

    class TestableRing : public Ring {
    public:
        using Ring::Ring;

        Beamline* line() { return fetchLine(); }
    };
}  // namespace

class RingTest : public ::testing::Test {
protected:
    Ring* makeRing(const std::string& name) { return new Ring(name); }
};

TEST_F(RingTest, PrintsRingDefinition) {
    TestableRing ring("TEST_RING");

    std::ostringstream output;
    ring.print(output);
    EXPECT_EQ(output.str(), "TEST_RING:RING=();\n");
    EXPECT_EQ(ring.line()->getBeamlineTopology(), BeamlineTopology::RING);
    EXPECT_EQ(ring.line()->getName(), "TEST_RING");
}

TEST(RingMembershipTest, DefaultsValidatesAndSurvivesClone) {
    Drift drift("D");
    EXPECT_EQ(drift.getBeamlineTopology(), BeamlineTopology::LINE);
    EXPECT_TRUE(drift.getBeamlineOwnerName().empty());

    EXPECT_THROW(drift.setBeamlineMembership(BeamlineTopology::RING), GeneralOpalException);
    EXPECT_THROW(
            drift.setBeamlineMembership(BeamlineTopology::LINE, "NOT_ALLOWED"),
            GeneralOpalException);

    drift.setBeamlineMembership(BeamlineTopology::RING, "R1");
    drift.addLinearTransferMap(LinearTransferMap{});
    drift.setOverlapping(true);
    std::unique_ptr<ElementBase> clone(drift.clone());
    EXPECT_EQ(clone->getBeamlineTopology(), BeamlineTopology::RING);
    EXPECT_EQ(clone->getBeamlineOwnerName(), "R1");
    EXPECT_TRUE(drift.hasLinearTransferMaps());
    EXPECT_FALSE(clone->hasLinearTransferMaps());
    EXPECT_TRUE(drift.isOverlapping());
    EXPECT_FALSE(clone->isOverlapping());

    drift.clearBeamlineMembership();
    EXPECT_EQ(drift.getBeamlineTopology(), BeamlineTopology::LINE);
    EXPECT_TRUE(drift.getBeamlineOwnerName().empty());
}

TEST(RingMembershipTest, ClonesAndTagsOccurrences) {
    auto prototype = std::make_shared<Drift>("D");
    auto marker    = std::make_shared<Marker>("#S");

    TestableRing ring("RING");
    ring.line()->push_back(marker);
    ring.line()->push_back(prototype);
    ring.line()->push_back(prototype);
    ring.prepareForTracking();

    auto member = ring.line()->begin();
    EXPECT_EQ(*member, marker);
    EXPECT_EQ((*member)->getBeamlineTopology(), BeamlineTopology::LINE);

    const auto first  = *(++member);
    const auto second = *(++member);
    EXPECT_NE(first, prototype);
    EXPECT_NE(second, prototype);
    EXPECT_NE(first, second);
    EXPECT_EQ(first->getBeamlineTopology(), BeamlineTopology::RING);
    EXPECT_EQ(first->getBeamlineOwnerName(), "RING");
    EXPECT_EQ(second->getBeamlineOwnerName(), "RING");
    EXPECT_EQ(prototype->getBeamlineTopology(), BeamlineTopology::LINE);
}

TEST(RingMembershipTest, KeepsSharedPrototypeIndependentAcrossRings) {
    auto prototype = std::make_shared<Drift>("SHARED");
    TestableRing first("R1");
    TestableRing second("R2");
    first.line()->push_back(prototype);
    second.line()->push_back(prototype);

    first.prepareForTracking();
    second.prepareForTracking();

    const auto firstOccurrence  = first.line()->front();
    const auto secondOccurrence = second.line()->front();
    EXPECT_NE(firstOccurrence, secondOccurrence);
    EXPECT_NE(firstOccurrence, prototype);
    EXPECT_NE(secondOccurrence, prototype);
    EXPECT_EQ(firstOccurrence->getBeamlineOwnerName(), "R1");
    EXPECT_EQ(secondOccurrence->getBeamlineOwnerName(), "R2");
    EXPECT_EQ(prototype->getBeamlineTopology(), BeamlineTopology::LINE);
    EXPECT_TRUE(prototype->getBeamlineOwnerName().empty());
}

TEST(RingMembershipTest, CircumferenceCountsNominalArcsAndDriftsNotFringeSupports) {
    auto bend = std::make_shared<SBend>("B");
    bend->getGeometry().setElementLength(1.0);
    bend->getGeometry().setCurvature(0.5);
    auto drift = std::make_shared<Drift>("D");
    drift->getGeometry().setElementLength(0.4);
    // Two uses of a nested LINE (bend, drift) are copied in as their elements.
    TestableRing ring("RING");
    for (int cell = 0; cell < 2; ++cell) {
        ring.line()->push_back(bend);
        ring.line()->push_back(drift);
    }
    for (const double gap : {0.0, 0.1, 0.2}) {
        bend->setFullGap(gap);
        bend->setFringeIntegral(0.1);
        EXPECT_DOUBLE_EQ(ring.getLength(), 2.8);
        double begin = 0.0, end = 0.0;
        bend->getFieldExtent(begin, end);
        EXPECT_DOUBLE_EQ(end - begin, 1.0 + 10.0 * gap);
        EXPECT_TRUE(bend->isInsideBody(Vector_t<double, 3>(0, 0, 0.1)));
        EXPECT_FALSE(bend->isInsideBody(Vector_t<double, 3>(0, 0, -0.01)));
        EXPECT_EQ(bend->isInside(Vector_t<double, 3>(0, 0, -0.01)), gap > 0.0);
    }
    // Rectangular-bend geometry stores a chord, not a design arc.
    drift->getGeometry() = Geometry::makeRBend(0.4, 0.6);
    EXPECT_NEAR(ring.getLength(), 2.0 * (1.0 + 0.4 * 0.3 / std::sin(0.3)), 1.e-14);
}

TEST_F(RingTest, RejectsReflection) {
    Statement::TokenList tokens{
            delimiter('='), delimiter('('), delimiter('-'), word("DRIFT"), delimiter(')')};
    SimpleStatement input = statement(tokens);
    std::unique_ptr<Ring> ring(makeRing("REFLECTED_RING"));

    EXPECT_THROW(ring->parse(input), ParseError);
}

TEST_F(RingTest, RejectsRepetition) {
    Statement::TokenList tokens{delimiter('='), delimiter('('), Token(testFile, 1, "2", 2),
                                delimiter('*'), word("DRIFT"),  delimiter(')')};
    SimpleStatement input = statement(tokens);
    std::unique_ptr<Ring> ring(makeRing("REPEATED_RING"));

    EXPECT_THROW(ring->parse(input), ParseError);
}
