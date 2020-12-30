#ifndef __ARGUMENTS_CLASS_HPP__
#define __ARGUMENTS_CLASS_HPP__

#include <cstdlib>
#include <std_utilities.hpp>
#include <sstream>
#include <iostream>
#include <string.h>
#include <vector>

//! a class to handle binary command line arguments
class arguments
{
public:
	//! constructor, takes as input the commandline arguments from main
	arguments(int argc, char* _argv[]);

	//! for each argument you give:
	//!   (long name, default value, short name (optional), help text (optional), mandatory argument flag (optional) )
	double get_arg_double(std::string arg_name, double default_value,
		std::string arg_name_abbrev = "", std::string help_text ="", bool mandatory = false);
	float get_arg_float(std::string arg_name, float default_value,
		std::string arg_name_abbrev = "", std::string help_text ="", bool mandatory = false);
	size_t get_arg_size_t(std::string arg_name, size_t default_value,
		std::string arg_name_abbrev = "", std::string help_text ="", bool mandatory = false);
	int get_arg_int(std::string arg_name, int default_value,
		std::string arg_name_abbrev = "", std::string help_text ="", bool mandatory = false);
	std::string get_arg_string(std::string arg_name, std::string default_value,
		std::string arg_name_abbrev = "", std::string help_text ="", bool mandatory = false);
	bool get_arg_bool(std::string arg_name, bool default_value,
		std::string arg_name_abbrev = "", std::string help_text ="", bool mandatory = false);
	void display_help(std::string title = "", std::string usage_line= "", std::string info_line = "");

	//! check whether the minumum number of arguments acceptable is acheved, otherwise display help and exit
	void check_min_narguments(size_t n) { if (args.size()<n) { display_help(); std::exit(1); } };

	//! check whether all mandatory arguments are set, otherwise display help and exit
	void check_mandatory_arguments() { if(mandatory_arg_not_set)  { display_help(); std::exit(1); } };

	//! also allow -h and --help as flags to display help. This has to be the last argument
	void get_help_argument();

private:
	//! the input
	std::vector< std::string > args; 

	//! collects all help texts to display help
	std::vector< std::string > help_texts;
	std::vector< std::string > arg_names;
	std::vector< std::string > arg_name_abbrevs;
	std::vector< std::string > default_values;
	std::vector< bool > mandatories;

