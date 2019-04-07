#include "LedPattern.h"

namespace APP {

LedPatternSet const TEST_LED_PATTERN_SET = {
    // UW 2019 - Change the number of patterns to 4 after appending two of yours below.
	2,	// Number of patterns.
    {
        // Pattern 0
        {21, 
            {
                {10,50}, {20,50}, {30,50}, {50,50}, {70,50}, {100,50}, {200,50}, {400,50}, {500,50}, {900,50},  // ramp up
                {900, 2000},                                                                                    // constant high
                {900,50}, {500,50}, {400,50}, {200,50}, {100,50}, {70,50}, {50,50}, {30,50}, {20,50}, {10,50},  // ramp down          
            }
        },
        // Pattern 1
        {4, 
            {
                {500,200}, {0, 200}, {500, 200}, {0, 1000}      // two short blinks.
            }
        }
        // UW 2019 - Add two of your own patterns here.
        // Pattern 2 ...
        // Pattern 3 ...
    }
};

} // namespace APP
