#include <iostream>  // Include the iostream library for input/output
#include "Frozen_Lake/frozenLake.cpp"
#include <cmath> 
#include <random>
#include <string>
#include <vector>

double** trainAgent(float learningRate, int nTraining, int nEvaluation, State state, int maxSteps,
float gamma, float maxEpsilon, float minEpsilon, float decayRate, unsigned int seed);
vector<char> evaluateAgent(State state, double** qTable, int maxSteps);

int main() 
{   
    //Get the map number from the user 
    string mapNumber;
    std::cout << "Enter the map number (1-3): ";
    std::cin >> mapNumber;  

    //Load Frozen Lake environment
    string environmentPath = "Frozen_Lake/maps/map" + mapNumber+ ".txt";
    State state(environmentPath);
    int qTableRows = state.getDimension();
    int qTableCols = state.getActionSpace();

    //Parameters
    float learningRate = 0.7;
    int nTraining = 10000;
    int nEvaluation = 100;
    int maxSteps = 99;
    float gamma  = 0.95;
    float maxEpsilon = 1.0;           
    float minEpsilon = 0.05;           
    float decayRate = 0.0005;
    unsigned int seed = 12345;
    
    double** qTable = trainAgent(learningRate, nTraining, nEvaluation,state,maxSteps,gamma,maxEpsilon,minEpsilon,decayRate,seed);
    
    cout << "Final Q Table\n";
    for (int i = 0; i < qTableRows; ++i) {
        cout << "State " << i << ":";
        for (int j = 0; j < qTableCols; ++j) {
            cout << qTable[i][j] << " ";
        }
        cout << "\n";
    }

    vector<char> solution = evaluateAgent(state,qTable,100);
    
    cout << "Solution actions: ";
    for (size_t i = 0; i < solution.size(); ++i) {
        cout << solution[i] << " ";
    }
    cout << endl;

    return 0;
}

vector<char> evaluateAgent(State state, double** qTable, int maxSteps){
    vector<char> solActionSet;
    int qTableRows = state.getDimension();
    int qTableCols = state.getActionSpace();

    for (int step = 0; step < maxSteps; step++){
        char action;
        int stateIndex = state.getVal()-1;

        //Find the best action in the q-table at the corresponding state
        int maxIndex = -1;
        double maxVal = -100;
            
        //Find the max value in a state
        for (int n = 0; n < qTableCols; n++){
            if(qTable[stateIndex][n] > maxVal){
                maxVal = qTable[stateIndex][n];
                maxIndex = n;
            }
        }
        action = state.actions[maxIndex];
        solActionSet.push_back(action);

        //Apply the best action to the state
        state.makeAction(action);
     
        if(!state.gameRunning) {
            break;
        }
    }
    return solActionSet;
}

double** trainAgent(float learningRate, int nTraining, int nEvaluation, State state, int maxSteps,
float gamma, float maxEpsilon, float minEpsilon, float decayRate, unsigned int seed){
        
    //Random 
    mt19937 gen(seed);
    //Random distribution for exploitative vs exploration probability
    uniform_real_distribution<float> dis(0.0f, 1.0f);
    //Random distribution for 2, 3 and 4 valid action sets
    std::uniform_int_distribution<int> validActions4(0, 3);
    std::uniform_int_distribution<int> validActions3(0, 2);
    std::uniform_int_distribution<int> validActions2(0, 1);
  

    //Create the q-table by allocating it to memory
    int qTableRows = state.getDimension();
    int qTableCols = state.getActionSpace();
    double** qTable = new double*[qTableRows];
    for (int i = 0; i < qTableRows; ++i) {
        qTable[i] = new double[qTableCols]();
    }
    
    //Initialise values inside the q-table by setting it to 0
    for (int i = 0; i < qTableRows; ++i) {
        for (int j = 0; j < qTableCols; ++j) {
            qTable[i][j] = 0;
        }
    }

    //Iterate through each episode
    for (int episodeN = 0; episodeN < nTraining; episodeN++) {
        //Set the epsilon value which decreases over episodes
        double epsilon = minEpsilon + (maxEpsilon - minEpsilon) * pow(M_E, (-decayRate * episodeN));

        //Reset the initial environment state
        state.reset();

        //Iterate through steps
        for (int step = 0; step < maxSteps; step++){
            //Get the action based on a random value
            float value = dis(gen);
            char action;
            int stateIndex = state.getVal()-1;
            //Exploit
            if(value > epsilon) {
                //Find the best action in the q-table at the corresponding state
                int maxIndex = -1;
                double maxVal = -100;

                string validMoves = state.getValidActions(state.curPos);
                string possibleMoves = "uldr";
                 
                //Find the max value in a state
                for (int n = 0; n < qTableCols; n++){
                    //Check if move is a valid move
                    if(validMoves.find(possibleMoves[n]) == string::npos){
                        continue;
                    }
                    if(qTable[stateIndex][n] > maxVal){
                        maxVal = qTable[stateIndex][n];
                        maxIndex = n;
                    }
                }
                action = state.actions[maxIndex];
            }
            //Explore
            else {
                //Pick a random valid move
                string moves = state.getValidActions(state.curPos);
                if(moves.size() == 2) {
                    action = moves[validActions2(gen)];
                } else if(moves.size() == 3) {
                    action = moves[validActions3(gen)];
                } else if (moves.size() == 4) {
                    action = moves[validActions4(gen)];
                } else {
                    cout << "Invalid actions";
                }
            }

            //Get the reward value of the new state
            string result = state.makeAction(action);
            int newStateIndex = state.getVal()-1;
            int reward = 0;
            if(result == "Finish") {
                reward = 1;
            } 

            //Get the best value of the new state
            double newStateMaxVal = -1;

            for (int n = 0; n < qTableCols; n++){
                if(qTable[newStateIndex][n] > newStateMaxVal){
                    newStateMaxVal = qTable[newStateIndex][n];
                }
            }

            int actionIndex = state.actions.find(action);

            //Update qTable value based on the expected value of next state
            qTable[stateIndex][actionIndex] = qTable[stateIndex][actionIndex] + learningRate * (reward + gamma * newStateMaxVal - qTable[stateIndex][actionIndex]);
            
            //Stop if the game is finished
            if(!state.gameRunning) {
                break;
            }
        }
    } 
    return qTable;
}