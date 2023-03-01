#include "causal_furniture.h"
#include "beliefstate.h"
#include "utils.h"
#include <math.h>
#include <iomanip>
#include <iostream>
using namespace std;
using namespace UTILS;

CAUSAL_FURNITURE::CAUSAL_FURNITURE(std::string name="kerosene_lamp", int numparts = 6, bool causal = true)
:   furniture_name(name),
    numParts(numparts),
    causal(causal)
{
    NumActions = numparts * (numparts - 1) / 2;
    NumObservations = 2;
    //RewardRange = 100;// NumActions / 4.0;
    //Discount = 0.95;
    names = { "wall plug","backplate", "lightbulb", "lamp body"};
    cout << "numparts" << numParts << endl;
    //numPorts = { 2, 1, 3,1,2,1 };
    //found = {};
    /*TotalRemaining = 0;
    for (int length = MaxLength; length >= 2; --length)
    {
        int numShips = MaxLength + 1 - length;
        TotalRemaining += numShips * length;
    }*/
   // TotalRemaining = MaxLength - 1;
}

STATE* CAUSAL_FURNITURE::Copy(const STATE& state) const
{
    assert(state.IsAllocated());
    const CAUSAL_FURNITURE_STATE& oldstate = safe_cast<const CAUSAL_FURNITURE_STATE&>(state);
    CAUSAL_FURNITURE_STATE* newstate = MemoryPool.Allocate();
    *newstate = oldstate;
    return newstate;
}

void CAUSAL_FURNITURE::Validate(const STATE& state) const
{
    /*const CAUSAL_FURNITURE_STATE& cfstate = safe_cast<const CAUSAL_FURNITURE_STATE&>(state);
    for (int i = 0; i < XSize * YSize; ++i)
    {
        if (bsstate.Cells(i).Diagonal && bsstate.Cells(i).Occupied)
        {
            DisplayState(bsstate, cout);
            assert(false);
        }
    }*/
}
STATE* CAUSAL_FURNITURE::CreateStartStateReal(int real) const
{
    CAUSAL_FURNITURE_STATE* cfstate = MemoryPool.Allocate();
    cfstate->numleft = 0;
    cfstate->unassembled.clear();
    for (int i = 0; i < NumActions; i++)
    {
        OBJECT_PARTS obj;
        obj.num = i;
        int n = numParts;
        int x_ind = n - 2 - floor(sqrt(-8 * i + 4 * n * (n - 1) - 7) / 2.0 - 0.5);
        int y_ind = i + x_ind + 1 - n * (n - 1) / 2 + (n - x_ind) * ((n - x_ind) - 1) / 2;
        std::string obj1name = names[x_ind];
        std::string obj2name = names[y_ind];
        std::pair<std::string, std::string> connection(obj1name, obj2name);
        if (isValid(connection))
        {
            obj.legit = 1;
        }
        else
        {
            obj.legit = 0;
        }
        //obj.legit = Random(2);
        if (obj.legit)
            cfstate->numleft++;
        obj.visited = false;
        cfstate->unassembled.push_back(obj);
    }

    return cfstate;
}

