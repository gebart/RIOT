/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       SPI test application
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "board.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "periph_conf.h"
#include "thread.h"
#include "vtimer.h"
#include "shell.h"
#include "shell_commands.h"

#define USE_SPI_MASTER             0
#if USE_SPI_MASTER == 0
#define TESTPORT_MASTER  SPI_0
#define TESTDEV_MASTER   SPI_0_DEV
#elif USE_SPI_MASTER == 1
#define TESTPORT_MASTER  SPI_1
#define TESTDEV_MASTER   SPI_1_DEV
#endif

#define USE_SPI_SLAVE              0
#if USE_SPI_SLAVE == 0
#define TESTPORT_SLAVE  SPI_0
#define TESTDEV_SLAVE   SPI_0_DEV
#elif USE_SPI_SLAVE == 1
#define TESTPORT_SLAVE  SPI_1
#define TESTDEV_SLAVE   SPI_1_DEV
#endif

#define SPI_SLAVE_FIRST_RESPONSE 0xcc

#define SHELL_BUFFER_SIZE        128
#define BUF_SEND_LEN             15

static int shell_read(void);
static void shell_write(int);
static void cmd_init_slave(int argc, char **argv);
static void cmd_init_master(int argc, char **argv);
static void cmd_send_master_byte(int argc, char **argv);
static void cmd_send_master_bytes(int argc, char **argv);
static void cmd_send_master_8x1_byte(int argc, char **argv);
static void cmd_transfer_string(int argc, char **argv);
static void cmd_prxbuf(int argc, char **argv);
static void cmd_clearbuf(int argc, char **argv);

char test_buf[BUF_SEND_LEN] = {0};
char buf_count = 0;

char on_data(char data)
{

    test_buf[buf_count] = data;
    buf_count++;

    if (buf_count > (BUF_SEND_LEN-1)) {
        buf_count = 0;
    }
    return data;
}

void on_cs(void *arg) {
    LD3_ON;
    spi_transmission_begin(TESTPORT_SLAVE, SPI_SLAVE_FIRST_RESPONSE);
    LD3_OFF;
}


void print_bytes(char* title, char* chars, int length)
{
    printf("%4s", title);
    for (int i = 0; i < length; i++) {
        printf(" %2i ", i);
    }
    printf("\n ");
    for (int i = 0; i < length; i++) {
        printf(" 0x%02x", (int)chars[i]);
    }
    printf("\n ");
    for (int i = 0; i < length; i++) {
        if (chars[i] < ' ' || chars[i] > '~') {
            printf(" ?? ");
        }
        else {
            printf(" %c ", chars[i]);
        }
    }
    printf("\n\n");
}

/**
 * define some additional shell commands
 */
static const shell_command_t shell_commands[] = {
    { "is", "init slave", cmd_init_slave },
    { "im", "init master", cmd_init_master },
    { "smb", "send master byte", cmd_send_master_byte },
    { "smbs", "send master bytes", cmd_send_master_bytes },
    { "smb81", "send master 8x1 bytes", cmd_send_master_8x1_byte },
    { "smst", "send master string", cmd_transfer_string },
    { "prxbuf", "print rx buffer from cb function", cmd_prxbuf },
    { "clearbuf", "clear rx buffer from cb function", cmd_clearbuf },
    { NULL, NULL, NULL }
};

/**
 * @init start slave
 */
void cmd_init_slave(int argc, char **argv)
{
    printf("Init Slave USE_SPI_SLAVE: %i\n", USE_SPI_SLAVE);

    (void) argc;
    (void) argv;

    /** 
     * be aware that this takes some time so it will just work correctly for lower 
     * data rates or with a short delay between CS low and spi_transfer_byte() 
     * at the master side
     */
    gpio_init_int(GPIO_3, GPIO_PULLUP, GPIO_FALLING, on_cs, 0);

    spi_poweron(TESTPORT_SLAVE);
    spi_init_slave(TESTPORT_SLAVE, SPI_CONF_FIRST_RISING, on_data);
}

/**
 * @init master
 */
