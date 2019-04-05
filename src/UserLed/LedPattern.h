#ifndef LED_PATTERN_H
#define LED_PATTERN_H

#include "bsp.h"
#include <stdint.h>
#include "fw_log.h"
#include "qassert.h"

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
    
    /*
    uint32_t GetCount() const { 
        return ...
    }

    // Must perform range check. Assert if invalid.
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
    
    /*
    uint32_t GetCount() const {
        return ...
    }

    // Must perform range check. Return NULL if invalid.
    LedPattern const *GetPattern(uint32_t index) const {
        ...
        return ...
    }
    */
};

extern LedPatternSet const TEST_LED_PATTERN_SET;

} // namespace APP

#endif // LED_PATTERN_H
