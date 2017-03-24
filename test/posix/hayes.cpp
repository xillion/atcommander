#include "catch.hpp"
#include "atcommander.h"

#include "hayes.h"

TEST_CASE( "Simple hayes commands", "[hayes]" )
{
    ATCommander atc(fstd::cin, fstd::cout);

    GIVEN("Info request 1")
    {
        char infobuffer[128];

        hayes::standard_at::information(atc, 0, infobuffer, 128);
    }
    GIVEN("Info request 2")
    {
        hayes::standard_at::information::command2::request(atc, 0);
    }
}
