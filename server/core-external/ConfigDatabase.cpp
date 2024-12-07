
#include "ConfigDatabase.hpp"



void ConfigDatabase::Init(unsigned int &var, const std::string &filename)
{
	if(ConfigDatabase::Create(filename))
   	{
	    if(ConfigDatabase::Load(var, filename))
   	    {
   	        std::cout << "\n[*] : " << filename << " Has Been Created (Please Shutdown Server And Edit " << filename << "!" << '\n'; 
   	    }else { ConfigDatabase::Create(filename); }
   	}else 
   	{
   	    if(ConfigDatabase::Load(var, filename))
   	    {
    	       std::cout << "\n[*] : " << filename << " Has Been Loaded !" << '\n'; 
   	    }
   	}
}

bool ConfigDatabase::Create(const std::string &filename)
{
	std::ifstream file(filename.c_str());
	if(!file.good()) 
	{
		/*mINI::INIFile file(filename);

		mINI::INIStructure ini;

		//ini["Server Settings"][""]
		ini["Server Settings"]["Port"] = "0";

		file.generate(ini);*/
		inicpp::schema schema;


		inicpp::section_schema_params section_ini_schema;
		section_ini_schema.comment = "This Section For Server Variables Configurations like Port and Ip";
		section_ini_schema.name = "Server Settings";
		section_ini_schema.requirement = inicpp::item_requirement::mandatory;
		schema.add_section(section_ini_schema);

		inicpp::option_schema_params<inicpp::signed_ini_t> option_ini_schema;
		option_ini_schema.name = "Port";
		option_ini_schema.requirement = inicpp::item_requirement::mandatory;
		option_ini_schema.type = inicpp::option_item::single;
		option_ini_schema.default_value = "0";
		schema.add_option("Server Settings", option_ini_schema);

		std::ostringstream str;
		str << schema;
		std::cout << str.str();
		std::string file_stream = str.str();
		//file.open(str.str(), std::ios_base::in);
		FILE* file_o = fopen(filename.c_str(), "w");	
		if(file_o)
		{
			fwrite(file_stream.c_str(), 1, strlen(file_stream.c_str()), file_o);
			fclose(file_o);
			return true;
		}
	}
	return false;
}

bool ConfigDatabase::Load(unsigned int &var, const std::string &filename)
{
	std::ifstream file(filename.c_str());
	if(file.good())
	{

		/*mINI::INIFile file(filename);

		mINI::INIStructure ini;

		//auto port = ini["Server Settings"]["Port"];
		var = static_cast<unsigned int>(std::strtoul(ini["Server Settings"]["Port"].c_str(), 0, 10));

		file.read(ini);*/

		inicpp::config conf = inicpp::parser::load_file(filename);

		inicpp::unsigned_ini_t var = conf["Server Settings"]["Port"].get<inicpp::unsigned_ini_t>();


		return true;
	}
	else 
		printf("ERROR");
	return false;
}