- Tasks:

  - [ ]  User level

    - [X]  Getting User input
    - [X]  Creating process for each shop
    - [X]  Waiting for lower level to give you options
    - [ ]  Check if already have shoped
    - [ ]  Create a thread to get scores from user
    - [X]  Send the result to lower level
    - [X]  Wait for all the process
  - [ ]  Store level

    - [X]  Create a process for each category
    - [ ]  Wait for the lower level result
    - [ ]  Merge the result to a final result
    - [ ]  Send result to upper level
    - [ ]  Wait for the upper level result
    - [ ]  Send result to lower level
    - [ ]  Wait for all the process
  - [ ]  Category level

    - [ ]  Create mutex locks that is shared between processes (mmap)
    - [ ]  Create a mutex lock that is shared in self process
    - [ ]  Create thread for each file
    - [ ]  Wait for the results! ( Semaphore init to 0)
    - [ ]  Send the result
    - [ ]  Wait for the end result!
    - [ ]  Wait for all threads (Termination!)
  - [ ]  Thread level

    - [X]  Read file and extract the item (RW problem)
    - [X]  Check if the item is in wanted list
    - [ ]  Send data to upper level ( Need mutex lock for putting the result )
    - [ ]  signal the upper level
    - [ ]  Wait for the upper level
    - [ ]  See the message, change entity and modify the file (RW problem)
    - [ ]  Termination
  - [X]  Structs:

    - [X]  Adding encode and decode for bottom up
