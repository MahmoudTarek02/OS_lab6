#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <sstream>
#include <algorithm>
#include <iomanip>

using namespace std;

// Process struct
struct Process
{
    string processName = "";
    int arrivalTime;
    int serviceTime;
    int remainingTime = 0;
    bool hasCPU = false;
    int finishTime = -1;
    int turnaroundTime = 0;
    float normTurn = 0;
    float responseRatio = 0;
    int quantum = 0;
    int started_executing = 0;
    int time_completed = 0;
    bool running = false;
    bool waiting = false;
    bool finished = false;
    int time_completed_in_a_row = 0;

    int waiting_time = 0;
    int initial_priority = 0;
    int current_priority = 0;
    bool was_executing = false;

    int prev_queue = 0; // for fd 1i
};

struct SPN_compare
{
    bool operator()(const Process &left, const Process &right)
    {
        return left.serviceTime > right.serviceTime; // Shortest service time first
    }
};

struct SRTF_compare
{
    bool operator()(const Process &left, const Process &right)
    {
        if (left.remainingTime == right.remainingTime)
        {
            // If remaining times are equal, prioritize the process with lower arrival time
            return left.arrivalTime > right.arrivalTime;
        }
        return left.remainingTime > right.remainingTime; // Otherwise, prioritize by shortest remaining time
    }
};

struct CompareProcess
{
    bool operator()(const Process &p1, const Process &p2)
    {
        if (p1.current_priority == p2.current_priority)
        {
            return p1.waiting_time < p2.waiting_time;
        }
        return p1.current_priority < p2.current_priority;
    }
};

int getProcessIndex(string name);
void Feedback_2i(const vector<string> &lines, bool trace);
void printFirstLine(const string &line, int total_time);
void SPN(const vector<string> &lines, bool trace);
void HRRN(const vector<string> &lines, bool trace);
void printResults(bool trace, vector<Process> &processes, const vector<vector<char>> &simulation, int total_time, int numProcesses, string type);
void Aging(const vector<string> &lines, int numberAfterDash, bool trace);
void FCFS(const vector<string> &lines, bool trace);
void SRTF(const vector<string> &lines, bool trace);
void Feedback_1i(const vector<string> &lines, bool trace);
void RoundRobin(const vector<string> &lines, int numberAfterDash, bool trace);

#include <iostream>
#include <vector>
#include <sstream>
#include <string>

using namespace std;

#include <iostream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

int main()
{
    vector<string> lines;
    string line;
    int numberAfterDash = -1;

    // Read input lines
    while (getline(cin, line))
    {
        lines.push_back(line);
    }

    // Split the second line into separate algorithm strings
    vector<string> algorithms;
    stringstream ss(lines[1]);
    string algo;

    while (getline(ss, algo, ','))
    {
        algorithms.push_back(algo);
    }

    // Process each algorithm
    for (const auto &algorithm : algorithms)
    {
        string algoName;
      
        size_t pos = algorithm.find('-');
        if (pos != string::npos)
        {
            numberAfterDash = stoi(algorithm.substr(pos + 1));
            algoName = algorithm.substr(0, pos); 
        }
        else
        {
            numberAfterDash = -1; 
            algoName = algorithm;
        }

        if (algoName == "3")
        {
            if (lines[0] == "trace")
            {
                SPN(lines, true); 
            }
            else
            {
                SPN(lines, false); 
            }
        }
        else if (algoName == "5")
        {
            if (lines[0] == "trace")
            {
                HRRN(lines, true); 
            }
            else
            {
                HRRN(lines, false); 
            }
        }
        else if (algoName == "7")
        {
            if (lines[0] == "trace")
            {
                Feedback_2i(lines, true); 
            }
            else
            {
                Feedback_2i(lines, false); 
            }
        }
        else if (algoName == "8")
        {
            if (lines[0] == "trace")
            {
                Aging(lines, numberAfterDash, true); // trace
            }
            else
            {
                Aging(lines, numberAfterDash, false); // stats
            }
        }
        else if (algoName == "1")
        {
            if (lines[0] == "trace")
            {
                FCFS(lines, true); // trace
            }
            else
            {
                FCFS(lines, false); // stats
            }
        }
        else if (algoName == "4")
        {
            if (lines[0] == "trace")
            {
                SRTF(lines, true); // trace
            }
            else
            {
                SRTF(lines, false); // stats
            }
        }
        else if (algoName == "6")
        {
            if (lines[0] == "trace")
            {
                Feedback_1i(lines, true); // trace
            }
            else
            {
                Feedback_1i(lines, false); // stats
            }
        }
        else if (algoName == "2")
        {
            if (lines[0] == "trace")
            {
                RoundRobin(lines, numberAfterDash, true); // trace
            }
            else
            {
                RoundRobin(lines, numberAfterDash, false); // stats
            }
        }
    }

    return 0;
}



