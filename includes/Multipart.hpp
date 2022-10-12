#pragma once
#include "Parser.hpp"

class File_Multipart
{
	public:
		File_Multipart();
		File_Multipart(const std::string& var);
		File_Multipart(const File_Multipart &cpy);
		File_Multipart	&operator+=(const std::string &src);
		File_Multipart &operator=(const File_Multipart &src);
		~File_Multipart();
		const std::string &get_filename(void) const;
		const std::string &get_content_type(void) const;
		const std::string &get_file(void) const;
		std::string _filename;
		std::string _content_type;
		std::string _file;
};
	

// bool	operator<=(const File_Multipart& lhs, const File_Multipart& rhs)
// {
// 	return (lhs.get_raw_str() <= rhs.get_raw_str());
// }

// bool	operator>(const File_Multipart& lhs, const File_Multipart& rhs)
// {
// 	return (lhs.get_raw_str() > rhs.get_raw_str());
// }

// bool	operator>=(const File_Multipart& lhs, const File_Multipart& rhs)
// {
// 	return (lhs.get_raw_str() >= rhs.get_raw_str());
// }

// bool	operator==(const File_Multipart& lhs, const File_Multipart& rhs)
// {
// 	return (lhs.get_raw_str() == rhs.get_raw_str());
// }

// bool	operator!=(const File_Multipart& lhs, const File_Multipart& rhs)
// {
// 	return (lhs.get_raw_str() != rhs.get_raw_str());
// }

class Multipart : public Parser
{
	public:
	
	Multipart(const std::string& raw_multipart, const std::string& boundary);
	Multipart(const std::string& boundary);
	Multipart(const Multipart &cpy);
	Multipart &operator=(const Multipart &src);
	~Multipart();


	void									set_new_multipart(std::string const &new_str);
	void									set_boundary(std::string const &boundary);
	std::map<std::string, File_Multipart>		get_files();
	
	private:
	bool									_is_finish;
	bool									_is_valid;
	std::string								_boundary;
	std::string								_current_header_field;	
	std::map<std::string, File_Multipart>		_files;
	std::string								_error_msg;
	void expand_va_arg(std::string::const_iterator &fct_it_tag, va_list *arg);
	void finish_expand(std::string::const_iterator start, std::string::const_iterator end, va_list *arg);
	void n_star_m(int n, int m,void (Multipart::*fct)(void));

	void start_parsing(void);
	void is_boundary(void);
	void is_boundary_end(void);
	void header_field(void);
	void field_name(void);
	void field_value(void);
	void field_content(void);
	void field_vchar(void);
	void VCHAR(void);
	void obs_text(void);
	void obs_fold(void);
	void message_body(void);


};

