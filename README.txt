# PA-2-Concurrent-Hash-Table

Instructions to execute:
1. Unzip file
2. Start WSL Terminal
3. cd PA-2-Concurrent-Hash-Table
4. make
5. ./chash
6. To observe changes in the output, keep running the ./chash command
7. To start fresh, run make clean

List of Resources used:
1.OSTEP rwlocks.c & rwlocks.h (modified and got rid of any unecessary functions and definitions)
2.Created and adjusted MAKEFILE and started initial implementation using Chatgpt 
links: https://chat.openai.com/share/d4c2b9ab-1a9e-483b-81c5-1f0e95cdaca3
      https://chat.openai.com/share/9236388a-da10-4e2a-9bdd-a97d823372fa
3. Explanation for what we used the AI prompt for: We used Chatgpt to understand the implementation of rwlocks and how to use them in our code.
   Further, we used it to understand the implementation of the hash table and how to use it in our code. The reason so was because since we weren't techncially using
   a true hash table but rather a pseudo one, we just used it to get our heads wrapped up on how we were going to go about implementing it as when using AI, it's just
   a tool to guide us for when we just have general issues as the complexity of this project can not be fully reliant with it. We used it also to help guide us for the
   Makefile as we ran into two issues when creating it as when trying to exceute make in the terminal, it would give us spacing errors and also creating segmentation faults.
   So, when running the program. We used it to help guide us on how to properly create the Makefile and how to properly compile the program.