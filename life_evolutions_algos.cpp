
# include "life_evolutions_algos.h"
# include "life_data_models.h"

# include <array>
# include <iostream>
# include <fstream>
using namespace std;

//------Static inits--------------------//
bool clife_manager::m_big_bang_state = false;
//------Static inits--------------------//

clife_manager::clife_manager(const big_bang_events& start_cond) :m_big_bang_data(start_cond), m_phase_age(0) {
	m_life_mutator = create_life_mutator(m_big_bang_data.m_seed, m_big_bang_data.m_canvas_length, m_big_bang_data.m_canvas_breath, compute_num_life_cells());
	m_cell_evaluator = create_cell_evaluator();
	m_life_statistics = new clife_stat_gen();
	create_big_bang_event(start_cond);
}

void clife_manager::create_big_bang_event(const big_bang_events& start_cond) {
	// initialise life only once
	if (big_bang_event_happened())
		return;

	cout << std::endl << "Creating big bang event";
	// Give memory to canvas
	clife_canvas::get_unique_instance(start_cond.m_canvas_length, start_cond.m_canvas_breath);
	cout << std::endl << "Created life canvas of dimension(s) " << start_cond.m_canvas_length << "X" << start_cond.m_canvas_breath << endl;

	// create life cells and assign them t o canvas randomly
	for (size_t attempts = 0; attempts < compute_num_life_cells(); attempts++) {
		LIFE->generate_life_at(get_barren_location());
	}
	set_big_bang_state(true);
	// Dump life state at big bang
	m_life_statistics->gen_life_snapshot(0);
}

COORD clife_manager::get_barren_location() {

	COORD canvas_pos(m_life_mutator->gen_row_placement(), m_life_mutator->gen_col_placement());

	while (!LIFE->is_loc_empty(canvas_pos)) {
		canvas_pos.first = m_life_mutator->gen_row_placement();
		canvas_pos.second = m_life_mutator->gen_col_placement();
	}
	return canvas_pos;
}

void clife_manager::start_evolution(const evolution_data& universe_time_data) {
	
	size_t life_age = universe_time_data.univer_age;
	size_t phase_age = universe_time_data.m_evolution_phase_age;

	for (size_t time = 1; time <= life_age; time++) {
	
		do_evolution();
		register_evolution_event();

		if (phase_evolution_acheived(phase_age)) {
		
			evaluate_life();
			do_next_generation_transiton();
			//Dump life state
			m_life_statistics->gen_life_snapshot(time);
		}

	}

}

void clife_manager::do_evolution() {
	// randomly pick a cell and try to place it at some unoccupied position on life canvas
	ccell* cell_ob = LIFE->get_life_cells()[m_life_mutator->get_random_cell()];
	COORD barren_loc = get_barren_location();

	LIFE->un_lock_loc(cell_ob);
	LIFE->map_cell_at(barren_loc, cell_ob);


}

void clife_manager::evaluate_life() {

	clife_canvas::CELLS& cell_list = LIFE->get_life_cells();

	for (auto cell : cell_list) {
		eCellType placement_type = m_cell_evaluator->evaluate_cell_neighbours(cell);
		eLifeType evolution_type = m_cell_evaluator->evaluate_life_type(placement_type);
		cell->set_life_type(evolution_type);
	}
}
/**
 * Mark the live cells as dead and next gen as alive
*/
bool clife_manager::do_next_generation_transiton() {

	size_t live_cell_count(0);
	static size_t generation(0);
	clife_canvas::CELLS& cell_list = LIFE->get_life_cells();

	for (auto cell : cell_list) {
		if (cell->is_next_gen_cell()) {
	
			live_cell_count++;
			cell->set_life_type(elive_cell);
		}
		else
			cell->set_life_type(edead_cell);
	}
	if (live_cell_count > 0)
		std::cout << std::endl << live_cell_count << "\t cells contine to generation\t" << ++generation << std::endl;
	else
		std::cout << std::endl << "Life ceases at generation\t" << ++generation;

	m_phase_age = 0;

	return (0 != live_cell_count);
}

