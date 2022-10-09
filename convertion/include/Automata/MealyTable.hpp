#ifndef AUTOMATA_MEALY_TABLE_HPP_
#define AUTOMATA_MEALY_TABLE_HPP_

#include <vector>
#include <list>

#include "../CSV/csv.hpp"
#include "State.hpp"

class MealyTable
{
public:
	MealyTable(const std::string_view& csvFileName, char delimeter = ';')
		: m_reader(csvFileName)
		, m_mealyStates()
		, m_transitions()
		, m_states()
		, m_delimeter(delimeter)
	{
		ReadColumnNames();
		ReadRows();
	}

	friend std::ostream& operator<<(std::ostream& lhs, const MealyTable& rhs)
	{
		std::ostream::sentry sentry(lhs);
		if (!sentry)
		{
			return lhs;
		}

		for (auto& columnName : rhs.m_states)
		{
			lhs << rhs.m_delimeter << columnName;
		}

		size_t rowIndex = 0;
		for (auto& transition : rhs.m_transitions)
		{
			lhs << '\n';
			lhs << transition;
			for (auto& field : rhs.m_mealyStates[rowIndex++])
			{
				lhs << rhs.m_delimeter << field;
			}
		}

		return (lhs << std::endl);
	}

private:
	void ReadColumnNames()
	{
		for (auto& cName : m_reader.get_col_names())
		{
			if (cName.empty())
			{
				continue;
			}
			m_states.emplace_back(State{ cName });
		}
	}

	void ReadRows()
	{
		for (auto& row : m_reader)
		{
			m_transitions.emplace_back(State{ row[0].get_sv() });

			auto stateRow = MealyStateRow();
			auto i = 0;
			for (auto& field : row)
			{
				if (i == 0) { ++i; continue; }
				stateRow.emplace_back(MealyState{ field.get_sv() });
			}
			m_mealyStates.emplace_back(std::move(stateRow));
		}
	}

	csv::CSVReader m_reader;

	using MealyStateRow = std::list<MealyState>;

	std::vector<MealyStateRow> m_mealyStates;
	std::list<State> m_states;
	std::list<State> m_transitions;

	char m_delimeter;
};

#endif // !AUTOMATA_MEALY_TABLE_HPP_