STATE* CAUSAL_FURNITURE::CreateStartState() const
{
    CAUSAL_FURNITURE_STATE* cfstate = MemoryPool.Allocate();
    
    /*if (furniture_name == "kerosene lamp")
    {
        cfstate->furniture_name = furniture_name;
        //cfstate->numParts = 6;
    }
    /*int k = Random(14);
    for (int i = 0; i < k; i++)
    {
        int obj1 = Random(5);
        int obj2 = Random(5);
        std::pair<std::string, std::string> connection = make_pair(names[obj1], names[obj2]);
        auto it3 = std::find_if(cfstate->connections.begin(), cfstate->connections.end(), [&](std::pair<std::string, std::string> obj) { return ((obj.first == connection.first) && (obj.second == connection.second)); });
        auto it4 = std::find_if(cfstate->connections.begin(), cfstate->connections.end(), [&](std::pair<std::string, std::string> obj) { return ((obj.first == connection.second) && (obj.second == connection.first)); });
        if ((cfstate->connections.end() == it3) && (cfstate->connections.end() == it4))
        {
            cfstate->connections.push_back(connection);
        }
    }*/

    //cfstate->unassembled.resize(numParts);
    //std::vector<std::string> names{ "fuel tank","kerosene", "burner", "chimney", "wick", "lighter" };
    //std::vector<int> numPorts{ 2, 1, 3,1,2,1 };
    /*for (int i = 0; i < numParts; i++)
    {
        OBJECT_PARTS obj; 
        obj.name = names[i];
        obj.ports_remaining = numPorts[i];
        cfstate->unassembled.push_back(obj);
       // cout << "done" << i << endl;
    }
    return cfstate;*/
    cfstate->unassembled.clear();
    cfstate->numleft = 0;
    if (causal)
    {
        //cout << "in causal" << endl;
        int toss = Random(2);
        if (toss)
        {
            for (int i = 0; i < NumActions; i++)
            {
                OBJECT_PARTS obj;
                obj.num = i;
                int n = numParts;
                int x_ind = n - 2 - floor(sqrt(-8 * i + 4 * n * (n - 1) - 7) / 2.0 - 0.5);
                int y_ind = i + x_ind + 1 - n * (n - 1) / 2 + (n - x_ind) * ((n - x_ind) - 1) / 2;
                std::string obj1name = names[x_ind];
                std::string obj2name = names[y_ind];
                std::pair<std::string, std::string> connection(obj1name, obj2name);
                if (isCausal(connection))
                {
                    obj.legit = 1;
                }
                else {
                    obj.legit = Random(2);
                }
                if (obj.legit)
                    cfstate->numleft++;
                obj.visited = false;
                cfstate->unassembled.push_back(obj);
            }
        }
        else
        {
            for (int i = 0; i < NumActions; i++)
            {
                OBJECT_PARTS obj;
                obj.num = i;
                int n = numParts;
                int x_ind = n - 2 - floor(sqrt(-8 * i + 4 * n * (n - 1) - 7) / 2.0 - 0.5);
                int y_ind = i + x_ind + 1 - n * (n - 1) / 2 + (n - x_ind) * ((n - x_ind) - 1) / 2;
                std::string obj1name = names[x_ind];
                std::string obj2name = names[y_ind];
                std::pair<std::string, std::string> connection(obj1name, obj2name);
                if (isCausal(connection))
                {
                    obj.legit = Random(2);
                    /*if ((connection.first == "burner" && connection.second == "fuel tank") || (connection.first == "fuel tank" && connection.second == "burner"))
                    {
                        obj.legit = 1;
                    }
                    else {
                        obj.legit = Random(2);
                    }*/
                }
                else {
                    obj.legit = 0;
                }
                if (obj.legit)
                    cfstate->numleft++;
                obj.visited = false;
                cfstate->unassembled.push_back(obj);
            }
        }

        return cfstate;
    }
    else
    {
        //cout << " NOT in causal" << endl;
        for (int i = 0; i < NumActions; i++)
        {
            OBJECT_PARTS obj;
            obj.num = i;
            
            obj.legit = Random(2);
            
            if (obj.legit)
                cfstate->numleft++;
            obj.visited = false;
            cfstate->unassembled.push_back(obj);
        }
        return cfstate;
    }
}

