#ifndef LED_PATTERN_H
#define LED_PATTERN_H

#include "bsp.h"
#include <stdint.h>
#include "fw_log.h"
#include "fw_assert.h"

#define LED_PATTERN_ASSERT(t_) ((t_)? (void)0: Q_onAssert("LedPattern.h", (int32_t)__LINE__))

namespace APP {

class LedInterval {
public:
    uint16_t m_levelPermil;     // Brightness level 0-1000.
    uint16_t m_durationMs;      // Duration in millisecond.
    
    uint16_t GetLevelPermil() const { return m_levelPermil; }
    uint16_t GetDurationMs() const { return m_durationMs; }
};

class LedPattern {
public:
    enum {
        COUNT = 256
    };
    uint32_t m_count;                // Number of intervals in use.
    LedInterval m_interval[COUNT];   // Array of intervals. Used ones start from index 0.
    
    // UW 2019
    /*
    uint32_t GetCount() const { 
        return ...
    }

    // Returns a reference to the indexed LED interval object.
    // Use FW_ASSERT() to ensure index is in range.
    LedInterval const &GetInterval(uint32_t index) const {
        return ...
    }
    */
};

class LedPatternSet {
public:
    enum {
        COUNT = 4
    };
    uint32_t m_count;               // Number of patterns in use.
    LedPattern m_pattern[COUNT];    // Array of patterns. Used ones start from index 0.
    
    // UW 2019
    /*
    uint32_t GetCount() const {
        return ...
    }

    // Returns NULL if index is out-of-range. Otherwise returns a pointer to the indexed LED pattern object.
    LedPattern const *GetPattern(uint32_t index) const {
        ...
        return ...
    }
    */
};

// Declaration of an instance of LedPatternSet defined in LedPattern.cpp.
extern LedPatternSet const TEST_LED_PATTERN_SET;

} // namespace APP

#endif // LED_PATTERN_H
