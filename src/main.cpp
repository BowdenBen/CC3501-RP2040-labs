#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define INPUT_BUFFER_SIZE 64

void flush_input() {
    while (true) {
        int ch = getchar_timeout_us(0);
        if (ch == PICO_ERROR_TIMEOUT) break;
    }
}

void trim(char *str) {
    char *end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
}

void read_line(char *buffer, int max_len) {
    int index = 0;
    while (true) {
        int ch = getchar_timeout_us(0);
        if (ch != PICO_ERROR_TIMEOUT) {
            if (ch == '\r' || ch == '\n') {
                printf("\r\n"); // Move to new line after Enter
                buffer[index] = '\0';
                // Flush extra CR/LF
                while (true) {
                    int extra = getchar_timeout_us(0);
                    if (extra == PICO_ERROR_TIMEOUT || (extra != '\r' && extra != '\n')) break;
                }
                return;
            }
            if (index < max_len - 1) {
                buffer[index++] = (char)ch;
                printf("%c", ch); // ✅ Echo the character
            }
        }
        sleep_ms(10);
    }
}


int main() {
    stdio_init_all();
    sleep_ms(3000);
    printf("=== GPIO CONTROL TOOL ===\n");

    char input[INPUT_BUFFER_SIZE];

    while (true) {
        printf("Enter GPIO (0-29) or -1 to quit: ");
        read_line(input, INPUT_BUFFER_SIZE);
        trim(input);
        if (strlen(input) == 0) continue;

        int pin;
        if (strncasecmp(input, "GPIO", 4) == 0) {
            pin = atoi(input + 4);
        } else {
            pin = atoi(input);
        }

        if (pin == -1) break;
        if (pin < 0 || pin > 29) {
            printf("Invalid GPIO pin: %d. Use 0-29.\n", pin);
            continue;
        }

        gpio_init(pin);
        gpio_set_function(pin, GPIO_FUNC_SIO);
        gpio_set_dir(pin, GPIO_OUT);

        printf("Enter command for GPIO %d (HIGH/LOW): ", pin);
        read_line(input, INPUT_BUFFER_SIZE);
        trim(input);

        for (char *p = input; *p; ++p) *p = toupper(*p);

        if (strcmp(input, "HIGH") == 0) {
            gpio_put(pin, 1);
            printf("GPIO %d set HIGH\n", pin);
        } else if (strcmp(input, "LOW") == 0) {
            gpio_put(pin, 0);
            printf("GPIO %d set LOW\n", pin);
        } else {
            printf("Unknown command: %s\n", input);
        }

        flush_input(); // Ensure buffer is clean before next loop
    }

    printf("Exiting program.\n");
    return 0;
}
