#pragma once

extern void mcuboot_assert_handler(const char *file, int line, const char *func);

#ifdef assert
#undef assert
#endif
#define assert(arg)                                               \
    do {                                                          \
        if (!(arg)) {                                             \
            mcuboot_assert_handler(__FILE__, __LINE__, __func__); \
        }                                                         \
    } while (0)
