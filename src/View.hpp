#pragma once
#ifndef VIEW_HPP
#define VIEW_HPP
#include "Error.hpp"

namespace UrSQL {

enum class ViewType {
	show_databases
};

class View {
public:
	View() = default;
	virtual ~View() = default;

	View(const View&) = delete;
	View& operator=(const View&) = delete;

	virtual void show() const = 0;
protected:
	static constexpr char vertex = '+';
	static constexpr char vertical_edge = '|';
	static constexpr char horizontal_edge = '-';

	static void print_horizontal_line(const std::vector<size_type>& aWidths);
	static void print_line(const StringList& aFieldNames, const std::vector<size_type>& aWidths);
};

/* -------------------------------ShowDBView------------------------------- */
class ShowDBView : public View {
public:
	ShowDBView(const StringList& aDBNames);
	~ShowDBView() override = default;

	void show() const override;
private:
	const StringList& m_dbnames;
};

/* -------------------------------DescDBView------------------------------- */
enum class BlockType : char;

class DescDBView : public View {
public:
	DescDBView(const std::vector<BlockType>& theTypes);
	~DescDBView() override = default;

	void show() const override;
private:
	const std::vector<BlockType>& m_types;
};

}

#endif