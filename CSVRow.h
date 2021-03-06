#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "dirent.h"

class CSVRow
{
public:
	std::string const& operator[](std::size_t index) const;

	std::size_t size() const;

	void readNextRow(std::istream& str);

private:
	std::vector<std::string>    m_data;

};

struct traininfo
{
	std::string csvname;
	std::vector<std::string> imagenames;
	int label;
};

std::istream& operator>>(std::istream& str, CSVRow& data);
