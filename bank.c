#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define N 100 

int accounts[100];
char mode;
int from = 0, to = 0, amount = 0;


void mine_cmd(char* cmd){
    int ctr = 0;
    int i = 0;
    int part = 0;
    char mine[4][10];
    while (1){

        if(cmd[ctr] == '\0')
            break;
        
        else if(cmd[ctr] == ','){

            i = 0;
            part++;
        }
        
        else{
            mine[part][i] = cmd[ctr];
            i++;
        }

        ctr++;
    }

    mode = mine[0][0];

    for(int i=1; i < 4; i++){

        if(i == 1){
            if(mine[i][0] == '-') from = -1;
    
            else from = atoi(mine[i]);
        }
        else if(i == 2){
            if(mine[i][0] == '-') to = -1;
    
            else 
                to = atoi(mine[i]);
        }
        else if(i == 3){

            amount = atoi(mine[i]);
        }
    }
}

void trx_bank(char mode, int from, int to, int amount){
    if(mode == 'e'){
        accounts[from] -= amount;
        accounts[to] += amount;
    }
    else if(mode == 'v'){
        accounts[to] += amount;
    }
    else if(mode == 'b'){
        accounts[from] -= amount;
    }
}

void show_accounts(){

    for(int i=0; i<N; i++){

        printf("[Balance of %d is %d],", i, accounts[i]);
        if(i % 10 == 9) printf("\n");
    
    }
}


int main(){

    for(int i=0; i<100; i++) accounts[i] = 2000;
    
    char str[256];

    while (1){

        scanf("%s", str);

        mine_cmd(str);
        printf("%c-%d-%d-%d\n",mode, from, to, amount);
        trx_bank(mode, from, to, amount);
        show_accounts();
    }
}