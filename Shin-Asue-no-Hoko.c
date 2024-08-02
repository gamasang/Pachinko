#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>
//#include <windows.h>

#define TamaYen 4 //円パチ
#define HesoKakuritsu 68 //(17/250)==0.068
#define AtariKakuritsu 3199 //*(1/10) %
#define RushKakuritsu 810 //*(1/10) %
#define ChanceKakuritsu 269 //*(1/10) %


int p[3], heso,nyukin,soukin=0, n = 0, hit, r, mode = 0, rush = 0, con = 0, kaiten = 0, soukaiten = 0, atari = 0, souatari = 0, num1, num2, digit, pro, hir, keizokusu = 1, atsu = 0, zantama, rantama = 0;

void beep(){
        system("osascript -e 'beep'");   //mac
    //Beep(750, 300); //Windows 750Hz の音を 300ms 鳴らす
}

// 端末設定を非カノニカルモードに変更
void set_conio_mode(int fd, struct termios *old_termios) {
    struct termios new_termios;
    
    // 現在の端末設定を取得
    if (tcgetattr(fd, old_termios) != 0) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }
    
    // 端末設定を変更
    new_termios = *old_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);  // 非カノニカルモード、エコーを無効にする
    if (tcsetattr(fd, TCSANOW, &new_termios) != 0) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

// 端末設定を元に戻す
void reset_conio_mode(int fd, const struct termios *old_termios) {
    if (tcsetattr(fd, TCSANOW, old_termios) != 0) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

int getch_nonblocking() {
    struct termios old_termios;
    struct termios new_termios;
    int ch;
    fd_set read_fds;
    struct timeval timeout;

    // 端末設定を変更
    tcgetattr(STDIN_FILENO, &old_termios);
    new_termios = old_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);

    FD_ZERO(&read_fds);
    FD_SET(STDIN_FILENO, &read_fds);
    
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    
    int result = select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &timeout);
    if (result > 0 && FD_ISSET(STDIN_FILENO, &read_fds)) {
        ch = getchar();
    } else {
        ch = -1; // データがない場合は -1 を返す
    }

    // 端末設定を元に戻す
    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);

    return ch;
}

void spin(int p[3]) {
    do {
        p[0] = rand() % 9 + 1;
        p[1] = rand() % 9 + 1;
        p[2] = rand() % 9 + 1;
    } while ((p[0] == p[1] && p[1] == p[2]) || (p[0] == 7 && p[2] == 7));
}

void anten(){

        printf("\x1b[40m\033[1;1H\033[0J\x1b[37m");
            //////////////////////
            struct termios old_termios;
            int ch;
            int continue_loop = 1;

            // 端末設定を変更
            set_conio_mode(STDIN_FILENO, &old_termios);

            // 開始時刻を記録
            time_t start_time = time(NULL);

            // カウントダウン表示のためにカーソル位置を移動
            printf("\033[1A\033[1A\033[K"); // 上に1行移動し、行末まで消去

            while (continue_loop) {
                fd_set read_fds;
                struct timeval timeout;

                // 残り時間を計算（5秒以内）
                int elapsed_time = (int)difftime(time(NULL), start_time);
                int remaining_time = 2 - elapsed_time;
                if (remaining_time <= 0) {
                    // 5秒経過した場合
                    remaining_time = 0;
                    continue_loop = 0; // ループを終了する
                  //  printf("\r   %d秒\n", remaining_time); // カウントダウン終了
                    fflush(stdout); // 出力を即時に反映
                    continue;
                }

                FD_ZERO(&read_fds);
                FD_SET(STDIN_FILENO, &read_fds);

                // selectを使用して、標準入力にデータがあるかを確認
                timeout.tv_sec = 1; // 1秒ごとに確認
                timeout.tv_usec = 0;

                int result = select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &timeout);

                if (result == -1) {
                    perror("select");
                    exit(EXIT_FAILURE);
                } else if (result > 0 && FD_ISSET(STDIN_FILENO, &read_fds)) {
                    // 標準入力にデータがある場合にのみgetcharを呼び出す
                    ch = getchar();
                    if (ch == ' ') {
                        // スペースキーを入力すると終了
                        continue_loop = 0; // ループを終了する
                    }
                    // ' ' 以外の文字は完全に無視
                }

                // カウントダウン表示
                //printf("\r   %d秒", remaining_time);
                fflush(stdout); // 出力を即時に反映
            }

            // 端末設定を元に戻す
            reset_conio_mode(STDIN_FILENO, &old_termios);
            //////////////////////
        printf("\x1b[0m\033[1;1H\033[0J");
}

