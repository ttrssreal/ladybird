#include <AK/ByteBuffer.h>
#include <AK/Format.h>
#include <AK/Function.h>
#include <AK/StringView.h>
#include <LibJS/Bytecode/Interpreter.h>
#include <LibJS/Parser.h>

#include <fcntl.h>
#include <stddef.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "Coverage.h"
#include "FuzzilliGlobal.h"

int main(int, char**)
{
    char* reprl_input = nullptr;

    char helo[] = "HELO";
    if (write(REPRL_CWFD, helo, 4) != 4 || read(REPRL_CRFD, helo, 4) != 4) {
        VERIFY_NOT_REACHED();
    }

    VERIFY(memcmp(helo, "HELO", 4) == 0);
    reprl_input = (char*)mmap(nullptr, REPRL_MAX_DATA_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, REPRL_DRFD, 0);
    VERIFY(reprl_input != MAP_FAILED);

    auto vm = MUST(JS::VM::create());
    auto root_execution_context = JS::create_simple_execution_context<TestRunnerGlobalObject>(*vm);
    auto& realm = *root_execution_context->realm;

    while (true) {
        unsigned action;
        VERIFY(read(REPRL_CRFD, &action, 4) == 4);
        VERIFY(action == 'cexe');

        size_t script_size;
        VERIFY(read(REPRL_CRFD, &script_size, 8) == 8);
        VERIFY(script_size < REPRL_MAX_DATA_SIZE);
        ByteBuffer data_buffer;
        data_buffer.resize(script_size);
        VERIFY(data_buffer.size() >= script_size);
        memcpy(data_buffer.data(), reprl_input, script_size);

        int result = 0;

        auto const js = StringView(data_buffer.data(), script_size);

        // FIXME: https://github.com/SerenityOS/serenity/issues/17899
        if (!Utf8View(js).validate()) {
            result = 1;
        } else {
            auto parse_result = JS::Script::parse(js, realm);
            if (parse_result.is_error()) {
                result = 1;
            } else {
                auto completion = vm->bytecode_interpreter().run(parse_result.value());
                if (completion.is_error()) {
                    result = 1;
                }
            }
        }
        fflush(stdout);
        fflush(stderr);

        int status = (result & 0xff) << 8;
        VERIFY(write(REPRL_CWFD, &status, 4) == 4);
        sanitizer_cov_reset_edgeguards();
    }
}
