#include <iostream>
#include <string.h>
#include <fstream>
#include <list>
#include <queue>
#include <stdio.h>
#include <cstdio>

using namespace std;

//structure Task representing a single line of task in task.txt
typedef struct Task
{
    int priority;
    char task[250];
    int line_no;
}Task;

// compare function to compare priority of 2 Tasks
bool compare_task(Task t1, Task t2)
{
    return t1.priority < t2.priority;
}

void showHelp();
list<Task> getIncompleteTask();
void addTask(int priority, char *task);
void listTasks();
void taskComplete(char* index);
void delTask(char* index);
void getReport();


// $ ./task help 
void showHelp()
{
    char help[] = "Usage :-\n$ ./task add 2 hello world    # Add a new item with priority 2 and text \"hello world\" to the list\n$ ./task ls                   # Show incomplete priority list items sorted by priority in ascending order\n$ ./task del INDEX            # Delete the incomplete item with the given index\n$ ./task done INDEX           # Mark the incomplete item with the given index as complete\n$ ./task help                 # Show usage\n$ ./task report               # Statistics";
    cout<<help<<endl;
}


//returns list of incomplete task in sorted order of priority
list<Task> getIncompleteTask()
{
    //open task.txt and complete.txt in read mode
    ifstream task_fin,cmplt_fin;
    task_fin.open("task.txt",ios::in);
    cmplt_fin.open("complete.txt",ios::in);

    int priority;
    char wspace;
    char task[250], cmplt_task[250];
    list <Task> task_list;
    int line_no = 1;

    //stores in order: [priority][white space][task description]
    while(task_fin>>priority && task_fin.get(wspace) && task_fin.getline(task,250))
    {
        //check if task present is completed (task present in complete.txt)
        bool task_incomplete = true;
        if(cmplt_fin.good())                            //to check if complete.txt is opened
        {
            while (cmplt_fin.getline(cmplt_task,250))   //retrieve task from complete.txt
            {
                if(strcmp(cmplt_task,task)==0)          //task from task.txt is compared with each task of complete.txt
                {
                    task_incomplete = false;            //if task matches then we won't consider it as incomplete
                    break;
                }
            }   
        }

        //move read pointer to start of file(complete.txt)
        cmplt_fin.clear();
        cmplt_fin.seekg(0,ios::beg);

        //if task is incomplete: add to list
        if(task_incomplete == true)
        {
            Task task_obj;
            task_obj.priority = priority;
            strcpy(task_obj.task,task);
            task_obj.line_no = line_no;
            task_list.push_back(task_obj);
        }

        line_no++;
    }

    //sort on priority - Time complexity: N*O(logN), Stable sort
    task_list.sort(compare_task);

    task_fin.close();
    cmplt_fin.close();

    return task_list;
}


// $ ./task add [priority] [description]
void addTask(int priority, char *task)
{
    /*
    add command logic:
        check if task in task.txt
            - present:
                - do not add duplicate,
                - remove all occurances of task from complete.txt (make task pending)
            - absent: append to task.txt
    
    */

    ifstream task_fin("task.txt");
    bool task_already_added = false;
    char task_desc[250], wspace;
    int task_priority;

    if(task_fin.good() == true)         //if file exists
    {
        while (task_fin>>task_priority && task_fin.get(wspace) && task_fin.getline(task_desc,250))
        {
            if(strcmp(task,task_desc) == 0)
            {
                task_already_added = true;
                break;
            }
        }
    }
    task_fin.close();

    if(task_already_added == true)
    {
        //do not add new task if task with same name already present

        ifstream cmplt_fin("complete.txt");

        if(cmplt_fin.good() == true)        // if file exists
        {
            ofstream temp_fout("temp_complete.txt");

            for(int count=1; cmplt_fin.getline(task_desc,250); count++)
            {
                if(strcmp(task,task_desc) == 0)
                {
                    //do not write
                }
                else
                {
                    temp_fout<<task_desc<<endl;
                }
            }

            temp_fout.close();
            cmplt_fin.close();    
            
            remove("complete.txt");
            rename("temp_complete.txt","complete.txt");
        }
        else
            cmplt_fin.close();

        cout<<"Added task: \""<<task<<"\" with priority "<<priority<<endl;
    }
    else
    {
        //new task is appended at the end of task.txt file

        fstream task_fout;
        task_fout.open("task.txt",ios::app | ios::out);
        
        task_fout<<priority<<" "<<task<<"\n";       //writing to task.txt

        if(task_fout.fail() == false)
            cout<<"Added task: \""<<task<<"\" with priority "<<priority<<endl;
        else
            cout<<"Error: Missing tasks string. Nothing added!\n";

        task_fout.close();
    }
}


// $ ./task ls
void listTasks()
{
    list <Task> task_list = getIncompleteTask();        // get list of pending tasks in sorted order

    if(task_list.empty())
    {
        cout<<"There are no pending tasks!\n";
        return;
    }

    // output pending tasks
    int index = 1;
    for(auto itr = task_list.begin(); itr != task_list.end(); itr++, index++)
    {
        cout<<index<<". "<<itr->task<<" ["<<itr->priority<<"]\n";
    }

    task_list.clear();
}