/*STATE* CAUSAL_FURNITURE::CreateStartState(int real = 0) const
{
    CAUSAL_FURNITURE_STATE* cfstate = MemoryPool.Allocate();
    cfstate->numleft = 0;
    if (real)
    {
        for (int i = 0; i < NumActions; i++)
        {
            OBJECT_PARTS obj;
            obj.num = i;
            int n = numParts;
            int x_ind = n - 2 - floor(sqrt(-8 * i + 4 * n * (n - 1) - 7) / 2.0 - 0.5);
            int y_ind = i + x_ind + 1 - n * (n - 1) / 2 + (n - x_ind) * ((n - x_ind) - 1) / 2;
            std::string obj1name = names[x_ind];
            std::string obj2name = names[y_ind];
            std::pair<std::string, std::string> connection(obj1name, obj2name);
            if (isValid(connection))
            {
                obj.legit = true;
            }
            else
            {
                obj.legit = false;
            }
            //obj.legit = Random(2);
            if (obj.legit)
                cfstate->numleft++;
            obj.visited = false;
            cfstate->unassembled.push_back(obj);
        }
    }
    else
    {
        for (int i = 0; i < NumActions; i++)
        {
            OBJECT_PARTS obj;
            obj.num = i;
            obj.legit = Random(2);
            if (obj.legit)
                cfstate->numleft++;
            obj.visited = false;
            cfstate->unassembled.push_back(obj);
        }

        
    }

    return cfstate;
}*/

void CAUSAL_FURNITURE::FreeState(STATE* state) const
{
    CAUSAL_FURNITURE_STATE* cfstate = safe_cast<CAUSAL_FURNITURE_STATE*>(state);
    MemoryPool.Free(cfstate);
}

