#include <stdio.h>
#include <stdlib.h>
#include "message.h"
#include "logging.h"
#include <string.h>
#include <stdbool.h>
#include <sys/select.h>
#include <sys/wait.h>


typedef struct bomber{
    pid_t pid;
    coordinate coor;
    int arg_count;
    char **args;
    bool is_alive;
    // last added field : CHECK ALL STRUCT INITILIZATION. TO DO
    bool is_last;
    bool is_served;
} bomber;

typedef struct bomb{
    pid_t pid;
    coordinate coor;
    bool is_alive;
    long interval;
    unsigned int radius;
} bomb;

// TO DO
//  !!!!!!!!!! BEN WRITE VE READ KULLANDIM.BUNLAR YERINE message.h'da verilmiş send_message, ... ları kullanacağım.

int width,height,obstacle_count,bomber_count;
obsd *obstacles;
bomber *bombers;
bomb *bombs;
bool gameOver = false; // set true when alive bomber count is 0.Alive bomber count is bomber count at start.
int **maze; // Bunun yerine int ** kullanabilirim.0 means empty, 1 means bomber, 3 means bomb, 4 means obstacle, 2 means both.
// TO DO.Bunu 0 olarak init ettik.Fakat obstacle, bomber ve bomb coordinatlarına bakarak 1 mi 3 mü 2 mi olacağına karar ver.
// Bu yapıldı.

int stat;
int bombCount = 0;
int aliveBomberCount;

int max(int a, int b){
    if(a >= b){
        return a;
    }
    return b;
}

void readInput(){

    scanf("%d %d %d %d",&width, &height, &obstacle_count, &bomber_count);
    obstacles = (obsd *)malloc(sizeof(obsd) * obstacle_count);
    coordinate coord;
    int durability;
    for(int i = 0 ; i < obstacle_count ; i++){
        scanf("%d %d %d",&coord.x, &coord.y,&durability);
        obstacles[i].position = coord;
        obstacles[i].remaining_durability = durability;
    }
    bombers =(bomber *) malloc(sizeof(bomber) * bomber_count);
    for(int i = 0 ; i < bomber_count ; i++){
        scanf("%d %d %d",&(bombers[i].coor.x),&(bombers[i].coor.y), &(bombers[i].arg_count));

        bombers[i].args = (char **)malloc((bombers[i].arg_count + 1) * sizeof(char *));
        scanf("%ms", &(bombers[i].args[0]));
        for(int j = 1 ; j < bombers[i].arg_count ; j++){
            scanf("%ms", &(bombers[i].args[j]));
        }
        bombers[i].args[bombers[i].arg_count] = NULL;
        bombers[i].is_alive = 1;
        bombers[i].is_last = 0;
    }

    aliveBomberCount = bomber_count;
}

void initMaze(){
    coordinate coor;
    maze = (int **) malloc(height * sizeof(int *)); // Check this with prev.
    for(int i = 0 ; i < height ; i++){
        maze[i] = (int *) malloc(width * sizeof(int));
    }

    for(int i = 0; i < height ; i++){  // First, set all cells in the maze as empty that means 0.
        for(int j = 0 ; j < width ; j++){
            maze[i][j] = 0;
        }
    }

    for(int i = 0 ; i < obstacle_count ; i++){ // Then, set the cells in which there is an obstacle.
        coor = obstacles[i].position;
        maze[coor.x][coor.y] = 4;
    }

    for(int i = 0 ; i < bomber_count ; i++){ // Then, set the cells in which there is a bomber.
        coor = bombers[i].coor;
        maze[coor.x][coor.y] = 1;
    }
}
/*
void createPipes(int **fds){
    for(int i = 0; i < bomber_count ; i++){

    }
}
 */

void forkBomber(int **fds){
    pid_t pid;
    for(int i = 0; i < bomber_count ; i++){
        PIPE(fds[i]);
        pid = fork();
        if(pid > 0){ //parent
            close(fds[i][1]);
            //dup2(fds[i][0],0);
            //dup2(fds[i][0],1);
            //close(fds[i][0]);
            bombers[i].pid = pid;
        }
        else{ // child means it is a bomber.
            close(fds[i][0]);
            dup2(fds[i][1],0);
            dup2(fds[i][1],1);
            close(fds[i][1]);
            execvp(bombers[i].args[0], bombers[i].args); // !!! check execv arguments
            printf("Child process never reaches here\n");

        }
    }
}

