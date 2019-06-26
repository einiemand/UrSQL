#pragma once
#ifndef VIEW_HPP
#define VIEW_HPP
#include "Error.hpp"

namespace UrSQL {

class View {
public:
	View() = default;
	virtual ~View() = default;

	View(const View&) = delete;
	View& operator=(const View&) = delete;

	virtual void show() const = 0;
protected:
	static constexpr char vertex = '+';
	static constexpr char verticalEdge = '|';
	static constexpr char horizontalEdge = '-';

	static void printHorizontalLine(const std::vector<size_type>& aWidths);
	static void printLine(const StringList& aFieldNames, const std::vector<size_type>& aWidths);
};

/* -------------------------------ShowDatabasesView------------------------------- */
class ShowDatabasesView : public View {
public:
	ShowDatabasesView(const StringList& aDBNames);
	~ShowDatabasesView() override = default;

	void show() const override;
private:
	const StringList& m_dbNames;
};

/* -------------------------------DescDatabaseView------------------------------- */
enum class BlockType : char;

class DescDatabaseView : public View {
public:
	DescDatabaseView(const std::vector<BlockType>& theTypes);
	~DescDatabaseView() override = default;

	void show() const override;
private:
	const std::vector<BlockType>& m_types;
};

/* -------------------------------DescDatabaseView------------------------------- */
class Entity;

class DescTableView : public View {
public:
	DescTableView(const Entity& anEntity);
	~DescTableView() override = default;

	void show() const override;
private:
	const Entity& m_entity;
};

}

#endif