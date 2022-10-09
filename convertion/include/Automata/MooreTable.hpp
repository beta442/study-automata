#ifndef AUTOMATA_MOORETABLE_HPP
#define AUTOMATA_MOORETABLE_HPP

#include <map>
#include <vector>

#include "State.hpp"

#include "MealyTable.hpp"

class MooreTable
{
public:
	using Signals = std::vector<Signal>;
	using States = std::vector<State>;
	using Transitions = std::vector<State>;

	using MooreStatesRow = std::list<MooreState>;
	using MooreStatesTable = std::vector<MooreStatesRow>;

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

	friend std::ostream& operator<<(std::ostream& lhs, const MooreTable& rhs)
	{
		std::ostream::sentry sentry(lhs);
		if (!sentry)
		{
			return lhs;
		}

		for (auto& signal : rhs.m_signals)
		{
			lhs << rhs.m_delimeter << signal;
		}
		lhs << '\n';

		for (auto& state : rhs.m_states)
		{
			lhs << rhs.m_delimeter << state;
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
					lhs << rhs.m_delimeter << field;
				}
			}
		}

		return (lhs << std::endl);
	}

private:
	void GetSignalsFromMealy(const MealyTable& mealyTable)
	{
		auto signalsQuantity = mealyTable.GetStates().size() * mealyTable.GetTransitions().size();
		m_signals.reserve(signalsQuantity);

		auto& mealyStates = mealyTable.GetMealyStates();
		for (auto& mealyStatesRow : mealyStates)
		{
			for (auto& field : mealyStatesRow)
			{
				m_signals.push_back(field.m_signal);
			}
		}
	}

	void GetStatesFromMealy(const MealyTable& mealyTable)
	{
		auto signalsQuantity = mealyTable.GetStates().size() * mealyTable.GetTransitions().size();
		m_states.reserve(signalsQuantity);

		for (unsigned int i = 0; i < signalsQuantity; ++i)
		{
			m_states.emplace_back(State{ 'q', i });
		}
	}

	void GetTransitionsFromMealy(const MealyTable& mealyTable)
	{
		auto transitionsQuantity = mealyTable.GetTransitions().size();
		m_transitions.reserve(transitionsQuantity);

		for (auto& transition : mealyTable.GetTransitions())
		{
			m_transitions.push_back(transition);
		}
	}

	void ComputeMooreTableWithMealy(const MealyTable& mealyTable)
	{
		std::map<State, MealyState> stateToMealyState{};

		{
			auto& mealyStatesRows = mealyTable.GetMealyStates();
			size_t stateCount = 0;
			for (auto& mealyStateRow : mealyStatesRows)
			{
				for (auto& mealyState : mealyStateRow)
				{
					stateToMealyState.emplace(std::make_pair(m_states[stateCount++], mealyState));
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

	char m_delimeter = ';';
};

#endif // !AUTOMATA_MOORETABLE_HPP
