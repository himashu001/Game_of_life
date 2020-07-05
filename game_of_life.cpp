
# include "life_evolutions_algos.h"
# include <iostream>
# include <stdexcept>
using namespace std;

int main() {
	try {
		// Create the big bang event
		// 1. create life canvas 
		// 2. create life cells
		// 3. assign life to locations on canvas;
		big_bang_events big_bang = { 200/*seed*/, 20/*life densitry*/, 10, 10/*life canvas dimensions*/ };

		// How long the universe evolution lasts as well as intervals of each evolutuon
		evolution_data  thresholds = { 10/*universe age*/, 5/*each evolutuion phase*/ };

		clife_manager universe(big_bang);/*Big bang event */
		universe.start_evolution(thresholds);/*Start evolution*/
	}
	catch (exception& e) {
		cout << e.what() <<endl;
	}
	return 1;

}