void createBombPipeandFork(bd bomb_info, bomber currentBomber, bomb *newBomb, int **bombfds, int count){
    pid_t pid;
    newBomb->interval = bomb_info.interval;
    newBomb->radius = bomb_info.radius;
    newBomb->coor = currentBomber.coor;
    newBomb->is_alive = 1;

    char str[10 * sizeof(char)];
    sprintf(str, "%ld", newBomb->interval);
    char *args[] = {"./bomb", str, NULL};

    PIPE(bombfds[count]);
    pid = fork();
    if(pid > 0){ // parent
        close(bombfds[count][1]);
        newBomb->pid = pid;
    }
    else{
        close(bombfds[count][0]);
        dup2(bombfds[count][1], 0);
        dup2(bombfds[count][1], 1);
        close(bombfds[count][1]);
        execvp(args[0], args);
        printf("Child process never reaches here\n");
    }
}



void sendLocationRespondFromController(bomber currentBomber, int i, int **fds, coordinate data){
    om out_message = {
            .type = BOMBER_LOCATION,
            .data.new_position = data
    };
    send_message(fds[i][0], &out_message);

    omp out_message_print = {
            .pid = currentBomber.pid,
            .m = &out_message
    };

    print_output(NULL, &out_message_print, NULL, NULL);
}

void sendVisionRespondFromController(bomber currentBomber, int i, int **fds, int obj_count, od object_data[]){
    om out_message = {
            .type = BOMBER_VISION,
            .data.object_count = obj_count
    };
    send_message(fds[i][0], &out_message);
    // send also n object_data structures
    for(int i = 0 ; i < obj_count ; i++){
        send_object_data(fds[i][0],obj_count, &object_data[i]);
    }

    omp  out_message_print = {
            .pid = currentBomber.pid,
            .m = &out_message
    };

    print_output(NULL, &out_message_print, NULL, object_data);

}
void sendBomberPlantRespondFromController(bomber currentBomber, int i, int **fds, int is_success){
    om out_message = {
            .type = BOMBER_PLANT_RESULT,
            .data.planted = is_success
    };
    send_message(fds[i][0], &out_message);

    omp  out_message_print = {
            .pid = currentBomber.pid,
            .m = &out_message
    };

    print_output(NULL, &out_message_print, NULL, NULL);

}
void sendBomberDieRespondFromController(bomber currentBomber, int i, int **fds){
    om out_message = {
            .type = BOMBER_DIE,
    };
    send_message(fds[i][0], &out_message);

    omp out_message_print = {
            .pid = currentBomber.pid,
            .m = &out_message
    };

    print_output(NULL, &out_message_print, NULL, NULL);
}

void sendBomberWinRespondFromController(bomber currentBomber, int i, int **fds){
    om out_message = {
            .type = BOMBER_WIN,
    };
    send_message(fds[i][0], &out_message);

    omp out_message_print = {
            .pid = currentBomber.pid,
            .m = &out_message
    };

    print_output(NULL, &out_message_print, NULL, NULL);

    // TO DO
    gameOver = true;
}


bool checkMovementValidity(bomber currentBomber, coordinate target_coordinate){
    coordinate current_coordinate = currentBomber.coor;
    if(target_coordinate.x < 0 || target_coordinate.x > width || target_coordinate.y < 0 || target_coordinate.y > height){
        return false;
    }
    if((abs(target_coordinate.x - current_coordinate.x) == 1 && target_coordinate.y == current_coordinate.y) || (abs(target_coordinate.y - current_coordinate.y) == 1 && target_coordinate.x == current_coordinate.x)){
        if(maze[target_coordinate.x][target_coordinate.y] == 0 || maze[target_coordinate.x][target_coordinate.y] == 3){ // !!! diğer durumları da kontrol et. TO DO yapıldı.
            // Bunu yaptım !!! Bombersların coordinatını yeni yerleri ile güncelle.Also, mazeyi güncelle. TO DO
            if(maze[currentBomber.coor.x][currentBomber.coor.y] == 2){ // if the location was 2 meaning both, set as only bomb meaning 3.
                maze[currentBomber.coor.x][currentBomber.coor.y] = 3;
            }
            if(maze[currentBomber.coor.x][currentBomber.coor.y] == 1){
                maze[currentBomber.coor.x][currentBomber.coor.y] = 0;
            }
            if(maze[target_coordinate.x][target_coordinate.y] == 3){
                maze[target_coordinate.x][target_coordinate.y] = 2;
            }
            if(maze[target_coordinate.x][target_coordinate.y] == 0){
                maze[target_coordinate.x][target_coordinate.y] = 1;
            }

            //currentBomber.coor = target_coordinate;

            return true;
        }
    }
    return false;
}