void SRTF(const vector<string> &lines, bool trace)
{
    int total_time = stoi(lines[2]);
    int numProcesses = stoi(lines[3]);

    priority_queue<Process, vector<Process>, SRTF_compare> ready_queue;
    Process currentProcess;
    bool cpuIdle = true;

    vector<Process> processes;
    vector<vector<char>> simulation(numProcesses, vector<char>(total_time, ' ')); // 2D array for simulation

    for (int i = 4; i < numProcesses + 4; i++)
    {
        stringstream ss(lines[i]);
        string processName, arrivalTime, serviceTime;

        getline(ss, processName, ',');
        getline(ss, arrivalTime, ',');
        getline(ss, serviceTime, ',');

        Process p;
        p.processName = processName;
        p.arrivalTime = stoi(arrivalTime);
        p.serviceTime = stoi(serviceTime);
        p.remainingTime = p.serviceTime;

        processes.push_back(p);
    }

    for (int current_time = 0; current_time < total_time; current_time++)
    {

        for (auto &p : processes)
        {
            if (p.arrivalTime == current_time && !p.hasCPU && p.remainingTime > 0)
            {
                ready_queue.push(p);
            }
        }

        if (!cpuIdle && !ready_queue.empty() &&
            ready_queue.top().remainingTime < currentProcess.remainingTime)
        {
            ready_queue.push(currentProcess);
            currentProcess = ready_queue.top();
            ready_queue.pop();
        }

        if (cpuIdle && !ready_queue.empty())
        {
            currentProcess = ready_queue.top();
            ready_queue.pop();
            cpuIdle = false;
        }

        if (!cpuIdle)
        {
            for (int i = 0; i < numProcesses; i++)
            {
                if (processes[i].processName == currentProcess.processName)
                {
                    simulation[i][current_time] = '*';
                    break;
                }
            }

            currentProcess.remainingTime--;
            if (currentProcess.remainingTime == 0)
            {
                cpuIdle = true;
                for (int i = 0; i < numProcesses; i++)
                {
                    if (processes[i].processName == currentProcess.processName)
                    {
                        processes[i].finishTime = current_time + 1;
                        processes[i].turnaroundTime = processes[i].finishTime - processes[i].arrivalTime;
                        break;
                    }
                }
            }
        }
        vector<Process> temp_queue;

        while (!ready_queue.empty())
        {
            Process p = ready_queue.top();
            ready_queue.pop();
            for (int i = 0; i < numProcesses; i++)
            {
                if (processes[i].processName == p.processName)
                {
                    simulation[i][current_time] = '.';
                    break;
                }
            }
            temp_queue.push_back(p);
        }

        for (auto &p : temp_queue)
        {
            ready_queue.push(p);
        }
    }

    printResults(trace, processes, simulation, total_time, numProcesses, "SRT");
}

void FCFS(const vector<string> &lines, bool trace)
{
    int total_time = stoi(lines[2]);

    int numProcesses = stoi(lines[3]);

    vector<Process> processes;
    vector<vector<char>> simulation(numProcesses, vector<char>(total_time, ' '));

    vector<Process> ready_queue;

    for (int i = 4; i < numProcesses + 4; i++)
    {
        stringstream ss(lines[i]);
        string processName, arrivalTime, serviceTime;

        getline(ss, processName, ',');
        getline(ss, arrivalTime, ',');
        getline(ss, serviceTime, ',');

        Process p;
        p.processName = processName;
        p.arrivalTime = stoi(arrivalTime);
        p.serviceTime = stoi(serviceTime);
        p.remainingTime = p.serviceTime;

        processes.push_back(p);
    }
    bool cpuIdle = true;
    for (int current_time = 0; current_time <= total_time; current_time++)
    {
        // Add processes to the ready queue when they arrive
        for (int i = 0; i < numProcesses; i++)
        {
            if (processes[i].arrivalTime == current_time)
            {
                processes[i].waiting = true;
                ready_queue.push_back(processes[i]);
            }
        }

        sort(ready_queue.begin(), ready_queue.end(), [](const Process &left, const Process &right)
             { return left.arrivalTime < right.arrivalTime; });

        if (!cpuIdle)
        {
            for (int i = 0; i < numProcesses; i++)
            {
                if (processes[i].hasCPU)
                {
                    if (processes[i].time_completed == processes[i].serviceTime)
                    {
                        processes[i].finishTime = current_time;
                        processes[i].turnaroundTime = processes[i].finishTime - processes[i].arrivalTime;
                        processes[i].hasCPU = false;
                        processes[i].running = false;
                        processes[i].waiting = false;
                        processes[i].finished = true;
                        simulation[i][current_time] = ' ';
                        cpuIdle = true;
                    }
                    else
                    {
                        processes[i].remainingTime--;
                        processes[i].time_completed++;
                        processes[i].running = true;
                        simulation[i][current_time] = '*';
                    }
                }
            }
        }
        if (cpuIdle)
        {
            if (!ready_queue.empty())
            {
                int index = getProcessIndex(ready_queue[0].processName);
                ready_queue.erase(ready_queue.begin());
                processes[index].hasCPU = true;
                processes[index].running = true;
                processes[index].time_completed++;
                cpuIdle = false;
                simulation[index][current_time] = '*';
            }
        }

        for (int i = 0; i < numProcesses; i++)
        {
            if (processes[i].running)
            {
                simulation[i][current_time] = '*';
            }
            else if (processes[i].waiting)
            {
                simulation[i][current_time] = '.';
            }
            else if (processes[i].finished)
            {
                simulation[i][current_time] = ' ';
            }
        }
    }
    printResults(trace, processes, simulation, total_time, numProcesses, "FCFS");
}

