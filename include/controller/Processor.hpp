#pragma once

#include <functional>
#include <istream>
#include <list>
#include <memory>

#include "controller/Interpreter.hpp"

namespace UrSQL {

class Processor {
public:
    using InterpreterList = std::list<std::unique_ptr<Interpreter>>;
    using CommandConsumer =
      std::function<bool(const std::string& aCommandString)>;

    Processor() = default;
    ~Processor() = default;

    URSQL_DISABLE_COPY(Processor);

    void consume(std::istream& anInput, CommandConsumer aConsumer);
    bool consumeOne(const std::string& aCommandString);

    template<typename T,
             typename = std::enable_if_t<std::is_base_of_v<Interpreter, T>>>
    void addInterpreter() {
        Interpreter* theNextInterpreter = nullptr;
        if (!m_interpreters.empty()) {
            theNextInterpreter = m_interpreters.front().get();
        }
        m_interpreters.push_front(std::make_unique<T>(theNextInterpreter));
    }

private:
    InterpreterList m_interpreters;
};

} /* namespace UrSQL */