void gawa(int mode, int a, int rush, int kaiten, int soukaiten, int zantama, int atari, int souatari,int soukin,int rantama) {
    if (mode == 0) {
        if (a == 0) {
            printf("\033[1;1H\033[0J\n総当り:%d 最終当り:%d\n総回転:%d 投入金:¥%dk \n回転数:%d 残玉数:%d\n", souatari, atari, soukaiten,soukin, kaiten, zantama);
            printf("/-------\\\n");
        } else {
            printf("+-------+@\n");
        }
    } else {
        if (a == 0) {
            printf("\033[1;1H\033[0J\nRUSH中残り:%d回 total:%dpt\n総当り:%d 最終当り:%d\n総回転:%d 投入金:¥%dk \n回転数:%d 残玉数:%d\n",rush,rantama, souatari, atari, soukaiten,soukin, kaiten, zantama);
            printf("/\x1b[33m*******\x1b[m\\\n");
        } else {
            printf("+\x1b[33m*******\x1b[m+@\n");
        }
    }
}

void screen(int p[3], int *mode, int *rush, int *con, int *kaiten, int *soukaiten, int *zantama, int *atari, int *hir, int *souatari, int *atsu, int *rantama,int soukin) {
    char bot = '0';
    (*kaiten)++;
    (*soukaiten)++;
    usleep(200000);
    gawa(*mode, 0, *rush, *kaiten, *soukaiten, *zantama, *atari, *souatari,soukin,*rantama);
    printf("\n[%d][?][?]\n\n", p[0]);
    gawa(*mode, 1, *rush, *kaiten, *soukaiten, *zantama, *atari, *souatari,soukin,*rantama);
    usleep(200000);
    gawa(*mode, 0, *rush, *kaiten, *soukaiten, *zantama, *atari, *souatari,soukin,*rantama);
    printf("\n[%d][?][%d]\n\n", p[0], p[2]);
    gawa(*mode, 1, *rush, *kaiten, *soukaiten, *zantama, *atari, *souatari,soukin,*rantama);
   //p[0]=2;p[1]=2;p[2]=2;*atsu=1;
    if (p[0] == p[2]) {
        if (*atsu == 1) {
            beep();
        gawa(*mode, 0, *rush, *kaiten, *soukaiten, *zantama, *atari, *souatari,soukin,*rantama);
            printf("\x1b[31m 押 せ !\x1b[0m\n");
            printf("\x1b[36m[%d][?][%d]\x1b[0m\n\n", p[0], p[2]);
        gawa(*mode, 1, *rush, *kaiten, *soukaiten, *zantama, *atari, *souatari,soukin,*rantama);
        beep();
        gawa(*mode, 0, *rush, *kaiten, *soukaiten, *zantama, *atari, *souatari,soukin,*rantama);
            printf(" 押 せ !\n");
            printf("[%d][?][%d]\n\n", p[0], p[2]);
        gawa(*mode, 1, *rush, *kaiten, *soukaiten, *zantama, *atari, *souatari,soukin,*rantama);

            beep();
            //////////////////////
            struct termios old_termios;
            int ch;
            int continue_loop = 1;

            // 端末設定を変更
            set_conio_mode(STDIN_FILENO, &old_termios);

            // 開始時刻を記録
            time_t start_time = time(NULL);

            // カウントダウン表示のためにカーソル位置を移動
            printf("\033[1A\033[1A\033[K"); // 上に1行移動し、行末まで消去

            while (continue_loop) {
                fd_set read_fds;
                struct timeval timeout;

                // 残り時間を計算（5秒以内）
                int elapsed_time = (int)difftime(time(NULL), start_time);
                int remaining_time = 3 - elapsed_time;
                if (remaining_time <= 0) {
                    // 5秒経過した場合
                    remaining_time = 0;
                    continue_loop = 0; // ループを終了する
                    //printf("\r   %d秒\n", remaining_time); // カウントダウン終了
                    fflush(stdout); // 出力を即時に反映
                    continue;
                }

                FD_ZERO(&read_fds);
                FD_SET(STDIN_FILENO, &read_fds);

                // selectを使用して、標準入力にデータがあるかを確認
                timeout.tv_sec = 1; // 1秒ごとに確認
                timeout.tv_usec = 0;

                int result = select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &timeout);

                if (result == -1) {
                    perror("select");
                    exit(EXIT_FAILURE);
                } else if (result > 0 && FD_ISSET(STDIN_FILENO, &read_fds)) {
                    // 標準入力にデータがある場合にのみgetcharを呼び出す
                    ch = getchar();
                    if (ch == ' ') {
                        // スペースキーを入力すると終了
                        continue_loop = 0; // ループを終了する
                    }
                    // ' ' 以外の文字は完全に無視
                }

                // カウントダウン表示
                printf("\r   %d秒", remaining_time);
                fflush(stdout); // 出力を即時に反映
            }

            // 端末設定を元に戻す
            reset_conio_mode(STDIN_FILENO, &old_termios);
            //////////////////////
            beep();
            *atsu = 0;
        } else {
            for(int m=0;m<2;m++){
            gawa(*mode, 0, *rush, *kaiten, *soukaiten, *zantama, *atari, *souatari,soukin,*rantama);
            printf("\n[%d][?][%d]\n", p[0], p[2]);
            printf(" リーチ！\n");
            gawa(*mode, 1, *rush, *kaiten, *soukaiten, *zantama, *atari, *souatari,soukin,*rantama); 
            usleep(50000); 
            gawa(*mode, 0, *rush, *kaiten, *soukaiten, *zantama, *atari, *souatari,soukin,*rantama);
            printf("\n\x1b[36m[%d][?][%d]\x1b[0m\n", p[0], p[2]);
            printf("\x1b[35m リーチ！\x1b[0m\n");
            gawa(*mode, 1, *rush, *kaiten, *soukaiten, *zantama, *atari, *souatari,soukin,*rantama);  
            beep();
            }       
            gawa(*mode, 0, *rush, *kaiten, *soukaiten, *zantama, *atari, *souatari,soukin,*rantama);
            printf("\n[%d][?][%d]\n", p[0], p[2]);
            printf(" リーチ！\n");
            gawa(*mode, 1, *rush, *kaiten, *soukaiten, *zantama, *atari, *souatari,soukin,*rantama); 
        }
    }
    usleep(300000);
    gawa(*mode, 0, *rush, *kaiten, *soukaiten, *zantama, *atari, *souatari,soukin,*rantama);
    printf("\n[%d][%d][%d]\n\n", p[0], p[1], p[2]);
    gawa(*mode, 1, *rush, *kaiten, *soukaiten, *zantama, *atari, *souatari,soukin,*rantama);  
    usleep(500000);      
    if (p[0] == p[1] && p[1] == p[2]) {
            beep();
    gawa(*mode, 0, *rush, *kaiten, *soukaiten, *zantama, *atari, *souatari,soukin,*rantama);
    printf("\n\x1b[33m[%d][%d][%d]\x1b[0m\n\n", p[0], p[1], p[2]);
    gawa(*mode, 1, *rush, *kaiten, *soukaiten, *zantama, *atari, *souatari,soukin,*rantama);  
            beep();
    usleep(500000);      
        if (*mode == 0) {
            anten();
        for (int i = 1; i <= 3; i++) {
            beep();
        }
        for (int i = 1; i <= 50; i++) {
        usleep(50000);    
        gawa(*mode, 0, *rush, *kaiten, *soukaiten, *zantama, *atari, *souatari,soukin,*rantama);
            printf("\n\x1b[33m<<\x1b[3%dm突 \x1b[3%dm入\x1b[33m>>\x1b[0m\n\n",i%8,i%7+1);
        gawa(*mode, 1, *rush, *kaiten, *soukaiten, *zantama, *atari, *souatari,soukin,*rantama);
        }
            *atari = 0;
            *rantama = 0;
        }
        *kaiten = 0;
        *con = rand() % 1000;
        *hir = rand() % (*rush) + 1;
        (*atari)++;    
        (*souatari)++; 
        if(*mode==1||*mode==2){
        if(*mode==1){   
            anten();
        for (int i = 1; i <= 5; i++) {
            beep();
        }
        for (int i = 1; i <= 80; i++) {
        usleep(50000);    
        gawa(*mode, 0, *rush, *kaiten, *soukaiten, *zantama, *atari, *souatari,soukin,*rantama);
        printf("\n\x1b[33m[\x1b[3%dm7\x1b[33m][\x1b[3%dm7\x1b[33m][\x1b[3%dm7\x1b[33m]\x1b[0m\n\n",i%8,i%7+1,i%6+2);
        gawa(*mode, 1, *rush, *kaiten, *soukaiten, *zantama, *atari, *souatari,soukin,*rantama);   
        }
        gawa(*mode, 0, *rush, *kaiten, *soukaiten, *zantama, *atari, *souatari,soukin,*rantama);
        printf("\n\x1b[33m[7][7][7]\x1b[0m\n\n");
        gawa(*mode, 1, *rush, *kaiten, *soukaiten, *zantama, *atari, *souatari,soukin,*rantama);   
        }
        anten();
        for (int i = 1; i <= 3; i++) {
            beep();
        }
        for (int i = 1; i <= 50; i++) {
        usleep(50000);    
        gawa(*mode, 0, *rush, *kaiten, *soukaiten, *zantama, *atari, *souatari,soukin,*rantama);
            printf("\x1b[33m<<\x1b[3%dm継 \x1b[3%dm続\x1b[33m>>\n\x1b[35m total\n %dpt\x1b[0m\n",i%8,i%7+1,*rantama);
        gawa(*mode, 1, *rush, *kaiten, *soukaiten, *zantama, *atari, *souatari,soukin,*rantama);
        } 
        }
        if(p[0]%2==0&&*mode==0){
        *zantama += 450;
        *rantama += 450;
        *rush = 100;
        *mode = 2;
        }else{
        *zantama += 1500;
        *rantama += 1500;
        *rush = 164;
        *mode = 1; 
        }
    }
}