void cmd_init_master(int argc, char **argv)
{
    printf("Init Master USE_SPI_MASTER: %i\n", USE_SPI_MASTER);

    (void) argc;
    (void) argv;

    gpio_init_out(GPIO_7, GPIO_NOPULL);
    gpio_set(GPIO_7);

    spi_poweron(TESTPORT_MASTER);
    spi_init_master(TESTPORT_MASTER, SPI_CONF_FIRST_RISING, SPI_SPEED_1MHZ);
}

/**
 * @send master one byte
 */
void cmd_send_master_byte(int argc, char **argv)
{

    (void) argc;
    (void) argv;

    puts("Send Master 1 Byte\n");

    char data_return, data_send = 0x99;

    gpio_clear(GPIO_7);
    vtimer_usleep(2);
    spi_transfer_byte(TESTPORT_MASTER, data_send, &data_return);
    gpio_set(GPIO_7);

    printf("One Byte transferred: %x, received: %x\n", data_send, data_return);
}

/**
 * @send master multiple byte
 */
void cmd_send_master_bytes(int argc, char **argv)
{

    (void) argc;
    (void) argv;

    puts("Send Master multiple Bytes\n");

    char buf_send[BUF_SEND_LEN] = {0xca, 9, 8, 7, 6, 5, 4, 3, 2, 1,};
    char buf_return[BUF_SEND_LEN];

    gpio_clear(GPIO_7);
    vtimer_usleep(2);
    spi_transfer_bytes(TESTPORT_MASTER, buf_send, buf_return, BUF_SEND_LEN);
    gpio_set(GPIO_7);

    for (int i = 0; i < BUF_SEND_LEN; i++) {
        printf("Send %d : %x\n Reveice %d: %x\n", i, buf_send[i], i, buf_return[i]);
    }
}

/**
 * @send master one 8 x 1 byte
 */
void cmd_send_master_8x1_byte(int argc, char **argv)
{

    (void) argc;
    (void) argv;

    puts("Send Master 8 x 1 Byte\n");

    char data_return = 0, data_send = 1;

    for (int i = 0; i < 8; i++) {

        gpio_clear(GPIO_7);
        vtimer_usleep(2);
        spi_transfer_byte(TESTPORT_MASTER, data_send, &data_return);
        gpio_set(GPIO_7);

        printf("One Byte transferred: %x, received: %x\n", data_send, data_return);
        data_send++;
    }
}

/**
 * @send master string
 */
void cmd_transfer_string(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    int res;
    char buffer[256]; 

    char *hello = "Hello";

    gpio_clear(GPIO_7);
    vtimer_usleep(1);
    res = spi_transfer_bytes(TESTPORT_MASTER, hello, buffer, strlen(hello));
    gpio_set(GPIO_7);

    /* look at the results */
    if (res < 0) {
        printf("error: unable to transfer data to slave (code: %i)\n", res);
    }
    else {
        printf("Transfered %i bytes:\n", res);
        print_bytes("MOSI", hello, res);
        print_bytes("MISO", buffer, res);
    }
}

/**
 * @print buffer written in cb function
 */
void cmd_prxbuf(int argc, char **argv)
{

    (void) argc;
    (void) argv;

    puts("Buffer print function and clearing\n");

    buf_count = 0;

    for (int i = 0; i < BUF_SEND_LEN; i++) {
        printf("Sequence buffer: %x \n", test_buf[i]);
    }
    for (int i = 0; i < BUF_SEND_LEN; i++) {
        test_buf[i] =  0;
    }
}

/**
 * @clear buffer written in cb function
 */
void cmd_clearbuf(int argc, char **argv)
{

    (void) argc;
    (void) argv;

    puts("Buffer clear function\n");

    buf_count = 0;

    for (int i = 0; i < BUF_SEND_LEN; i++) {
        test_buf[i] =  0;
    }
}

/**
 * @brief proxy for reading a char from std-in and passing it to the shell
 */
int shell_read(void)
{
    return (int) getchar();
}

/**
 * @brief proxy for taking a character from the shell and writing it to std-out
 */
void shell_write(int c)
{
    putchar((char)c);
}

int main(void)
{
    shell_t shell;

    puts("Welcome to RIOT!");

    puts("Initializing shell...");
    shell_init(&shell, shell_commands, SHELL_BUFFER_SIZE, shell_read,
               shell_write);

    puts("Starting shell...");
    shell_run(&shell);
    return 0;
}


