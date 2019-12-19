#ifndef ACCESSMODIFIER_H
#define ACCESSMODIFIER_H

#include <string>
#include <iostream>
#include <cstdlib>

namespace kpl {
    namespace reflection {

        enum class AccessModifier
        {
            PRIVATE,
            PUBLIC,
            PROTECTED
        };

        const std::string& AccessModifierString(AccessModifier accessModifier);
    }
}

#endif // ACCESSMODIFIER_H
