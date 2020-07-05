#pragma once

#include<cstdint>
#include<utility>

#define LIFE clife_canvas::get_unique_instance()



enum class eCellType: std::uint8_t  {
								eunder_population/*live cell with less than 2 live neighbours*/,
								eover_population /*live cell with more than 3 live neighbours*/,
								enext_gen/*live cell with 2 or 3 live neighbours*/,
								ealive/*dead cell with three live neighbours*/,
								etype_unknow
								
							};	

enum  eLifeType /*: std::uint8_t */{
	elive_cell/*live cell with less than 2 live neighbours*/,
	edead_cell /*eover_population and eunder_population*/,
	enext_gen  /*eCellType::enext_gen, implicitly means live*/,
	etype_unknown/*init state*/

};
using UNSIGNED = std::size_t;
using COORD = std::pair< UNSIGNED, UNSIGNED>;
