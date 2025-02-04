#include <LibJS/Forward.h>
#include <LibJS/Runtime/Value.h>

#include "Coverage.h"
#include "FuzzilliGlobal.h"

TestRunnerGlobalObject::TestRunnerGlobalObject(JS::Realm& realm)
    : GlobalObject(realm)
{
}

TestRunnerGlobalObject::~TestRunnerGlobalObject()
{
}

void TestRunnerGlobalObject::initialize(JS::Realm& realm)
{
    Base::initialize(realm);
    define_direct_property("global", this, JS::Attribute::Enumerable);
    define_native_function(realm, "fuzzilli", fuzzilli, 2, JS::default_attributes);
}

JS_DEFINE_NATIVE_FUNCTION(TestRunnerGlobalObject::fuzzilli)
{
    if (!vm.argument_count())
        return JS::js_undefined();

    auto operation = TRY(vm.argument(0).to_string(vm));
    if (operation == "FUZZILLI_CRASH") {
        auto type = TRY(vm.argument(1).to_i32(vm));
        switch (type) {
        case 0:
            *((int*)0x41414141) = 0x1337;
            break;
        default:
            VERIFY_NOT_REACHED();
            break;
        }
    } else if (operation == "FUZZILLI_PRINT") {
        static FILE* fzliout = fdopen(REPRL_DWFD, "w");
        if (!fzliout) {
            dbgln("Fuzzer output not available");
            fzliout = stdout;
        }

        auto string = TRY(vm.argument(1).to_string(vm));
        outln(fzliout, "{}", string);
        fflush(fzliout);
    }

    return JS::js_undefined();
}