void Aging(const vector<string> &lines, int numberAfterDash, bool trace)
{

    int total_time = stoi(lines[2]);

    int numProcesses = stoi(lines[3]);

    int quantum = numberAfterDash;

    vector<Process> processes;
    vector<vector<char>> simulation(numProcesses, vector<char>(total_time, ' '));
    priority_queue<Process, vector<Process>, CompareProcess> ready_queue;
    // vector<Process> ready_queue;

    bool cpuIdle = true;
    Process current_process;

    for (int i = 4; i < numProcesses + 4; i++)
    {
        stringstream ss(lines[i]);
        string processName, arrivalTime, initial_priority;

        getline(ss, processName, ',');
        getline(ss, arrivalTime, ',');
        getline(ss, initial_priority, ',');

        Process p;
        p.processName = processName;
        p.arrivalTime = stoi(arrivalTime);
        p.initial_priority = stoi(initial_priority);
        p.remainingTime = p.serviceTime;

        processes.push_back(p);
    }

    for (int current_time = 0; current_time < total_time; current_time++)
    {
        // cout << endl
        //      << "Current time: " << current_time << endl;
        // // // Add processes to the ready queue when they arrive
        for (int i = 0; i < numProcesses; i++)
        {
            if (processes[i].arrivalTime == current_time)
            {
                processes[i].waiting = true;
                processes[i].running = false;
                processes[i].waiting_time = 1;
                processes[i].current_priority = processes[i].initial_priority;
                ready_queue.push(processes[i]);

                // cout << "Process: " << processes[i].processName << " added to the ready queue ";
                // cout << "with initial priority: " << processes[i].initial_priority << endl;
                // cout << "Current priority: " << processes[i].current_priority << endl;
            }
        }

        // update the priority queue due to changes of current priority after each time iteration

        vector<Process> temp_queue;

        while (!ready_queue.empty())
        {
            int index = getProcessIndex(ready_queue.top().processName);
            if (processes[index].waiting)
            {
                processes[index].waiting_time++;
                simulation[index][current_time] = '.';
            }
            if (processes[index].waiting)
            {
                processes[index].current_priority++;
            }
            processes[index].was_executing = false;
            ready_queue.pop();
            temp_queue.push_back(processes[index]);
        }
        if (!temp_queue.empty())
        {
            for (auto &p : temp_queue)
            {
                ready_queue.push(p);
            }
        }

        if (!cpuIdle)
        {
            if (!ready_queue.empty())
            {
                Process top_process = ready_queue.top();
                if (top_process.current_priority >= current_process.initial_priority)
                { // switching between processes
                    // cout << endl
                    //      << "Switching between processes" << endl;
                    // cout << "Current process: " << current_process.processName << " with priority: " << current_process.current_priority << endl;
                    // cout << "Top process: " << top_process.processName << " with priority: " << top_process.current_priority << endl
                    //      << endl;
                    int current_process_index = getProcessIndex(current_process.processName);
                    processes[current_process_index].hasCPU = false;
                    processes[current_process_index].waiting = true;
                    processes[current_process_index].running = false;
                    processes[current_process_index].waiting_time = 1;
                    processes[current_process_index].was_executing = true;
                    processes[current_process_index].current_priority = processes[current_process_index].initial_priority;
                    ready_queue.push(processes[current_process_index]);
                    simulation[current_process_index][current_time] = '.';

                    int top_process_index = getProcessIndex(top_process.processName);
                    processes[top_process_index].was_executing = false;
                    processes[top_process_index].hasCPU = true;
                    processes[top_process_index].waiting = false;
                    processes[top_process_index].running = true;
                    processes[top_process_index].waiting_time = 0;
                    processes[top_process_index].current_priority = processes[top_process_index].initial_priority;
                    simulation[top_process_index][current_time] = '*';

                    current_process = top_process;

                    ready_queue.pop();
                }
                else
                {
                    processes[getProcessIndex(current_process.processName)].hasCPU = true;
                    processes[getProcessIndex(current_process.processName)].was_executing = true;
                    simulation[getProcessIndex(current_process.processName)][current_time] = '*';
                    processes[getProcessIndex(current_process.processName)].running = true;
                    processes[getProcessIndex(current_process.processName)].waiting = false;
                    processes[getProcessIndex(current_process.processName)].current_priority = processes[getProcessIndex(current_process.processName)].initial_priority;

                    current_process = processes[getProcessIndex(current_process.processName)];
                }
            }
            else if (current_process.processName != "")
            {

                processes[getProcessIndex(current_process.processName)].hasCPU = true;
                processes[getProcessIndex(current_process.processName)].was_executing = true;
                simulation[getProcessIndex(current_process.processName)][current_time] = '*';
                processes[getProcessIndex(current_process.processName)].running = true;
                processes[getProcessIndex(current_process.processName)].waiting = false;
                processes[getProcessIndex(current_process.processName)].current_priority = processes[getProcessIndex(current_process.processName)].initial_priority;

                current_process = processes[getProcessIndex(current_process.processName)];
            }
        }
        else if (cpuIdle)
        {
            int size = ready_queue.size();
            if (!ready_queue.empty())
            {
                Process top_process = ready_queue.top();
                int top_process_index = getProcessIndex(top_process.processName);

                processes[top_process_index].hasCPU = true;
                processes[top_process_index].was_executing = true;
                processes[top_process_index].waiting = false;
                processes[top_process_index].running = true;
                processes[top_process_index].waiting_time = 0;
                processes[top_process_index].current_priority = processes[top_process_index].initial_priority;
                simulation[top_process_index][current_time] = '*';
                ready_queue.pop();
                current_process = processes[top_process_index];
                cpuIdle = false;
            }
        }
    }
    printResults(trace, processes, simulation, total_time, numProcesses, "Aging");
}

