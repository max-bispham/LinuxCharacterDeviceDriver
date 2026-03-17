
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

struct sht30_reading {
    uint16_t temperature;
    uint16_t humidity;
};

int main() {
    struct sht30_reading reading;
    int fd = open("/dev/sht30", O_RDONLY);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    read(fd, &reading, sizeof(reading));

    double temp = -49.0 + 315.0 * reading.temperature / 65535.0;
    double hum  = 100.0 * reading.humidity / 65535.0;

    printf("Temperature: %.2f F\n", temp);
    printf("Humidity: %.2f%%\n", hum);

    close(fd);
    return 0;
}
