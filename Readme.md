# User level

This level mostly deal with the user. The functions which are coded in this level are:

### Gui_user_input: This function get the user_id, user budget and the user groceries using gui. After done with giving INFO, by pressing the submit button a new terminal opens (one for each user) in which you can see the logs and monitor the processes and threads.

### user_input: This function works the same as gui_user_input but instead of using gui, it gets data from terminal

### Three_thread_process: After finding items from 3 stores, one thread is created for calculating the max score from each one by multiplying the score and prize of each item in reciept. After calculation the result shown to the user. Now one thread is created for geting the scores from user for each item in the receipt with max score. By finishing this thread another thread is created to send the user scores from pipe to be written to each .txt file. Thus this funtion creates 3 thread. Note that if the prize of the selected receipt is beyond the user budget the user get the massage that can not afford those items.

### User_level_process: This is the main function of the user_level file. This function creates 3 child process for each 3 stores and send the path of each stores through pipes to the lower processes.

# Store level

# Category level

This filel has a main function called category_level_thread which creates threads for each .txt file in all the categories it opens (The path of the categories are given from upper process). In this function there are 2 main semaphore named sem_process and sem_thread which have been initialized to 0. After thread creation for a given category the process which creates those thread should wait until all the threads done with their works in thread_level (The function runner is implemented in thread_level). After sending and receiving the desired massaged through pipes to/from upper and lower process recpectively, the threads now must update their own file files they responsible for but before that they should signaled because they have been waited in thread_level.

# Thread level

# Database

This database is designed to store the user_id and order_id which shows how much the user has purchased so far. Columns store1, store2 and store3  show whether the user has already bought from these stores or not. The reason for saving these data, is that if user has already bought from these stores and intends to buy from them again (remember the max score from user_level), the user will be given 10% discount.

# Encoding

# Shared memory!

# How to use it ?!