bool CAUSAL_FURNITURE::Step(STATE& state, int action,
    int& observation, double& reward) const
{
    CAUSAL_FURNITURE_STATE& cfstate = safe_cast<CAUSAL_FURNITURE_STATE&>(state);

    /*int num = numParts;
    if (cfstate.unassembled.size() == 0)
    {
        reward += (num * (num - 1) * 0.5);
        return true;
    }
    else
    {
        int sum = 0;
        for (int i = 0; i < cfstate.unassembled.size(); i++)
        {

            sum += cfstate.unassembled[i].ports_remaining;
        }
        if (sum == 0)
        {
            return true;
        }
        else
        {



            int n = numParts;
            int x_ind = n - 2 - floor(sqrt(-8 * action + 4 * n * (n - 1) - 7) / 2.0 - 0.5);
            int y_ind = action + x_ind + 1 - n * (n - 1) / 2 + (n - x_ind) * ((n - x_ind) - 1) / 2;
            std::string obj1name = names[x_ind];
            std::string obj2name = names[y_ind];

            auto it = std::find_if(cfstate.unassembled.begin(), cfstate.unassembled.end(), [&](OBJECT_PARTS obj) { return (obj.name == obj1name); });

            auto it2 = std::find_if(cfstate.unassembled.begin(), cfstate.unassembled.end(), [&](OBJECT_PARTS obj) { return obj2name == obj.name; });
            if ((cfstate.unassembled.end() != it) && (cfstate.unassembled.end() != it2))
            {
                if (((*it).ports_remaining > 0) && ((*it2).ports_remaining > 0))
                {
                    std::pair<std::string, std::string> connection((*it).name, (*it2).name);
                    auto it3 = std::find_if(cfstate.connections.begin(), cfstate.connections.end(), [&](std::pair<std::string, std::string> obj) { return ((obj.first == connection.first) && (obj.second == connection.second)); });
                    auto it4 = std::find_if(cfstate.connections.begin(), cfstate.connections.end(), [&](std::pair<std::string, std::string> obj) { return ((obj.first == connection.second) && (obj.second == connection.first)); });
                    if ((cfstate.connections.end() == it3) && (cfstate.connections.end() == it4))
                    {
                        if (isValid(connection) == true)
                        {
                            reward = 10;
                            //cout << "connection"<< connection.first << " " << connection.second << endl;
                            observation = 1;
                            cfstate.connections.push_back(connection);
                            //found.push_back(connection);
                            (*it).ports_remaining = (*it).ports_remaining - 1;
                            (*it2).ports_remaining = (*it2).ports_remaining - 1;
                            if ((*it).ports_remaining <= 0)
                            {
                                it = cfstate.unassembled.erase(it);
                            }
                            if ((*it2).ports_remaining <= 0)
                            {
                                /*cout << "initial unassembled" << endl;
                                for (int i = 0; i < cfstate.unassembled.size(); i++)
                                {

                                    cout << cfstate.unassembled[i].name << " , " << cfstate.unassembled[i].ports_remaining << endl;
                                }
                                cout << "Removing from unassembled " << (*it2).name << (*it2).ports_remaining << endl;
                                it2 = cfstate.unassembled.erase(it2);
                                /*cout << "remaining unassembled" << endl;
                                for (int i = 0; i < cfstate.unassembled.size(); i++)
                                {

                                    cout << cfstate.unassembled[i].name << " , " << cfstate.unassembled[i].ports_remaining << endl;
                                }

                            }

                        }
                        else
                        {
                            reward = -100;
                            observation = 0;
                            return false;

                        }
                    }
                    else
                    {
                        
                        reward = -1;
                        observation = 1;
                        return false;

                    }


                }
                else
                {
                    reward = -10;
                    observation = 0;
                    //return false;
                }

            }
            else
            {
                reward = -10;
                observation = 0;
                //return true;
            }

            if (cfstate.unassembled.size() == 0)
            {
                reward += (num * (num - 1) * 0.5);
                return true;
            }
            else
            {
                int sum = 0;
                for (int i = 0; i < cfstate.unassembled.size(); i++)
                {

                    sum += cfstate.unassembled[i].ports_remaining;
                }
                if (sum == 0)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
        }
    }*/

    /*COORD actionPos = bsstate.Cells.Coord(action);

    BATTLESHIP_STATE::CELL& cell = bsstate.Cells(actionPos);
    if (cell.Visited)
    {
        reward = -10;
        observation = 0;
        DisplayState(state, cout);
        DisplayAction(action, cout);
        assert(false);
    }
    else
    {
        if (cell.Occupied) // hit
        {
            reward = -1;
            observation = 1;
            bsstate.NumRemaining--;

            // Mark four diagonals, not possible for ships to be here
            for (int d = 4; d < 8; ++d)
                if (bsstate.Cells.Inside(actionPos + COORD::Compass[d]))
                    bsstate.Cells(actionPos + COORD::Compass[d]).Diagonal = true;
        }
        else // miss
        {
            reward = -1;
            observation = 0;
        }
        cell.Visited = true;
    }

    if (bsstate.NumRemaining == 0)
    {
        reward += XSize * YSize;
        return true;
    }
    else
    {
        return false;
    }*/
//cout << cfstate.unassembled.size() << endl;
OBJECT_PARTS& obj = cfstate.unassembled[action];
if (obj.visited)
{
    reward = -10;
    observation = 0;

}
else
{
    if (obj.legit)
    {
        reward = -1;
        //reward = 10;
        observation = 1;
        cfstate.numleft = cfstate.numleft - 1;
    }
    else
    {
        reward = -1;
        observation = 0;
    }
    obj.visited = true;
}
if (cfstate.numleft == 0)
{
    reward += NumActions;
    //cout << "num left " << cfstate.numleft << endl;
    return true;
}
else
{
    return false;
}
}
bool CAUSAL_FURNITURE::isValid(std::pair<std::string, std::string> connection) const
{
    std::vector<std::pair<std::string, std::string>> validconnections{ std::make_pair("wall plug", "backplate"),std::make_pair("lightbulb", "backplate"), std::make_pair("lightbulb","lamp body") };
    auto it3 = std::find_if(validconnections.begin(), validconnections.end(), [&](std::pair<std::string, std::string> obj) { return ((obj.first == connection.first) && (obj.second == connection.second)); });
    auto it4 = std::find_if(validconnections.begin(), validconnections.end(), [&](std::pair<std::string, std::string> obj) { return ((obj.first == connection.second) && (obj.second == connection.first)); });
    if ((validconnections.end() == it3) && (validconnections.end() == it4))
        return false;
    else
        return true;
}

