/*
 * Copyright (c) 2021, Ali Mohammad Pur <mpfard@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/StackInfo.h>
#include <LibWasm/AbstractMachine/Configuration.h>
#include <LibWasm/AbstractMachine/Interpreter.h>

namespace Wasm {

struct BytecodeInterpreter : public Interpreter {
    explicit BytecodeInterpreter(StackInfo const& stack_info)
        : m_stack_info(stack_info)
    {
    }

    virtual void interpret(Configuration&) final;

    virtual ~BytecodeInterpreter() override = default;
    virtual bool did_trap() const final { return !m_trap.has<Empty>(); }
    virtual Trap trap() const final
    {
        return m_trap.get<Trap>();
    }
    virtual void clear_trap() final { m_trap = Empty {}; }
    virtual void visit_external_resources(HostVisitOps const& host) override
    {
        if (auto ptr = m_trap.get_pointer<Trap>())
            if (auto data = ptr->data.get_pointer<ExternallyManagedTrap>())
                host.visit_trap(*data);
    }

    struct CallFrameHandle {
        explicit CallFrameHandle(BytecodeInterpreter& interpreter, Configuration& configuration)
            : m_configuration_handle(configuration)
            , m_interpreter(interpreter)
        {
        }

        ~CallFrameHandle() = default;

        Configuration::CallFrameHandle m_configuration_handle;
        BytecodeInterpreter& m_interpreter;
    };

    enum class CallAddressSource {
        DirectCall,
        IndirectCall,
    };

protected:
    void interpret_instruction(Configuration&, InstructionPointer&, Instruction const&);
    void branch_to_label(Configuration&, LabelIndex);
    template<typename ReadT, typename PushT>
    void load_and_push(Configuration&, Instruction const&);
    template<typename PopT, typename StoreT>
    void pop_and_store(Configuration&, Instruction const&);
    template<size_t N>
    void pop_and_store_lane_n(Configuration&, Instruction const&);
    template<size_t M, size_t N, template<typename> typename SetSign>
    void load_and_push_mxn(Configuration&, Instruction const&);
    template<size_t N>
    void load_and_push_lane_n(Configuration&, Instruction const&);
    template<size_t N>
    void load_and_push_zero_n(Configuration&, Instruction const&);
    template<size_t M>
    void load_and_push_m_splat(Configuration&, Instruction const&);
    template<size_t M, template<size_t> typename NativeType>
    void set_top_m_splat(Configuration&, NativeType<M>);
    template<size_t M, template<size_t> typename NativeType>
    void pop_and_push_m_splat(Configuration&, Instruction const&);
    template<typename M, template<typename> typename SetSign, typename VectorType = Native128ByteVectorOf<M, SetSign>>
    VectorType pop_vector(Configuration&);
    void store_to_memory(Configuration&, Instruction::MemoryArgument const&, ReadonlyBytes data, u32 base);
    void call_address(Configuration&, FunctionAddress, CallAddressSource = CallAddressSource::DirectCall);

    template<typename PopTypeLHS, typename PushType, typename Operator, typename PopTypeRHS = PopTypeLHS, typename... Args>
    void binary_numeric_operation(Configuration&, Args&&...);

    template<typename PopType, typename PushType, typename Operator, typename... Args>
    void unary_operation(Configuration&, Args&&...);

    template<typename T>
    T read_value(ReadonlyBytes data);

    ALWAYS_INLINE bool trap_if_not(bool value, StringView reason)
    {
        if (!value)
            m_trap = Trap { ByteString(reason) };
        return !m_trap.has<Empty>();
    }

    template<typename... Rest>
    ALWAYS_INLINE bool trap_if_not(bool value, StringView reason, CheckedFormatString<StringView, Rest...> format, Rest const&... args)
    {
        if (!value)
            m_trap = Trap { ByteString::formatted(move(format), reason, args...) };
        return !m_trap.has<Empty>();
    }

    Variant<Trap, Empty> m_trap;
    StackInfo const& m_stack_info;
};

struct DebuggerBytecodeInterpreter : public BytecodeInterpreter {
    DebuggerBytecodeInterpreter(StackInfo const& stack_info)
        : BytecodeInterpreter(stack_info)
    {
    }
    virtual ~DebuggerBytecodeInterpreter() override = default;

    Function<bool(Configuration&, InstructionPointer&, Instruction const&)> pre_interpret_hook;
    Function<bool(Configuration&, InstructionPointer&, Instruction const&, Interpreter const&)> post_interpret_hook;

private:
    void interpret_instruction(Configuration&, InstructionPointer&, Instruction const&);
};

}
