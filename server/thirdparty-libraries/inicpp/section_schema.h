#ifndef INICPP_SECTION_SCHEMA_H
#define INICPP_SECTION_SCHEMA_H

#include <iostream>
#include <vector>

#include "dll.h"
#include "exception.h"
#include "option_schema.h"
#include "section.h"
#include "types.h"


namespace inicpp
{
	/**
	 * Creation arguments which has to be supplied to section_schema.
	 */
	struct section_schema_params {
		/** Name of section_schema */
		std::string name;
		/** Determines whether this section is mandatory in configuration */
		item_requirement requirement = item_requirement::mandatory;
		/** Description of section */
		std::string comment;
	};


	/** Forward declaration, stated because of ring dependencies */
	class section;


	/**
	 * In section_schema are defined rules which ini section should fulfil.
	 * Section can be validated against section_schema objects.
	 */
	class INICPP_API section_schema
	{
	private:
		using opt_schema_vector = std::vector<std::shared_ptr<option_schema>>;
		using opt_schema_map = std::map<std::string, std::shared_ptr<option_schema>>;
		using opt_schema_map_pair = std::pair<std::string, std::shared_ptr<option_schema>>;

		/** Section name */
		std::string name_;
		/** Determines if section is mandatory or not */
		item_requirement requirement_;
		/** Description of this section */
		std::string comment_;

		/** Options stored in this section */
		opt_schema_vector options_;
		/** Options stored in map for better searching */
		opt_schema_map options_map_;

	public:
		/**
		 * Default constructor is deleted.
		 */
		section_schema() = delete;
		/**
		 * Copy constructor.
		 */
		section_schema(const section_schema &source);
		/**
		 * Copy assignment.
		 */
		section_schema &operator=(const section_schema &source);
		/**
		 * Move constructor.
		 */
		section_schema(section_schema &&source);
		/**
		 * Move assignment.
		 */
		section_schema &operator=(section_schema &&source);

		/**
		 * Construct section_schema from given arguments.
		 * @param arguments creation parameters
		 */
		section_schema(const section_schema_params &arguments);

		/**
		 * Gets name of section.
		 * @return constant reference
		 */
		const std::string &get_name() const;
		/**
		 * Determines whether this section is mandatory.
		 * @return true if section has to be in configuration
		 */
		bool is_mandatory() const;
		/**
		 * Get textual description of section.
		 * @return constant reference
		 */
		const std::string &get_comment() const;

		/**
		 * Add option_schema to options list from given parameter.
		 * @param opt option_schema which will be added to this instance
		 * @throws ambiguity_exception if option_schema with given name exists
		 */
		void add_option(const option_schema &opt);
		/**
		 * Creates option_schema from given arguments and add it to options list.
		 * @param arguments creation paramaters
		 * @throws ambiguity_exception if option_schema with given name exists
		 */
		template <typename ArgType> void add_option(const option_schema_params<ArgType> &arguments)
		{
			auto add_it = options_map_.find(arguments.name);
			if (add_it == options_map_.end()) {
				std::shared_ptr<option_schema> add = std::make_shared<option_schema>(arguments);
				options_.push_back(add);
				options_map_.insert(opt_schema_map_pair(add->get_name(), add));
			} else {
				throw ambiguity_exception(arguments.name);
			}
		}
		/**
		 * Remove containing option schema of given name.
		 * @param name name of option schema to be removed
		 * @throws not_found_exception if given option does not exist
		 */
		void remove_option(const std::string &name);

		/**
		 * Returns size of option schemas list
		 * @return unsigned integer
		 */
		size_t size() const;
		/**
		 * Access option_schema on specified index.
		 * @param index
		 * @return modifiable reference to stored option_schema
		 * @throws not_found_exception in case of out of range
		 */
		option_schema &operator[](size_t index);
		/**
		 * Access constant reference on option_schema specified index.
		 * @param index
		 * @return constant reference to stored option_schema
		 * @throws not_found_exception in case of out of range
		 */
		const option_schema &operator[](size_t index) const;
		/**
		 * Access option_schema with specified name
		 * @param option_name
		 * @return modifiable reference to stored option_schema
		 * @throws not_found_exception if option_schema with given name does not exist
		 */
		option_schema &operator[](const std::string &option_name);
		/**
		 * Access constant reference on option_schema with specified name
		 * @param option_name
		 * @return constant reference to stored option_schema
		 * @throws not_found_exception if option_schema with given name does not exist
		 */
		const option_schema &operator[](const std::string &option_name) const;
		/**
		 * Tries to find option_schema with specified name inside this section.
		 * @param option_name name which is searched
		 * @return true if option_schema with this name is present, false otherwise
		 */
		bool contains(const std::string &option_name) const;

		/**
		 * Validate given section againts this section_schema.
		 * @param sect validated section
		 * @param mode validation mode
		 * @throws validation_exception if schema cannot be validated
		 */
		void validate_section(section &sect, schema_mode mode) const;

		/**
		 * To given output stream writes additional information about section.
		 * Written info is in ini comment format.
		 * @param os output stream
		 * @return reference to output stream for better chaining
		 */
		std::ostream &write_additional_info(std::ostream &os) const;
		/**
		 * To given output stream writes name of this section.
		 * Written name is in ini format.
		 * @param os output stream
		 * @return reference to output stream for better chaining
		 */
		std::ostream &write_section_name(std::ostream &os) const;
		/**
		 * Classic stream operator for printing this instance to output stream.
		 * @param os output stream
		 * @param sect_schema reference to section_schema instance to be written
		 * @return reference to output stream which allows chaining
		 */
		INICPP_API friend std::ostream &operator<<(std::ostream &os, const section_schema &sect_schema);
	};

	INICPP_API std::ostream &operator<<(std::ostream &os, const section_schema &sect_schema);
} // namespace inicpp

#endif