bool CAUSAL_FURNITURE::isCausal(std::pair<std::string, std::string> connection) const
{
    std::vector<std::pair<std::string, std::string>> validconnections{ std::make_pair("wall plug", "backplate"),std::make_pair("lightbulb", "backplate"), std::make_pair("lightbulb","lamp body") };
    auto it3 = std::find_if(validconnections.begin(), validconnections.end(), [&](std::pair<std::string, std::string> obj) { return ((obj.first == connection.first) && (obj.second == connection.second)); });
    auto it4 = std::find_if(validconnections.begin(), validconnections.end(), [&](std::pair<std::string, std::string> obj) { return ((obj.first == connection.second) && (obj.second == connection.first)); });
    
    if ((validconnections.end() == it3) && (validconnections.end() == it4))
        return false;
    else
        return true;
}
bool CAUSAL_FURNITURE::LocalMove(STATE& state, const HISTORY& history,
    int stepObs, const STATUS& status) const
{
    CAUSAL_FURNITURE_STATE& cfstate = safe_cast<CAUSAL_FURNITURE_STATE&>(state);
    while (1)
    {
        int action = Random(NumActions);
        OBJECT_PARTS& obj = cfstate.unassembled[action];
        if (obj.visited)
            continue;
        else
        {
            obj.legit = !obj.legit;
            if (obj.legit)
                cfstate.numleft++;
            else
                cfstate.numleft--;
            return true;
        }


    }
    return false;
    
    /*
    BATTLESHIP_STATE& bsstate = safe_cast<BATTLESHIP_STATE&>(state);
    bool refreshDiagonals = history.Size() &&
        bsstate.Cells(history.Back().Action).Occupied != history.Back().Observation;

    int mode = Random(3);
    bool success;
    switch (mode)
    {
        case 0:
            success = MoveShips(bsstate);
            break;
        case 1:
            success = SwitchTwoShips(bsstate);
            break;
        case 2:
            success = SwitchThreeShips(bsstate);
            break;
    }
    if (!success)
        return false;

    if (refreshDiagonals)
        for (int i = 0; i < XSize * YSize; ++i)
            bsstate.Cells(i).Diagonal = false;

    for (int t = 0; t < history.Size(); ++t)
    {
        // Ensure that ships are consistent with observation history
        int a = history[t].Action;
        COORD pos = bsstate.Cells.Coord(a);
        const BATTLESHIP_STATE::CELL& cell = bsstate.Cells(a);
        assert(cell.Visited);
        if (cell.Occupied != history[t].Observation)
            return false;

        if (refreshDiagonals && cell.Occupied)
            for (int d = 4; d < 8; ++d)
                if (bsstate.Cells.Inside(pos + COORD::Compass[d]))
                    bsstate.Cells(pos + COORD::Compass[d]).Diagonal = true;
    }

    return true;*/
    //return true;
}
/*
bool BATTLESHIP::MoveShips(BATTLESHIP_STATE& bsstate) const
{
    // Number of ships to move
    int numMoves = Random(1, 4);
    static vector<int> shipIndices;
    shipIndices.clear();

    for (int move = 0; move < numMoves; ++move)
    {
        int shipIndex = Random(bsstate.Ships.size());
        if (Contains(shipIndices, shipIndex))
            return false;
        shipIndices.push_back(shipIndex);
        UnmarkShip(bsstate, bsstate.Ships[shipIndex]);
    }

    for (int move = 0; move < numMoves; ++move)
    {
        SHIP& ship = bsstate.Ships[shipIndices[move]];
        ship.Direction = Random(4);
        ship.Position = COORD(Random(XSize), Random(YSize));
        if (Collision(bsstate, ship))
            return false;
        MarkShip(bsstate, ship);
    }

    return true;
}

bool BATTLESHIP::SwitchTwoShips(BATTLESHIP_STATE& bsstate) const
{
    int longShipIndex = Random(bsstate.Ships.size());
    int shortShipIndex = Random(bsstate.Ships.size());
    SHIP& longShip = bsstate.Ships[longShipIndex];
    SHIP& shortShip = bsstate.Ships[shortShipIndex];

    int sizeDiff = longShip.Length - shortShip.Length;
    if (sizeDiff <= 0)
        return false;

    int longOffset = Random(0, sizeDiff + 1);
    int shortOffset = Random(0, sizeDiff + 1);

    SHIP oldShortShip = shortShip;
    SHIP oldLongShip = longShip;
    longShip.Direction = oldShortShip.Direction;
    longShip.Position = oldShortShip.Position
        + COORD::Compass[COORD::Opposite(oldShortShip.Direction)] * longOffset;
    shortShip.Direction = oldLongShip.Direction;
    shortShip.Position = oldLongShip.Position
        + COORD::Compass[oldLongShip.Direction] * shortOffset;

    UnmarkShip(bsstate, oldLongShip);
    UnmarkShip(bsstate, oldShortShip);

    if (Collision(bsstate, longShip))
        return false;
    MarkShip(bsstate, longShip);
    if (Collision(bsstate, shortShip))
        return false;
    MarkShip(bsstate, shortShip);

    return true;
}

bool BATTLESHIP::SwitchThreeShips(BATTLESHIP_STATE& bsstate) const
{
    int longShipIndex = Random(bsstate.Ships.size());
    int shortShipIndex1 = Random(bsstate.Ships.size());
    int shortShipIndex2 = Random(bsstate.Ships.size());
    SHIP& longShip = bsstate.Ships[longShipIndex];
    SHIP& shortShip1 = bsstate.Ships[shortShipIndex1];
    SHIP& shortShip2 = bsstate.Ships[shortShipIndex2];

    int sizeDiff = longShip.Length - shortShip1.Length - shortShip2.Length;
    if (sizeDiff <= 0 || shortShipIndex1 == shortShipIndex2)
        return false;

    int longOffset = Random(0, longShip.Length - shortShip1.Length + 1);
    int shortOffset1 = Random(0, sizeDiff);
    int shortOffset2 = Random(shortOffset1 + 2, longShip.Length - shortShip2.Length + 1);

    SHIP oldShortShip1 = shortShip1;
    SHIP oldShortShip2 = shortShip2;
    SHIP oldLongShip = longShip;
    longShip.Direction = oldShortShip1.Direction;
    longShip.Position = oldShortShip1.Position
        + COORD::Compass[COORD::Opposite(oldShortShip1.Direction)] * longOffset;
    shortShip1.Direction = oldLongShip.Direction;
    shortShip1.Position = oldLongShip.Position
        + COORD::Compass[oldLongShip.Direction] * shortOffset1;
    shortShip2.Direction = oldLongShip.Direction;
    shortShip2.Position = oldLongShip.Position
        + COORD::Compass[oldLongShip.Direction] * shortOffset2;

    UnmarkShip(bsstate, oldLongShip);
    UnmarkShip(bsstate, oldShortShip1);
    UnmarkShip(bsstate, oldShortShip2);

    if (Collision(bsstate, longShip))
        return false;
    MarkShip(bsstate, longShip);
    if (Collision(bsstate, shortShip1))
        return false;
    MarkShip(bsstate, shortShip1);
    if (Collision(bsstate, shortShip2))
        return false;
    MarkShip(bsstate, shortShip2);

    return true;
}*/

