
# include"life_data_models.h"
#include <iostream>
#include <fstream>
using namespace std;

//------Static inits--------------------//
clife_canvas* clife_canvas::m_uinque_instance = nullptr;
//------Static inits--------------------//

clife_canvas* clife_canvas::get_unique_instance(size_t life_length, size_t life_breath) {
	m_uinque_instance = (!m_uinque_instance ? new clife_canvas(life_length, life_breath) : m_uinque_instance);
	return m_uinque_instance;
}

std::ofstream& operator << (std::ofstream& out, crow& row) {
	for (size_t col_c = 0; col_c < row.size(); col_c++) {
		ccell* pcell = row.m_column[col_c];
		out << "\t" << (pcell ? to_string(pcell->get_id()) : "null");
	}
	out << std::endl;
	return out;
}

// ------------------Overloaded operator << ----------------------------
std::ofstream& operator << (std::ofstream& out, clife_canvas* ptr) {
	out << "===============================" << endl;
	out << "\tLife canvas details" << endl;

	for (size_t row_c = 0; row_c < ptr->get_row_count(); row_c++) {

		out << "row\t" << row_c;
		out << (*ptr)[row_c] << endl;
	}

	out << "===============================" << endl;

	out << "\tLife cell details" << endl;
	for (auto cell : ptr->m_cell_list)
		out << cell << endl;
	out << "===============================" << endl;
	return out;

}

std::ofstream& operator << (std::ofstream& out, ccell* pcell) {
	const char* life_type[] = { "elive_cell", "edead_cell","enext_gen","etype_unknown" };

	out << pcell->m_id << "(" << pcell->m_position.first << "," << pcell->m_position.second << ")" << life_type[pcell->m_elife_type];
	return out;
}

// ------------------Overloaded operator << ----------------------------