#ifndef INICPP_SCHEMA_H
#define INICPP_SCHEMA_H

#include <iostream>
#include <vector>

#include "config.h"
#include "dll.h"
#include "exception.h"
#include "option_schema.h"
#include "section_schema.h"


namespace inicpp
{
	/** Forward declaration, stated because of ring dependencies */
	class config;

	/**
	 * Represents base point of schema validation.
	 * Object of config class is validated against this one.
	 * Can contain schemes of sections and in them schemes for options.
	 */
	class INICPP_API schema
	{
	private:
		using sect_schema_vector = std::vector<std::shared_ptr<section_schema>>;
		using sect_schema_map = std::map<std::string, std::shared_ptr<section_schema>>;
		using sect_schema_map_pair = std::pair<std::string, std::shared_ptr<section_schema>>;

		/** Container for section_schema objects */
		sect_schema_vector sections_;
		/** Map of section_schema object for better searching by name */
		sect_schema_map sections_map_;

	public:
		/**
		 * Default constructor.
		 */
		schema();
		/**
		 * Copy constructor.
		 */
		schema(const schema &source);
		/**
		 * Copy assignment.
		 */
		schema &operator=(const schema &source);
		/**
		 * Move constructor.
		 */
		schema(schema &&source);
		/**
		 * Move assignment.
		 */
		schema &operator=(schema &&source);

		/**
		 * Adds section from given attribute to internal container.
		 * @param sect_schema constant reference to section_schema object
		 * @throws ambiguity_exception if section_schema with given name exists
		 */
		void add_section(const section_schema &sect_schema);
		/**
		 * From given section_schema_params structure
		 * section_schema is created and added to this scheme.
		 * @param arguments non-editable reference to input arguments
		 * @throws ambiguity_exception if section_schema with given name exists
		 */
		void add_section(const section_schema_params &arguments);

		/**
		 * Adds option to the section_schema with specified name.
		 * @param section_name name of existing section
		 * @param opt_schema options_schema which will be added to section
		 * @throws not_found_exception if section_name does not exist
		 * @throws ambiguity_exception if option_schema with given name exists
		 */
		void add_option(const std::string &section_name, const option_schema &opt_schema);
		/**
		 * Creates option_schema from given arguments
		 * and adds it to specified section.
		 * @param section_name
		 * @param arguments option_schema creation parameters
		 * @throws not_found_exception if section_name does not exist
		 * @throws ambiguity_exception if option_schema with given name exists
		 */
		template <typename ArgType>
		void add_option(const std::string &section_name, option_schema_params<ArgType> &arguments)
		{
			auto sect_it = sections_map_.find(section_name);
			if (sect_it != sections_map_.end()) {
				option_schema opt_schema(arguments);
				sect_it->second->add_option(opt_schema);
			} else {
				throw not_found_exception(section_name);
			}
		}

		/**
		 * Returns size of section schemas list
		 * @return unsigned integer
		 */
		size_t size() const;
		/**
		 * Access section_schema on specified index.
		 * @param index index of requested value
		 * @return modifiable reference to stored section_schema
		 * @throws not_found_exception if index is out of range
		 */
		section_schema &operator[](size_t index);
		/**
		 * Access constant reference on section_schema on specified index.
		 * @param index index of requested value
		 * @return constant reference to stored section_schema
		 * @throws not_found_exception if index is out of range
		 */
		const section_schema &operator[](size_t index) const;
		/**
		 * Access section_schema with specified name.
		 * @param section_name name of requested section_schema
		 * @return modifiable reference to stored section_schema
		 * @throws not_found_exception if section_schema with given name does not exist
		 */
		section_schema &operator[](const std::string &section_name);
		/**
		 * Access constant reference on section_schema with specified name.
		 * @param section_name name of requested section_schema
		 * @return constant reference to stored section_schema
		 * @throws not_found_exception if section_schema with given name does not exist
		 */
		const section_schema &operator[](const std::string &section_name) const;
		/**
		 * Tries to find section_schema with specified name inside this config.
		 * @param section_name name which is searched
		 * @return true if section_schema with this name is present, false otherwise
		 */
		bool contains(const std::string &section_name) const;

		/**
		 * Validate cfg against this schema in specified mode.
		 * @param cfg configuration which will be validated
		 * @param mode validation mode
		 * @throws validation_exception if schema cannot be validated
		 */
		void validate_config(config &cfg, schema_mode mode) const;

		/**
		 * Classic stream operator for printing this instance to output stream.
		 * @param os output stream
		 * @param schm reference to schema instance to be written
		 * @return reference to output stream which allows chaining
		 */
		INICPP_API friend std::ostream &operator<<(std::ostream &os, const schema &schm);
	};

	INICPP_API std::ostream &operator<<(std::ostream &os, const schema &schm);
} // namespace inicpp

#endif
