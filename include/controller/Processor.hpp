#pragma once
#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include <istream>
#include <list>
#include <memory>
#include <functional>
#include "controller/Interpreter.hpp"

namespace UrSQL {

class Processor {
public:
	using InterpreterList = std::list<std::unique_ptr<Interpreter>>;
	using CommandConsumer = std::function<StatusResult(const std::string& aCommandString)>;

	Processor() = default;
	~Processor() = default;

	Processor(const Processor&) = delete;
	Processor& operator=(const Processor&) noexcept = delete;

	void consume(std::istream& anInput, CommandConsumer aConsumer);
	StatusResult consumeOne(const std::string& aCommandString);

	template<typename T, typename = std::enable_if_t< std::is_base_of_v<Interpreter, T> >>
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

#endif /* Processor.hpp */
