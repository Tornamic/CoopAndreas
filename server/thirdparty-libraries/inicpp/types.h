#ifndef INICPP_TYPES_H
#define INICPP_TYPES_H

#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>
#include <cstdint>


namespace inicpp
{
	/**
	 * Inicpp enumeration type.
	 */
	class internal_enum_type
	{
	public:
		/** Default constructor */
		internal_enum_type() : internal_enum_type("")
		{
		}
		/** Constructor with initial value */
		internal_enum_type(const std::string &value) : data_(value)
		{
		}
		/** Constructor with initial value */
		internal_enum_type(const char *value) : data_(value)
		{
		}
		/** Copy constructor */
		internal_enum_type(const internal_enum_type &other) : data_()
		{
			this->operator=(other);
		}
		/** Conversion contructor - only for template compilation, allways throws std::runtime_error */
		[[noreturn]] explicit internal_enum_type(bool) : data_()
		{
			throw std::runtime_error("");
		}
		/** Conversion contructor - only for template compilation, allways throws std::runtime_error */
		[[noreturn]] explicit internal_enum_type(int64_t) : data_()
		{
			throw std::runtime_error("");
		}
		/** Conversion contructor - only for template compilation, allways throws std::runtime_error */
		[[noreturn]] explicit internal_enum_type(uint64_t) : data_()
		{
			throw std::runtime_error("");
		}
		/** Conversion contructor - only for template compilation, allways throws std::runtime_error */
		[[noreturn]] explicit internal_enum_type(double) : data_()
		{
			throw std::runtime_error("");
		}

		/** Assignment operator */
		internal_enum_type &operator=(const internal_enum_type &other)
		{
			data_ = other.data_;
			return *this;
		}
		/** Conversion operator to std::string type */
		operator std::string() const
		{
			return data_;
		}
		/**
		 * Conversion operator to double type - allways throws, implemented
		 * because of some template usage.
		 * @throws allways std::runtime_error exception
		 */
		operator double() const
		{
			throw std::runtime_error("Enum type cannot be converted to double");
		}
		/** Equality operator */
		bool operator==(const internal_enum_type &other)
		{
			return data_ == other.data_;
		}
		/** Inequality operator */
		bool operator!=(const internal_enum_type &other)
		{
			return !(*this == other);
		}
		/** Comparation less operator */
		bool operator<(const internal_enum_type &other)
		{
			return data_ < other.data_;
		}

	private:
		/** Value of instance */
		std::string data_;
	};


	// modern C++11 way of typedef
	using boolean_ini_t = bool;
	using signed_ini_t = int64_t;
	using unsigned_ini_t = uint64_t;
	using float_ini_t = double;
	using enum_ini_t = internal_enum_type;
	using string_ini_t = std::string;

	using option_value =
		std::variant<boolean_ini_t, signed_ini_t, unsigned_ini_t, float_ini_t, enum_ini_t, string_ini_t>;

	// helper type for the type visitor
	template <class... Ts> struct overloaded : Ts... {
		using Ts::operator()...;
	};
	template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

	/**
	 * Enumeration type used in schema specification which distinguishes
	 * between single item and list of items.
	 */
	enum class option_item : bool { single, list };

	/**
	 * Type used to distinguish between mandatory and optional values
	 * in schema specification.
	 */
	enum class item_requirement : bool { mandatory, optional };

	/**
	 * Mode in which schema can be validated.
	 *  Strict - everything has to be right and in appropriate type.
	 *  Relaxed - config can contain unknown sections and options.
	 */
	enum class schema_mode : bool { strict, relaxed };

} // namespace inicpp

#endif // INICPP_TYPES_H
