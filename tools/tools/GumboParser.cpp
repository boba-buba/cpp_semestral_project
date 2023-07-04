#include "GumboParser.h"

namespace Parser {
	//Children
	std::size_t ChildrenNodesCount(const GumboNode* node)
	{
		if (node && node->type == GUMBO_NODE_ELEMENT)
		{
			return node->v.element.children.length;
		}
		return 0;
	}

	const GumboNode** ChildrenNodesBegin(const GumboNode* node)
	{
		if (ChildrenNodesCount(node))
		{
			return (const GumboNode**)node->v.element.children.data;
		}
		return 0;
	}

	const GumboNode** ChildrenNodesEnd(const GumboNode* node)
	{
		const GumboNode** it = ChildrenNodesBegin(node);
		if (it)	return it + node->v.element.children.length;
		return 0;
	}

	//Text
	std::string getText(const GumboNode* node)
	{
		if (node && node->type == GUMBO_NODE_TEXT)
			return node->v.text.text;

		std::string textHTML = "";
		const GumboNode** bnode = ChildrenNodesBegin(node);
		const GumboNode** lnode = ChildrenNodesEnd(node);

		for (auto it = bnode; it != lnode; ++it)
		{
			textHTML += getText(*it);
		}
		return textHTML;
	}

	//Attributes
	std::size_t AttributesCount(const GumboNode* node)
	{
		return (node && node->type == GUMBO_NODE_ELEMENT) ? node->v.element.attributes.length : 0;
	}

	bool AttributeName(const GumboNode* node, const std::string& name)
	{
		if (AttributesCount(node))
			return gumbo_get_attribute(&node->v.element.attributes, name.c_str());
		return false;
	}

	std::string AttributeValue(const GumboNode* node, const std::string& name)
	{
		if (AttributesCount(node))
		{
			const GumboAttribute* attribute = gumbo_get_attribute(&node->v.element.attributes, name.c_str());
			return attribute ? attribute->value : "";
		}
		return "";
	}

	//Tag
	bool NodeTag(const GumboNode* node)
	{
		return node && node->type == GUMBO_NODE_ELEMENT;
	}

	bool NodeTagEqual(const GumboNode* node, GumboTag tag)
	{
		return (NodeTag(node) && node->v.element.tag == tag);
	}

	GumboTag getNodeTag(const GumboNode* node)
	{
		return Parser::NodeTag(node) ? node->v.element.tag : GUMBO_TAG_UNKNOWN;
	}

	//Table
	
	std::vector<std::string> getCellsFromRow(const GumboNode* row)
	{
		auto r = FindIf(row, [](const GumboNode* cell) {
			return (NodeTagEqual(cell, GUMBO_TAG_TH) || NodeTagEqual(cell, GUMBO_TAG_TD));
		});
		
		std::vector<std::string> result;
		for (auto it = r.begin(); it != r.end(); ++it)
		{
			result.push_back( getText(*it));
		}
		return result;
	}

	std::vector<const GumboNode*> getRowsFromTable(const GumboNode* node)
	{
		std::vector<const GumboNode*> r;
		r = FindIf(node, [](const GumboNode* row) {
			return NodeTagEqual(row, GUMBO_TAG_TR);
		});
		return r;			
	}

	const GumboNode* GetTBody(const GumboNode* node)
	{
		auto table = FirstIf(node, [](const GumboNode* table) {
			return NodeTagEqual(table, GUMBO_TAG_TBODY);
		});
		return table;
	}

	const GumboNode* GetTHead(const GumboNode* node)
	{
		auto header = FirstIf(node, [](const GumboNode* table) {
			return NodeTagEqual(table, GUMBO_TAG_THEAD);
			});
		return header;
	}

	void GetDataFromNodeTable(char*& data_html, std::vector<double>& ClosePrices, std::vector<std::string>& Dates)
	{
		std::vector<std::string> result;
		auto output = gumbo_parse(data_html);
		auto table = FirstIf(output->root, [](const GumboNode* table) {
			return NodeTagEqual(table, GUMBO_TAG_TABLE);
		});
		if (!table)
		{
			throw std::runtime_error("Can not find company with such ticker");
		}
		else
		{
			auto tbody = GetTBody(table);
			auto header = GetTHead(table);
			auto rows = getRowsFromTable(tbody);

			std::vector<std::string> headers = getCellsFromRow(header); std::size_t maximum = headers.size();
			std::size_t columnClose = maximum; std::size_t columnDates = maximum;
			std::string close = "Close";
			std::string dates = "Date";

			for (std::size_t i = 0; i < headers.size(); ++i)
			{
				if (strstr(headers[i].c_str(), close.c_str())) { columnClose = i; }
				if (strstr(headers[i].c_str(), dates.c_str())) { columnDates = i; }
			}
			if (columnClose != maximum && columnDates != maximum)
			{
				for (std::size_t i = 0; i < rows.size(); ++i)
				{
					auto row = getCellsFromRow(rows[i]);
					if (row.size() == maximum && row[columnClose] != "-")
					{
						ClosePrices.push_back(std::stod(row[columnClose]));
						Dates.push_back(row[columnDates]);
					
					}
				}
			}	
		}
	}
	
	vvs GetEPSTable(char*& data_html)
	{
		vvs eps;
		auto output = gumbo_parse(data_html);
		std::vector<const GumboNode*> tables = FindIf(output->root, [](const GumboNode* table) {
			return NodeTagEqual(table, GUMBO_TAG_TABLE);
		});
		if (tables.size() == 0) throw std::runtime_error("no Analysis link to create EPS data");
		auto rows = getRowsFromTable(tables[2]);
		std::vector<std::string> headers = getCellsFromRow(rows[0]);
		std::vector<std::string> epsEst = getCellsFromRow(rows[1]);
		std::vector<std::string> epsActual = getCellsFromRow(rows[2]);
		for (std::size_t i = 1; i < headers.size(); ++i)
		{
			std::vector<std::string> row{headers[i], epsEst[i], epsActual[i]};
			eps.push_back(row);
		}
		return eps;
	}

	template<typename T>
	const GumboNode* FirstIf(const GumboNode* node, T condition)
	{
		if (condition(node))
		{
			return node;
		}

		const GumboNode** it = ChildrenNodesBegin(node);
		const GumboNode** lt = ChildrenNodesEnd(node);

		while (it != lt)
		{
			if (node = FirstIf(*it++, condition))
			{
				return node;
			}
		}
		return 0;
	}

	template<typename T>
	std::vector<const GumboNode*> FindIf(const GumboNode* node, T condition)
	{
		std::vector<const GumboNode*> result;
		if (condition(node))
		{
			result.push_back(node);
		}
		const GumboNode** it = ChildrenNodesBegin(node);
		const GumboNode** lt = ChildrenNodesEnd(node);

		while (it != lt)
		{
			std::vector<const GumboNode*> sub = FindIf(*it++, condition);
			result.insert(result.end(), sub.begin(), sub.end());
		}
		return result;
	}
};