// $./task done [index]
void taskComplete(char* str_index)
{
    list <Task> task_list = getIncompleteTask();
    int index;

    //covert string index to integer
    sscanf(str_index,"%d",&index);

    //get task description
    char task_desc[250];
    
    //select task with mentioned index
    auto itr = task_list.begin();
    int i;
    for (i=1; i < index && itr != task_list.end(); i++, itr++);

    if(itr != task_list.end() && i==index)
    {
        strcpy(task_desc,itr->task);

        //insert task into complete.txt
        ofstream cmplt_fout;
        cmplt_fout.open("complete.txt",ios::app | ios::out);

        if(cmplt_fout.good())
        {
            cmplt_fout<<task_desc<<"\n";
            cout<<"Marked item as done.\n";
        }
        cmplt_fout.close();
    }
    else
    {
        //index is invalid
        cout<<"Error: no incomplete item with index #"<<index<<" exists.\n";
    }
    
}


// $ ./task del [index]
void delTask(char* str_index)
{
    //covert string index to integer
    long unsigned int index;
    sscanf(str_index,"%ld",&index);

    //open task.txt and complete.txt in read mode
    ifstream task_fin,cmplt_fin;
    task_fin.open("task.txt",ios::in);

    int line_no;

    list <Task> task_list = getIncompleteTask();        // get list of pending tasks

    //check if index within bounds
    if(index < 1 || index > task_list.size())
    {
        cout<<"Error: task with index #"<<index<<" does not exist. Nothing deleted.\n";
        return;
    }

    //iterate through the list to get task of specified index
    auto itr = task_list.begin();
    long unsigned int i;
    for (i=1; i < index && itr != task_list.end(); i++, itr++);
    
    //get line number and task description
    char task_desc[250];
    line_no = itr->line_no;
    strcpy(task_desc,itr->task);

    //contents from task.txt are written in temp_task.txt
    ofstream temp_fout("temp_task.txt");

    //overwriting task.txt omitting lines meant to be deleted
    char line[270];
    for(int count=1; task_fin.getline(line,270); count++)
    {
        if(count != line_no)
            temp_fout<<line<<endl;
        else
        {
            cout<<"Deleted task #"<<index<<endl;
        }
    }

    task_fin.close();
    temp_fout.close();    
    
    //renaming temp file as task.txt
    remove("task.txt");
    rename("temp_task.txt","task.txt");
}


// $ ./task report
void getReport()
{
    //get pending tasks
    list <Task> pending_task = getIncompleteTask();

    //output size
    cout<<"Pending : "<<pending_task.size()<<endl;

    //output each task
    int index = 1;
    for (auto itr = pending_task.begin(); itr != pending_task.end(); itr++,index++)
    {
        cout<<index<<". "<<itr->task<<" ["<<itr->priority<<"]\n";
    }
    cout<<"\n";
    
    //get completed tasks
    ifstream cmplt_fin("complete.txt");

    if(cmplt_fin.good())
    {
        int index = 1;
        char *cmplt_task = new char[250];
        queue <char*> task_desc;

        //store completed tasks in queue
        while (cmplt_fin.getline(cmplt_task,250))
        {
            task_desc.push(cmplt_task);
            cmplt_task = new char[250];
            index++;
        }

        // output total no. of completed tasks
        cout<<"Completed : "<<index-1<<endl;

        // output tasks
        index = 1;
        while(!task_desc.empty())
        {
            char *task_val = task_desc.front();
            task_desc.pop();
            cout<<index<<". "<<task_val<<endl;
            delete task_val;
            index++;
        }
    }
    else
    {
        cout<<"Completed : 0\n";
    }

    cmplt_fin.close();
    pending_task.clear();

}


int main(int argc, char* argv[])
{
    /*
        Respective function calls for particular commands:
            ./task help --> showHelp()
            ./task      --> showHelp()
            ./task report   --> getReport()
            ./task ls       --> listTasks()
            ./task del [index]  --> delTask()
            ./task done [index] --> taskComplete()
            ./task add [priority] [task]    --> addTask()
    */

    if(argc == 1)               // no arguments
        showHelp();
    else
    {
        if(strcmp(argv[1],"help") == 0)         // $ ./task help
        {
            showHelp();
        }
        else if(strcmp(argv[1],"ls") == 0)      // $ ./task ls
        {
            //items present in task.txt but not in complete.txt
            listTasks();
        }
        else if(strcmp(argv[1],"report") == 0)  // $ ./task report
        {
            //list from complete.txt and (task.txt - complete.txt)
            getReport();
        }
        else if(strcmp(argv[1],"del") == 0)     // $ ./task del [index]
        {
            //delete from task.txt
            if(argc == 3)
                delTask(argv[2]);
            else if (argc == 2)
                cout<<"Error: Missing NUMBER for deleting tasks.\n";
        }
        else if(strcmp(argv[1],"done") == 0)    // $ ./task done [index]
        {
            //add incomplete task to complete.txt
            if(argc == 3)
                taskComplete(argv[2]);
            else if (argc == 2)
                cout<<"Error: Missing NUMBER for marking tasks as done.\n";
        }
        else if(strcmp(argv[1],"add") == 0)     // $ ./task add [priority] [task_desc]
        {
            //check if priority is a number
            char *endp;
            int priority;
            long converted = strtol(argv[2], &endp, 10);
            if(*endp)
            {
                //not a number
                cout<<"Error: Invalid type, Expecting int, but char* was passed\n Format: ./task add [int] [string]\n";
                return 0;
            }
            else
            {
                priority = (int) converted;
            }

            //add to task.txt
            if(argc == 4)
                addTask(priority,argv[3]);
            else
                cout<<"Error: Missing tasks string. Nothing added!\n";
        }
        else
        {
            cout<<"Invalid argument"<<endl;
        }
    }

    return 0;
}