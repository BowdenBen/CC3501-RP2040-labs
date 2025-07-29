// motion_detection.h
#ifndef MOTION_DETECTION_H
#define MOTION_DETECTION_H

#include <string>
#include <cstdint>
#include <hardware/gpio.h>
#include <hardware/timer.h>
#include <hardware/uart.h>

// Encapsulates a single motion event and formats it as a log string.
class MotionAlert {
public:
    // Construct with an optional label (default: "Motion Threshold").
    explicit MotionAlert(const char* label = "Motion Threshold");

    // Returns a string of the form "<timestamp_ms>-<label>\n".
    std::string getLogString() const;

private:
    uint64_t    timestamp_ms_;  // milliseconds since boot
    const char* label_;         // event label
};

// Initialize the motion detector on the given GPIO pin and UART interface (default: uart0).
// This will configure the pin as input with pull-up and install an IRQ handler.
void init_motion_detector(uint gpio_pin, uart_inst_t* uart = uart0);

#endif // MOTION_DETECTION_H