#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "drivers/lis3dh.h" // Your LIS3DH driver
#include "drivers/board.h"  // Your board-specific config
#include <math.h>
#include "hardware/i2c.h"
#include "drivers/HDC2010/hdc2010.h"

#define INPUT_BUFFER_SIZE 64

void flush_input()
{
    while (true)
    {
        int ch = getchar_timeout_us(0);
        if (ch == PICO_ERROR_TIMEOUT)
            break;
    }
}

void trim(char *str)
{
    char *end;
    while (isspace((unsigned char)*str))
        str++;
    if (*str == 0)
        return;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;
    end[1] = '\0';
}

void read_line(char *buffer, int max_len)
{
    int index = 0;
    while (true)
    {
        int ch = getchar_timeout_us(0);
        if (ch != PICO_ERROR_TIMEOUT)
        {
            if (ch == '\r' || ch == '\n')
            {
                printf("\r\n");
                buffer[index] = '\0';
                while (true)
                {
                    int extra = getchar_timeout_us(0);
                    if (extra == PICO_ERROR_TIMEOUT || (extra != '\r' && extra != '\n'))
                        break;
                }
                return;
            }
            if (index < max_len - 1)
            {
                buffer[index++] = (char)ch;
                printf("%c", ch);
            }
        }
        sleep_ms(10);
    }
}

void accelerometer_test()
{
    printf("\nInitializing accelerometer...\n");
    LIS3DH_init(); // ✅ Initialize the accelerometer
    printf("Accelerometer initialized successfully.\n");
    printf("Streaming X, Y, Z in g (press any key to return to menu)\n");

    while (true)
    {
        int16_t x_raw, y_raw, z_raw;
        float gx, gy, gz;

        LIS3DH_read_raw(&x_raw, &y_raw, &z_raw);
        LIS3DH_convert_to_g(x_raw, y_raw, z_raw, &gx, &gy, &gz);

        printf("X: %.3fg  Y: %.3fg  Z: %.3fg\r\n", gx, gy, gz);
        sleep_ms(100); // ~10 Hz refresh
                       // Check for any key press to cancel
        int ch = getchar_timeout_us(0);
        if (ch != PICO_ERROR_TIMEOUT)
        {
            printf("Stopping accelerometer test, returning to menu...\n");
            flush_input();
            break;
        }
    }
}

void hdc2010_test()
{
    printf("\nInitializing HDC2010 sensor...\n");
    HDC2010_init();
    printf("Streaming temperature and humidity (press any key to return to menu)\n");

    while (true)
    {
        // Trigger a new measurement
        uint8_t trigger[] = {0x0F, 0x01}; // MEAS_CONF register: bit0 = trigger
        i2c_write_blocking(I2c_INSTANCE, HDC2010_ADDR, trigger, 2, false);
        sleep_ms(50); // allow time for measurement to complete

        float temperature, humidity;
        if (HDC2010_read(&temperature, &humidity))
        {
            printf("Temperature: %.2f°C  Humidity: %.2f%%\n", temperature, humidity);
        }
        else
        {
            printf("Failed to read from HDC2010 sensor.\n");
        }

        sleep_ms(1000); // 1 Hz sampling rate

        int ch = getchar_timeout_us(0);
        if (ch != PICO_ERROR_TIMEOUT)
        {
            printf("Stopping HDC2010 test, returning to menu...\n");
            flush_input();
            break;
        }
    }
}

int main()

{
    stdio_init_all();
    sleep_ms(3000);
    printf("=== GPIO & Accelerometer Tool ===\n");

    // ✅ I2C SCAN BEFORE MENU
    i2c_init(I2c_INSTANCE, 400 * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);

    printf("Scanning I2C bus with debug...\n");
    for (uint8_t addr = 0x08; addr < 0x78; addr++)
    {
        int result = i2c_write_blocking(I2c_INSTANCE, addr, NULL, 0, false);
        printf("Addr 0x%02X -> result %d\n", addr, result);
    }
    printf("Scan complete.\n");

    char input[INPUT_BUFFER_SIZE];

    while (true)
    {
        printf("\nSelect an option:\n");
        printf("1 - GPIO Control\n");
        printf("2 - Accelerometer Test\n");
        printf("3 - HDC2010 Test\n");
        printf("Enter choice: ");
        read_line(input, INPUT_BUFFER_SIZE);
        trim(input);

        if (strcmp(input, "1") == 0)
        {
            printf("Enter GPIO (0-29) or -1 to quit: ");
            read_line(input, INPUT_BUFFER_SIZE);
            trim(input);
            if (strlen(input) == 0)
                continue;

            int pin;
            if (strncasecmp(input, "GPIO", 4) == 0)
            {
                pin = atoi(input + 4);
            }
            else
            {
                pin = atoi(input);
            }

            if (pin == -1)
                continue;

            if (pin < 0 || pin > 29)
            {
                printf("Invalid GPIO pin: %d. Use 0-29.\n", pin);
                continue;
            }

            gpio_init(pin);
            gpio_set_function(pin, GPIO_FUNC_SIO);
            gpio_set_dir(pin, GPIO_OUT);

            printf("Enter command for GPIO %d (HIGH/LOW): ", pin);
            read_line(input, INPUT_BUFFER_SIZE);
            trim(input);

            for (char *p = input; *p; ++p)
                *p = toupper(*p);

            if (strcmp(input, "HIGH") == 0)
            {
                gpio_put(pin, 1);
                printf("GPIO %d set HIGH\n", pin);
            }
            else if (strcmp(input, "LOW") == 0)
            {
                gpio_put(pin, 0);
                printf("GPIO %d set LOW\n", pin);
            }
            else
            {
                printf("Unknown command: %s\n", input);
            }

            flush_input();
        }
        else if (strcmp(input, "2") == 0)
        {
            accelerometer_test();
        }
        else if (strcmp(input, "3") == 0)
        {
            hdc2010_test();
        }
        else
        {
            printf("Unknown option: %s\n", input);
        }
    }

    printf("Exiting program.\n");
    return 0;
}
