#pragma once
# include "life_utilities.h"

//class clife_mutator;
//class ccell_evaluator;

/**
* Class responsibe for mutating life
* maps life cells to life canvas randomly
*/
class clife_mutator {

public:
	clife_mutator(size_t seed, size_t row_limit, size_t col_limit, size_t cell_limit) :m_rand_loc_gen(seed, row_limit, col_limit), m_rand_cell_gen(seed, cell_limit, 0) {}
	// The nascent state from which the life canvas starts
	void create_initial_life(
		size_t init_seed/*seed for random number generator*/, \
		size_t fill_size/*richness of life to be filled*/, \
		size_t canvas_size/*matrix row*/\
	);

	// randomly get a row location on canvas
	size_t gen_row_placement() { return m_rand_loc_gen.get_row_distribution(); }
	// randomly get a column location on canvas
	size_t gen_col_placement() { return m_rand_loc_gen.get_col_distribution(); }
	// randomly get a cell 
	size_t get_random_cell() { return m_rand_cell_gen.get_row_distribution(); };
private:

private:
	// Random number generator for location (coordinate) selection
	crandom_generator m_rand_loc_gen;

	// Random number generator for cell selection
	crandom_generator m_rand_cell_gen;

};

/**
* class that evaluates a cell based on its neighbours
* and sets cells life status accordingly
*/
class ccell;
class ccell_evaluator {
public:
	eCellType evaluate_cell_neighbours(ccell* pcell);
	eLifeType evaluate_life_type(eCellType etype);

private:
	/*under || overpopulation condition check*/
	bool is_under_populated(size_t live_threshold) { return (live_threshold < m_under_threshold); }
	bool is_over_populated(size_t live_threshold) { return (live_threshold > m_over_threshold); }
	/*Next generation crossover*/
	bool is_next_gen(size_t live_threshold) { return (m_under_threshold == live_threshold || m_over_threshold == live_threshold); }
	bool is_live_populated(size_t live_threshold) { return (m_over_threshold == live_threshold); }
private:
	const size_t m_under_threshold = 2;
	const size_t m_over_threshold = 3;
};


/**
* class responsible for managing life such as
* 1. Creating big bang (initial life generation)
* 2. Creating life cycles (generation transition)
* 3. Deciding when life ends and
* 4. Releasing life resources
*/

class clife_manager {
public:

	// Constructor which creates big bang event from given bing bang data
	clife_manager(const big_bang_events& start_cond);

	// After bing bang event, start evolving life by reorganising colonies 
	// and evaluating if life multiples or dies 
	void start_evolution(const evolution_data& universe_time_data);

	// Universe collapse conditions
	~clife_manager();
private:

	// Randomy create life (number influenced by big bang parameters) and assign them
	// randomly to life canvas
	void create_big_bang_event(const big_bang_events& start_cond);

	// Create a random move of cell on the canvas
	void do_evolution();

	void register_evolution_event() { m_phase_age++; };
	
	// At a given time instance, evaluate how life is placed on the canvas and 
	// updates their status (under, over population, next generation transition etc)
	void evaluate_life();

	// Checks if the given generation has to come to an end
	// Either the specified gen age is acheived or	
	bool phase_evolution_acheived(size_t phase_age) { return m_phase_age == phase_age; }
	
	// Mark conditions for next generation transition
	// Evaluate if any live cell exists to carry forward to this generation
	// Life end and GOL finishes if no live cell exists to be carried forward
	bool do_next_generation_transiton();

	// Factory methods which can be extened in the future to
	// return different derived types in case clife_mutator and ccell_evaluator are extended
	clife_mutator* create_life_mutator(size_t seed, size_t row_limit, size_t col_limit, size_t cell_limit) { return new clife_mutator(seed, row_limit - 1, col_limit - 1, cell_limit - 1); }
	ccell_evaluator* create_cell_evaluator() { return new ccell_evaluator(); }

	//----------HELPERS----------------------//
	// Returns an empty/unoccupied location on the canvas
	COORD get_barren_location();
	
		// Compute the number of life cells to be created 
	size_t compute_num_life_cells();
	//Getters and setters
	bool big_bang_event_happened() { return m_big_bang_state; }
	void set_big_bang_state(bool value) { m_big_bang_state = value; }

	//----------HELPERS----------------------//

private:
	
	// Attribute responsible for moving life on the canvas randomly
	// Can be extended in future to get more variants, factor function needs to be update accordingly
	clife_mutator* m_life_mutator = nullptr;

	// Evualates a life placement on canvas to actually decide wether life lives, dies or goes to next gen
	ccell_evaluator* m_cell_evaluator = nullptr;
	clife_stat_gen* m_life_statistics = nullptr;

	// Data of big bang event based on which initial life is created
	big_bang_events m_big_bang_data;

	// How long a generation (iron age etc) lasts
	size_t m_phase_age;

	// Tracks whether the big bang event (initial life creation)
	// occured or not so as to avoid multiple initial life creation
	static bool m_big_bang_state;
};

inline size_t clife_manager::compute_num_life_cells() {
	return (m_big_bang_data.m_canvas_length * m_big_bang_data.m_canvas_breath) / m_big_bang_data.m_life_density_factor;
}


