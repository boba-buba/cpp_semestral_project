#ifndef GumboParser_h
#define GumboParser_h
#include "gumbo.h"
#include <iostream>
#include<string>
#include <vector>
#include <map>

namespace Parser
{
	using vvs = std::vector<std::vector<std::string>>;
	//Children
	std::size_t ChildrenNodesCount(const GumboNode* node);

	const GumboNode** ChildrenNodesBegin(const GumboNode* node);

	const GumboNode** ChildrenNodesEnd(const GumboNode* node);

	//Text
	std::string getText(const GumboNode* node);


	//Attributes
	std::size_t AttributesCount(const GumboNode* node);

	bool AttributeName(const GumboNode* node, const std::string& name);

	std::string AttributeValue(const GumboNode* node, const std::string &name);

	//Tag
	bool NodeTag(const GumboNode* node);

	bool NodeTagEqual(const GumboNode* node, GumboTag tag);

	GumboTag getNodeTag(const GumboNode* node);

	//Algorithm
	template<typename T>
	const GumboNode* FirstIf(const GumboNode* node, T condition);

	template<typename T> 
	std::vector<const GumboNode*> FindIf(const GumboNode* node, T condition);

	//Table
	std::vector<std::string> getCellsFromRow(const GumboNode* row);
	
	std::vector<const GumboNode*> getRowsFromTable(const GumboNode* node);

	void GetDataFromNodeTable(char*& data_html, std::vector<double>& ClosePrices, std::vector<std::string>& Dates);
	
	vvs GetEPSTable(char*& data_html);
};

#endif // !GumboParser_h

#pragma once