bool checkPlantValidity(bomber currentBomber){
    coordinate bomber_coordinate = currentBomber.coor;
    if(maze[bomber_coordinate.x][bomber_coordinate.y] == 3 || maze[bomber_coordinate.x][bomber_coordinate.y] == 2){ // means there is already a bomb in that location.
        // I cannot check the other conditions because I already checked that the movement is valid, so
        // the bomber can move this location and can plant a bomb if there is no other bombs at that location.
        return false;
    }
    return true;
}


int calcSeeCount(bomber currentBomber, od *objects){

    int i,j;
    int count = 0;
    coordinate bc = currentBomber.coor;
    if(maze[bc.x][bc.y] == 2){ // Check the bomber's current location.If there is a bomb with it, count it too.
        objects[count].position.x = bc.x;
        objects[count].position.y = bc.y;
        objects[count].type = BOMB;
        count++;
    }
    for(i = 1 ; i <= 3 && bc.x + i < height; i++){ // Check the right side
        if(maze[bc.x + i][bc.y] == 4){
            objects[count].position.x = bc.x + i;
            objects[count].position.y = bc.y;
            objects[count].type = OBSTACLE;
            count++;
            break; // Means there is an obstacle along that direction.
        }
        if(maze[bc.x + i][bc.y] != 0){
            objects[count].position.x = bc.x + i;
            objects[count].position.y = bc.y;
            if(maze[bc.x + i][bc.y] == 2){
                objects[count].type = BOMB;
                count++;
                objects[count].position.x = bc.x + i;
                objects[count].position.y = bc.y;
                objects[count].type = BOMBER;
                count++;
            }
            else{
                objects[count].type = (maze[bc.x + i][bc.y] == 1) ? BOMBER : BOMB;
                count++;
            }

        }
    }
    for(i = 1 ; i <= 3 && ((int)(bc.x - i)) >= 0 ; i++) { // Check the left side
        int first = bc.x -i;
        if (maze[first][bc.y] == 4) {
            objects[count].position.x = first;
            objects[count].position.y = bc.y;
            objects[count].type = OBSTACLE;
            count++;
            break;
        }
        if (maze[first][bc.y] != 0) {
            objects[count].position.x = first;
            objects[count].position.y = bc.y;
            if(maze[first][bc.y] == 2){
                objects[count].type = BOMB;
                count++;
                objects[count].position.x = first;
                objects[count].position.y = bc.y;
                objects[count].type = BOMBER;
                count++;
            }
            else {
                objects[count].type = (maze[first][bc.y] == 1) ? BOMBER : BOMB;
                count++;
            }
        }
    }
    for(j = 1 ; j <= 3 && ((int)(bc.y - j)) >= 0 ; j++){ // Check the up side
        int second = bc.y - j;
        if(maze[bc.x][second] == 4){
            objects[count].position.x = bc.x;
            objects[count].position.y = second;
            objects[count].type = OBSTACLE;
            count++;
            break;
        }
        if(maze[bc.x][second] != 0){
            objects[count].position.x = bc.x;
            objects[count].position.y = second;
            if(maze[bc.x][second] == 2){
                objects[count].type = BOMB;
                count++;
                objects[count].position.x = bc.x;
                objects[count].position.y = second;
                objects[count].type = BOMBER;
                count++;
            }
            else{
                objects[count].type = (maze[bc.x][second] == 1) ? BOMBER : BOMB;
                count++;
            }
        }
    }
    for(j = 1 ; j <= 3 && bc.y + j < width ; j++){ // Check the bottom side
        if(maze[bc.x][bc.y + j] == 4){
            objects[count].position.x = bc.x;
            objects[count].position.y = bc.y + j;
            objects[count].type = OBSTACLE;
            count++;
            break;
        }
        if(maze[bc.x][bc.y + j] != 0){
            objects[count].position.x = bc.x;
            objects[count].position.y = bc.y + j;
            if(maze[bc.x][bc.y + j] == 2){
                objects[count].type = BOMB;
                count++;
                objects[count].position.x = bc.x;
                objects[count].position.y = bc.y + j;
                objects[count].type = BOMBER;
                count++;
            }
            else{
                objects[count].type = (maze[bc.x][bc.y + j] == 1) ? BOMBER : BOMB;
                count++;
            }
        }
    }
    return count;
}

