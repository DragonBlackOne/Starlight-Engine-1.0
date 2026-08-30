#include "core/SIMDUtils.hpp"

#if defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))
#include <intrin.h>
#endif

namespace starlight::simd {

namespace {

struct IsaFlags {
    uint32_t leaf1Ecx = 0;  // SSE3, SSSE3, SSE4.1, SSE4.2, AVX, FMA
    uint32_t leaf1Edx = 0;  // SSE, SSE2
    uint32_t leaf7Ebx = 0;  // AVX2
};

IsaFlags DetectFlags() {
    IsaFlags f;
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
    int regs[4] = {0, 0, 0, 0};
#if defined(_MSC_VER)
    __cpuid(regs, 1);
    f.leaf1Ecx = static_cast<uint32_t>(regs[2]);
    f.leaf1Edx = static_cast<uint32_t>(regs[3]);
    __cpuidex(regs, 7, 0);
    f.leaf7Ebx = static_cast<uint32_t>(regs[1]);
#else
    __asm__ __volatile__("cpuid" : "=a"(regs[0]), "=b"(regs[1]), "=c"(regs[2]), "=d"(regs[3]) : "a"(1));
    f.leaf1Ecx = static_cast<uint32_t>(regs[2]);
    f.leaf1Edx = static_cast<uint32_t>(regs[3]);
    __asm__ __volatile__("cpuid" : "=a"(regs[0]), "=b"(regs[1]), "=c"(regs[2]), "=d"(regs[3]) : "a"(7), "c"(0));
    f.leaf7Ebx = static_cast<uint32_t>(regs[1]);
#endif
#endif
    return f;
}

}  // namespace

Isa GetHighestIsa() {
    static const Isa cached = [] {
        const IsaFlags f = DetectFlags();
        if (f.leaf7Ebx & (1u << 5)) {
            return Isa::AVX2;
        }
        if (f.leaf1Ecx & (1u << 28)) {
            return Isa::AVX;
        }
        if (f.leaf1Ecx & (1u << 19)) {
            return Isa::SSE4_1;
        }
        if (f.leaf1Ecx & (1u << 20)) {
            return Isa::SSE4_2;
        }
        if (f.leaf1Ecx & (1u << 9)) {
            return Isa::SSSE3;
        }
        if (f.leaf1Ecx & (1u << 0)) {
            return Isa::SSE3;
        }
        if (f.leaf1Edx & (1u << 26)) {
            return Isa::SSE2;
        }
        if (f.leaf1Edx & (1u << 25)) {
            return Isa::SSE;
        }
        return Isa::None;
    }();
    return cached;
}

}  // namespace starlight::simd
