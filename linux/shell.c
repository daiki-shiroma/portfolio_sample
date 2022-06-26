#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#define BUFLEN    1024     /* コマンド用のバッファの大きさ */
#define MAXARGNUM  256     /* 最大の引数の数 */
#define PATHNAME_SIZE 512
#define MAXLENGTH 256

typedef struct Node{
  char *data;
  struct Node *next;
}stack;
void push(stack**, char*);
void print_stack(stack** );
char* pop(stack **);

typedef struct command_log{
  char *data;
  struct command_log *next;
}hisotry;
void pre_register_log(hisotry**,char**);
void register_log(hisotry**,char*);
void print_log(hisotry**);
char* latest_log(hisotry **);
char* search_log(hisotry **,char*);

typedef struct alias{
  char *original;
  char *ali_name;
  struct alias *next;
}ali;
void register_ali(ali**,char*,const char*);
void print_ali(ali**);
char* search_ali(ali**,char*);
void del_ali(ali** ,char* );

int parse(char [], char *[]);
void execute_command(char *[],int,stack **,hisotry**,ali **,char**);
int main(int argc, char *argv[])
{
    char command_buffer[BUFLEN]; /* コマンド用のバッファ */
    char *args[MAXARGNUM];       /* 引数へのポインタの配列 */
    int command_status;          /* コマンドの状態を表す
                                    command_status = 0 : フォアグラウンドで実行
                                    command_status = 1 : バックグラウンドで実行
                                    command_status = 2 : シェルの終了
                                    command_status = 3 : 何もしない */
    int cc;
    FILE *fp;
    char string[MAXLENGTH], *tmp;
    stack* stack_root = NULL;
    hisotry*  log_root=NULL;
    ali* ali_root=NULL;
    char init_name[]="Command";
    char *prompt_name=init_name;
    /*
     *  無限にループする
     */
    //スクリプト機能

      /*  fp=fopen("script_file.txt","r");
        if(fp == NULL){
                fprintf(stderr, "script_file.txt is not found.\n");
            }
        else{
        cc = getc(fp);                            // ファイルからの1文字読みだし

           while(cc != EOF){

               while(isspace(cc))  cc = getc(fp);
              
               if(cc == EOF) break;

               tmp = string;
               while(!(isspace(cc) || cc == EOF)){
                  if((tmp - string) >= (MAXLENGTH - 1)){
                       fprintf(stderr, "Too long sentence.\n");
                       fclose(fp);
                       exit(1);
                   }
                   *tmp++ = cc;
                   cc = getc(fp);
               }
               *tmp = '\0';
               system(string);
               }
        fclose(fp);
            return 0;
        }*/
    //スクリプト機能ここまで



    for(;;) {
    

        /*
         *  プロンプトを表示する
         */

        printf("%s : ",init_name);

        /*
         *  標準入力から１行を command_buffer へ読み込む
         *  入力が何もなければ改行を出力してプロンプト表示へ戻る
         */

        if(fgets(command_buffer, BUFLEN, stdin) == NULL) {
            printf("\n");
            continue;
        }

        /*
         *  入力されたバッファ内のコマンドを解析する
         *
         *  返り値はコマンドの状態
         */

        command_status = parse(command_buffer, args);

        /*
         *  終了コマンドならばプログラムを終了
         *  引数が何もなければプロンプト表示へ戻る
         */

        if(command_status == 2) {
            printf("done.\n");
            exit(EXIT_SUCCESS);
        } else if(command_status == 3) {
            continue;
        }
        
        /*
         *  コマンドを実行する
         */

        execute_command(args,command_status,&stack_root
                        ,&log_root,&ali_root,&prompt_name);
    }
    free(tmp);
    free(stack_root);
    free(log_root);
    free(ali_root);
    return 0;
}

/*----------------------------------------------------------------------------
 *
 *  関数名   : parse
 *
 *  作業内容 : バッファ内のコマンドと引数を解析する
 *
 *  引数     :
 *
 *  返り値   : コマンドの状態を表す :
 *                0 : フォアグラウンドで実行
 *                1 : バックグラウンドで実行
 *                2 : シェルの終了
 *                3 : 何もしない
 *
 *  注意     :
 *
 *--------------------------------------------------------------------------*/