void RoundRobin(const vector<string> &lines, int quantum, bool trace)
{
    int total_time = stoi(lines[2]);
    int numProcesses = stoi(lines[3]);

    vector<Process> queue;
    vector<Process> processes;
    vector<vector<char>> simulation(numProcesses, vector<char>(total_time, ' '));

    bool cpuIdle = true;

    for (int i = 4; i < numProcesses + 4; i++)
    {
        stringstream ss(lines[i]);
        string processName, arrivalTime, serviceTime;

        getline(ss, processName, ',');
        getline(ss, arrivalTime, ',');
        getline(ss, serviceTime, ',');

        Process p;
        p.processName = processName;
        p.arrivalTime = stoi(arrivalTime);
        p.serviceTime = stoi(serviceTime);
        p.remainingTime = p.serviceTime;
        processes.push_back(p);
    }

    int number_of_processes_in_the_system = 0;

    for (int current_time = 0; current_time <= total_time; current_time++)
    {
        // Check for new arrivals
        for (int i = 0; i < numProcesses; i++)
        {
            if (processes[i].arrivalTime == current_time)
            {
                number_of_processes_in_the_system++;
                processes[i].waiting = true;
                processes[i].time_completed_in_a_row = 0; // Initialize time tracker for the quantum
                queue.push_back(processes[i]);
            }
        }

        if (!cpuIdle)
        {
            for (int i = 0; i < numProcesses; i++)
            {
                if (processes[i].hasCPU)
                {
                    if (processes[i].time_completed == processes[i].serviceTime)
                    {
                        processes[i].finishTime = current_time;
                        processes[i].turnaroundTime = processes[i].finishTime - processes[i].arrivalTime;
                        processes[i].hasCPU = false;
                        processes[i].running = false;
                        processes[i].finished = true;
                        processes[i].waiting = false;
                        cpuIdle = true;
                        number_of_processes_in_the_system--;
                    }
                    else if ((processes[i].time_completed_in_a_row >= quantum) && (number_of_processes_in_the_system > 1))
                    {   // Preempt the CPU and put the process back in the queue
                        processes[i].hasCPU = false;
                        processes[i].started_executing = -1;
                        processes[i].remainingTime = processes[i].serviceTime - processes[i].time_completed;
                        processes[i].running = false;
                        processes[i].waiting = true;
                        processes[i].time_completed_in_a_row = 0;
                        cpuIdle = true;
                        queue.push_back(processes[i]);
                    }
                    else
                    {   // Continue executing the process
                        processes[i].remainingTime--;
                        processes[i].time_completed++;
                        processes[i].time_completed_in_a_row++;
                        processes[i].running = true;
                        processes[i].waiting = false;
                        processes[i].hasCPU = true;
                    }

                    break;
                }
            }
        }

        if (cpuIdle && !queue.empty())
        {
            int index = getProcessIndex(queue[0].processName);
            queue.erase(queue.begin());
            processes[index].hasCPU = true;
            processes[index].running = true;
            processes[index].waiting = false;
            processes[index].remainingTime--;
            processes[index].started_executing = current_time;
            processes[index].time_completed++;
            processes[index].time_completed_in_a_row++;
            cpuIdle = false;
        }

        for (int i = 0; i < numProcesses; i++)
        {
            if (processes[i].running)
            {
                simulation[i][current_time] = '*';
            }
            else if (processes[i].waiting)
            {
                simulation[i][current_time] = '.';
            }
            else if (processes[i].finished)
            {
                simulation[i][current_time] = ' ';
            }
        }
    }

    printResults(trace, processes, simulation, total_time, numProcesses, "RR-" + to_string(quantum));
}


