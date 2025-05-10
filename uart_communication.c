#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <termios.h>

#define UART_FILE_PATH "/dev/ttyS%d"

int uart_file_access(uint8_t uart_file_number);
uint8_t configure_uart_settings(int file_handler, int uart_baud_rate);
void establish_uart_communication(int file_handler);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        perror("arguments = uart 0/1/2/3/4/5, buad_rate = 9600/115200\n");
        return EXIT_FAILURE;
    }

    uint8_t uart_file_number = atoi(argv[1]);
    int uart_baud_rate = atoi(argv[2]);

    int file_handler;

    if (uart_file_number >= 1 && uart_file_number <= 5) // UART 0 is not accessible
    {
        file_handler = uart_file_access(uart_file_number);
    }

    if (configure_uart_settings(file_handler, uart_baud_rate) != EXIT_SUCCESS)
    {
        printf("Unable to configure the uart%d settings\n", uart_file_number);
        return EXIT_FAILURE;
    }
    printf("Successfully configured uart%d settings\n", uart_file_number);

    establish_uart_communication(file_handler);

    if (close(file_handler) < 0)
    {
        printf("Unable to close UART%d file\n", uart_file_number);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int uart_file_access(uint8_t uart_file_number)
{
    char buff[256];
    snprintf(buff, sizeof(buff), UART_FILE_PATH, uart_file_number);

    int fd = open(buff, O_RDONLY | O_NOCTTY);
    if (fd < 0)
    {
        printf("Unable to open UART: %d file\n", uart_file_number);
        exit(EXIT_FAILURE);
    }
    printf("Successfully open UART%d file\n", uart_file_number);

    // Clear the non-blocking flag so that read() will block.
    fcntl(fd, F_SETFL, 0);

    return fd;
}

uint8_t configure_uart_settings(int file_handler, int uart_baud_rate)
{
    struct termios tty; // to configure UART terminal

    // Retrieve the current settings for the serial port
    if (tcgetattr(file_handler, &tty))
    {
        perror("tcgetattr\n");
        close(file_handler);
        return EXIT_FAILURE;
    }

    printf("UART predefined configuration \
        = %d, %d, %d, %d, %d , %d\n",
           tty.c_iflag,
           tty.c_oflag, tty.c_cflag,
           tty.c_lflag, tty.c_ispeed, tty.c_ospeed);

    // Setting baud rate
    if (uart_baud_rate == 115200)
    {
        cfsetispeed(&tty, B115200);
        cfsetospeed(&tty, B115200);
    }
    else
    {
        cfsetispeed(&tty, B9600);
        cfsetospeed(&tty, B9600);
    }

    // Configure tty for 8N1 (8 data bits, No parity, 1 stop bit)
    tty.c_cflag &= ~PARENB; // No parity
    tty.c_cflag &= ~CSTOPB; // 1 stop bit
    tty.c_cflag &= ~CSIZE;  // Clear current data size setting
    tty.c_cflag |= CS8;     // 8 data bits

    // Enable the port and set it as local (do not change "ownership" of the port)
    tty.c_cflag |= (CLOCAL | CREAD);
    // tty.c_iflag = IGNPAR | ICRNL;

    // Raw output mode (disable post-processing of output).
    tty.c_oflag &= ~OPOST; // tty.c_oflag = 0;

    // Set raw input mode: disable canonical mode, echo, and signals.
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // tty.c_lflag = 0;

    // Configure blocking read: wait until at least 1 char is available
    tty.c_cc[VTIME] = 0;
    tty.c_cc[VMIN] = 1;

    // Flush any data received but not read, then apply the settings
    tcflush(file_handler, TCIFLUSH);

    // Setting UART configuration
    if (tcsetattr(file_handler, TCSANOW, &tty) < 0)
    {
        perror("tcsetattr");
        close(file_handler);
        return EXIT_FAILURE;
    }

    printf("UART configuration \
        = %d, %d, %d, %d, %d , %d\n\t",
           tty.c_iflag,
           tty.c_oflag, tty.c_cflag,
           tty.c_lflag, tty.c_ispeed, tty.c_ospeed);

    return EXIT_SUCCESS;
}

void establish_uart_communication(int file_handler)
{
    char buffer[256];

    while (1)
    {
        snprintf(buffer, 50, "hello please type something\n\t");
        printf("Sending = %s", buffer);
        write(file_handler, buffer, strlen(buffer));

        int n = read(file_handler, buffer, sizeof(buffer));

        if (n > 0)
        {
            buffer[n] = '\0';
            printf("%s\n\n", buffer);
        }
        if (n < 0)
        {
            perror("Error reading from UART");
            break;
        }
    }
}
