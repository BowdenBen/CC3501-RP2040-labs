#include "motion_detection.h"
#include <cstdio>
#include <inttypes.h>

// Internal storage for callback
static uart_inst_t* motion_uart = nullptr;
static uint motion_gpio_pin = 0;

// IRQ handler: called in interrupt context on falling edge
static void motion_irq_handler(uint gpio, uint32_t events) {
    if (gpio != motion_gpio_pin) return;
    // Create event and get formatted log
    MotionAlert evt;
    std::string log = evt.getLogString();
    // Send over UART (blocking in IRQ context)
    uart_write_blocking(motion_uart,
                        reinterpret_cast<const uint8_t*>(log.data()),
                        log.size());
}

MotionAlert::MotionAlert(const char* label)
    : timestamp_ms_(time_us_64() / 1000), label_(label) {}

std::string MotionAlert::getLogString() const {
    char buf[64];
    int len = snprintf(buf, sizeof(buf), "%" PRIu64 "-%s\n", timestamp_ms_, label_);
    return std::string(buf, static_cast<size_t>(len));
}

void init_motion_detector(uint gpio_pin, uart_inst_t* uart) {
    motion_uart = uart;
    motion_gpio_pin = gpio_pin;

    // Initialize UART (if not already), e.g., 115200 baud
    uart_init(motion_uart, 115200);
    gpio_set_function(0, GPIO_FUNC_UART);  // TX on GPIO0, RX on GPIO1 (customize if needed)

    // Configure motion pin
    gpio_init(gpio_pin);
    gpio_set_dir(gpio_pin, GPIO_IN);
    gpio_pull_up(gpio_pin);
    
    // Enable IRQ on falling edge with our handler
    gpio_set_irq_enabled_with_callback(gpio_pin,
                                       GPIO_IRQ_EDGE_FALL,
                                       true,
                                       &motion_irq_handler);
}
