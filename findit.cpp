#include "findit.h"
#include "utils.h"
using namespace std;
using namespace UTILS;

// NOTE: Very preliminary, currently, there isn't much "intelligence" (aka, we're not currently really 
// using the statistic that keeps track of the probability that the object is here--class variable for FINDIT_STATE)

FINDIT::FINDIT(int xsize, int ysize)
:   sensor_uncertainty(0.10),
    board_size(xsize)
{
    NumActions = (xsize * xsize) + 1;
    // 0: PICK_UP
    // 1 - xsize = coordinates
    NumObservations = 3;
    // 0: OBS_HIT
    // 1: OBS_MISS
    // 2: OBS_NONE
    RewardRange = 100;
    Discount = 0.95;

    // RandomSeed((unsigned) time(0)); 
    // int obj_pos_x = Random(board_size);
    // int obj_pos_y = Random(board_size);
    // object_position = COORD(obj_pos_x, obj_pos_y);
}


STATE* FINDIT::Copy(const STATE& state) const {
    const FINDIT_STATE& fi_state = safe_cast<const FINDIT_STATE&>(state);
    FINDIT_STATE* new_state = MemoryPool.Allocate();
    *new_state = fi_state;
    
    return new_state;
}

void FINDIT::Validate(const STATE& state) const {
    const FINDIT_STATE& fi_state = safe_cast<const FINDIT_STATE&>(state);
    assert(fi_state.object_position.X < board_size);
    assert(fi_state.object_position.Y < board_size);
}

STATE* FINDIT::CreateStartState() const {
    FINDIT_STATE* fi_state = MemoryPool.Allocate();
    fi_state->positions.Resize(board_size, board_size);

    fi_state->agent_position = COORD(0, 0);

    for (int i = 0; i < (board_size * board_size); i++) { 
        FINDIT_STATE::CELL& cell = fi_state->positions(i);
        cell.occupied = false;
        cell.visited = false;
        cell.likelihood_hit = 1.0;
        cell.likelihood_miss = 1.0;
        cell.probability_obj_here = (1.0 / (board_size * board_size));
    }

    // I honestly am wondering if object position should be a private class variable instead of a state variable. 
    // In Rocksample, the rock positions are a private class variable, but in battleship, the ships are a state variable
    // It might be worth trying to move the next four lines to FINDIT::FINDIT(int xsize, int ysize)
    int obj_pos_x = Random(board_size);
    int obj_pos_y = Random(board_size);
    fi_state->object_position = COORD(obj_pos_x, obj_pos_y); // if moving, change just to object_position and uncomment priv variable in header file
    // cout << "X = " << fi_state->object_position.X << " Y = " << fi_state->object_position.Y << endl;
    fi_state->positions(fi_state->object_position).occupied = true;

    return fi_state;
}

void FINDIT::FreeState(STATE* state) const {
    FINDIT_STATE* fi_state = safe_cast<FINDIT_STATE*>(state);
    MemoryPool.Free(fi_state);
}

/**
 * Convert coordinate to corresponding action
 */
int FINDIT::AgentPosToAction(COORD agent_pos) const {
    return 1 + agent_pos.X + (agent_pos.Y * board_size);
}

bool FINDIT::Step(STATE& state, int action, int& observation, double& reward) const {
    FINDIT_STATE& fi_state = safe_cast<FINDIT_STATE&>(state);
    reward = 0;
    observation = OBS_NONE;
    switch (action) {
        case PICK_UP: {
            if (fi_state.agent_position == fi_state.object_position) {
                reward = +100;
            } else {
                reward = -100;
            }

            return true;
	    }

        default: {
            // We'll probably want to change this to use the robot's input, but for now I encoded it in software using 
            // a 10% error rate (encoded by sensor_uncertainty)
            fi_state.agent_position = COORD((action - 1) % board_size, (action - 1) / board_size);

            FINDIT_STATE::CELL& visited_cell = fi_state.positions(action);
            if (visited_cell.visited) { // Discourages robot from continually touching the same cell
                reward = -3;
            } else {
                reward = -1;
                visited_cell.visited = true;
            }

            if (Bernoulli(sensor_uncertainty)) {
                observation = (fi_state.agent_position == fi_state.object_position) ? OBS_MISS : OBS_HIT;
            } else {
                observation = (fi_state.agent_position == fi_state.object_position) ? OBS_HIT : OBS_MISS;
            }

            // Do some stats thing here to take into account the uncertainty in the observation (such that having multiple
            // observations that report the same thing will increase the amount of confidence we have) 
            // Don't think this following if block is necessary (because I don't think recieved_hit_obs is necessary)
            if (observation == OBS_HIT) {
                visited_cell.likelihood_hit *= (1.0 - sensor_uncertainty);
                visited_cell.likelihood_miss *= sensor_uncertainty;
            } else {
                visited_cell.likelihood_hit *= sensor_uncertainty;
                visited_cell.likelihood_miss *= (1.0 - sensor_uncertainty);
            }
            
            // taken from rocksample
            double denom = (0.5 * visited_cell.likelihood_miss) + (0.5 * visited_cell.likelihood_hit);
		    visited_cell.probability_obj_here = (0.5 * visited_cell.likelihood_hit) / denom;

            break;
        }
    }

    return false;
}

// ############################################################################

/**
 * Honestly not so sure of what this is supposed to do
 */
bool FINDIT::LocalMove(STATE& state, const HISTORY& history, int stepObs, const STATUS& status) const {
    return false;
    
    // Try and change object posiiton for particle reinvigoration
    // FINDIT_STATE& fi_state = safe_cast<FINDIT_STATE&>(state);
    // COORD current_agent_pos = fi_state.agent_position;
    // double probability_of_obj = fi_state.positions(current_agent_pos).probability_obj_here;

    // // Don't want to change object position if we're pretty sure object could be here 
    // if (probability_of_obj >= 0.75) { 
    //     return false;
    // }

    // COORD new_obj_pos = COORD(Random(board_size), Random(board_size));

    // if (fi_state.positions(new_obj_pos).visited) {
    //     return false;
    // } else {
    //     fi_state.object_position = new_obj_pos;
    //     return true;
    // }
}

/**
 * Strictly, what actions are we *allowed* to perform
 */
void FINDIT::GenerateLegal(const STATE& state, const HISTORY& history, 
                            std::vector<int>& legal, const STATUS& status) const {
    
    const FINDIT_STATE& fi_state = safe_cast<const FINDIT_STATE&>(state);

    // COORD current_agent_pos = fi_state.agent_position;
    // double probability_of_obj = fi_state.positions(current_agent_pos).probability_obj_here;

    // We can always choose to pick up
    legal.push_back(PICK_UP);

    // We can pick any square in our grid to touch 
    for (int i = 1; i < (board_size + 1); i++) {
        legal.push_back(i);
    }
}

// ############################################################################

void FINDIT::DisplayBeliefs(const BELIEF_STATE& beliefState, std::ostream& ostr) const {
    // Not implemented
    return;
}

void FINDIT::DisplayState(const STATE& state, std::ostream& ostr) const {
    // Not implemented
    return;
}

void FINDIT::DisplayObservation(const STATE& state, int observation, std::ostream& ostr) const {
    // Not implemented
    return;
}

void FINDIT::DisplayAction(int action, std::ostream& ostr) const {
    // Not implemented
    return;
}
