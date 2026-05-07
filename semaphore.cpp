// Compile:  g++ -Wall -o sub1 semaphore.cpp
// Run:      ./sub1
// Save output to file:  ./sub1 > semaphore_output.txt

#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;


const int READER1 = 0; 
const int READER2 = 1; 
const int READER3 = 2; 
const int WRITER1 = 3; 
const int WRITER2 = 4; 

const int PROCESSES = 5;

const int MAXIMUM_STEPS = 80;

//semaphores that well have
int readers_allowed = 2; // only 2 of the readers are allowed at a time
int writers_allowed = 1; // One writer is able to run and nothing else

// we need to count how many readers and writers are runninng 
int readers_running = 0;
int writers_running = 0;
//we have an array that'll keep track of what reader and/or writers are running
int pc[PROCESSES] = {0,0,0,0,0}; // pc[0] will be reader1 and pc[3] will be writer1. They're index is based on the const value we gave them in the top

//function that'll print if there are more then 1 wrtier or more then 2 reader
void warning(){
    if (writers_running > 1){
        cout << "WARNING! There are more then 1 writer running in your Critical Section\n";
    }
    if (writers_running >= 1 && readers_running >= 1){
        cout << "WARNING! There is a writer and a reader running in your Critical Section at the same time.\n";
    }
    if (readers_running > 2){
        cout << "WARNING! There is more then 2 readers running in your Critical section at the same time.\n";
    }
}

//create a function for the reader functions.
void reader(int id){
    switch(pc[id]){
        case 0: // this is a case we are trying to enter the critical section and checking if there are a reader slot available to take
            cout << "Reader " << (id + 1) << " attempting to enter slots = " << readers_allowed << "\n";
            if (readers_allowed > 0){
                readers_allowed--;  // the process takes an open slot
                readers_running++; // the process is running
                pc[id] = 1; // this makes it so that the next time we do the swtiich statement it'll go to case 1
            }// this is good since if there are no opne slot, the process doesnt go pass case 0
            break;
        case 1:// now were inside the critical section
            cout << "Reader " << (id + 1) << " has entered the critical section.\n";
            cout << "There are " << (readers_running-1) << " other readers running.\n";
            cout << "There are " << writers_running << " writers running.\n";

            warning();
            pc[id] = 2;
            break;
        
        case 2: //read shared data
            cout << "The Reader " << (id + 1) << " is currently reading the shared data.\n";
            pc[id] = 3;
            break;
        
        case 3: //Process leaves the critical section
            readers_running--; // The reader stops  running
            readers_allowed++; // gives  up  the spot that was  taken
            cout<< "The Reader " << (id + 1) << " has left the critical section and now there are " << readers_allowed << " slots that are open.\n";
            pc[id] = 0;
            break;
    }
}

void writer(int id){
    switch(pc[id]){
        case 0:
            cout << "Writer " << (id - 2) << " tries to do writer lock.\nThere are " << writers_allowed << " allowed to run.\n";
            if (writers_allowed > 0) {
                writers_allowed--; // writer lock
                pc[id] = 1;
            }
            break;
        case 1://needs to wait for there to be a fully empty room first. So there needs to be 2 readers allowed showing that there are no readers in Criticall section
            if (readers_allowed == 2){
                readers_allowed = 0;
                writers_running++;
                pc[id] = 2;
            } else{
                cout << "The writer " << (id - 2) << " Waiting for all of the readers to finish first. Right now there are " << readers_running << " currently running.\n";
            }
            break;
        case 2:
            // writer is in the critical section
            cout << "Writer " << (id - 2) << " has entered the critical section. The other writers = " 
                 << (writers_running - 1) << ", readers = " << readers_running << "\n";
            warning();
            pc[id] = 3;
            break;
            
        case 3:
            // writer updates the shared data
            cout << "Writer " << (id - 2) << " is now updates the shared data.\n";
            pc[id] = 4;
            break;
            
        case 4:
            // Release the reader slots 
            writers_running--;           // opened  slot
            readers_allowed = 2;         // now both reader slots are open
            cout << "Writer " << (id - 2) << " released. There are " << readers_allowed << " reader slots\n";
            pc[id] = 5;
            break;
            
        case 5:
            // writer lock released 
            writers_allowed++;
            cout << "Writer " << (id - 2) << " writer lock is released\n";
            pc[id] = 0;                  // go back to the start
            break;

    }
}

int main(){
    srand(time(0));   // use the random number generator with current time
    
    cout << "Part 1: semaphore \n";
    cout << "Readers allowed=" << readers_allowed << ", Writers allowed=" << writers_allowed << "\n\n";
    
    // Run for MAXIMUM_STEPS turns
    for (int step = 0; step < MAXIMUM_STEPS; step++){
        int x = rand() % PROCESSES;   // random number 0 to 4
        cout << " Step " << (step + 1) << ": ";
        
        // Call the functiion based on the x
        switch(x){
            case READER1: reader(READER1); break;
            case READER2: reader(READER2); break;
            case READER3: reader(READER3); break;
            case WRITER1: writer(WRITER1); break;
            case WRITER2: writer(WRITER2); break;
        }
    }
    
    cout << "\nDone";
    return 0;
}