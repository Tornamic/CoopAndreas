#ifndef INICPP_OPTION_H
#define INICPP_OPTION_H

#include <cctype>
#include <iostream>
#include <memory>
#include <vector>

#include "dll.h"
#include "exception.h"
#include "option_schema.h"
#include "string_utils.h"
#include "types.h"


namespace inicpp
{
	/** Forward declaration, stated because of ring dependencies */
	class option_schema;


	namespace
	{
		std::string escape_option_value(const std::string &str)
		{
			std::string result(str);
			if (str.length() > 0 && std::isspace(result[0])) { result.insert(result.begin(), '\\'); }
			if (str.length() > 1 && std::isspace(result[result.length() - 1])) {
				result.insert(result.end() - 1, '\\');
			}

			return result;
		}

		template <typename ReturnType>
		ReturnType convert_single_value(const option_value &value, const std::string &name)
		{
			if (std::holds_alternative<string_ini_t>(value) && !std::is_same_v<string_ini_t, ReturnType>) {
				// Try to parse the value (have string, want typed value)
				try {
					auto v = std::get<string_ini_t>(value);
					return string_utils::parse_string<ReturnType>(v, name);
				} catch (invalid_type_exception &e) {
					throw bad_cast_exception(e.what());
				}
			} else {
				// Try to return the requested type
				return std::get<ReturnType>(value);
			}
		}

		template <> string_ini_t convert_single_value(const option_value &value, const std::string &)
		{
			// disable compiler warning that this function is unused when building the library
			(void) convert_single_value<string_ini_t>;

			try {
				// Try to return the strign
				return std::get<string_ini_t>(value);
			} catch (const std::bad_variant_access &) {
				// It didn't work, convert actual value to string
				std::stringstream s;
				std::visit(overloaded{[&](boolean_ini_t x) { s << (x ? "yes" : "no"); },
							   [&](enum_ini_t x) { s << escape_option_value(static_cast<std::string>(x)); },
							   [&](float_ini_t x) { s << x; },
							   [&](signed_ini_t x) { s << x; },
							   [&](unsigned_ini_t x) { s << x; },
							   [&](string_ini_t x) { s << escape_option_value(x); }},
					value);
				return s.str();
			}
		}
	} // namespace

	/**
	 * Represent ini configuration option.
	 * Can store one element or list of elements.
	 * Stored elements should have only supported types from option_type enum.
	 */
	class INICPP_API option
	{
	private:
		/** Name of this ini option */
		std::string name_;
		/** Values which corresponds with this option */
		std::vector<option_value> values_;
		/** Corresponding option_schema if any */
		std::shared_ptr<option_schema> option_schema_;

	public:
		/**
		 * Default constructor is deleted.
		 */
		option() = delete;
		/**
		 * Copy constructor.
		 */
		option(const option &source);
		/**
		 * Copy assignment.
		 */
		option &operator=(const option &source);
		/**
		 * Move constructor.
		 */
		option(option &&source);
		/**
		 * Move assignment.
		 */
		option &operator=(option &&source);

		/**
		 * Construct ini option with specified value of specified type.
		 * @param name name of newly created option
		 * @param value initial value
		 */
		option(const std::string &name, const std::string &value = "");
		/**
		 * Construct ini option with specified value of specified list type.
		 * @param name name of newly created option
		 * @param values initial value
		 */
		option(const std::string &name, const std::vector<std::string> &values);

		/**
		 * Gets this option name.
		 * @return constant reference to name
		 */
		const std::string &get_name() const;

		/**
		 * Gets current type of inner value.
		 * @return current type
		 */
		template <typename ValueType> bool holds_type() const
		{
			if (values_.empty()) { return true; }
			return std::holds_alternative<ValueType>(values_[0]);
		}
		/**
		 * Determines if option is list or not.
		 * @return true if option is list, false otherwise
		 */
		bool is_list() const;

		/**
		 * Set single element value.
		 * If option cointaned list of values,
		 *   than its transformed to single value.
		 * @param value newly assigned option value
		 */
		template <typename ValueType> void set(ValueType value)
		{
			this->operator=(value);
		}
		/**
		 * Overloaded alias for set() function.
		 * @param arg boolean_t
		 * @return reference to this
		 */
		option &operator=(boolean_ini_t arg);
		/**
		 * Overloaded alias for set() function.
		 * @param arg signed_t
		 * @return reference to this
		 */
		option &operator=(signed_ini_t arg);
		/**
		 * Overloaded alias for set() function.
		 * @param arg unsigned_t
		 * @return reference to this
		 */
		option &operator=(unsigned_ini_t arg);
		/**
		 * Overloaded alias for set() function.
		 * @param arg float_t
		 * @return reference to this
		 */
		option &operator=(float_ini_t arg);
		/**
		 * Overloaded alias for set() function.
		 * @param arg string_t
		 * @return reference to this
		 */
		option &operator=(const char *arg);
		/**
		 * Overloaded alias for set() function.
		 * @param arg string_t
		 * @return reference to this
		 */
		option &operator=(string_ini_t arg);
		/**
		 * Overloaded alias for set() function.
		 * @param arg enum_t
		 * @return reference to this
		 */
		option &operator=(enum_ini_t arg);