void findObstacleAndDecrease(unsigned int x, unsigned int y){
    for(int i = 0 ; i < obstacle_count ; i++){
        if(obstacles[i].position.x == x && obstacles[i].position.y == y){
            if(obstacles[i].remaining_durability > 0){
                obstacles[i].remaining_durability--;
                if(obstacles[i].remaining_durability == 0){
                    // The obstacle's durability reaches zero, so it should be removed means that update maze.
                    // Set this location as empty which is 0.
                    maze[x][y] = 0;
                }
                obsd obstacle_data_print_info = {
                        .position = obstacles[i].position,
                        .remaining_durability = obstacles[i].remaining_durability
                };
                print_output(NULL, NULL, &obstacle_data_print_info, NULL);
                break;
            }
        }
    }
}
void findBomberAndKill(unsigned int x, unsigned int y){
    for(int i = 0 ; i < bomber_count ; i++){
        if(bombers[i].is_alive && bombers[i].coor.x == x && bombers[i].coor.y == y){
            // The bomber should die.
            // The controller/server marks the bomber.
            // When it send its next request, BOMBER_DIE should be sent without any other data.
            // If that bomber is second to last bomber, the last remaining bomber should be marked as well.
            // When the last bomber makes a request, BOMBER_WIN should be sent to that bomber to let them know, they
            // had won the game.
            // TO DO After this point,
            // There are other conditions as well.TO DO

            maze[x][y] = 0;
            if(aliveBomberCount > 1){
                bombers[i].is_alive = false;
                aliveBomberCount--;
            }
            if(aliveBomberCount == 1){
                for(int j = 0 ; j < bomber_count ; j++){
                    if(bombers[j].is_alive){
                        bombers[j].is_last = true;
                    }
                }
            }
        }
    }
}

void findBombAndKill(unsigned int x, unsigned int y){
    for(int i = 0 ; i < bombCount ; i++){
        if(bombs[i].is_alive && bombs[i].coor.x == x && bombs[i].coor.y == y){
            maze[x][y] = 0;
            bombs[i].is_alive = false;
        }
    }
}

void findBomberBombAndKill(unsigned int x, unsigned int y){
    for(int i = 0 ; i < bombCount ; i++){
        if(bombs[i].is_alive && bombs[i].coor.x == x && bombs[i].coor.y == y){
            maze[x][y] = 1;
            bombs[i].is_alive = false;
        }
    }
    for(int i = 0 ; i < bomber_count ; i++){
        if(bombers[i].is_alive && bombers[i].coor.x == x && bombers[i].coor.y == y){
            // The bomber should die.
            // The controller/server marks the bomber.
            // When it send its next request, BOMBER_DIE should be sent without any other data.
            // If that bomber is second to last bomber, the last remaining bomber should be marked as well.
            // When the last bomber makes a request, BOMBER_WIN should be sent to that bomber to let them know, they
            // had won the game.
            // TO DO After this point,
            // There are other conditions as well.TO DO

            maze[x][y] = 0;

            if(aliveBomberCount > 1){
                aliveBomberCount--;
                bombers[i].is_alive = false;

            }
            if(aliveBomberCount == 1){
                for(int j = 0 ; j < bomber_count ; j++){
                    if(bombers[j].is_alive && !(bombers[j].is_last)){
                        bombers[j].is_last = true;
                    }
                }
            }

        }
    }
}

