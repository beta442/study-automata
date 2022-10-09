#ifndef AUTOMATA_STATE_HPP_
#define AUTOMATA_STATE_HPP_

#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace state_excps
{

constexpr auto FAILED_CONSTRUCT_SIGNAL_IS_ALPHA_MSG = "Failed to construct Signal. Arg[0] must be isAlpha()";
constexpr auto FAILED_CONSTRUCT_SIGNAL_IS_DIGIT_MSG = "Failed to construct Signal. Arg[1] must be isDigit()";

constexpr auto FAILED_CONSTRUCT_SIGNAL_MSG = "Failed to construct Signal. _1 must contain at least 2 characters";
constexpr auto FAILED_LESS_COMPARE_SIGNAL_MSG = "Can't less-compare signals with different labels";

constexpr auto FAILED_CONSTRUCT_MEALY_MSG = "Failed to construct Mealy's State. _1 must contain at least 5 characters";

}; // namespace state_excps

struct Signal
{
	unsigned int m_index{};
	unsigned char m_label{};

	Signal() = default;

	explicit Signal(unsigned char label, unsigned int index)
		: m_index(index)
		, m_label(label)
	{
	}

	explicit Signal(const Signal& other)
		: m_index(other.m_index)
		, m_label(other.m_label)
	{
	}

	Signal(Signal&& other) noexcept
		: m_index()
		, m_label()
	{
		*this = std::move(other);
	}

	template <typename ST, typename = std::enable_if_t<!std::is_same_v<decltype(std::declval<ST>()[0]), void>>>
	explicit Signal(ST&& src)
		: m_index()
		, m_label()
	{
		if (std::ssize(src) < 2)
		{
			throw std::invalid_argument(state_excps::FAILED_CONSTRUCT_SIGNAL_MSG);
		}

		if (!std::isalpha(src[0]))
		{
			throw std::invalid_argument(state_excps::FAILED_CONSTRUCT_SIGNAL_IS_ALPHA_MSG);
		}
		if (!std::isdigit(src[1]))
		{
			throw std::invalid_argument(state_excps::FAILED_CONSTRUCT_SIGNAL_IS_DIGIT_MSG);
		}

		m_index = static_cast<unsigned char>(src[1]) - '0';
		m_label = static_cast<unsigned char>(src[0]);
	}

	Signal& operator=(const Signal& other)
	{
		if (std::addressof(*this) != &other)
		{
			m_index = other.m_index;
			m_label = other.m_label;
		}

		return *this;
	}

	Signal& operator=(Signal&& other) noexcept
	{
		if (std::addressof(*this) != &other)
		{
			m_index = 0;
			m_label = 0;
			std::swap(m_index, other.m_index);
			std::swap(m_label, other.m_label);
		}

		return *this;
	}

	template <typename T>
	bool operator==(T&& other) const noexcept
	{
		return (m_index == std::forward<decltype(other.m_index)>(other.m_index)) && (m_label == std::forward<decltype(other.m_label)>(other.m_label));
	}

	template <typename T>
	bool operator<(T&& other) const
	{
		bool labelsAreEqual = (m_label == std::forward<decltype(other.m_label)>(other.m_label));
		if (!labelsAreEqual)
		{
			throw std::invalid_argument(state_excps::FAILED_LESS_COMPARE_SIGNAL_MSG);
		}

		return labelsAreEqual && (m_index < std::forward<decltype(other.m_index)>(other.m_index));
	}

	friend std::ostream& operator<<(std::ostream& lhs, const Signal& rhs)
	{
		std::ostream::sentry sentry(lhs);
		if (!sentry)
		{
			return lhs;
		}

		lhs << rhs.m_label << rhs.m_index;

		return lhs;
	}
};

using State = Signal;

struct MooreState
{
	State m_state{};

	MooreState() = default;

	explicit MooreState(const State& state)
		: m_state(state)
	{
	}

	template <typename ST>
	explicit MooreState(ST&& src)
		: m_state(src)
	{
	}

	template <typename T>
	bool operator==(T&& other) const noexcept
	{
		return m_state == std::forward<decltype(other.m_state)>(other.m_state);
	}

	template <typename T>
	bool operator<(T&& other) const
	{
		return m_state < std::forward<decltype(other.m_state)>(other.m_state);
	}

	friend std::ostream& operator<<(std::ostream& lhs, const MooreState& rhs)
	{
		std::ostream::sentry sentry(lhs);
		if (!sentry)
		{
			return lhs;
		}

		lhs << rhs.m_state;

		return lhs;
	}
};

struct MealyState
{
	State m_state{};
	Signal m_signal{};

	MealyState() = default;

	template <typename ST>
	explicit MealyState(ST&& src)
		: m_state()
		, m_signal()
	{
		if (std::ssize(src) < 5)
		{
			throw std::invalid_argument(state_excps::FAILED_CONSTRUCT_MEALY_MSG);
		}

		m_state = std::move(State(src));

		if constexpr (std::is_same_v<char*, ST>)
		{
			m_signal = std::move(Signal(src + 3));
		}
		else if constexpr (std::is_same_v<std::string, ST> || std::is_same_v<std::string_view, ST>)
		{
			m_signal = std::move(Signal(src.substr(3, 2)));
		}
	}

	template <typename ST1, typename ST2>
	explicit MealyState(ST1&& state, ST2&& signal)
		: m_state(state)
		, m_signal(signal)
	{
	}

	template <typename T>
	bool operator==(T&& other) const noexcept
	{
		return (m_state == std::forward<decltype(other.m_state)>(other.m_state)) && (m_signal == std::forward<decltype(other.m_signal)>(other.m_signal));
	}

	friend std::ostream& operator<<(std::ostream& lhs, const MealyState& rhs)
	{
		std::ostream::sentry sentry(lhs);
		if (!sentry)
		{
			return lhs;
		}

		lhs << rhs.m_state << '/' << rhs.m_signal;

		return lhs;
	}
};

#endif // !AUTOMATA_STATE_HPP_