void CAUSAL_FURNITURE::GenerateLegal(const STATE& state, const HISTORY& history,
    vector<int>& legal, const STATUS& status) const
{
    const CAUSAL_FURNITURE_STATE& cfstate = safe_cast<const CAUSAL_FURNITURE_STATE&>(state);
    for (int a = 0; a < NumActions; ++a)
    {
        if (!cfstate.unassembled[a].visited)
        {
            legal.push_back(a);
        }
    }
    //bool diagonals = Knowledge.Level(status.Phase) == KNOWLEDGE::SMART;
    /*if (diagonals)
    {
        for (int a = 0; a < NumActions; ++a)
            if (!bsstate.Cells(a).Visited && !bsstate.Cells(a).Diagonal)
                legal.push_back(a);
    }
    else
    {*/
    /*for (int a = 0; a < NumActions; ++a)
    {
        // reference: https://stackoverflow.com/questions/27086195/linear-index-upper-triangular-matrix
        int n = numParts;
        int x_ind = n - 2 - floor(sqrt(-8 * a + 4 * n * (n - 1) - 7) / 2.0 - 0.5);
        int y_ind = a + x_ind + 1 - n * (n - 1) / 2 + (n - x_ind) * ((n - x_ind) - 1) / 2;
        std::string obj1name = names[x_ind];
        std::string obj2name = names[y_ind];

        auto it = std::find_if(cfstate.unassembled.begin(), cfstate.unassembled.end(), [&](OBJECT_PARTS obj) { return (obj.name == obj1name); });
        auto it2 = std::find_if(cfstate.unassembled.begin(), cfstate.unassembled.end(), [&](OBJECT_PARTS obj) { return obj2name == obj.name; });
        if ((cfstate.unassembled.end() != it) && (cfstate.unassembled.end() != it2))
        {
            if ((*it).ports_remaining > 0 && (*it2).ports_remaining > 0)
            {
                legal.push_back(a);
            }
            //cout << "legal" << a ;
        }
        //cout << endl;

    }
            //if (!bsstate.Cells(a).Visited)
                
    //}*/
}

