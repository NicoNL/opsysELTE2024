We are now approaching the deadline for the submission of theses. In the neptun (parent), the option opens for the student (child) to upload his/her topic and ask a topic leader (child) to either take the lead or not.

1. It is a busy time of the semester, so both the student and the subject leader log into neptun to manage the administrative tasks of subject submission. Both the student and the subject leader send a signal to neptun that they have logged in, which neptun "acknowledges" and displays a message on the screen that both the student and the subject leader have successfully logged in. The student then "uploads" - sends to neptun via a pipe - the notification document for his/her thesis topic, which contains the following information. (Thesis title, student name, Year, Supervisor name). The neptun sends this to the subject supervisor, also via pipe, who prints this information/details on the screen. Neptun waits until both the student and the subject leader have completed their administration. (Parent waits until the child processes and maintains the connection throughout.)

2. The subject leader asks an interpretive question to the student via message queue, "What technology do you want to use to complete your assignment?". The student does not answer, but considers what would be the right choice for him/her. (Nothing is sended back.)
The question the student receives is printed on the screen.

3. The topic leader then decides whether or not to take the topic, with a 20% probability of rejecting the topic, writes his/her decision in a shared memory which the student reads, and writes the result on the screen.

4. Protect the use of shared memory with semaphore.

Create a C language program to do this, to help you work. The solution should run either on the opsys.inf.elte.hu server or on a similar system.

Upload the source file here in .c format (only one source file is expected to be uploaded).