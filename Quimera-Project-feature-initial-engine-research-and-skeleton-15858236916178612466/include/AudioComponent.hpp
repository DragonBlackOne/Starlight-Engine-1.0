#pragma once
#include <string>

namespace Vamos {
    struct AudioComponent {
        std::string soundPath;
        bool loop = false;
    };
}
