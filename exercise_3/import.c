#include <stdint.h>

static __attribute((noinline)) uint64_t resolve_import(const char* module, const char* function)
{
    register uint64_t a0 asm("a0") = (uint64_t)module;
    register uint64_t a1 asm("a1") = (uint64_t)function;
    register uint64_t a7 asm("a7") = 10105;
    asm volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a7) : "memory");
    return a0;
}

static __attribute((noinline)) uint64_t host_call(uint64_t fn, uint64_t args[13])
{
    register uint64_t a0 asm("a0") = fn;
    register uint64_t a1 asm("a1") = (uint64_t)args;
    register uint64_t a7 asm("a7") = 20000;
    asm volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a7) : "memory");
    return a0;
}

static __attribute((noinline)) uint64_t exit(int exit_code)
{
    register uint64_t a0 asm("a0") = exit_code;
    register uint64_t a1 asm("a1") = 0; // unused
    register uint64_t a7 asm("a7") = 10000;
    asm volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a7) : "memory");
    return a0;
}

void _start() __attribute__((section(".text.start")));

void _start()
{
    // TODO: call resolve_import and host_call here
    uint64_t addr = resolve_import("libc.so.6", "puts");
    uint64_t args[13];
    args[0]         = (uint64_t)"Hello from RISC-V!";
    uint64_t result = host_call(addr, args);

    uint64_t fopen_call = resolve_import("libc.so.6", "open");
    args[0]             = (uint64_t)"secret.txt";
    args[1]             = (uint64_t)"r";
    uint64_t fd         = host_call(fopen_call, args);
    char     buf[256];
    uint64_t fread_call = resolve_import("libc.so.6", "read");
    args[0]             = fd;
    args[1]             = (uint64_t)buf;
    args[2]             = sizeof(buf);
    uint64_t size       = host_call(fread_call, args);

    uint64_t args2[13];
    args2[0] = (uint64_t)buf;

    result = host_call(addr, args2);
    exit(result);
    asm volatile("ebreak");
}
