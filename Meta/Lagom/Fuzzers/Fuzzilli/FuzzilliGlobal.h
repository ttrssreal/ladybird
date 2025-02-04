#pragma once

#include <LibJS/Runtime/GlobalObject.h>
#include <LibJS/Runtime/Object.h>

class TestRunnerGlobalObject final : public JS::GlobalObject {
    JS_OBJECT(TestRunnerGlobalObject, JS::GlobalObject);

public:
    TestRunnerGlobalObject(JS::Realm&);
    virtual void initialize(JS::Realm&) override;
    virtual ~TestRunnerGlobalObject() override;

private:
    JS_DECLARE_NATIVE_FUNCTION(fuzzilli);
};