void Feedback_1i(const vector<string> &lines, bool trace)
{
    int total_time = stoi(lines[2]);

    int numProcesses = stoi(lines[3]);

    vector<Process> queue1;
    vector<Process> queue2;
    vector<Process> queue3;
    vector<Process> processes;

    Process currentProcess;

    vector<vector<char>> simulation(numProcesses, vector<char>(total_time, ' '));

    bool cpuIdle = true;

    for (int i = 4; i < numProcesses + 4; i++)
    {
        stringstream ss(lines[i]);
        string processName, arrivalTime, serviceTime;

        getline(ss, processName, ',');
        getline(ss, arrivalTime, ',');
        getline(ss, serviceTime, ',');

        Process p;
        p.processName = processName;
        p.arrivalTime = stoi(arrivalTime);
        p.serviceTime = stoi(serviceTime);
        p.remainingTime = p.serviceTime;

        processes.push_back(p);
    }

    int number_of_processes_in_the_system = 0;

    for (int current_time = 0; current_time <= total_time; current_time++)
    {

        // cout << endl << "current time: " << current_time << endl<< endl;
        // Check for new arrivals
        for (int i = 0; i < numProcesses; i++)
        {
            if ((processes[i].arrivalTime == current_time))
            {
                number_of_processes_in_the_system++;
                // cout << "Process: " << processes[i].processName << " added to queue 1 " << "and number of processes in the system: " << number_of_processes_in_the_system << endl;
                processes[i].quantum = 1;
                processes[i].prev_queue = 1;
                processes[i].waiting = true;
                queue1.push_back(processes[i]);
            }
        }
        int flag = 1;
        if (!cpuIdle)
        {
            // cout << "Current time: !idle " << current_time << endl;
            for (int i = 0; i < numProcesses; i++)
            {
                if (processes[i].hasCPU)
                {
                    // cout << "Process: " << processes[i].processName << " has the CPU, " ;
                    // cout << "time completed: " << processes[i].time_completed;
                    // cout << " time completed in a row: " << processes[i].time_completed_in_a_row;
                    // cout << ", Service time: " << processes[i].serviceTime << endl << endl;

                    if (processes[i].time_completed == processes[i].serviceTime)
                    {
                        processes[i].finishTime = current_time;
                        processes[i].turnaroundTime = processes[i].finishTime - processes[i].arrivalTime;
                        processes[i].hasCPU = false;
                        processes[i].running = false;
                        // processes[i].waiting = false;
                        processes[i].finished = true;
                        cpuIdle = true;
                        number_of_processes_in_the_system--;
                        processes[i].time_completed_in_a_row = 0; // redundant
                        // cout << "at time " << current_time << " process " << processes[i].processName << " finishes " << "and number of processes in the system: " << number_of_processes_in_the_system << endl;
                        continue;
                    }
                    else if ((processes[i].time_completed_in_a_row >= processes[i].quantum) && (number_of_processes_in_the_system > 1))
                    { // preempte the cpu and put the process in the next queue
                        // cout << "Process: " << processes[i].processName << " with quantum: " << processes[i].quantum << " will be preempted " << endl;
                        processes[i].hasCPU = false;
                        processes[i].started_executing = -1;
                        processes[i].remainingTime = processes[i].serviceTime - processes[i].time_completed;
                        processes[i].running = false;
                        processes[i].waiting = true;
                        processes[i].time_completed_in_a_row = 0;
                        cpuIdle = true;
                        if (processes[i].prev_queue == 1)
                        {
                            processes[i].prev_queue = 2;
                            queue2.push_back(processes[i]);
                        }
                        else
                        {
                            processes[i].prev_queue = 3;
                            queue3.push_back(processes[i]);
                        }
                    }
                    else
                    { // continue executing the process
                        processes[i].remainingTime--;
                        processes[i].time_completed++;
                        processes[i].time_completed_in_a_row++;
                        processes[i].running = true;
                        processes[i].waiting = false;
                    }
                }
            }
        }
        if (cpuIdle)
        {
            // cout << "Current time: idle " << current_time << endl;
            if (!queue1.empty())
            {
                int index = getProcessIndex(queue1[0].processName);
                queue1.erase(queue1.begin());
                processes[index].hasCPU = true;
                processes[index].running = true;
                processes[index].waiting = false;
                processes[index].started_executing = current_time;
                processes[index].remainingTime--;
                processes[index].time_completed++;
                processes[index].time_completed_in_a_row++;
                cpuIdle = false;
            }
            else if (!queue2.empty())
            {
                int index = getProcessIndex(queue2[0].processName);
                queue2.erase(queue2.begin());
                processes[index].hasCPU = true;
                processes[index].running = true;
                processes[index].waiting = false;
                processes[index].started_executing = current_time;
                processes[index].remainingTime--;
                processes[index].time_completed++;
                processes[index].time_completed_in_a_row++;
                cpuIdle = false;
            }
            else if (!queue3.empty())
            {
                int index = getProcessIndex(queue3[0].processName);
                queue3.erase(queue3.begin());
                processes[index].hasCPU = true;
                processes[index].running = true;
                processes[index].waiting = false;
                processes[index].started_executing = current_time;
                processes[index].remainingTime--;
                processes[index].time_completed++;
                processes[index].time_completed_in_a_row++;
                cpuIdle = false;
            }
        }
        // to preemp the cpu if needed, or complete the process

        for (int i = 0; i < numProcesses; i++)
        {
            if (processes[i].running)
            {
                simulation[i][current_time] = '*';
            }
            else if (processes[i].waiting)
            {
                simulation[i][current_time] = '.';
            }
            else
            {
                simulation[i][current_time] = ' ';
            }
        }
    }

    printResults(trace, processes, simulation, total_time, numProcesses, "FB-1");
}

