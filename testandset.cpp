// Compile:  g++ -Wall -o sub2 testandset.cpp
// Run:      ./sub2
// Save output to file:  ./sub2 > testandset_output.txt

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

// the locks for TestAndSet, false means free and true means someone took it
bool count_lock = false; // small lock thats only used when updating the counter
bool writer_lock = false; // big lock thats held during reading or writing

// counter for how many readers are inside the critical section right now
int reader_count = 0;

// same as subproject 1, were keeping track of who is running
int readers_running = 0;
int writers_running = 0;

// program counter array, one for each process
int pc[PROCESSES] = {0,0,0,0,0};

// TestAndSet function from the lecture notes slide 18
// it grabs the lock by setting it to true, but it returns the old value first
bool TestAndSet(bool *target){
    bool old = *target; // remember what the lock was before
    *target = true; // set the lock to taken
    return old; // give back the old value so we know if we got the lock or not
}

//function that prints if any of the rules got broken in the critical section
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

//create a function for the reader
void reader(int id){
    switch(pc[id]){
        case 0: // try to grab the count_lock so we can safely update the counter
            cout << "Reader " << (id + 1) << " is trying to get the count_lock\n";
            if (TestAndSet(&count_lock) == false){
                // we got the lock since it returned false
                pc[id] = 1;
            }// if it returned true that means somone else has it, stay at case 0
            break;

        case 1: // we have the count_lock, now we check if we can actually enter
            if (reader_count < 2 && writer_lock == false){
                // theres room for us and no writer is in. we can enter
                if (reader_count == 0){
                    // were the first reader so we grab the writer_lock too
                    writer_lock = true;
                }
                reader_count++; // we are now counted as inside
                readers_running++; // bookkeeping
                count_lock = false; // give back the count_lock so other readers can use it
                cout << "Reader " << (id + 1) << " has entered the critical section.\n";
                cout << "There are " << (readers_running - 1) << " other readers running.\n";
                cout << "There are " << writers_running << " writers running.\n";
                warning();
                pc[id] = 2;
            } else {
                // either a writer is in or there are already 2 readers, we cant enter
                count_lock = false; // give back the lock and try again later
                cout << "Reader " << (id + 1) << " is blocked. reader_count=" << reader_count << ", writer_lock=" << writer_lock << "\n";
                pc[id] = 0;
            }
            break;

        case 2: // read shared data
            cout << "The Reader " << (id + 1) << " is currently reading the shared data.\n";
            pc[id] = 3;
            break;

        case 3: // we need to grab count_lock again so we can safely decrement reader_count
            if (TestAndSet(&count_lock) == false){
                pc[id] = 4;
            } else {
                cout << "Reader " << (id + 1) << " is waiting for count_lock to leave\n";
            }
            break;

        case 4: // we have count_lock, now we leave for real
            reader_count--; // were not counted anymore
            readers_running--; // bookkeeping
            if (reader_count == 0){
                // were the last reader leaving so we let go of writer_lock
                writer_lock = false;
            }
            count_lock = false; // give back count_lock
            cout << "The Reader " << (id + 1) << " has left the critical section. reader_count=" << reader_count << "\n";
            pc[id] = 0;
            break;
    }
}

//writer function for the TestAndSet version
void writer(int id){
    switch(pc[id]){
        case 0: // try to grab the writer_lock with TestAndSet
            cout << "Writer " << (id - 2) << " is trying to get the writer_lock\n";
            if (TestAndSet(&writer_lock) == false){
                // we got the lock, no readers can come in and no other writer either
                writers_running++;
                pc[id] = 1;
            }// if it returned true somone else has the lock, stay at case 0
            break;

        case 1: // we are inside the critical section
            cout << "Writer " << (id - 2) << " has entered the critical section. The other writers = " 
                 << (writers_running - 1) << ", readers = " << readers_running << "\n";
            warning();
            pc[id] = 2;
            break;

        case 2: // writer updates the shared data
            cout << "Writer " << (id - 2) << " is now updates the shared data.\n";
            pc[id] = 3;
            break;

        case 3: // release the writer_lock so others can come in
            writers_running--;
            writer_lock = false;
            cout << "Writer " << (id - 2) << " writer_lock is released\n";
            pc[id] = 0; // go back to start
            break;
    }
}

int main(){
    srand(time(0));   // use the random number generator with current time

    cout << "Part 2: Test and Set\n";
    cout << "count lock = " << count_lock << ", writer lock = " << writer_lock << ", reader count = " << reader_count << "\n\n";

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