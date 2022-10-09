#ifndef AUTOMATA_MEALY_TABLE_HPP_
#define AUTOMATA_MEALY_TABLE_HPP_

#include <list>
#include <vector>

#include "../CSV/csv.hpp"
#include "State.hpp"

class MealyTable
{
public:
	using MealyStateRow = std::list<MealyState>;
	using MealyStates = std::vector<MealyStateRow>;

	using States = std::list<State>;
	using Transition = State;
	using Transitions = std::list<Transition>;

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

	const MealyStates& GetMealyStates() const noexcept
	{
		return m_mealyStates;
	}

	const States& GetStates() const noexcept
	{
		return m_states;
	}

	const Transitions& GetTransitions() const noexcept
	{
		return m_transitions;
	}

	const MealyState& GetCertainMealyState(const Transition& transition, const State& state) const
	{
		auto it = std::find(m_transitions.begin(), m_transitions.end(), transition);
		if (it == m_transitions.end())
		{
			throw std::out_of_range("MealyTable doesn't contain given transition");
		}

		auto columnIt = std::find(m_states.begin(), m_states.end(), state);
		if (columnIt == m_states.end())
		{
			throw std::out_of_range("MealyTable doesn't contain given state");
		}

		auto rowIndex = std::distance(m_transitions.begin(), it);
		auto columnIndex = std::distance(m_states.begin(), columnIt);

		auto begIt = m_mealyStates[rowIndex].begin();
		for (size_t i = 0; i < columnIndex; ++i)
		{
			++begIt;
		}

		return *begIt;
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
				if (i == 0)
				{
					++i;
					continue;
				}
				stateRow.emplace_back(MealyState{ field.get_sv() });
			}
			m_mealyStates.emplace_back(std::move(stateRow));
		}
	}

	csv::CSVReader m_reader;

	MealyStates m_mealyStates;
	States m_states;
	Transitions m_transitions;

	char m_delimeter;
};

#endif // !AUTOMATA_MEALY_TABLE_HPP_
