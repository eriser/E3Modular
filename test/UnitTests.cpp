
#include <gtest/gtest.h>

#include "LibCommon_AllTests.inc"
#include "Core_AllTests.inc"
#include "Gui_AllTests.inc"


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



