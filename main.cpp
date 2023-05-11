#include "battleship.h"
#include "mcts.h"
#include "network.h"
#include "pocman.h"
#include "rocksample.h"
#include "tag.h"
#include "causal_furniture.h"
#include "findit.h"
#include "findit_improved.h"
#include "experiment.h"
#include <boost/program_options.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;
using namespace boost::program_options;

void UnitTests()
{
    cout << "Testing UTILS" << endl;
    UTILS::UnitTest();
    cout << "Testing COORD" << endl;
    COORD::UnitTest();
    cout << "Testing MCTS" << endl;
    MCTS::UnitTest();
}

void disableBufferedIO(void)
{
    setbuf(stdout, NULL);
    setbuf(stdin, NULL);
    setbuf(stderr, NULL);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
}

int main(int argc, char* argv[])
{
    MCTS::PARAMS searchParams;
    EXPERIMENT::PARAMS expParams;
    SIMULATOR::KNOWLEDGE knowledge;
    string problem, outputfile, policy, furniture;
    //std::vector<std::pair<std::string, std::string>> causalconnections{};
    string causalfilename = "";
    int size, number, treeknowledge = 1, rolloutknowledge = 1, smarttreecount = 10;
    double smarttreevalue = 1.0;
    bool causal = false;
    options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("test", "run unit tests")
        ("problem", value<string>(&problem), "problem to run")
        ("outputfile", value<string>(&outputfile)->default_value("output.txt"), "summary output file")
        ("policy", value<string>(&policy), "policy file (explicit POMDPs only)")
        ("size", value<int>(&size), "size of problem (problem specific)")
        ("number", value<int>(&number), "number of elements in problem (problem specific)")
        ("furniture", value<string>(&furniture), "furniture name (problem specific)")
        ("causal", value<bool>(&causal), "causal (problem specific)")
        ("causalfilename", value<string>(&causalfilename), "causal connections file name (problem specific)")
        ("timeout", value<double>(&expParams.TimeOut), "timeout (seconds)")
        ("mindoubles", value<int>(&expParams.MinDoubles), "minimum power of two simulations")
        ("maxdoubles", value<int>(&expParams.MaxDoubles), "maximum power of two simulations")
        ("runs", value<int>(&expParams.NumRuns), "number of runs")
        ("accuracy", value<double>(&expParams.Accuracy), "accuracy level used to determine horizon")
        ("horizon", value<int>(&expParams.UndiscountedHorizon), "horizon to use when not discounting")
        ("num steps", value<int>(&expParams.NumSteps), "number of steps to run when using average reward")
        ("verbose", value<int>(&searchParams.Verbose), "verbosity level")
        ("autoexploration", value<bool>(&expParams.AutoExploration), "Automatically assign UCB exploration constant")
        ("exploration", value<double>(&searchParams.ExplorationConstant), "Manual value for UCB exploration constant")
        ("usetransforms", value<bool>(&searchParams.UseTransforms), "Use transforms")
        ("transformdoubles", value<int>(&expParams.TransformDoubles), "Relative power of two for transforms compared to simulations")
        ("transformattempts", value<int>(&expParams.TransformAttempts), "Number of attempts for each transform")
        ("userave", value<bool>(&searchParams.UseRave), "RAVE")
        ("ravediscount", value<double>(&searchParams.RaveDiscount), "RAVE discount factor")
        ("raveconstant", value<double>(&searchParams.RaveConstant), "RAVE bias constant")
        ("treeknowledge", value<int>(&knowledge.TreeLevel), "Knowledge level in tree (0=Pure, 1=Legal, 2=Smart)")
        ("rolloutknowledge", value<int>(&knowledge.RolloutLevel), "Knowledge level in rollouts (0=Pure, 1=Legal, 2=Smart)")
        ("smarttreecount", value<int>(&knowledge.SmartTreeCount), "Prior count for preferred actions during smart tree search")
        ("smarttreevalue", value<double>(&knowledge.SmartTreeValue), "Prior value for preferred actions during smart tree search")
        ("disabletree", value<bool>(&searchParams.DisableTree), "Use 1-ply rollout action selection")
        ;

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.count("help"))
    {
        cout << desc << "\n";
        return 1;
    }

    if (vm.count("problem") == 0)
    {
        cout << "No problem specified" << endl;
        return 1;
    }

    if (vm.count("test"))
    {
        cout << "Running unit tests" << endl;
        UnitTests();
        return 0;
    }

    SIMULATOR* real = 0;
    SIMULATOR* simulator = 0;

    if (problem == "battleship")
    {
        real = new BATTLESHIP(size, size, number);
        simulator = new BATTLESHIP(size, size, number);
    }
    else if (problem == "pocman")
    {
        real = new FULL_POCMAN;
        simulator = new FULL_POCMAN;
    }
    else if (problem == "network")
    {
        real = new NETWORK(size, number);
        simulator = new NETWORK(size, number);
    }
    else if (problem == "rocksample")
    {
        real = new ROCKSAMPLE(size, number);
        simulator = new ROCKSAMPLE(size, number);
    }
    else if (problem == "tag")
    {
        real = new TAG(number);
        simulator = new TAG(number);
    }
    else if (problem == "findit")
    {
        real = new FINDIT(size, number);
        simulator = new FINDIT(size, number);
    }
    else if (problem == "findit_improved")
    {
        real = new FINDIT_IMPROVED(size, number);
        simulator = new FINDIT_IMPROVED(size, number);
    }
    else if (problem == "causal_furniture")
    {
        //cout << "numparts" << numParts << endl;
        std::vector<std::vector<std::pair<std::string, std::string>>> allusers;
        std::vector<std::pair<std::string, std::string>> allpairs;
        string line,pair,conn;
        std::pair<std::string, std::string> connection;
        std::vector<std::string> parts;
        cout << "args" << number << " " << causalfilename << endl;
        ifstream file;
        file.open(causalfilename);
        cout << file.is_open() << endl;
        while (getline(file, line))
        {
            cout << "hello" << line << endl;
            allpairs.clear();
            if (line[line.size() - 1] == '\r')
                line.erase(line.size() - 1);
            istringstream str(line);
            while (getline(str, pair, '|'))
            {
                cout << "pair" << pair << endl;
                
                istringstream substr(pair);
                parts.clear();
                while (getline(substr, conn, ','))
                {
                    cout << conn << endl;
                    parts.push_back(conn);

                }
                    cout << "parts " << parts.size() << endl;
                    allpairs.push_back(make_pair(parts[0], parts[1]));
                
            }
            allusers.push_back(allpairs);
            cout << "all pairs " << allpairs.size() << endl;
        }
        //real = new CAUSAL_FURNITURE(furniture, number, causal, allusers[0]);
        //simulator = new CAUSAL_FURNITURE(furniture, number, causal, allusers[0]);
        cout << "all " << allusers.size() << endl;
        string outputfname;
        //std::vector<std::vector<std::pair<std::string, std::string>>> allusers2 = allusers;
       for (int i = 0; i < allusers.size(); i++)
        {
            cout << "user " << i + 1 << endl;
            outputfname = outputfile;
            real = new CAUSAL_FURNITURE(furniture, number, causal, allusers[i]);
            simulator = new CAUSAL_FURNITURE(furniture, number, causal, allusers[i]);
            simulator->SetKnowledge(knowledge);
            /*if (i == 0)
            {
                EXPERIMENT experiment(*real, *simulator, outputfile, expParams, searchParams);
            }
            else
            {*/
                //EXPERIMENT* experiment = new EXPERIMENT(*real, *simulator, outputfile, expParams, searchParams);
            //}
            outputfname = outputfname + "user_" + to_string((i + 1)) + ".txt";
            cout << outputfname << endl;
            EXPERIMENT experiment(*real, *simulator, outputfname, expParams, searchParams);
            experiment.DiscountedReturn();
            delete real;
            delete simulator;
            //delete experiment;
            SIMULATOR* real = 0;
            SIMULATOR* simulator = 0;
        }
        
        return 0;

        //simulator = new CAUSAL_FURNITURE("kerosene lamp", 6);
    }
    else 
    {
        cout << "Unknown problem" << endl;
        exit(1);
    }


    simulator->SetKnowledge(knowledge);
    EXPERIMENT experiment(*real, *simulator, outputfile, expParams, searchParams);
    experiment.DiscountedReturn();

    delete real;
    delete simulator;
    return 0;
}
