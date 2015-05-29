#include <string.h>
#include <stdio_ext.h>
#include "Client.h"

void checkBalance(int pipe, int outputPipe, int name);

int main()
{
    /**/
    int userName  = 123;
    char buff[32];
    int registerPipe, inputPipe, outputPipe;/*Input and Output Refer to MissionControl*/
    int answer = -1;
    int sF = 0;

    /*Get User Credentials*/

    printf("Welcome, Please Enter Your Username (31 Char Limit): ");
    scanf("%d", &userName);
    printf("\n");

    /*END*/

    /*Warn the Server of Client*/
    sprintf(buff, "%d", userName);
    registerPipe = open("/tmp/missionControlInput.pipe", O_WRONLY);
    write(registerPipe, buff, 4);

    /*Get the pipes to the server*/
    sprintf(buff, "/tmp/%04xInput.pipe", userName);
    do{
        inputPipe = open( buff , O_WRONLY);
    }
    while(inputPipe <= 0);/*Hold Until the Server Creates the Pipe*/
    sprintf(buff, "/tmp/%04xOutput.pipe", userName);
    do{
        outputPipe = open( buff , O_RDONLY);
    }
    while(outputPipe <= 0);/*Hold Until the Server Creates the Pipe*/


    //dup2(STDOUT_FILENO, outputPipe);


    printf("PIPED");
    //write(inputPipe, "\0", 1);
    fflush(stdin);
    while(answer)
    {
        printf("What do you wish to do?\n1 - Input Commands\n2 - Check Balance\n3 - Update Balance\n0 - Exit\n");
        fflush(stdin);
        __fpurge(stdin);
        fflush(stdout);
        sF = scanf("%d", &answer);
        if(sF)
            switch (answer)
            {
                case 1:
                    commandDialog(inputPipe, outputPipe, userName);
                    break;
                case 2:
                    checkBalance(inputPipe, outputPipe, userName);
                    break;
                case 3:
                    updateBalance(inputPipe, outputPipe, userName);
                    break;
                default:
                    answer = 0;
            }
    }

    Intent quit;
    quit.msgId = MSG_MC_CLOSE;
    write(inputPipe, &quit, sizeof quit);
    return 0;
}

void checkBalance(int inputPipe, int outputPipe, int name)
{
    Intent i;
    i.msgId = MSG_BAL_CHECK;
    i.dataSize = 0;
    float bal;

    write(inputPipe, &i, sizeof i); /*Write the Intent*/

    read(outputPipe, &bal, sizeof(float));
    printf("Your Current Balance is %f\n", bal);
}

void updateBalance(int inputPipe, int outputPipe, int name)
{
    Intent i;
    i.msgId = MSG_BAL_UPDATE;
    i.dataSize = 0;
    float bal = 5.0f;

    write(inputPipe, &i, sizeof i); /*Write the Intent*/

    write(inputPipe, &bal, sizeof bal);

    read(outputPipe, &bal, sizeof(float));

    printf("Your Current Balance is %f\n", bal);
}

void commandDialog(int inputPipe, int outputPipe, int userName)
{

    char stB[64];
    printf("You are now connected to the server, Ctlr+D disconnects\n");
    Intent i;
    i.msgId = MSG_EXEC_CMD;

    //memcpy(stB, &i, sizeof i);
    //write(inputPipe, stB, sizeof i)
    while(1)
    {
        char buff[1024];
        char null[1];
        null[0] = '\0';
        int asd;
        //scanf("%s", buff);
        do{
            //fflush(stdout);
            fgets(buff, 64, stdin);
        }while((asd = strcmp(buff, " \n")) <= 0);


        strtok(buff, "\n");


        if(!strcmp(buff, "MC_QUIT"))
        {
            i.msgId = MSG_MC_CLOSE;
            i.dataSize = -1;
            memcpy(stB, &i, sizeof i);
            //write(inputPipe, stB, sizeof i);
            return;
        } else{

            i.dataSize = (int) (strlen(buff));
            memcpy(stB, &i, sizeof i);

            write(inputPipe, stB, sizeof i);

            write(inputPipe, buff, i.dataSize);
        }

    }
}