	//! flag to indicate that a mandatory argument is not set
	bool mandatory_arg_not_set;


};
arguments::arguments(int argc, char* argv[])
{
	mandatory_arg_not_set = false;
	for (int i=1 ; i < argc; i++)
		args.push_back(argv[i]);
}
//---------------------------------------------------------------------------------
void arguments::get_help_argument()
{
	std::string arg_name = "--help";
	std::string arg_name_abbrev = "-h";
	help_texts.push_back("displays help and exits the binary");
	default_values.push_back("none" );
	mandatories.push_back( false );
	arg_names.push_back(arg_name);
	arg_name_abbrevs.push_back(arg_name_abbrev);

	for(size_t k=0;k < args.size(); k++)
		if ( (strcmp(args[k].c_str(),arg_name.c_str())==0) || (strcmp(args[k].c_str(),arg_name_abbrev.c_str())==0) )
			display_help();
}
//---------------------------------------------------------------------------------
double arguments::get_arg_double(std::string arg_name, double default_value,
								 std::string arg_name_abbrev, std::string help_text, bool mandatory)
{
	arg_name = "--"+arg_name;
	arg_name_abbrev = "-"+arg_name_abbrev;
	help_texts.push_back(help_text);
	arg_names.push_back(arg_name);
	default_values.push_back(to_string(default_value) );
	arg_name_abbrevs.push_back(arg_name_abbrev);
	mandatories.push_back( mandatory );

	for(size_t k=0;k < args.size(); k++)
		if ( (strcmp(args[k].c_str(),arg_name.c_str())==0) || (strcmp(args[k].c_str(),arg_name_abbrev.c_str())==0) ) {
			k++; 
			return (double)atof( args[k].c_str() );
		}

		if (mandatory)
			mandatory_arg_not_set = true;

		return default_value;
}
//---------------------------------------------------------------------------------
float arguments::get_arg_float(std::string arg_name, float default_value,
							   std::string arg_name_abbrev, std::string help_text, bool mandatory)
{
	arg_name = "--"+arg_name;
	arg_name_abbrev = "-"+arg_name_abbrev;
	help_texts.push_back(help_text);
	default_values.push_back(to_string(default_value) );
	arg_names.push_back(arg_name);
	arg_name_abbrevs.push_back(arg_name_abbrev);
	mandatories.push_back( mandatory );

	for(size_t k=0;k < args.size(); k++)
		if ( (strcmp(args[k].c_str(),arg_name.c_str())==0) || (strcmp(args[k].c_str(),arg_name_abbrev.c_str())==0) ) {
			k++; 
			return (float)atof( args[k].c_str() );
		}
		if (mandatory)
			mandatory_arg_not_set = true;

		return default_value;
}
//---------------------------------------------------------------------------------
size_t arguments::get_arg_size_t(std::string arg_name, size_t default_value,
								 std::string arg_name_abbrev, std::string help_text, bool mandatory)
{
	arg_name = "--"+arg_name;
	arg_name_abbrev = "-"+arg_name_abbrev;
	help_texts.push_back(help_text);
	default_values.push_back(to_string(default_value) );
	arg_names.push_back(arg_name);
	arg_name_abbrevs.push_back(arg_name_abbrev);
	mandatories.push_back( mandatory );

	for(size_t k=0;k < args.size(); k++)
		if ( (strcmp(args[k].c_str(),arg_name.c_str())==0) || (strcmp(args[k].c_str(),arg_name_abbrev.c_str())==0) ) {
			k++; 
			return (size_t)atoi( args[k].c_str() );
		}
		if (mandatory)
			mandatory_arg_not_set = true;

		return default_value;
}
//---------------------------------------------------------------------------------
int arguments::get_arg_int(std::string arg_name, int default_value,
						   std::string arg_name_abbrev, std::string help_text, bool mandatory)
{
	arg_name = "--"+arg_name;
	arg_name_abbrev = "-"+arg_name_abbrev;
	help_texts.push_back(help_text);
	default_values.push_back(to_string(default_value) );
	arg_names.push_back(arg_name);
	arg_name_abbrevs.push_back(arg_name_abbrev);
	mandatories.push_back( mandatory );

	for(size_t k=0;k < args.size(); k++)
		if ( (strcmp(args[k].c_str(),arg_name.c_str())==0) || (strcmp(args[k].c_str(),arg_name_abbrev.c_str())==0) ) {
			k++; 
			return atoi( args[k].c_str() );
		}
		if (mandatory)
			mandatory_arg_not_set = true;

		return default_value;
}
//---------------------------------------------------------------------------------
std::string arguments::get_arg_string(std::string arg_name, std::string default_value,
									  std::string arg_name_abbrev, std::string help_text, bool mandatory)
{
	arg_name = "--"+arg_name;
	arg_name_abbrev = "-"+arg_name_abbrev;
	help_texts.push_back(help_text);
	default_values.push_back(default_value);
	arg_names.push_back(arg_name);
	arg_name_abbrevs.push_back(arg_name_abbrev);
	mandatories.push_back( mandatory );

	for(size_t k=0;k < args.size(); k++)
		if ( (strcmp(args[k].c_str(),arg_name.c_str())==0) || (strcmp(args[k].c_str(),arg_name_abbrev.c_str())==0) ) {
			k++; 
			return args[k].c_str();
		}
		if (mandatory)
			mandatory_arg_not_set = true;

		return default_value;
}
//---------------------------------------------------------------------------------
bool arguments::get_arg_bool(std::string arg_name, bool default_value,
							 std::string arg_name_abbrev, std::string help_text, bool mandatory)
{
	arg_name = "--"+arg_name;
	arg_name_abbrev = "-"+arg_name_abbrev;
	help_texts.push_back(help_text);
	default_values.push_back(to_string(default_value) );
	arg_names.push_back(arg_name);
	arg_name_abbrevs.push_back(arg_name_abbrev);
	mandatories.push_back( mandatory );

	for(size_t k=0;k < args.size(); k++)
		if ( (strcmp(args[k].c_str(),arg_name.c_str())==0) || (strcmp(args[k].c_str(),arg_name_abbrev.c_str())==0) ) {
			return !(default_value);
		}
		if (mandatory)
			mandatory_arg_not_set = true;

		return default_value;
}
//---------------------------------------------------------------------------------
void arguments::display_help(std::string title, std::string usage_line, std::string info_line)
{
	using namespace std;
	cout << endl;

	if (title.size()>0)
		cout << title << endl;

	if (usage_line.size()>0)
		cout << "Usage: " << usage_line << endl;

	if (info_line.size()>0)
		cout << info_line << endl;

	cout << endl;

	if (in(mandatories,true))
		cout << "Mandatory arguments:" << endl;

	for (size_t i=0; i < help_texts.size(); i++) 
		if (mandatories[i]==true)
			cout << "  " << arg_name_abbrevs[i] << ", " << arg_names[i] << ", " << help_texts[i] << endl
			<< "        (Default value: [" << default_values[i] << "])" << endl; 

	cout << "Optional arguments:" << endl;

	for (size_t i=0; i < help_texts.size(); i++) 
		if (mandatories[i]==false)
			cout << "  " << arg_name_abbrevs[i] << ", " << arg_names[i] << ", " << help_texts[i] << endl
			<< "        (Default value: [" << default_values[i] << "])" << endl; 

	cout << endl;
	exit(0);
}
//---------------------------------------------------------------------------------

#endif //__ARGUMENTS_CLASS_HPP__
