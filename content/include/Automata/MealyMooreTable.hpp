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

	using Signals = std::vector<Signal>;
	using OutSignalColumns = std::list<Signals>;

	MealyTable(const MooreTable& mooreTable);

	MealyTable(const States& states, const Transitions& transitions, const MealyStates& mealyStates)
		: m_mealyStates(mealyStates)
		, m_states(states)
		, m_transitions(transitions)
	{
	}

	void Minimize()
	{
		RemoveUnreachableStates();

		/*using Group = State;
		std::map<State, Group> stateToGroup = 
		auto signalColumns = CollectSignalColumns();

		signalColumns.unique([columnIndex = 0](const auto& lhs, const auto& rhs) mutable {

		});*/
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
		std::advance(begIt, columnIndex);

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

	std::vector<size_t> GetUnreachableStatesIndexes() const
	{
		std::vector<size_t> result{};
		result.reserve(m_states.size());

		size_t stateIndex{};
		for (const auto& state : m_states)
		{
			bool hasTransitionFromOtherStates = true;
			for (const auto& row : m_mealyStates)
			{
				hasTransitionFromOtherStates = std::any_of(row.begin(), row.end(),
					[&state, mealyStateIndex = 0, stateIndex](const auto& mealyState) mutable noexcept {
						bool areStatesEqual = mealyState.m_state == state;
						bool isTransitionFromOtherState = mealyStateIndex++ != stateIndex;

						return areStatesEqual && isTransitionFromOtherState;
					});
				if (hasTransitionFromOtherStates)
				{
					break;
				}
			}
			if (!hasTransitionFromOtherStates)
			{
				result.push_back(stateIndex);
			}
			++stateIndex;
		}

		result.shrink_to_fit();
		return result;
	}

	void EraseCertainColumns(const std::vector<size_t>& indexes)
	{
		if (indexes.empty())
		{
			return;
		}

		for (auto& row : m_mealyStates)
		{
			size_t erasedCounter{};
			for (const auto& index : indexes)
			{
				auto itToErase = row.begin();
				std::advance(itToErase, index - erasedCounter++);
				row.erase(itToErase);
			}
		}
	}

	void EraseCertainStates(const std::vector<size_t>& indexes)
	{
		size_t erasedCounter{};
		for (const auto& index : indexes)
		{
			auto itToErase = m_states.begin();
			std::advance(itToErase, index - erasedCounter++);
			m_states.erase(itToErase);
		}
	}

	OutSignalColumns CollectSignalColumns() const
	{
		OutSignalColumns res{};

		for (size_t i = 0; i < m_mealyStates.front().size(); ++i)
		{
			res.emplace_back(Signals{});
		}

		for (const auto& row : m_mealyStates)
		{
			auto resIt = res.begin();
			for (const auto& mealyState : row)
			{
				(*resIt++).emplace_back(mealyState.m_signal);
			}
		}

		return res;
	}

	void RemoveUnreachableStates()
	{
		auto columnIndexesToIgnore{ std::move(GetUnreachableStatesIndexes()) };

		if (!columnIndexesToIgnore.empty())
		{
			EraseCertainStates(columnIndexesToIgnore);
			EraseCertainColumns(columnIndexesToIgnore);
		}
	}

	MealyStates m_mealyStates;
	States m_states;
	Transitions m_transitions;
};

class MooreTable
{
public:
	using Signals = std::vector<Signal>;
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
		, m_stateToMealyState()
	{
		CollectStatesFromMealy(mealyTable);
		CollectSignalsFromMealy(mealyTable);
		CollectTransitionsFromMealy(mealyTable);
		ComputeMooreTableWithMealy(mealyTable);
	}

