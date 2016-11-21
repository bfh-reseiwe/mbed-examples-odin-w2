#define BUG_ON

#include <sstream>

#include <stdio.h>
#include <string.h>

#include "mbed-drivers/mbed.h"
#include "minar/minar.h"
#include "core-util/FunctionPointer.h"
#include "mbed-drivers/Buffer.h"

#include "ublox-odin-w2-drivers/cb_wlan.h"
#include "ublox-odin-w2-drivers/cb_main.h"


DigitalOut out_r(PE_0);
DigitalOut out_g(PB_6);
DigitalOut out_b(PB_8);

static constexpr int serial_read_buffer_size = 255;
static uint8_t serial_read_buffer[serial_read_buffer_size];

Serial uart3(PD_8, NC);

void uart_callback(Buffer buf, int event)
{
    out_b = !out_b;

    auto &uart1 = get_stdio_serial();
    uart1.printf("callback, buf size: %u, event: %d\r\n", buf.length, event);
    uart3.printf("callback, buf size: %u, event: %d\r\n", buf.length, event);

    memset(serial_read_buffer, 0, serial_read_buffer_size);
    uart1.read(serial_read_buffer, serial_read_buffer_size, &uart_callback, SERIAL_EVENT_RX_CHARACTER_MATCH, '\r');
}

void periodic(void)
{
    auto &uart1 = get_stdio_serial();

    uart3.putc('u');
    uart3.putc('a');
    uart3.putc('r');
    uart3.putc('t');
    uart3.putc('3');
    uart3.putc('\n');
    uart3.putc('\r');

    uart1.putc('u');
    uart1.putc('a');
    uart1.putc('r');
    uart1.putc('t');
    uart1.putc('1');
    uart1.putc('\n');
    uart1.putc('\r');

    if (out_r)
        out_g = !out_g;

    out_r = !out_r;
}

void app_start(int argc, char *argv[])
{
    printf("Starting up...\n");

    (void)argc;
    (void)argv;

#ifdef BUG_ON
    cb_int32 wlanTargetId = cbMAIN_initWlan();
#endif
    
    out_r = 1;
    out_g = 1;
    out_b = 1;

    auto &uart1 = get_stdio_serial();
    memset(serial_read_buffer, 0, serial_read_buffer_size);
    uart1.read(serial_read_buffer, serial_read_buffer_size, &uart_callback, SERIAL_EVENT_RX_CHARACTER_MATCH, '\n');

    minar::Scheduler::postCallback(periodic).period(minar::milliseconds(1000));
}
