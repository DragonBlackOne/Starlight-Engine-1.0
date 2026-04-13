#pragma once
// ============================================================
// Vamos Engine - Profiling Module (Tracy)
// Licença: BSD-3 | Origem: wolfpld/tracy
// ============================================================

#include <tracy/Tracy.hpp>

// Macros de conveniência para profiling
// Em Release, estas macros são eliminadas pelo compilador (TRACY_ENABLE não definido)

#ifdef TRACY_ENABLE
    #define VAMOS_PROFILE_SCOPE(name)    ZoneScopedN(name)
    #define VAMOS_PROFILE_FUNCTION()     ZoneScoped
    #define VAMOS_PROFILE_FRAME()        FrameMark
    #define VAMOS_PROFILE_FRAME_N(name)  FrameMarkNamed(name)
    #define VAMOS_PROFILE_TAG(text)      ZoneText(text, strlen(text))
    #define VAMOS_PROFILE_VALUE(name, v) TracyPlot(name, v)
    #define VAMOS_PROFILE_ALLOC(ptr, sz) TracyAlloc(ptr, sz)
    #define VAMOS_PROFILE_FREE(ptr)      TracyFree(ptr)
    #define VAMOS_PROFILE_MSG(msg)       TracyMessage(msg, strlen(msg))
    #define VAMOS_PROFILE_COLOR(color)   ZoneColor(color)
#else
    #define VAMOS_PROFILE_SCOPE(name)
    #define VAMOS_PROFILE_FUNCTION()
    #define VAMOS_PROFILE_FRAME()
    #define VAMOS_PROFILE_FRAME_N(name)
    #define VAMOS_PROFILE_TAG(text)
    #define VAMOS_PROFILE_VALUE(name, v)
    #define VAMOS_PROFILE_ALLOC(ptr, sz)
    #define VAMOS_PROFILE_FREE(ptr)
    #define VAMOS_PROFILE_MSG(msg)
    #define VAMOS_PROFILE_COLOR(color)
#endif