int parse(char buffer[],        /* バッファ */
          char *args[])         /* 引数へのポインタ配列 */
{
    int arg_index;   /* 引数用のインデックス */
    int status;   /* コマンドの状態を表す */

    /*
     *  変数の初期化
     */

    arg_index = 0;
    status = 0;

    /*
     *  バッファ内の最後にある改行をヌル文字へ変更
     */

    *(buffer + (strlen(buffer) - 1)) = '\0';

    /*
     *  バッファが終了を表すコマンド（"exit"）ならば
     *  コマンドの状態を表す返り値を 2 に設定してリターンする
     */

    if(strcmp(buffer, "exit") == 0) {

        status = 2;
        return status;
    }

    /*
     *  バッファ内の文字がなくなるまで繰り返す
     *  （ヌル文字が出てくるまで繰り返す）
     */

    while(*buffer != '\0') {

        /*
         *  空白類（空白とタブ）をヌル文字に置き換える
         *  これによってバッファ内の各引数が分割される
         */

        while(*buffer == ' ' || *buffer == '\t') {
            *(buffer++) = '\0';
        }

        /*
         * 空白の後が終端文字であればループを抜ける
         */

        if(*buffer == '\0') {
            break;
        }

        /*
         *  空白部分は読み飛ばされたはず
         *  buffer は現在は arg_index + 1 個めの引数の先頭を指している
         *
         *  引数の先頭へのポインタを引数へのポインタ配列に格納する
         */

        args[arg_index] = buffer;
        ++arg_index;

        /*
         *  引数部分を読み飛ばす
         *  （ヌル文字でも空白類でもない場合に読み進める）
         */

        while((*buffer != '\0') && (*buffer != ' ') && (*buffer != '\t')) {
            ++buffer;
        }
    }

    /*
     *  最後の引数の次にはヌルへのポインタを格納する
     */

    args[arg_index] = NULL;

    /*
     *  最後の引数をチェックして "&" ならば
     *
     *  "&" を引数から削る
     *  コマンドの状態を表す status に 1 を設定する
     *
     *  そうでなければ status に 0 を設定する
     */

    if(arg_index > 0 && strcmp(args[arg_index - 1], "&") == 0) {

        --arg_index;
        args[arg_index] = NULL;
        status = 1;

    } else {

        status = 0;

    }

    /*
     *  引数が何もなかった場合
     */

    if(arg_index == 0) {
        status = 3;
    }

    /*
     *  コマンドの状態を返す
     */

    return status;
}

void push(stack** stack_root, char* str) {
  stack* new_stack;
  new_stack = (stack*)malloc(sizeof(stack));
    if(new_stack == NULL)
   {
    printf("ERROR\n");
    exit(-1);
   }
   new_stack->data =(char*)malloc(strlen(str)+1);
   if(new_stack->data == NULL)
   {
    printf("ERROR\n");
    exit(-1);
   }
   strcpy(new_stack->data, str);
   new_stack->next = NULL;
  if(*stack_root==NULL)
  {
   *stack_root = new_stack;
  }
  else
  {
      new_stack->next =*stack_root ;
      *stack_root = new_stack;
  }
  return;
}

char* pop(stack **stack_root){
  char *directory;
  stack *next, *fr;
    next = (stack*)malloc(sizeof(stack));
  if(next == NULL){
    printf("ERROR\n");
    exit(-1);
  }
  if(*stack_root == NULL){
    return NULL;
  }
  directory = (*stack_root) -> data;
  next = (*stack_root) -> next;
  fr = *stack_root;
  *stack_root = next;
  free(fr);
  return directory;
}

void print_stack(stack **stack_root) {
    stack *selectNode = *stack_root;
    printf("Directory stack = ");
    while(selectNode != NULL) {
        printf("%s",selectNode->data);
        selectNode = selectNode->next;
        if( selectNode != NULL ) {
            printf(", ");
        }
    }
    printf("\n");
    return;
}

void pre_register_log(hisotry** log_root,char* args[]){
    int i=1;
    char command[100];
    command[0]='\0';
    strcpy(command, args[0]);
    while (args[i]!=NULL) {
          strcat(command," ");
          strcat(command, args[i]);
        if(args[i+1]==NULL)break;
        i++;
      }
      register_log(log_root,command);
}

void register_log(hisotry** log_root, char* str) {
  hisotry* new_history;
  hisotry* selectNode = *log_root;
  new_history = (hisotry*)malloc(sizeof(hisotry));
    if(new_history == NULL)
   {
    printf("ERROR\n");
    exit(-1);
   }
   new_history->data =(char*)malloc(strlen(str)+1);
   if(new_history->data == NULL)
   {
    printf("ERROR\n");
    exit(-1);
   }
   strcpy(new_history->data, str);
   new_history->next = NULL;

  if(*log_root==NULL)
  {
   *log_root = new_history;
  }

  else
  {
      int count=1;
      while (selectNode->next != NULL) {
            count++;
            selectNode = selectNode->next;
        }
        if(count==32){
          printf("historyの数が上限です\n");
        }
        else{
      selectNode->next = new_history;
      new_history->next =NULL ;
        }
  }
  return;
}

