#include <STM32Encoder.h>

class EncoderHandler {
    int32_t last_enc_value[4] = {0, 0, 0, 0};  // previous encoder values
    float velocity[4] = {0, 0, 0, 0};  // velocity of each motor
    STM32Encoder encoders[4];

public:
    EncoderHandler(uint8_t a1, uint8_t b1, uint8_t a2, uint8_t b2, uint8_t a3, uint8_t b3, uint8_t a4, uint8_t b4) {
        // Initialize encoders
        encoders[0].attach(a1, b1);
        encoders[1].attach(a2, b2);
        encoders[2].attach(a3, b3);
        encoders[3].attach(a4, b4);
    }

    int32_t getCount(int motor) {
        // Get current encoder count
        int32_t val = encoders[motor - 1].getCount();
        // You might need to adjust the logic for motor reversal if needed
        return val;
    }

    // Update encoder velocities (call this method frequently)
    void updateVelocities() {
        int32_t curr_enc_value;
        for(int i = 0; i < 4; i++) {
            curr_enc_value = getCount(i + 1);
            // Calculate velocity logic here, similar to the original code
            velocity[i] = (float)(curr_enc_value - last_enc_value[i]) / elapsed_time;
            last_enc_value[i] = curr_enc_value;
        }
    }
};
