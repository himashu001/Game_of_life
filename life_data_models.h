#pragma once

# include "life_utilities.h"
#include <vector>


#include <cassert>
#include <limits.h>

#include<fstream>
#include<iostream>



/**
* Represents a unit of live which can take upon
* dead, alive or next gen status
*/
class ccell {
public:
	// overload the  ofstream operartor to dump a life cell
	friend std::ofstream& operator << (std::ofstream& out, ccell* pcell);
	//  life constructor
	ccell(const COORD& pos, size_t id) :m_position(pos), m_id(id) {};
	
	//-----------------------Helpers-----------------//
	// set the life status dead, alive or next generation life
	void set_life_type(eLifeType etype) { m_elife_type = etype; }
	const COORD& get_coord() { return m_position; }
	void set_coord(const COORD& value) { m_position = value; }
	bool  is_live_cell() const { return eLifeType::elive_cell == m_elife_type || eLifeType::enext_gen == m_elife_type; }
	bool is_next_gen_cell() { return (eLifeType::enext_gen == m_elife_type); }
	size_t get_id() { return m_id; }
	//-----------------------Helpers-----------------//
private:
	// basis of evaluating if cell will under ,over populate or go to next generation
	const eLifeType& get_life_type() { return m_elife_type; }
private:
	// Type of cell life, dead , alive or next gen
	// Cell is alive when born for the first time
	eLifeType m_elife_type = eLifeType::elive_cell;

	// cell position on canvas
	COORD m_position;
	size_t m_id;	
};

//-----------------------Life Canvas---------------------------//

/*
* Life canvas is modelled as matrix, 
* this class represents the column of the canvas
* Future enhancement: 
    - For memory optimisation Column::CELLS can be changes to a std::list and operator[] should use std::advance()
	- Current access then will change from O(1) to O(n)
*/
class ccolumn {/*one column of life canvas*/
	// can change to list to reduce memory std::advance will still work though it will be 
	// from const to liner time complexity
	using CELLS = std::vector<ccell*>;
public:
	// Constructor
	ccolumn(size_t size = 0) :m_dummy_cell(nullptr) { m_life_cells.assign(size, nullptr); }
	size_t get_size() { return m_life_cells.size(); }
	ccell*& operator [] (size_t pos) { return ((pos >= m_life_cells.size()) ? m_dummy_cell : m_life_cells[pos]); }
private:
	// List of cells in the column
	CELLS m_life_cells;
	// If operator[] goes beyond range, 
	// return dummy place holder cell which basically represents a null entry
	ccell* m_dummy_cell;
};

/**
* Row representation of life canvas
*/
class crow {/*one row of life matrix*/
public:

	// ofstream  overloaded to dump out row object directly
	friend std::ofstream& operator << (std::ofstream& out, crow& row);
	// row constructor
	crow(size_t size = 0) :m_column(size) {}	

	// operator [] overloaded
	ccell*& operator[](size_t pos) { return (m_column[pos]); }
	size_t size() {	return m_column.get_size();	}
private:
	// Column within a row
	ccolumn m_column;
};

/**
* Singleton design pattern based class
* Represents a physical container made of rows and column
* Maintains and manages cells 
*/

class clife_canvas {/*the matrix */

public:
	using MATRIX = std::vector<crow>;
	using CELLS = std::vector<ccell*>;

	friend std::ofstream& operator << (std::ofstream& out, clife_canvas* ptr);
	static clife_canvas* get_unique_instance(size_t life_length = 0, size_t life_breath = 0);

	// create a new life cell at give position on canvas
	// post.first = row , pos.seond = column
	void generate_life_at(const COORD& pos);

	// Does the location contains any life?
	bool is_loc_empty(const COORD& pos);

	// Assign the cell to given location on canvas
	void map_cell_at(const COORD& pos, ccell* pcell);

	// When a cell is moved, its coordinates are updated to new location
	// and its reference from current location removed
	void un_lock_loc(ccell* pcell);

	//operator [] overloaded
	crow& operator[](const size_t row_num);
	
	// Life destructor
	~clife_canvas();
	//-------------------Helpers-------------------------//
	size_t get_row_count() { return m_life_canvas.size(); }
	size_t get_col_count() { return m_life_canvas[0].size(); }		
	CELLS& get_life_cells() { return m_cell_list; }
	//-------------------Helpers-------------------------//
protected:
	// protected constructor for singleton DP
	clife_canvas(size_t life_length, size_t life_breath) : m_dummy_row() {
		m_life_canvas.assign(life_length, crow(life_breath));// assert size !=0
	}

private:
	// The actual physical canvas
	MATRIX m_life_canvas;

	// All life cells of game of life
	CELLS m_cell_list;
	
	// A null object to be returned in out of array access in operator []
	crow m_dummy_row;

	//singleton instance
	static clife_canvas* m_uinque_instance;

};
inline crow& clife_canvas::operator[](const size_t row_num) {
	return ((row_num >= get_row_count() || row_num < 0) ? m_dummy_row : m_life_canvas[row_num]);
}
inline clife_canvas::~clife_canvas() {
	for (auto pcell : m_cell_list)
		release_memory(pcell);
}

inline void clife_canvas::generate_life_at(const COORD& pos) {
	static int count(0);
	std::cout << std::endl << "Created alive cell" << count << " at location " << "(" << pos.first << "," << pos.second << ")";
	// create a new life cell and register the same	
	ccell* cell_ob = new ccell(pos, count);
	m_cell_list.push_back(cell_ob);
	map_cell_at(pos, cell_ob);
	count++;
}


inline bool clife_canvas::is_loc_empty(const COORD& pos) {
	return nullptr == (*this)[pos.first][pos.second];
}

inline void clife_canvas::map_cell_at(const COORD& pos, ccell* pcell) {
	m_life_canvas[pos.first][pos.second] = pcell;
	pcell->set_coord(pos);

}

inline void clife_canvas::un_lock_loc(ccell* pcell) {
	const COORD& pos = pcell->get_coord();
	assert(m_life_canvas[pos.first][pos.second] == pcell);
	m_life_canvas[pos.first][pos.second] = nullptr;
}