		/**
		 * Get single element value.
		 * If option value is list, than return first element of array.
		 * @return templated copy by value
		 * @throws bad_cast_exception if internal type cannot be casted
		 * @throws not_found_exception if there is no value
		 */
		template <typename ReturnType> ReturnType get() const
		{
			if (values_.empty()) { throw not_found_exception(0); }

			// Get the value and try to convert it
			return convert_single_value<ReturnType>(values_[0], get_name());
		}

		/**
		 * Set internal list of values to given one.
		 * If option contained single value than its transformed to list
		 *   with given values.
		 * @param list reference to list of new values
		 * @throws bad_cast_exception if ValueType cannot be casted
		 * to internal type
		 */
		template <typename ValueType> void set_list(const std::vector<ValueType> &list)
		{
			values_.clear();
			for (auto &&item : list) { add_to_list(item); }
		}

		/**
		 * Get list of internal values. Returning list is newly created.
		 * If option contains single value than its returned list
		 *   with one element.
		 * @return new list of all stored values
		 * @throws bad_cast_exception if internal type cannot be casted
		 * @throws not_found_exception if there is no value
		 */
		template <typename ReturnType> std::vector<ReturnType> get_list() const
		{
			if (values_.empty()) { throw not_found_exception(0); }
			std::vector<ReturnType> results;
			for (const auto &value : values_) {
				results.push_back(convert_single_value<ReturnType>(value, get_name()));
			}

			return results;
		}

		/**
		 * Adds element to internal value list.
		 * If option was single value than its transformed to list.
		 * @param value pushed value
		 * @throws bad_cast_exception if ValueType cannot be casted
		 */
		template <typename ValueType> void add_to_list(ValueType value)
		{
			if (!holds_type<ValueType>()) { throw bad_cast_exception("Cannot cast to requested type"); }
			option_value new_option_value = value;
			values_.push_back(std::move(new_option_value));
		}

		/**
		 * Add element to list on specified position.
		 * If option was single value than its transformed to list.
		 * @param value added value
		 * @param position position in internal list
		 * @throws bad_cast_exception if ValueType cannot be casted
		 * @throws not_found_exception if position is not in internal list
		 */
		template <typename ValueType> void add_to_list(ValueType value, size_t position)
		{
			if (!holds_type<ValueType>()) { throw bad_cast_exception("Cannot cast to requested type"); }
			if (position > values_.size()) { throw not_found_exception(position); }
			option_value new_option_value = value;
			auto p = static_cast<std::iterator_traits<decltype(values_.begin())>::difference_type>(position);
			values_.insert(std::next(values_.begin(), p), std::move(new_option_value));
		}

		/**
		 * Remove element with same value as given one.
		 * @param value
		 * @throws bad_cast_exception if ValueType cannot be casted
		 */
		template <typename ValueType> void remove_from_list(ValueType value)
		{
			if (!holds_type<ValueType>()) { throw bad_cast_exception("Cannot cast to requested type"); }
			for (auto it = values_.cbegin(); it != values_.cend(); ++it) {
				if (std::get<ValueType>(*it) == value) {
					values_.erase(it);
					break;
				}
			}
		}

		/**
		 * Remove list element on specified position.
		 * @param position
		 * @throws not_found_exception in case of out of range
		 */
		void remove_from_list_pos(size_t position);

		/**
		 * Validate this option against given option_schema.
		 * @param opt_schema validation schema
		 * @throws validation_exception if error occured
		 */
		void validate(const option_schema &opt_schema);

		/**
		 * Equality operator.
		 * @param other
		 * @return
		 */
		bool operator==(const option &other) const;
		/**
		 * Inequality operator.
		 * @param other
		 * @return
		 */
		bool operator!=(const option &other) const;

		/**
		 * Classic stream operator for printing this instance to output stream.
		 * @param os output stream
		 * @param opt reference to option instance to be written
		 * @return reference to output stream which allows chaining
		 */
		INICPP_API friend std::ostream &operator<<(std::ostream &os, const option &opt);
	};

	INICPP_API std::ostream &operator<<(std::ostream &os, const option &opt);
} // namespace inicpp

#endif