	void Minimize()
	{
		RemoveUnreachableStates();
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
	void CollectSignalsFromMealy(const MealyTable& mealyTable)
	{
		m_signals.reserve(m_stateToMealyState.size());
		for (auto& [_state, _mealyState] : m_stateToMealyState)
		{
			m_signals.emplace_back(_mealyState.m_signal);
		}
	}

	void CollectStatesFromMealy(const MealyTable& mealyTable)
	{
		std::list<MealyState> uniqueMealyStates{};
		for (auto& stateList : mealyTable.GetMealyStates())
		{
			for (auto& state : stateList)
			{
				uniqueMealyStates.emplace_back(state);
			}
		}
		uniqueMealyStates.sort();
		uniqueMealyStates.unique();

		unsigned int index = 0;
		for (auto& uMState : uniqueMealyStates)
		{
			auto state = State{ 'q', index++ };
			m_stateToMealyState.emplace(std::make_pair(state, uMState));
			m_states.emplace_back(std::move(state));
		}
	}

	void CollectTransitionsFromMealy(const MealyTable& mealyTable)
	{
		for (auto& transition : mealyTable.GetTransitions())
		{
			m_transitions.emplace_back(transition);
		}
	}

	void ComputeMooreTableWithMealy(const MealyTable& mealyTable)
	{
		if (m_states.empty())
		{
			throw std::logic_error("Failed to fill Moore table from Mealy.");
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
			auto& mealyState = m_stateToMealyState[state];
			auto& mealyInnerState = mealyState.m_state;

			size_t rowIndex = 0;
			for (auto& transition : m_transitions)
			{
				auto& certainMealyState = mealyTable.GetCertainMealyState(transition, mealyInnerState);
				auto _state = std::invoke([&] {
					for (auto& [_state, mealyState] : m_stateToMealyState)
					{
						if (mealyState == certainMealyState)
						{
							return _state;
						}
					}

					throw std::logic_error("Failed to fill Moore table from Mealy. Not enough states");
				});

				m_mooreTable[rowIndex++].emplace_back(_state);
			}
		}
	}

	void EraseCertainColumns(const std::vector<size_t>& columnIndexes)
	{
		if (columnIndexes.empty())
		{
			return;
		}

		for (auto& row : m_mooreTable)
		{
			size_t erasedCounter{};
			for (const auto& index : columnIndexes)
			{
				auto itToErase = row.begin();
				std::advance(itToErase, index - erasedCounter++);
				row.erase(itToErase);
			}
		}
	}

	void EraseCertainSignals(const std::vector<size_t>& statesIndexes)
	{
		size_t erasedCounter{};
		for (const auto& index : statesIndexes)
		{
			auto itToErase = m_signals.begin();
			std::advance(itToErase, index - erasedCounter++);
			m_signals.erase(itToErase);
		}
	}

	void EraseCertainStates(const std::vector<size_t>& statesIndexes)
	{
		size_t erasedCounter{};
		for (const auto& index : statesIndexes)
		{
			auto itToErase = m_states.begin();
			std::advance(itToErase, index - erasedCounter++);
			m_states.erase(itToErase);
		}
	}

	std::vector<size_t> GetUnreachableStatesIndexes() const
	{
		std::vector<size_t> result{};

		size_t stateIndex{};
		for (const auto& state : m_states)
		{
			bool hasTransitionFromOtherStates = true;
			for (const auto& row : m_mooreTable)
			{
				hasTransitionFromOtherStates = std::any_of(row.begin(), row.end(),
					[&state, mealyStateIndex = 0, stateIndex](const auto& mooreState) mutable noexcept {
						bool areStatesEqual = mooreState.m_state == state;
						bool isTransitionFromOtherState = mealyStateIndex++ != stateIndex;

						return areStatesEqual && isTransitionFromOtherState;
					});
				if (hasTransitionFromOtherStates)
				{
					break;
				}
			}
			if (!hasTransitionFromOtherStates)
			{
				result.push_back(stateIndex);
			}
			++stateIndex;
		}

		return result;
	}

	void RemoveUnreachableStates()
	{
		auto columnIndexesToIgnore{ std::move(GetUnreachableStatesIndexes()) };

		if (!columnIndexesToIgnore.empty())
		{
			EraseCertainSignals(columnIndexesToIgnore);
			EraseCertainStates(columnIndexesToIgnore);
			EraseCertainColumns(columnIndexesToIgnore);
		}
	}

	std::map<State, MealyState> m_stateToMealyState;

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

	auto& mooreTableContent = mooreTable.GetMooreStates();
	const auto& mooreSignals = mooreTable.GetSignals();

	auto itStates = m_mealyStates.begin();
	for (auto& row : mooreTableContent)
	{
		for (auto& field : row)
		{
			if (field.m_state.m_index >= mooreSignals.size())
			{
				throw std::out_of_range("Failed to fill Mealy Table from Moore. Not enough signals");
			}
			(*itStates).emplace_back(MealyState{
				field.m_state, mooreSignals[field.m_state.m_index] });
		}
		++itStates;
	}
}

#endif // !AUTOMATA_MEALY_MOORE_TABLE_HPP_