// TO DO.Bu fonksiyonu check et çünkü if blocklarına bir case daha eklenecek o da
// explosionın bir cellde hem bomber hem de bomb olduğu zaman ikisinin etkilenmesi.
void processBombExplode(coordinate bomb_coordinate, unsigned int explosion_radius){
    if(maze[bomb_coordinate.x][bomb_coordinate.y] == 3){
        findBombAndKill(bomb_coordinate.x, bomb_coordinate.y);
    }
    if(maze[bomb_coordinate.x][bomb_coordinate.y] == 2){
        //findBombAndKill(bomb_coordinate.x, bomb_coordinate.y);
        //findBomberAndKill(bomb_coordinate.x, bomb_coordinate.y);
        findBomberBombAndKill(bomb_coordinate.x, bomb_coordinate.y);
    }
    for(int i = 1 ; i <= explosion_radius && bomb_coordinate.x + i < height ; i++){
        if(maze[bomb_coordinate.x + i][bomb_coordinate.y] == 4){ // means obstacle
            // find the obstacle and decrease its durability, and if it reaches zero, remove it.
            findObstacleAndDecrease(bomb_coordinate.x + i, bomb_coordinate.y);
            break;
        }
        if(maze[bomb_coordinate.x + i][bomb_coordinate.y] == 1){ // means bomber
            // find the bomber affected by this boms, and the bomber should die.
            findBomberAndKill(bomb_coordinate.x + i, bomb_coordinate.y);
        }
        if(maze[bomb_coordinate.x + i][bomb_coordinate.y] == 2){ // means both bomber and bomb
            findBomberBombAndKill(bomb_coordinate.x + i, bomb_coordinate.y);
        }
        if(maze[bomb_coordinate.x + i][bomb_coordinate.y] == 3){ // means bomb
            findBombAndKill(bomb_coordinate.x + i, bomb_coordinate.y);
        }
    }
    for(int i = 1 ; i <= explosion_radius && ((int)(bomb_coordinate.x - i)) >= 0 ; i++){
        int first = bomb_coordinate.x - i;
        if(maze[first][bomb_coordinate.y] == 4){
            findObstacleAndDecrease(first, bomb_coordinate.y);
            break;
        }
        if(maze[first][bomb_coordinate.y] == 1){
            findBomberAndKill(first, bomb_coordinate.y);
        }
        if(maze[first][bomb_coordinate.y] == 2){ // means both bomber and bomb
            findBomberBombAndKill(first, bomb_coordinate.y);
        }
        if(maze[first][bomb_coordinate.y] == 3){ // means bomb
            findBombAndKill(first, bomb_coordinate.y);
        }
    }
    for(int j = 1 ; j <= explosion_radius && ((int)(bomb_coordinate.y - j)) >= 0 ; j++){
        int second = bomb_coordinate.y - j;
        if(maze[bomb_coordinate.x][second] == 4){
            findObstacleAndDecrease(bomb_coordinate.x, second);
            break;
        }
        if(maze[bomb_coordinate.x][second] == 1){
            findBomberAndKill(bomb_coordinate.x, second);
        }
        if(maze[bomb_coordinate.x][second] == 2) { // means both bomber and bomb
            findBomberBombAndKill(bomb_coordinate.x, second);
        }
        if(maze[bomb_coordinate.x][second] == 3){ // means bomb
            findBombAndKill(bomb_coordinate.x, second);
        }
    }
    for(int j = 1 ; j <= explosion_radius && bomb_coordinate.y + j < width ; j++){
        if(maze[bomb_coordinate.x][bomb_coordinate.y + j] == 4){
            findObstacleAndDecrease(bomb_coordinate.x, bomb_coordinate.y + j);
            break;
        }
        if(maze[bomb_coordinate.x][bomb_coordinate.y + j] == 1){
            findBomberAndKill(bomb_coordinate.x, bomb_coordinate.y + j);
        }
        if(maze[bomb_coordinate.x][bomb_coordinate.y + j] == 2) { // means both bomber and bomb
            findBomberBombAndKill(bomb_coordinate.x, bomb_coordinate.y + j);
        }
        if(maze[bomb_coordinate.x][bomb_coordinate.y + j] == 3){ // means bomb
            findBombAndKill(bomb_coordinate.x, bomb_coordinate.y + j);
        }
    }
}