/*bool BATTLESHIP::Collision(const BATTLESHIP_STATE& bsstate,
    const SHIP& ship) const
{
    COORD pos = ship.Position;
    for (int i = 0; i < ship.Length; ++i)
    {
        if (!bsstate.Cells.Inside(pos))
            return true;
        const BATTLESHIP_STATE::CELL& cell = bsstate.Cells(pos);
        if (cell.Occupied)
            return true;
        for (int adj = 0; adj < 8; ++adj)
            if (bsstate.Cells.Inside(pos + COORD::Compass[adj]) &&
                bsstate.Cells(pos + COORD::Compass[adj]).Occupied)
                return true;
        pos += COORD::Compass[ship.Direction];
    }
    return false;
}

void BATTLESHIP::MarkShip(BATTLESHIP_STATE& bsstate, const SHIP& ship) const
{
    COORD pos = ship.Position;
    for (int i = 0; i < ship.Length; ++i)
    {
        BATTLESHIP_STATE::CELL& cell = bsstate.Cells(pos);
        assert(!cell.Occupied);
        cell.Occupied = true;
        if (!cell.Visited)
            bsstate.NumRemaining++;
        pos += COORD::Compass[ship.Direction];
    }
}

void BATTLESHIP::UnmarkShip(BATTLESHIP_STATE& bsstate, const SHIP& ship) const
{
    COORD pos = ship.Position;
    for (int i = 0; i < ship.Length; ++i)
    {
        BATTLESHIP_STATE::CELL& cell = bsstate.Cells(pos);
        assert(cell.Occupied);
        if (!cell.Visited)
            bsstate.NumRemaining--;
        cell.Occupied = false;
        pos += COORD::Compass[ship.Direction];
    }
}

void BATTLESHIP::DisplayBeliefs(const BELIEF_STATE& beliefState,
    ostream& ostr) const
{
    GRID<int> counts(XSize, YSize);
    counts.SetAllValues(0);

    for (int i = 0; i < beliefState.GetNumSamples(); i++)
    {
        const BATTLESHIP_STATE* bsstate =
            safe_cast<const BATTLESHIP_STATE*>(
                beliefState.GetSample(i));
        for (int x = 0; x < XSize; ++x)
            for (int y = 0; y < YSize; ++y)
                counts(x, y) += bsstate->Cells(x, y).Occupied;
    }

    for (int y = YSize - 1; y >= 0; y--)
    {
        for (int x = 0; x < XSize; x++)
        {
            ostr.width(6);
            ostr.precision(2);
            ostr << fixed << (double) counts(x, y) / beliefState.GetNumSamples();
        }
        ostr << endl;
    }
}*/

