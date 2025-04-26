TO BE MANUALLY INSPECTED

group #16
Carlos Chasi-Mejia chasi009
Chrisopher Johnson 
csel-kh1250-01.cselabs.umn.edu
Changes to Makefile:
    Changes to make the make's by the TA annoucment
-------------
NOTE:
OUTPUT TO BE MANUALLY INSPECTED
Gradescope is failing to autograde our submission. It says it is unable to make the files, even though they are made when we run our projects on CSE machines ourselves.
We were told to submit this for manual inspection.
----------------
Contribution outline: both of us are gonna work together on an equal amount of things such as creating pipes and logic. 

Implentation Plan:

Plan on how you are going to construct the pipes and inter-process communication. 
Start in root_process.c 
Parse the command line arguments and fork first non-leaf child using fork() and construct the first pipe using pipe() with write end for child and read end for root. 
Use exec() with the pipe's write end as one of the arguments to start the nonleaf_process program.
Once in non_leaf process, use fork to create the child processes which can either be more nonleaf processes or leaf processes. 
Construst a pipe for every child process and collect all children's pipes meassages by reading from pipe constructed for child processes and then write this information to pipe constructed from for parent process. 
Use exec again for child leaf processes and non-leaf process, both passing in their pipe's write end as an argument. 
The parent non-leaf process will also use wait() for all of its child process to aggragate all pipe messages from their shared child processes pipes. 
Once in leaf_process, it will calculate the hash of the leaf file content and write to the write end of the pipe shared with its parent. 
New changes:
At the end when it returns to root process: 
In redirection, it will redirect output via dup2 to a buffer/temp standard output while it goes through the directories. Then it will finally print the redirections and use dup2 to redirect to the normal standard output. Then free everything to finish.