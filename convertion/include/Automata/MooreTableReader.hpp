#ifndef AUTOMATA_MOORE_TABLE_READER_HPP_
#define AUTOMATA_MOORE_TABLE_READER_HPP_

#include <list>
#include <vector>

#include "../CSV/csv.hpp"
#include "State.hpp"

class MooreTableReader
{
public:
	using Signals = std::vector<Signal>;
	using States = std::list<State>;
	using Transitions = std::list<State>;

	using MooreStatesRow = std::list<MooreState>;
	using MooreStatesTable = std::vector<MooreStatesRow>;

	MooreTableReader(csv::CSVReader& reader)
		: m_reader(reader)
		, m_signals()
		, m_states()
		, m_transitions()
		, m_mooreTable()
	{
		ReadSignals();
		ReadStates();
		ReadRows();
	}

	const Signals& GetSignals() const
	{
		return m_signals;
	}

	const States& GetStates() const
	{
		return m_states;
	}

	const Transitions& GetTransitions() const
	{
		return m_transitions;
	}

	const MooreStatesTable& GetMooreStates() const
	{
		return m_mooreTable;
	}

private:
	void ReadSignals()
	{
		for (auto& cName : m_reader.get_col_names())
		{
			if (cName.empty())
			{
				continue;
			}
			m_signals.emplace_back(Signal{ cName });
		}
	}

	void ReadStates()
	{
		csv::CSVRow row;
		m_reader.read_row(row);

		for (auto& field : row)
		{
			if (auto fieldContent = field.get_sv(); !fieldContent.empty())
			{
				m_states.emplace_back(State{ fieldContent });
			}
		}
	}

	void ReadRows()
	{
		for (auto& row : m_reader)
		{
			if (auto transitionContent = row[0].get_sv(); !transitionContent.empty())
			{
				m_transitions.emplace_back(State{ transitionContent });
			}

			auto stateRow = MooreStatesRow();
			auto i = 0;
			for (auto& field : row)
			{
				if (i == 0)
				{
					++i;
					continue;
				}
				stateRow.emplace_back(MooreState{ field.get_sv() });
			}
			m_mooreTable.emplace_back(std::move(stateRow));
		}
	}

	csv::CSVReader& m_reader;

	Signals m_signals;
	States m_states;
	Transitions m_transitions;
	MooreStatesTable m_mooreTable;
};

#endif // !AUTOMATA_MOORE_TABLE_READER_HPP_