int main() {
    readInput(); // true
    initMaze(); // true
    int **fds;
    int **bfds;
    fds = (int **) malloc(sizeof(int *) * bomber_count);
    for (int i = 0; i < bomber_count; i++) {
        fds[i] = (int *) malloc(sizeof(int) * 2);
    }

    forkBomber(fds);
    int nfds;
    fd_set rfds; // read file descriptors
    fd_set brfds;
    struct timeval tv;
    int max_fd = 0;
    int max_bfd = 0;

    int ready;
    int readyBomb;

    nfds = bomber_count;
    im *in_message = (im *)malloc(sizeof(im));
    unsigned int explosion_radius;
    coordinate bomb_coordinate;

    while (!gameOver) { // main loop
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        max_fd = 0;
        max_bfd = 0;

        FD_ZERO(&rfds);
        for (int i = 0; i < bomber_count; i++) {
            FD_SET(fds[i][0], &rfds); // CHECK fds[i][1] or fds[i][0] ?
            max_fd = max(max_fd, fds[i][0]);
        }

        FD_ZERO(&brfds);
        for(int i = 0 ; i < bombCount ; i++){
            FD_SET((bfds[i][0]), &brfds);
            max_bfd = max(max_bfd, bfds[i][0]);
        }

        //if(bombCount > 0){
        readyBomb = select(max_bfd +1, &brfds, NULL, NULL, &tv);
        if(readyBomb > 0){
            for(int i = 0 ; i < bombCount ; i++){
                if(bombs[i].is_alive && FD_ISSET(bfds[i][0], &brfds)){ // bombs[i].bfd[0] is ready for reading
                    read_data(bfds[i][0], in_message);
                    explosion_radius = bombs[i].radius;
                    bomb_coordinate = bombs[i].coor;

                    // The bomb will quit after sending this message.TO DO.

                    imp in_message_print = {
                            .pid = bombs[i].pid,
                            .m = in_message
                    };
                    print_output(&in_message_print, NULL, NULL, NULL);
                    if(in_message->type == BOMB_EXPLODE){
                        processBombExplode(bomb_coordinate, explosion_radius);
                        // waitpid for exploded bomb
                        // close(bfds[i][0]);
                        waitpid(bombs[i].pid, &stat, 1);
                    }

                }
            }
        }
        //}

        ready = select(max_fd + 1,&rfds, NULL, NULL, &tv);
        if(ready > 0){
            for(int i = 0 ; i < nfds ; i++){
                if(bombers[i].is_alive && bombers[i].is_last && FD_ISSET(fds[i][0], &rfds)){
                    read_data(fds[i][0], in_message);
                    imp in_message_print = {
                            .pid = bombers[i].pid,
                            .m = in_message
                    };
                    print_output(&in_message_print, NULL, NULL, NULL);
                    sendBomberWinRespondFromController(bombers[i], i, fds);

                    // waitpid for winner bomber

                    // close(fds[i][0]);
                    waitpid(bombers[i].pid, &stat, 1);
                    gameOver = true;

                }
                if(bombers[i].is_alive && !(bombers[i].is_last) && FD_ISSET(fds[i][0],&rfds)){ // fd[i][0] is ready for reading
                    read_data(fds[i][0], in_message);

                    imp in_message_print = {
                            .pid = bombers[i].pid,
                            .m = in_message
                    };
                    print_output(&in_message_print, NULL, NULL, NULL);

                    if(in_message->type == BOMBER_START){
                        // The controller/server should respond with their coordinates.
                        // The answer:
                        // type : BOMBER_LOCATION
                        // data : Location of the bomber
                        // After sending the out_message, call the print_output();
                        sendLocationRespondFromController(bombers[i], i, fds, bombers[i].coor);
                    }
                    if(in_message->type == BOMBER_MOVE){
                        // Check the validity of the movement : There are 4 conditions that needs to be met for a move
                        // to be accepted.
                        // The controller/server should respond with the new location.
                        // The answer:
                        // type : BOMBER_LOCATION
                        // data : Target coordinate the bomber is trying to move
                        if(checkMovementValidity(bombers[i], in_message->data.target_position)){
                            bombers[i].coor.x = in_message->data.target_position.x;
                            bombers[i].coor.y = in_message->data.target_position.y;

                            sendLocationRespondFromController(bombers[i], i, fds, in_message->data.target_position);
                        }
                        else{
                            sendLocationRespondFromController(bombers[i], i, fds, bombers[i].coor);
                        }

                    }
                    if(in_message->type == BOMBER_SEE){
                        // The controller/server should respond with the number of objects as the data argument.
                        // The answer:
                        // type : BOMBER_VISION
                        // data : number of objects
                        // After sending this message, it should follow it up with n object_data structures.
                        od *obj_data = (od *) malloc(sizeof(od *) * 25);
                        int see_count = calcSeeCount(bombers[i], obj_data);
                        sendVisionRespondFromController(bombers[i], i, fds, see_count, obj_data);


                        // After sending and printing the obj_data, DON'T FORGET FREEING IT !!!
                        free(obj_data);


                    }
                    if(in_message->type == BOMBER_PLANT){
                        // Check the validity of the bomb plant : It is successful when there is no other bombs at that location.
                        if(checkPlantValidity(bombers[i])){
                            // Create necessary pipes, fork the process, redirect the pipes and execute the bomb with its parameters.

                            // Update the maze
                            maze[bombers[i].coor.x][bombers[i].coor.y] = 2;

                            if(bombCount == 0){
                                bombs = (bomb *)malloc(sizeof(bomb));
                                bfds = (int **)malloc(sizeof(int *));
                                bfds[0] = (int *)malloc(sizeof(int) * 2);
                                createBombPipeandFork(in_message->data.bomb_info, bombers[i], &bombs[0], bfds, bombCount);
                                //FD_SET(bombs[0].bfd[0], &brfds);

                                bombCount++;
                            }
                            else{
                                bombs = realloc(bombs, sizeof(bomb) * (bombCount + 1));
                                bfds = realloc(bfds, sizeof(int *) * (bombCount + 1));
                                for(int k = 0 ; k <= bombCount ; k++){
                                    bfds[k] = realloc(bfds[k], sizeof(int) * 2);
                                }
                                createBombPipeandFork(in_message->data.bomb_info, bombers[i], &bombs[bombCount], bfds, bombCount);
                                //FD_SET(bombs[bombCount -1].bfd[0], &brfds);

                                bombCount++;
                            }

                            // The answer:
                            // type : BOMBER_PLANT_RESULT
                            // data : planted : a boolean to indicate the success or failure

                            sendBomberPlantRespondFromController(bombers[i], i, fds, 1);
                        }
                        else{
                            sendBomberPlantRespondFromController(bombers[i], i, fds, 0);
                        }
                    }
                }
                if(!(bombers[i].is_alive) && !(bombers[i].is_served) && !(bombers[i].is_last) && FD_ISSET(fds[i][0],&rfds)){
                    bombers[i].is_served = true;
                    read_data(fds[i][0], in_message);

                    imp in_message_print = {
                            .pid = bombers[i].pid,
                            .m = in_message
                    };
                    print_output(&in_message_print, NULL, NULL, NULL);
                    sendBomberDieRespondFromController(bombers[i], i, fds);

                    // waitpid for killed bomber
                    // close(fds[i][0]);
                    waitpid(bombers[i].pid, &stat, 1);





                }
            }
        }

        usleep(1000);
    }



    for (int i = 0; i < bomber_count; i++) {
        for (int j = 0; j < bombers[i].arg_count; j++) {
            free(bombers[i].args[j]);
        }
        free(bombers[i].args);
    }
    free(bombers);
    free(obstacles);

    for(int i = 0 ; i < height ; i++){
        free(maze[i]);
    }
    free(maze);

    for(int i = 0 ; i < bomber_count ; i++){
        free(fds[i]);
    }
    free(fds);

    free(in_message);
    free(bombs);

    for(int i = 0 ; i < bombCount ; i++){
        free(bfds[i]);
    }
    free(bfds);

    return 0;
}

// last2