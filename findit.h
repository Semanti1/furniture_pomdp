#ifndef FINDIT_H
#define FINDIT_H

#include "simulator.h"
#include "grid.h"
#include "coord.h"
#include <list>


class FINDIT_STATE : public STATE {
public: 
    struct CELL {
        bool occupied;
        bool visited;
        double likelihood_hit;
        double likelihood_miss;
        double probability_obj_here;
    };

    GRID<CELL> positions;
    COORD object_position;
    COORD agent_position;
};


class FINDIT : public SIMULATOR {
public:
    FINDIT(int xsize, int ysize);
    virtual STATE* Copy(const STATE& state) const;
    virtual void Validate(const STATE& state) const;
    virtual STATE* CreateStartState() const;
    virtual void FreeState(STATE* state) const;
    virtual bool Step(STATE& state, int action, 
        int& observation, double& reward) const;

    virtual bool LocalMove(STATE& state, const HISTORY& history,
        int stepObs, const STATUS& status) const;
    void GenerateLegal(const STATE& state, const HISTORY& history,
        std::vector<int>& legal, const STATUS& status) const;
    // void GeneratePreferred(const STATE& state, const HISTORY& history,
    //     std::vector<int>& preferred, const STATUS& status) const;

    virtual void DisplayBeliefs(const BELIEF_STATE& beliefState, 
        std::ostream& ostr) const;
    virtual void DisplayState(const STATE& state, std::ostream& ostr) const;
    virtual void DisplayObservation(const STATE& state, int observation, std::ostream& ostr) const;
    virtual void DisplayAction(int action, std::ostream& ostr) const;

protected: 
    int board_size;
    double sensor_uncertainty;
    
    // Observations
    enum {
        OBS_HIT = 0, 
        OBS_MISS = 1, 
        OBS_NONE = 2
    };

    // Extra actions (apart from the coordinate ones)
    enum {
        PICK_UP = 0
    };

    // int Listen(const FINDIT_STATE& fi_state) const;
    
private: 
    mutable MEMORY_POOL<FINDIT_STATE> MemoryPool;
    int AgentPosToAction(COORD agent_pos) const;

    // COORD object_position;
};

#endif