void CAUSAL_FURNITURE::DisplayState(const STATE& state, ostream& ostr) const
{
    const CAUSAL_FURNITURE_STATE& cfstate = safe_cast<const CAUSAL_FURNITURE_STATE&>(state);
    /*ostr << endl << "  ";
    for (int x = 0; x < XSize; x++)
        ostr << setw(1) << x << ' ';
    ostr << "  " << endl;
    for (int y = YSize - 1; y >= 0; y--)
    {
        ostr << setw(1) << y << ' ';
        for (int x = 0; x < XSize; x++)
        {
            const BATTLESHIP_STATE::CELL& cell = bsstate.Cells(x, y);
            char c = '.';
            if (cell.Occupied && cell.Visited)
                c = '@';
            else if (cell.Occupied && !cell.Visited)
                c = '*';
            else if (!cell.Occupied && cell.Visited)
                c = 'X';
            else if (!cell.Occupied && cell.Diagonal)
                c = '/';
            ostr << c << ' ';
        }
        ostr << setw(1) << y << endl;
    }
    ostr << "  ";
    for (int x = 0; x < XSize; x++)
        ostr << setw(1) << x << ' ';
    ostr << "  " << endl;
    ostr << "NumRemaining = " << bsstate.NumRemaining << endl;*/
    /*for (int i = 0; i < cfstate.connections.size(); i++)
    {
        cout << "connections" << endl;
        cout << cfstate.connections[i].first << " , " << cfstate.connections[i].second << endl;
    }
    for (int i = 0; i < cfstate.unassembled.size(); i++)
    {
        cout << "unassembled" << endl;
        cout << cfstate.unassembled[i].name << " , " << cfstate.unassembled[i].ports_remaining << endl;
    }*/
    cout << "unassembled" << endl;
    for (int i = 0; i < cfstate.unassembled.size(); i++)
    {
        
        int num = cfstate.unassembled[i].num;
        DisplayAction(num, cout);
        cout << "legit " << cfstate.unassembled[i].legit << ", visited" <<  cfstate.unassembled[i].visited << endl;
    }
}
/*
void BATTLESHIP::DisplayObservation(const STATE& state, int observation, ostream& ostr) const
{
    if (observation)
        cout << "Hit\n";
    else
        cout << "Miss\n";
}

void BATTLESHIP::DisplayAction(int action, ostream& ostr) const
{
    COORD actionPos = COORD(action % XSize, action / XSize);

    ostr << endl << "  ";
    for (int x = 0; x < XSize; x++)
        ostr << setw(1) << x << ' ';
    ostr << "  " << endl;
    for (int y = YSize - 1; y >= 0; y--)
    {
        ostr << setw(1) << y << ' ';
        for (int x = 0; x < XSize; x++)
        {
            char c = ' ';
            if (actionPos == COORD(x, y))
                c = '@';
            ostr << c << ' ';
        }
        ostr << setw(1) << y << endl;
    }
    ostr << "  ";
    for (int x = 0; x < XSize; x++)
        ostr << setw(1) << x << ' ';
    ostr << "  " << endl;
}
*/
void CAUSAL_FURNITURE::DisplayAction(int action, ostream& ostr) const
{


    int n = numParts;
    int x_ind = n - 2 - floor(sqrt(-8 * action + 4 * n * (n - 1) - 7) / 2.0 - 0.5);
    int y_ind = action + x_ind + 1 - n * (n - 1) / 2 + (n - x_ind) * ((n - x_ind) - 1) / 2;
    std::string obj1name = names[x_ind];
    std::string obj2name = names[y_ind];
    ostr << "connect " << obj1name << " to " << obj2name << endl;
}