void print_log(hisotry **log_root) {
    hisotry *selectNode = *log_root;
    int i=1;
    while(selectNode != NULL) {
        printf("%d %s\n",i,selectNode->data);
        selectNode = selectNode->next;
        i++;
    }
    return;
}

char* latest_log(hisotry **log_root) {
    hisotry *selectNode = *log_root;
    if(selectNode==NULL){
        printf("historyが空です\n");
        return NULL;
    }
    hisotry *p = NULL;
    while(selectNode != NULL) {
        p=selectNode;
        selectNode = selectNode->next;
    }
    return p->data;
}
char* search_log(hisotry **log_root,char*name) {
    hisotry *selectNode = *log_root;
    hisotry *p = NULL;
    char *str = NULL;
    while(selectNode != NULL) {
        p=selectNode;
        if(p->data[0]==*(name+0)){
            if(strstr(p->data,name)!=NULL){
            str=strstr(p->data,name);
            }}
        selectNode = selectNode->next;
        if (p==NULL)return NULL;
    }
    return str;
}

void register_ali(ali** ali_root, char* ali_str,const char* original_str) {
    if(strcmp(ali_str,"alias")==0){
        printf("aliasは引数1として指定できません\n");
        return;
    }
  ali* new_ali;
  ali* selectNode = *ali_root;
  new_ali = (ali*)malloc(sizeof(ali));
    if(new_ali == NULL)
   {
    printf("ERROR\n");
    exit(-1);
   }
   new_ali->original =(char*)malloc(strlen(original_str)+1);
   strcpy(new_ali->original, original_str);
   new_ali->ali_name =(char*)malloc(strlen(ali_str)+1);
   if(new_ali->ali_name == NULL)
   {
    printf("ERROR\n");
    exit(-1);
   }
   strcpy(new_ali->ali_name, ali_str);
   new_ali->next = NULL;

  if(*ali_root==NULL)
  {
   *ali_root = new_ali;
  }

  else
  {
      while (selectNode->next != NULL) selectNode = selectNode->next;
      selectNode->next = new_ali;
      new_ali->next =NULL ;
  }
  return;
}

void print_ali(ali **ali_root) {
    ali *selectNode = *ali_root;
    if (ali_root==NULL){
        printf("aliasで設定されているコマンドはありません\n");
        return;}
    while(selectNode != NULL) {
        printf("%s %s\n",selectNode->ali_name,selectNode->original);
        selectNode = selectNode->next;
    }
    return;
}

char* search_ali(ali **ali_root, char*name) {
    ali *selectNode = *ali_root;
    if(ali_root==NULL)return NULL;
    char *str = NULL;
    while(selectNode != NULL) {
        if(strcmp(selectNode->ali_name,name)==0){
            str=selectNode->original;
            return str;
        }
        selectNode = selectNode->next;
    }
    return "\0";
}

void del_ali(ali** ali_root, char* ali_str){
    ali *selectNode = *ali_root;
    
    if(ali_root==NULL)return;
    while(selectNode != NULL) {
        if(strcmp(selectNode->ali_name,ali_str)==0){
            selectNode->ali_name=selectNode->original;
        }
        selectNode = selectNode->next;
    }
    return;
}
/*----------------------------------------------------------------------------
 *
 *  関数名   : execute_command
 *
 *  作業内容 : 引数として与えられたコマンドを実行する
 *             コマンドの状態がフォアグラウンドならば、コマンドを
 *             実行している子プロセスの終了を待つ
 *             バックグラウンドならば子プロセスの終了を待たずに
 *             main 関数に返る（プロンプト表示に戻る）
 *
 *  引数     :
 *
 *  返り値   :
 *
 *  注意     :
 *
 *--------------------------------------------------------------------------*/