int main() {
    srand((unsigned int)time(NULL));
    
    printf("\033[2J何千円投入しますか？\n");
    scanf("%d",&nyukin);
    zantama = (nyukin*1000)/TamaYen; // 初期残玉数
    soukin = soukin+nyukin;

    do {
        if (mode == 0) {
            zantama--;
            usleep(200000);
            gawa(mode, 0, rush, kaiten, soukaiten, zantama, atari, souatari,soukin,rantama);
            printf("\n[?][?][?]\n\n");
            gawa(mode, 1, rush, kaiten, soukaiten, zantama, atari, souatari,soukin,rantama);
            heso = rand() % 1000;
            if (heso < HesoKakuritsu) {
            beep();
                hit = rand() % AtariKakuritsu;
                if (hit < 500) {
                    if (rand() % 10 == 0) {
            beep();
                    }
                }
                if (hit < 100) {
                    if (rand() % 5 == 0) {
            beep();
                    }
                    if (rand() % 5 == 0) {
                    atsu = 1;
                    }
                }
                if (hit < 50) {
                    if (rand() % 4 == 0) {
            beep();
                    }
                    if (rand() % 4 == 0) {
                    atsu = 1;
                    }
                }
                if (hit < 20) {
                    if (rand() % 2 == 0) {
            beep();
                    }
                    if (rand() % 2 == 0) {
                    atsu = 1;
                    }
                }
                usleep(200000);
                if (hit <= 10) {
                    r = rand() % 9 + 1;
                    for (int i = 0; i < 3; i++) {
                        p[i] = r;
                    }
                    screen(p, &mode, &rush, &con, &kaiten, &soukaiten, &zantama, &atari, &hir, &souatari, &atsu, &rantama,soukin);
                } else {
                    spin(p);
                    screen(p, &mode, &rush, &con, &kaiten, &soukaiten, &zantama, &atari, &hir, &souatari, &atsu, &rantama,soukin);
                }
            }
        } else {
            while (rush > 0) {
                zantama--;
                usleep(200000);
                gawa(mode, 0, rush, kaiten, soukaiten, zantama, atari, souatari,soukin,rantama);
                printf("\n[?][?][?]\n\n");
                gawa(mode, 1, rush, kaiten, soukaiten, zantama, atari, souatari,soukin,rantama);
            beep();
                if (rush - 3 == hir) {
                    for (int i = 0; i < 3; i++) {
            beep();
                    }
                }
                if (rush<rush/4*4) {
                    if(rand()%rush<rush/4*3){
                    if (rand() % 10 == 0) {
            beep();
                    }
                    if (rand() % 10 == 0) {
                    atsu = 1;
                    }
                    }
                }
                if (rush<rush/4*3) {
                    if(rand()%rush<rush/4*2){
                    if (rand() % 10 == 0) {
            beep();
                    }
                    if (rand() % 10 == 0) {
                    atsu = 1;
                    }
                    }
                }
                if (rush<rush/4*2) {
                    if(rand()%rush<rush/4*1){
                    if (rand() % 10 == 0) {
            beep();
                    }
                    if (rand() % 10 == 0) {
                    atsu = 1;
                    }
                    }
                }
                if (rush<rush/4*1) {
                    if(rand()%rush<1){
                    if (rand() % 10 == 0) {
            beep();
                    }
                    if (rand() % 10 == 0) {
                    atsu = 1;
                    }
                    }
                }
                if(mode==1){
                if (con < RushKakuritsu) {
                    if (rush == hir) {                
                        r = rand() % 9 + 1;
                        for (int i = 0; i < 3; i++) {
                            p[i] = r;
                        }
                        screen(p, &mode, &rush, &con, &kaiten, &soukaiten, &zantama, &atari, &hir, &souatari, &atsu, &rantama,soukin);
                        keizokusu++;
                    } else {
                        spin(p);
                        screen(p, &mode, &rush, &con, &kaiten, &soukaiten, &zantama, &atari, &hir, &souatari, &atsu, &rantama,soukin);    
                    }
                } else {
                    spin(p);
                    screen(p, &mode, &rush, &con, &kaiten, &soukaiten, &zantama, &atari, &hir, &souatari, &atsu, &rantama,soukin);                      
                }
                }if(mode==2){
                if (con < ChanceKakuritsu) {
                    if (rush == hir) {                
                        r = rand() % 9 + 1;
                        for (int i = 0; i < 3; i++) {
                            p[i] = r;
                        }
                        screen(p, &mode, &rush, &con, &kaiten, &soukaiten, &zantama, &atari, &hir, &souatari, &atsu, &rantama,soukin);
                        mode=1;
                        keizokusu++;
                    } else {
                        spin(p);
                        screen(p, &mode, &rush, &con, &kaiten, &soukaiten, &zantama, &atari, &hir, &souatari, &atsu, &rantama,soukin);    
                    }
                } else {
                    spin(p);
                    screen(p, &mode, &rush, &con, &kaiten, &soukaiten, &zantama, &atari, &hir, &souatari, &atsu, &rantama,soukin);                      
                }
                }
                rush--;
            }
            mode = 0;
            gawa(mode, 0, rush, kaiten, soukaiten, zantama, atari, souatari,soukin,rantama);
            printf("継続%d回\n %dpt\n", keizokusu, rantama);
            gawa(mode, 1, rush, kaiten, soukaiten, zantama, atari, souatari,soukin,rantama);
            usleep(500000);
        }if(zantama == 0){
            gawa(mode, 0, rush, kaiten, soukaiten, zantama, atari, souatari,soukin,rantama);
            printf("continue?\n y or n\n");
            gawa(mode, 1, rush, kaiten, soukaiten, zantama, atari, souatari,soukin,rantama);
            //////////////////
            struct termios old_termios;
            int ch;
            int continue_loop = 1;

            // 端末設定を変更
            set_conio_mode(STDIN_FILENO, &old_termios);    

            while (continue_loop) {
                fd_set read_fds;
                struct timeval timeout;

                FD_ZERO(&read_fds);
                FD_SET(STDIN_FILENO, &read_fds);
                
                timeout.tv_sec = 1; // タイムアウトを1秒に設定
                timeout.tv_usec = 0;

                // selectを使用して、標準入力にデータがあるかを確認
                int result = select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &timeout);

                if (result == -1) {
                    perror("select");
                    exit(EXIT_FAILURE);
                } else if (result > 0 && FD_ISSET(STDIN_FILENO, &read_fds)) {
                    // 標準入力にデータがある場合にのみgetcharを呼び出す
                    ch = getchar();
                    if (ch == 'y') {
                        // 'y'を入力すると継続        
                        continue_loop = 0;
                    } else if (ch == 'n') {
                        // 'n'を入力すると終了
                        n = 1;
                        continue_loop = 0; // ループを終了する
                    }
                    // 'y' または 'n' 以外の文字は完全に無視
                }
                // タイムアウトの場合は何もしない
            }

            // 端末設定を元に戻す
            reset_conio_mode(STDIN_FILENO, &old_termios);
            //////////////////
        if(n==0){
            printf("\033[1;1H\033[0J何千円投入しますか？\n");
            scanf("%d",&nyukin);
            zantama = (nyukin*1000)/4;
            soukin = soukin+nyukin;
        }
        }
    } while (n == 0);

    return 0;
}
