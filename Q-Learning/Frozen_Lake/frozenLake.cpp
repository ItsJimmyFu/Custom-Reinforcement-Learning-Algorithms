#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std; 

class State {
    private:
        
    public:
        vector<array<int,2> > holesPos;
        array<int,2> startPos;
        array<int,2> curPos;
        array<int,2> endPos;
        int gridHeight;
        int gridWidth;
        bool gameRunning;
        string actions;

        //Constructor to create a state from a string state
        State(string filePath) {
            gameRunning = true;
            ifstream file(filePath);  // Open the file
            stringstream buffer;
            
            if (!file) {  // Check if the file was successfully opened
                cerr << "Error: Could not open file " << filePath << endl;
                return; //Return if file can not be opened
            }
            
            buffer << file.rdbuf();  // Read the file into the stringstream
            string state = buffer.str();

            //Read the state string and get positions of start, finish, holes and row and column size
            int curRow = 1;
            int curCol = 1;
            //Iterate through the state string
            for (size_t i = 0; i < state.length(); i++) {
                //If reads the end of line, go to the position at the start of the next row
                if(state[i] == '\n'){
                    curRow += 1;
                    curCol = 1;
                }
                else {
                    //Check if the start position
                    if(state[i] == 'S'){
                        curPos[0] = curCol;
                        curPos[1] = curRow;
                        startPos[0] = curCol;
                        startPos[1] = curRow;
                    } else if (state[i] == 'H') {
                        // Add an array at the end using push_back
                        array<int, 2> holePos = {curCol, curRow};
                        holesPos.push_back(holePos);
                    } else if (state[i] == 'F') {
                        endPos[0] = curCol;
                        endPos[1] = curRow;
                    } 
                    curCol += 1;
                }
            }
            gridHeight = curRow;
            gridWidth = curCol-1;
            actions = "uldr";
        }

        int getDimension(){
            return gridHeight*gridWidth;
        }

        int getActionSpace(){
            return actions.size();
        }

        void reset(){
            gameRunning = true;
            curPos = startPos;
        }

        int getVal(){
            return (curPos[1] - 1) * gridWidth + curPos[0];
        }

        //Get all the valid actions in the current state
        string getValidActions(array<int,2> pos){
            string validActions = "";

             //Check if above the grid space
            if(pos[1] > 1) {
                validActions.push_back('u');
            }
            //Check if below the grid space
            if(pos[1] < gridHeight) {
                validActions.push_back('d');
            }
            //Check if to the left of the grid space
            if(pos[0] > 1) {
                validActions.push_back('l');
            }
            //Check if to the right of the grid space
            if(pos[0] < gridWidth) {
                validActions.push_back('r');
            }
            return validActions;
        }

        //Take the valid action and apply it to the state
        string makeAction(char direction) {
            if(gameRunning) {
                //Play the action
                switch (tolower(direction)) {
                    case 'u':
                        curPos[1] = curPos[1]-1;
                        break;
                    case 'l':
                        curPos[0] = curPos[0]-1;
                        break;
                    case 'd':
                        curPos[1] = curPos[1]+1;
                        break;
                    case 'r':
                        curPos[0] = curPos[0]+1;
                        break;
                }
            }

            //Return feedback of action
            if(curPos == endPos){
                gameRunning = false;
                return "Finish";
            } else {
                for (int n = 0; n < holesPos.size(); n++){
                    if(holesPos[n] == curPos){
                        gameRunning = false;
                        return "Hole";
                    }
                }
                return "Safe";
            }
        }

        //Print the output of the state
        void printState(){
            cout << "Dimensions:" << gridWidth << " " << gridHeight << endl;
            cout << "Current Pos:" << curPos[0] << "," << curPos[1] << endl;
            cout << "End Pos:" << endPos[0] << "," << endPos[1] << endl;
            cout << "Holes: ";
            for (int n = 0; n < holesPos.size(); n++) {
                cout << holesPos[n][0] << "," << holesPos[n][1] << " ";
            }
            cout << endl;

        }
};