void Feedback_2i(const vector<string> &lines, bool trace)
{
    int total_time = stoi(lines[2]);

    int numProcesses = stoi(lines[3]);

    vector<Process> queue1;
    vector<Process> queue2;
    vector<Process> queue3;
    vector<Process> processes;

    Process currentProcess;

    vector<vector<char>> simulation(numProcesses, vector<char>(total_time, ' '));

    bool cpuIdle = true;

    for (int i = 4; i < numProcesses + 4; i++)
    {
        stringstream ss(lines[i]);
        string processName, arrivalTime, serviceTime;

        getline(ss, processName, ',');
        getline(ss, arrivalTime, ',');
        getline(ss, serviceTime, ',');

        Process p;
        p.processName = processName;
        p.arrivalTime = stoi(arrivalTime);
        p.serviceTime = stoi(serviceTime);
        p.remainingTime = p.serviceTime;

        processes.push_back(p);
    }

    int number_of_processes_in_the_system = 0;

    for (int current_time = 0; current_time <= total_time; current_time++)
    {

        // cout << endl << "current time: " << current_time << endl<< endl;
        // Check for new arrivals
        for (int i = 0; i < numProcesses; i++)
        {
            if ((processes[i].arrivalTime == current_time))
            {
                number_of_processes_in_the_system++;
                // cout << "Process: " << processes[i].processName << " added to queue 1 " << "and number of processes in the system: " << number_of_processes_in_the_system << endl;
                processes[i].quantum = 1;
                processes[i].waiting = true;
                queue1.push_back(processes[i]);
            }
        }
        int flag = 1;
        if (!cpuIdle)
        {
            // cout << "Current time: !idle " << current_time << endl;
            for (int i = 0; i < numProcesses; i++)
            {
                if (processes[i].hasCPU)
                {
                    // cout << "Process: " << processes[i].processName << " has the CPU, " ;
                    // cout << "time completed: " << processes[i].time_completed;
                    // cout << " time completed in a row: " << processes[i].time_completed_in_a_row;
                    // cout << ", Service time: " << processes[i].serviceTime << endl << endl;

                    if (processes[i].time_completed == processes[i].serviceTime)
                    {
                        processes[i].finishTime = current_time;
                        processes[i].turnaroundTime = processes[i].finishTime - processes[i].arrivalTime;
                        processes[i].hasCPU = false;
                        processes[i].running = false;
                        // processes[i].waiting = false;
                        processes[i].finished = true;
                        cpuIdle = true;
                        number_of_processes_in_the_system--;
                        processes[i].time_completed_in_a_row = 0; // redundant
                        // cout << "at time " << current_time << " process " << processes[i].processName << " finishes " << "and number of processes in the system: " << number_of_processes_in_the_system << endl;
                        continue;
                    }
                    else if ((processes[i].time_completed_in_a_row >= processes[i].quantum) && (number_of_processes_in_the_system > 1))
                    { // preempte the cpu and put the process in the next queue
                        // cout << "Process: " << processes[i].processName << " with quantum: " << processes[i].quantum << " will be preempted " << endl;
                        processes[i].hasCPU = false;
                        processes[i].started_executing = -1;
                        processes[i].remainingTime = processes[i].serviceTime - processes[i].time_completed;
                        processes[i].running = false;
                        processes[i].waiting = true;
                        processes[i].time_completed_in_a_row = 0;
                        cpuIdle = true;
                        if (processes[i].quantum == 1)
                        {
                            processes[i].quantum = 2;
                            queue2.push_back(processes[i]);
                        }
                        else
                        {
                            processes[i].quantum = 4;
                            queue3.push_back(processes[i]);
                        }
                    }
                    else
                    { // continue executing the process
                        processes[i].remainingTime--;
                        processes[i].time_completed++;
                        processes[i].time_completed_in_a_row++;
                        processes[i].running = true;
                        processes[i].waiting = false;
                    }
                }
            }
        }
        if (cpuIdle)
        {
            // cout << "Current time: idle " << current_time << endl;
            if (!queue1.empty())
            {
                int index = getProcessIndex(queue1[0].processName);
                queue1.erase(queue1.begin());
                processes[index].hasCPU = true;
                processes[index].running = true;
                processes[index].waiting = false;
                processes[index].started_executing = current_time;
                processes[index].remainingTime--;
                processes[index].time_completed++;
                processes[index].time_completed_in_a_row++;
                cpuIdle = false;
            }
            else if (!queue2.empty())
            {
                int index = getProcessIndex(queue2[0].processName);
                queue2.erase(queue2.begin());
                processes[index].hasCPU = true;
                processes[index].running = true;
                processes[index].waiting = false;
                processes[index].started_executing = current_time;
                processes[index].remainingTime--;
                processes[index].time_completed++;
                processes[index].time_completed_in_a_row++;
                cpuIdle = false;
            }
            else if (!queue3.empty())
            {
                int index = getProcessIndex(queue3[0].processName);
                queue3.erase(queue3.begin());
                processes[index].hasCPU = true;
                processes[index].running = true;
                processes[index].waiting = false;
                processes[index].started_executing = current_time;
                processes[index].remainingTime--;
                processes[index].time_completed++;
                processes[index].time_completed_in_a_row++;
                cpuIdle = false;
            }
        }
        // to preemp the cpu if needed, or complete the process

        for (int i = 0; i < numProcesses; i++)
        {
            if (processes[i].running)
            {
                simulation[i][current_time] = '*';
            }
            else if (processes[i].waiting)
            {
                simulation[i][current_time] = '.';
            }
            else
            {
                simulation[i][current_time] = ' ';
            }
        }
    }

    printResults(trace, processes, simulation, total_time, numProcesses, "FB-2i");
}

