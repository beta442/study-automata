#ifndef AUTOMATA_MEALY_MOORE_TABLE_HPP_
#define AUTOMATA_MEALY_MOORE_TABLE_HPP_

#include <list>
#include <map>
#include <vector>

#include "State.hpp"

class MooreTable;

class MealyTable
{
public:
	using MealyStateRow = std::list<MealyState>;
	using MealyStates = std::vector<MealyStateRow>;

	using States = std::list<State>;
	using Transition = State;
	using Transitions = std::list<Transition>;

	MealyTable(const MooreTable& mooreTable);

	MealyTable(const MealyStates& mealyStates, const States& states, const Transitions& transitions)
		: m_mealyStates(mealyStates)
		, m_states(states)
		, m_transitions(transitions)
	{
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

		const auto delimeter = ';';
		for (auto& columnName : rhs.m_states)
		{
			lhs << delimeter << columnName;
		}

		size_t rowIndex = 0;
		for (auto& transition : rhs.m_transitions)
		{
			lhs << '\n';
			lhs << transition;
			for (auto& field : rhs.m_mealyStates[rowIndex++])
			{
				lhs << delimeter << field;
			}
		}

		return (lhs << std::endl);
	}

private:
	void ComputeMealyStatesFromMoore(const MooreTable& mooreTable);

	MealyStates m_mealyStates;
	States m_states;
	Transitions m_transitions;
};

class MooreTable
{
public:
	using Signals = std::list<Signal>;
	using States = std::list<State>;

	using Transition = State;
	using Transitions = std::list<Transition>;

	using MooreStatesRow = std::list<MooreState>;
	using MooreStatesTable = std::vector<MooreStatesRow>;

	MooreTable(const Signals& signals,
		const States& states,
		const Transitions& transitions,
		const MooreStatesTable& mooreTable)
		: m_signals(signals)
		, m_states(states)
		, m_transitions(transitions)
		, m_mooreTable(mooreTable)
	{
	}

	MooreTable(const MealyTable& mealyTable)
		: m_signals()
		, m_states()
		, m_transitions()
		, m_mooreTable()
	{
		GetSignalsFromMealy(mealyTable);
		GetStatesFromMealy(mealyTable);
		GetTransitionsFromMealy(mealyTable);
		ComputeMooreTableWithMealy(mealyTable);
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

	const MooreStatesTable& GetMooreTable() const
	{
		return m_mooreTable;
	}

	friend std::ostream& operator<<(std::ostream& lhs, const MooreTable& rhs)
	{
		std::ostream::sentry sentry(lhs);
		if (!sentry)
		{
			return lhs;
		}

		const auto delimeter = ';';
		for (auto& signal : rhs.m_signals)
		{
			lhs << delimeter << signal;
		}
		lhs << '\n';

		for (auto& state : rhs.m_states)
		{
			lhs << delimeter << state;
		}

		size_t rowIndex = 0;
		for (auto& transition : rhs.m_transitions)
		{
			lhs << '\n';
			lhs << transition;
			if (rhs.m_mooreTable.size() != 0)
			{
				for (auto& field : rhs.m_mooreTable[rowIndex++])
				{
					lhs << delimeter << field;
				}
			}
		}

		return (lhs << std::endl);
	}

private:
	void GetSignalsFromMealy(const MealyTable& mealyTable)
	{
		auto& mealyStates = mealyTable.GetMealyStates();
		for (auto& mealyStatesRow : mealyStates)
		{
			for (auto& field : mealyStatesRow)
			{
				m_signals.emplace_back(field.m_signal);
			}
		}
	}

	void GetStatesFromMealy(const MealyTable& mealyTable)
	{
		auto signalsQuantity = mealyTable.GetStates().size() * mealyTable.GetTransitions().size();

		for (unsigned int i = 0; i < signalsQuantity; ++i)
		{
			m_states.emplace_back(State{ 'q', i });
		}
	}

	void GetTransitionsFromMealy(const MealyTable& mealyTable)
	{
		for (auto& transition : mealyTable.GetTransitions())
		{
			m_transitions.emplace_back(transition);
		}
	}

	void ComputeMooreTableWithMealy(const MealyTable& mealyTable)
	{
		std::map<State, MealyState> stateToMealyState{};

		{
			auto& mealyStatesRows = mealyTable.GetMealyStates();
			auto stateIt = m_states.begin();
			for (auto& mealyStateRow : mealyStatesRows)
			{
				for (auto& mealyState : mealyStateRow)
				{
					stateToMealyState.emplace(std::make_pair(*(stateIt++), mealyState));
				}
			}
		}

		{
			m_mooreTable.reserve(m_transitions.size());
			for (auto& transition : m_transitions)
			{
				m_mooreTable.emplace_back(MooreStatesRow());
			}
		}

		for (auto& state : m_states)
		{
			auto& mealyState = stateToMealyState[state];
			auto& mealyInnerState = mealyState.m_state;

			size_t rowIndex = 0;
			for (auto& transition : m_transitions)
			{
				auto& certainMealyState = mealyTable.GetCertainMealyState(transition, mealyInnerState);
				auto state = std::invoke([&] {
					for (auto& [state, mealyState] : stateToMealyState)
					{
						if (mealyState == certainMealyState)
						{
							return state;
						}
					}
					return State{};
				});

				m_mooreTable[rowIndex++].emplace_back(state);
			}
		}
	}

	Signals m_signals;
	States m_states;
	Transitions m_transitions;
	MooreStatesTable m_mooreTable;
};

MealyTable::MealyTable(const MooreTable& mooreTable)
	: m_mealyStates()
	, m_states(mooreTable.GetStates())
	, m_transitions(mooreTable.GetTransitions())
{
	ComputeMealyStatesFromMoore(mooreTable);
}

void MealyTable::ComputeMealyStatesFromMoore(const MooreTable& mooreTable)
{
	m_mealyStates.reserve(m_transitions.size());
	for (auto& transition : m_transitions)
	{
		m_mealyStates.emplace_back(MealyStateRow());
	}

	auto& mooreTableContent = mooreTable.GetMooreTable();
	auto itStates = m_mealyStates.begin();
	for (auto& row : mooreTableContent)
	{
		for (auto& field : row)
		{
			auto itSignals = mooreTable.GetSignals().begin();
			for (size_t i = 0; i < field.m_state.m_index; ++i)
			{
				++itSignals;
				if (itSignals == mooreTable.GetSignals().end())
				{
					throw std::out_of_range("Failed to fill Mealy Table from Moore");
				}
			}
			(*itStates).emplace_back(MealyState{
				field.m_state, (*itSignals)
			});
		}
		++itStates;
	}
}

#endif // !AUTOMATA_MEALY_MOORE_TABLE_HPP_
