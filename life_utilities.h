#pragma once
#include "enums.h"
#include <random>
#include <string>

/**
* generic templatised destructor
*/
template <class T>
void release_memory(T* ptr) {
	if (ptr) {
		delete ptr;
		ptr = nullptr;
	}
}

/**
* Class responsible for injecting randomness in life
* Responsibilities including random selection of
  - Location on canvas
  - A Cell
*/
class crandom_generator {
public:
	// Constructor that takes start seed for generator and range within which numbers are to be generated
	crandom_generator(size_t seed, size_t row_limit, size_t col_limit) :\
		m_move_generator(seed),
		m_row_distributor(0, row_limit),
		m_col_distributor(0, col_limit) {}

	// Randomly get a row or cell of canvas
	size_t get_row_distribution() { return m_row_distributor(m_move_generator); }

	// Randomly get a column of canvas
	size_t get_col_distribution() { return m_col_distributor(m_move_generator); }
private:
	// Mersenne twister base generator
	std::mt19937 m_move_generator;
	// Distributors for the generator
	std::uniform_int_distribution<> m_row_distributor;/*(0, LIFE->get_row_count() - 1);*/
	std::uniform_int_distribution<> m_col_distributor;/*(0, LIFE->get_col_count() - 1);*/
};

/**
* inputs parameters for big bang event
*/
struct big_bang_events {
	// Seed/start value from which life gets created
	size_t m_seed;

	// Controls the percentage of initial life created with respect to 
	// Canvas size, num cells created = canvas possible locations/density_factor
	// Low value will create a large initial life
	size_t m_life_density_factor;

	// Canvas dimension
	size_t m_canvas_length;
	size_t m_canvas_breath;
};

/**
* Input parameters for how life evolves
*/
struct evolution_data {
	// Age of universe, time till which the life evolves and eventually comes to an end
	size_t univer_age;

	// Time span of one generation, beyond it life may transit to next generation
	// Like iron age->bronze age->agriculture ->age->science age
	size_t m_evolution_phase_age;
};


/**
* Class responsible for giving a snapshot of life and its colonies 
* at a given time stamp
*/
class clife_stat_gen {
public:
	// generate snapshot of live at given time interval
	void gen_life_snapshot(size_t time_interval);
private:
	std::string get_file_name(size_t time_interval) {
		std::string file_name("life_");
		file_name += std::to_string(time_interval);
		return file_name;
	}
};