void HRRN(const vector<string> &lines, bool trace)
{
    int total_time = stoi(lines[2]);

    int numProcesses = stoi(lines[3]);

    vector<Process> processes;
    vector<vector<char>> simulation(numProcesses, vector<char>(total_time, ' '));
    vector<Process> ready_queue;

    bool cpuIdle = true;
    Process currentProcess;

    for (int i = 4; i < numProcesses + 4; i++)
    {
        stringstream ss(lines[i]);
        string processName, arrivalTime, serviceTime;

        getline(ss, processName, ',');
        getline(ss, arrivalTime, ',');
        getline(ss, serviceTime, ',');

        Process p;
        p.processName = processName;
        p.arrivalTime = stoi(arrivalTime);
        p.serviceTime = stoi(serviceTime);
        p.remainingTime = p.serviceTime;

        processes.push_back(p);
    }

    for (int current_time = 0; current_time < total_time; current_time++)
    {
        // Add processes to the ready queue when they arrive
        for (auto &p : processes)
        {
            if (p.arrivalTime == current_time && !p.hasCPU)
            {
                ready_queue.push_back(p);

                sort(ready_queue.begin(), ready_queue.end(), [](const Process &left, const Process &right)
                     { return left.responseRatio > right.responseRatio; });
            }
        }

        if (cpuIdle && !ready_queue.empty())
        {
            currentProcess = ready_queue.front();
            ready_queue.erase(ready_queue.begin());
            cpuIdle = false;
            currentProcess.hasCPU = true;
        }

        if (currentProcess.hasCPU)
        {
            for (int i = 0; i < numProcesses; i++)
            {
                if (processes[i].processName == currentProcess.processName)
                {
                    simulation[i][current_time] = '*';
                    break;
                }
            }

            currentProcess.remainingTime--;
            if (currentProcess.remainingTime == 0)
            {
                cpuIdle = true;
                currentProcess.hasCPU = false;

                for (int i = 0; i < numProcesses; i++)
                {
                    if (processes[i].processName == currentProcess.processName)
                    {
                        processes[i].finishTime = current_time + 1;
                        processes[i].turnaroundTime = processes[i].finishTime - processes[i].arrivalTime;
                        break;
                    }
                }
            }
        }

        vector<Process> temp_queue;

        for (auto &p : ready_queue)
        {
            for (int i = 0; i < numProcesses; i++)
            {
                if (processes[i].processName == p.processName)
                {
                    simulation[i][current_time] = '.';
                    break;
                }
            }
        }

        for (auto &p : ready_queue)
        {
            p.responseRatio = (float)(current_time - p.arrivalTime + p.serviceTime) / p.serviceTime;
        }

        sort(ready_queue.begin(), ready_queue.end(), [](const Process &left, const Process &right)
             { return left.responseRatio > right.responseRatio; });
    }

    // Print simulation results

    printResults(trace, processes, simulation, total_time, numProcesses, "HRRN");
}