void execute_command(char *args[],    /* 引数の配列 */
                     int command_status,stack **stack_root,
                     hisotry** log_root,ali** ali_root, char**prompt_name)     /* コマンドの状態 */
{
    char *pcommand1="cd";
    char command1[30];
    command1[0]='\0';
    strcpy(command1,pcommand1);
    
    char *pcommand2="pushd";
    char command2[30];
    command2[0]='\0';
    strcpy(command2,pcommand2);
    
    char *pcommand3="dirs";
    char command3[30];
    command3[0]='\0';
    strcpy(command3,pcommand3);
    
    char *pcommand4="popd";
    char command4[30];
    command4[0]='\0';
    strcpy(command4,pcommand4);
    
    char *pcommand5="history";
    char command5[30];
    command5[0]='\0';
    strcpy(command5,pcommand5);
    
    char *pcommand6="prompt";
    char command6[30];
    command6[0]='\0';
    strcpy(command6,pcommand6);
    
    char *pcommand7="alias";
    char command7[30];
    command7[0]='\0';
    strcpy(command7,pcommand7);
    
    char *pcommand8="unalias";
    char command8[30];
    command8[0]='\0';
    strcpy(command8,pcommand8);
    
    char *pcommand9="sum";
    char command9[30];
    command9[0]='\0';
    strcpy(command9,pcommand9);
    
    
    char pathname[PATHNAME_SIZE];  // ファイルパス
    memset(pathname, '\0', PATHNAME_SIZE);
 
   
hisotry:
    //cdここから
    if(strcmp(args[0],command1)==0||
       strcmp(search_ali(ali_root, args[0]),command1)==0)
    {
        pre_register_log(log_root,args);
    // カレントディレクトリ取得
    getcwd(pathname, PATHNAME_SIZE);
    fprintf(stdout,"変更前のファイルパス:%s\n", pathname);

      if (args[1]==NULL)
       {
           
    // カレントディレクトリ変更
       char* str = getenv( "HOME");
       if( str == NULL ){
        fputs( "環境変数の取得に失敗しました。\n", stderr );
        exit(1);
       }
       chdir(str); // チェンジディレクトリ
       getcwd(str, PATHNAME_SIZE);
       fprintf(stdout,"現在のファイルパス:%s\n", str);
           return;
      }
      else
      {
        // カレントディレクトリ変更
       if(chdir(args[1])==-1)
       {
        printf("No such file or directory\n");
       }
       else
       {
        getcwd(pathname, PATHNAME_SIZE);
        fprintf(stdout,"現在のファイルパス:%s\n", pathname);
       }
      }
        strcat(command1," ");
        strcat(command1,args[1]);
        return;;
    }
      // cdここまで

      //pushd
    else if(strcmp(args[0],command2)==0||
            strcmp(search_ali(ali_root, args[0]),command2)==0)
    {
        pre_register_log(log_root,args);
        getcwd(pathname, PATHNAME_SIZE);
        char *str=pathname;
        push(stack_root,str);
    }
      //pushdここまで

      //dirs
     else if(strcmp(args[0],command3)==0||
             strcmp(search_ali(ali_root, args[0]),command3)==0)
     {
        pre_register_log(log_root,args);
        print_stack(stack_root);
     }
     //dirsここまで
     
     //popd
     else if(strcmp(args[0],command4)==0||
             strcmp(search_ali(ali_root, args[0]),command4)==0)
     {
      pre_register_log(log_root,args);
      char *str;
      str=pop(stack_root);
         if(str==NULL){
             printf("ディレクトリスタックが空です\n");
             return;
         }
      chdir(str);
      getcwd(str, PATHNAME_SIZE);
      fprintf(stdout,"現在のファイルパス:%s\n", str);
     }
     //popdここまで

     //histotry
     else if(strcmp(args[0],command5)==0||
             strcmp(search_ali(ali_root, args[0]),command5)==0){
        print_log(log_root);
        register_log(log_root,command5);
     }
    // !系コマンド

     else if (args[0][0]=='!'){
       if (args[0][1]=='!'){
           if( latest_log(log_root)==NULL) return;
        strcpy(args[0],latest_log(log_root));
        int j=0;
         char *p1,*p2;
         p1 =(char*)malloc(strlen(args[0])+1);
         if(p1 == NULL)
          {
              printf("ERROR\n");
              exit(-1);
          }
         strcpy(p1,args[0]);
         p2 = strtok(p1, " ");
         while (p2)
                 {
                     if(j!=0){
                         args[j] =(char*)malloc(strlen(args[0])+1);
                         if(args[j] == NULL)
                              {
                              printf("ERROR\n");
                              exit(-1);
                              }
                     }
                         strcpy(args[j],p2);
                         p2 = strtok(NULL, " ");
                     j++;
                 }
        goto hisotry;
        }

       if (*(args[0]+1)=='\0'){
         printf("!の後に文字を続けてください\n");
         return;
        }
        int i=1;
         char tmp[10];
        while(1) {
            if(i==1){
                strcpy(tmp,&args[0][i]);
                i++;}
            else{
            strcat(tmp,&args[0][i]);
             i++;
            if (*(args[0]+i)=='\0') break;
          }
        }
         
         if( search_log(log_root,tmp)==NULL) {
             printf("該当するコマンドがありません\n");
             return;}
           strcpy(args[0],search_log(log_root,tmp));
         //複数行ある場合args[0]に全て入ってしまっている
         int j=0;
         char *p1,*p2;
         p1 =(char*)malloc(strlen(args[0])+1);
         if(p1 == NULL)
          {
              printf("ERROR\n");
              exit(-1);
          }
         strcpy(p1,args[0]);
         p2 = strtok(p1, " ");
         while (p2)
                 {
                     if(j!=0){
                         args[j] =(char*)malloc(strlen(args[0])+1);
                         if(args[j] == NULL)
                              {
                              printf("ERROR\n");
                              exit(-1);
                              }
                     }
                         strcpy(args[j],p2);
                         p2 = strtok(NULL, " ");
                     j++;
                 }
           goto hisotry;
     }
    //!系コマンドここまで
    
    //prompt
     else if (strcmp(args[0],command6)==0||
              trcmp(search_ali(ali_root, args[0]),command6)==0){
         pre_register_log(log_root,args);
         if (args[1]!=NULL){
             char *temp=args[1];
             strcpy(*prompt_name,temp);
             return;
         }
         else{
             char original_name[]="Command";
             char *init_name=original_name;
             strcpy(*prompt_name,init_name);
             return;
         }
        //promptここまで
     }
     //alias
     else if (strcmp(args[0],command7)==0||
              strcmp(search_ali(ali_root, args[0]),command7)==0){
         pre_register_log(log_root,args);
         if(args[1]==NULL){
             print_ali(ali_root);
             return;
         }
         else {
             if(args[2]==NULL) {
                 printf("alias 引数1 引数2 と書いてください\n");
                 return;
             }
             register_ali(ali_root,args[1],args[2]);
             return;
         }
     }
    //aliasここまで
    
    //unailias
     else if (strcmp(args[0],command8)==0||
              strcmp(search_ali(ali_root, args[0]),command8)==0){
         pre_register_log(log_root,args);
         if(args[1]==NULL){
             printf("unalias 引数1と書いてください\n");
             return;
         }
         del_ali(ali_root,args[1]);
         return;
     }
    //unailiasここまで

    //sumここから
     else if(strcmp(args[0],command9)==0||
             strcmp(search_ali(ali_root, args[0]),command9)==0){
         pre_register_log(log_root,args);
         int sum = 0;
         if(args[1]==NULL||args[2]==NULL){
             printf("引数は二つ以上指定してください\n");
             return;
         }
         for (int i=1; args[i]!=NULL; i++) {
            sum += atoi(args[i]);
          }
        
         printf("%d\n",sum);
         return;
     }
    //sumここまで
    
      //自作コマンド以外
      else
      {
        pre_register_log(log_root,args);
        int i=1;
        char command[100];
        command[0]='\0';
        strcpy(command, args[0]);
        char *ali_command,*temp;
        ali_command=(char*)malloc(strlen(search_ali(ali_root, args[0]))+1);
        if(ali_command == NULL)
        {
          printf("ERROR\n");
          exit(-1);
        }
        temp=(char*)malloc(strlen(command)+1);
         if(temp == NULL)
        {
          printf("ERROR\n");
          exit(-1);
        }
        strcpy(ali_command,search_ali(ali_root, args[0]));
          
          if( strcmp(ali_command,"\0")!=0) strcpy(command, ali_command);
          
          while (args[i]!=NULL) {
              strcat(command," ");
              //ここに*の処理
              if(strcmp(args[i],"*")==0){
                  struct stat    filestat;
                  struct dirent *directory;
                  DIR           *dp;
                     dp = opendir(".");
                        while((directory=readdir(dp))!=NULL){
                          if(!strcmp(directory->d_name, ".") ||
                             !strcmp(directory->d_name, ".."))
                            continue;
                          if(stat(directory->d_name,&filestat)){
                            perror("main");
                            exit(1);
                          }else{
                              strcpy(args[i],directory->d_name);
                              strcpy(temp,command);
                              strcat(command, args[i]);
                              system(command);
                              strcpy(command,temp);
                          }
                        }
                        closedir(dp);
                  return;
              }
              strcat(command, args[i]);
            if(args[i+1]==NULL)break;
            i++;
          }
          system(command);
          return;
    }
    return;
}

/*-- END OF FILE -----------------------------------------------------------*/
