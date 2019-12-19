#ifndef MEMBERVARIABLE_H
#define MEMBERVARIABLE_H

#include <string>

#include <fmt/format.h>

#include <kpl/util.h>

#include <kpl/reflection/variable.h>
#include <kpl/reflection/accessmodifier.h>

namespace kpl {
    namespace reflection {

        class Class;

        class MemberVariable
        {
        public:
            MemberVariable( const kpl::reflection::Class& classRef,
                            const kpl::reflection::Variable& variable,
                            kpl::reflection::AccessModifier accessModifier = kpl::reflection::AccessModifier::PUBLIC,
                            bool isStatic = false   );

            std::string asGetterMethodDeclaration(bool fullyQualify = false) const;
            std::string asGetterMethodDefinition(std::string instanceName, bool fullyQualify = true) const;
            std::string fullyQualifiedName() const;
            std::string defaultInstanceName() const;

            const kpl::reflection::Variable varData() const;
            const kpl::reflection::Class& classData() const;

            const kpl::reflection::AccessModifier& accessModifier() const;
            const std::string& accessModifierString() const;

        private:
            const kpl::reflection::Class& mClass;
            kpl::reflection::Variable mVariable;
            kpl::reflection::AccessModifier mAccessModifier;

            bool mIsStatic;
        };
    }
}

#endif // MEMBERVARIABLE_H