void SPN(const vector<string> &lines, bool trace)
{
    int total_time = stoi(lines[2]);
    // if (trace)
    //     printFirstLine("SPN", total_time);

    int numProcesses = stoi(lines[3]);

    priority_queue<Process, vector<Process>, SPN_compare> ready_queue;
    Process currentProcess;
    bool cpuIdle = true;

    vector<Process> processes;
    vector<vector<char>> simulation(numProcesses, vector<char>(total_time, ' ')); // 2D array for simulation

    for (int i = 4; i < numProcesses + 4; i++)
    {
        stringstream ss(lines[i]);
        string processName, arrivalTime, serviceTime;

        getline(ss, processName, ',');
        getline(ss, arrivalTime, ',');
        getline(ss, serviceTime, ',');

        Process p;
        p.processName = processName;
        p.arrivalTime = stoi(arrivalTime);
        p.serviceTime = stoi(serviceTime);
        p.remainingTime = p.serviceTime;

        processes.push_back(p);
    }

    for (int current_time = 0; current_time < total_time; current_time++)
    {
        // Add processes to the ready queue when they arrive
        for (auto &p : processes)
        {
            if (p.arrivalTime == current_time && !p.hasCPU)
            {
                ready_queue.push(p);
            }
        }

        // If CPU is idle and there's a process in the queue, schedule the next process
        if (cpuIdle && !ready_queue.empty())
        {
            currentProcess = ready_queue.top();
            ready_queue.pop();
            cpuIdle = false;
            currentProcess.hasCPU = true;
        }

        // Execute the current process if it has the CPU
        if (currentProcess.hasCPU)
        {
            // Mark running state in the 2D array
            for (int i = 0; i < numProcesses; i++)
            {
                if (processes[i].processName == currentProcess.processName)
                {
                    simulation[i][current_time] = '*';
                    break;
                }
            }

            currentProcess.remainingTime--;
            if (currentProcess.remainingTime == 0)
            {
                cpuIdle = true;
                currentProcess.hasCPU = false; // Process finished

                for (int i = 0; i < numProcesses; i++)
                {
                    if (processes[i].processName == currentProcess.processName)
                    {
                        processes[i].finishTime = current_time + 1;
                        processes[i].turnaroundTime = processes[i].finishTime - processes[i].arrivalTime;
                        break;
                    }
                }
            }
        }

        // Mark waiting states for processes in the ready queue
        vector<Process> temp_queue;

        while (!ready_queue.empty())
        {
            Process p = ready_queue.top();
            ready_queue.pop();
            for (int i = 0; i < numProcesses; i++)
            {
                if (processes[i].processName == p.processName)
                {
                    simulation[i][current_time] = '.';
                    break;
                }
            }
            temp_queue.push_back(p);
        }
        // Restore processes back to the ready queue
        for (auto &p : temp_queue)
        {
            ready_queue.push(p);
        }
    }

    printResults(trace, processes, simulation, total_time, numProcesses, "SPN");
}

void printFirstLine(const string &line, int total_time)
{
    if (line.length() == 3)
    {
        cout << line << "   ";
    }
    else if (line.length() == 4)
    {
        cout << line << "  ";
    }
    else if (line == "FB-2i")
    {
        cout << line << " ";
    }
    else
    {
        cout << line << " ";
    }
    for (int i = 0; i <= total_time; i++)
    {
        if (i < 10)
        {
            cout << i << " ";
        }
        else
        {
            cout << i % 10 << " ";
        }
    }
    cout << endl;
    cout << "------------------------------------------------" << endl;
}

void printResults(bool trace, vector<Process> &processes, const vector<vector<char>> &simulation, int total_time, int numProcesses, string type)
{
    if (trace)
    {
        printFirstLine(type, total_time);
        for (int i = 0; i < numProcesses; i++)
        {
            cout << processes[i].processName << "     ";
            for (int j = 0; j < total_time; j++)
            {
                cout << "|" << simulation[i][j];
            }
            cout << "| " << endl;
        }
        cout << "------------------------------------------------" << endl
             << endl;
    }
    else // stats
    {
        cout << type << endl;
        cout << "Process    ";
        for (int i = 0; i < numProcesses; i++)
        {
            cout << "|  " << processes[i].processName << "  ";
        }
        cout << "|" << endl;

        cout << "Arrival    ";
        for (int i = 0; i < numProcesses; i++)
        {
            cout << "|  " << processes[i].arrivalTime << "  ";
        }
        cout << "|" << endl;

        cout << "Service    ";
        for (int i = 0; i < numProcesses; i++)
        {
            cout << "|  " << processes[i].serviceTime << "  ";
        }
        cout << "| " << "Mean|" << endl;

        cout << "Finish     ";
        for (int i = 0; i < numProcesses; i++)
        {
            if (processes[i].finishTime < 10)
            {
                cout << "|  " << processes[i].finishTime << "  ";
            }
            else
            {
                cout << "| " << processes[i].finishTime << "  ";
            }
        }
        cout << "|-----|" << endl;

        cout << "Turnaround ";
        float meanTurnaround = 0;
        float meanNormTurn = 0;
        for (int i = 0; i < numProcesses; i++)
        {
            if (processes[i].turnaroundTime < 10)
            {
                cout << "|  " << processes[i].turnaroundTime << "  ";
            }
            else
            {
                cout << "| " << processes[i].turnaroundTime << "  ";
            }
            meanTurnaround += processes[i].turnaroundTime;
            processes[i].normTurn = (float)processes[i].turnaroundTime / processes[i].serviceTime;
            meanNormTurn += processes[i].normTurn;
        }
        meanTurnaround /= numProcesses;
        meanNormTurn /= numProcesses;
        if (meanTurnaround >= 10)
        {
            cout << "|" << fixed << setprecision(2) << meanTurnaround << "|" << endl; // added
        }
        else
        {
            cout << "| " << fixed << setprecision(2) << meanTurnaround << "|" << endl;
        }

        cout << "NormTurn   ";

        for (int i = 0; i < numProcesses; i++)
        {
            cout << "| " << fixed << setprecision(2) << processes[i].normTurn;
        }
        cout << "| " << fixed << setprecision(2) << meanNormTurn << "|" << endl;

        cout << endl;
    }
}

int getProcessIndex(string name)
{
    if (name == "A")
        return 0;
    else if (name == "B")
        return 1;
    else if (name == "C")
        return 2;
    else if (name == "D")
        return 3;
    else if (name == "E")
        return 4;
    else
        return -1;
}