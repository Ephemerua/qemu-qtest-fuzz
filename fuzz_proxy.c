// take vga as a test example


#include "qemu/osdep.h"
#include "libqtest.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

QTestState* state;
int fd;
char **g_argv;
enum
{
    INB,
    INW,
    INL,
    
    OUTB,
    OUTW,
    OUTL,
    
    READ,
    WRITE,
    TYPENUM
}

void static inline do_command(unsigned char type, unsigned short port, unsigned long long addr, unsigned long long value)
{
    char buf[32]={0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00};
    siwtch(type)
    {
        case INB:
            qtest_inb(state, port);break;
        case INW:
            qtest_inw(state, port);break;
        case INL:
            qtest_inw(state, port);break;
        case OUTB:
            qtest_outb(state, port, (uint8_t)value);
            break;
        case OUTW:
            qtest_outw(state, port, (uint16_t)value);
            break;
        case OUTL:
            qtest_outl(state, port, (uint32_t)value);
            break;
        case READ:
            qtest_memread(state, addr, buf, value&0b11111);
            break;
        case WRITE:
            qtest_memwrite(state, addr, buf, value);
            break;

    }
}

void start_proxy(char* filename, int port_min, int port_max, unsigned long long addr_start, unsigned long long addr_end)
{
    iopl(3);
    fd = open(filename, O_RDONLY);
    if(fd == -1) abort();
    unsigned char type;
    unsigned short port;
    unsigned long long addr;
    unsigned long long value;
    int len;
    int port_range = port_max - port_min;
    unsigned long long addr_range = addr_end - addr_start;
    while(1)
    {
        len = read(fd, &type, 1);
        if(!len) return;
        len = read(fd, &port, 2);
        if(!len) return;
        len = read(fd, &addr, 8);
        if(!len) return;
        len = read(fd, &value, 8);
        type %= TYPENUM;
        port = port%port_range + port_min;
        addr = addr%addr_range + addr_start;
        do_command(type, port, addr, value);

    }
    
}

void static inline start_proxy_wrapper()
{
    start_proxy(argv[1], 0x3b0, 0x3df, 0xa0000, 0xc0000);
}


int main(int argc, char **argv)
{
    
    g_argv = argv;
    g_test_init(&argc, &argv, NULL);
    state = qtest_start("-vga none -device VGA");
    qtest_add_func("/test/vga-std", start_proxy_wrapper);
    return g_test_run();
}

/* 
static void pci_cirrus(void)
{
    qtest_start("-vga none -device cirrus-vga");
    qtest_end();
}

static void pci_stdvga(void)
{
    state = qtest_start("-vga none -device VGA");
    qtest_end();
}

static void pci_secondary(void)
{
    qtest_start("-vga none -device secondary-vga");
    qtest_end();
}

static void pci_multihead(void)
{
    qtest_start("-vga none -device VGA -device secondary-vga");
    qtest_end();
}

static void pci_virtio_gpu(void)
{
    qtest_start("-vga none -device virtio-gpu-pci");
    qtest_end();
}

static void pci_virtio_vga(void)
{
    qtest_start("-vga none -device virtio-vga");
    qtest_end();
}

 */
