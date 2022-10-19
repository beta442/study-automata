#ifndef AUTOMATA_MEALY_TABLE_READER_HPP_
#define AUTOMATA_MEALY_TABLE_READER_HPP_

#include <list>
#include <vector>

#include "../CSV/csv.hpp"
#include "State.hpp"

class MealyTableReader
{
public:
	using MealyStatesRow = std::list<MealyState>;
	using MealyStates = std::vector<MealyStatesRow>;

	using States = std::list<State>;
	using Transition = State;
	using Transitions = std::list<Transition>;

	MealyTableReader(csv::CSVReader& reader)
		: m_reader(reader)
		, m_mealyStates()
		, m_transitions()
		, m_states()
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

			auto stateRow = MealyStatesRow();
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

	csv::CSVReader& m_reader;

	MealyStates m_mealyStates;
	States m_states;
	Transitions m_transitions;
};

#endif // !AUTOMATA_MEALY_TABLE_READER_HPP_