clife_manager::~clife_manager() {

	std::cout << std::endl << std::endl << "Deleting Life..." << std::endl;
	release_memory(m_life_mutator);
	release_memory(m_cell_evaluator);
	release_memory(m_life_statistics);
	release_memory(LIFE);
	std::cout << "Life cell deleted" << std::endl;
	std::cout << "Life canvas deleted" << std::endl;
}

eCellType ccell_evaluator::evaluate_cell_neighbours(ccell* pcell) {

	eCellType ecell_type(eCellType::etype_unknow);
	// Evaluate the cell's 8 neighbours
	if (!pcell)
		return ecell_type;
	const COORD& cell_position = pcell->get_coord();

	COORD coord_l(cell_position.first, cell_position.second - 1);/*left cell coord*/
	COORD coord_r(cell_position.first, cell_position.second + 1); /*right cell coord*/
	COORD coord_t(cell_position.first - 1, cell_position.second);/*top cell coord*/
	COORD coord_b(cell_position.first + 1, cell_position.second);/*bottom cell coord*/
	COORD coord_dlt(cell_position.first - 1, cell_position.second - 1);/*diagnol left top cell coord*/
	COORD coord_dlb(cell_position.first + 1, cell_position.second - 1);/*diagnol left bottom cell coord*/
	COORD coord_drt(cell_position.first - 1, cell_position.second + 1);/*diagnol right cell top coord*/
	COORD coord_drb(cell_position.first + 1, cell_position.second + 1);/*diagnol right cell top coord*/
	ccell* pcell2 = (*LIFE)[coord_l.first][coord_l.second];
	std::array<const ccell*, 8> cell_neighbours = {
												(*LIFE)[coord_l.first][coord_l.second],
												(*LIFE)[coord_r.first][coord_r.second],
												(*LIFE)[coord_t.first][coord_t.second],
												(*LIFE)[coord_b.first][coord_b.second],
												(*LIFE)[coord_dlt.first][coord_dlt.second],
												(*LIFE)[coord_dlb.first][coord_dlb.second],
												(*LIFE)[coord_drt.first][coord_drt.second],
												(*LIFE)[coord_drb.first][coord_drb.second]
	};

	size_t num_live_negbours(0);
	for (auto neighbour : cell_neighbours) {
		if (!neighbour)
			continue;
		if (neighbour->is_live_cell())
			num_live_negbours++;

	}

	eLifeType elife_type(eLifeType::etype_unknown);
	if (pcell->is_live_cell()) {/*live or next gen*/
		if (is_under_populated(num_live_negbours))
			ecell_type = eCellType::eunder_population;
		else if (is_over_populated(num_live_negbours))
			ecell_type = eCellType::eover_population;
		else if (is_next_gen(num_live_negbours))
			ecell_type = eCellType::enext_gen;
	}
	else if (is_live_populated(num_live_negbours))/*if dead cell*/
		ecell_type = eCellType::ealive;

	//pcell->set_life_type(elife_type);
	return ecell_type;
}
eLifeType ccell_evaluator::evaluate_life_type(eCellType etype) {
	eLifeType elife_type(eLifeType::etype_unknown);

	switch (etype) {
	case eCellType::eunder_population:
	case eCellType::eover_population:
		elife_type = eLifeType::edead_cell;
		break;
	case eCellType::enext_gen:
		elife_type = eLifeType::enext_gen;

	case eCellType::ealive:
		elife_type = eLifeType::elive_cell;
	default:;
	}
	return elife_type;
}


void clife_stat_gen::gen_life_snapshot(size_t time_interval) {
	std::string file_name(get_file_name(time_interval));
	std::cout << std::endl << "Life status at time stamp " << time_interval << " genrated in " << file_name;
	std::ofstream out(file_name, std::ios::app);
	out << LIFE;
	out << std::endl;

}

