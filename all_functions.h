//structs

typedef struct Node{
    char* key;
    struct Node*next;
    struct Node*prev;
}Node;

typedef struct {
    Node* node;
    int size;
    int capacity;
} Log;

typedef struct {
    char name[50];
    char command[4096];
} Alias;
Alias alias_list[50];

typedef struct Process {
    pid_t pid;           
    char command[4096];     
} Process;


//main.c

extern int num_aliases;
extern int cont;
extern int add_time_to_prompt;
extern char *prev_dir;
int execute_command(char *command, char *prev_dir, Log* log) ;
void add_new_alias(char *line);
char *replace_alias(char *command);
void load_config_file();

//display.c

char* get_username(uid_t uid);
char* get_sysname();
void slice(char* str, char* result, size_t start, size_t end);
int is_substring(char* str1, char* str2);
char* get_curr_directory();
char* create_prompt(char* username, char* hostname, int flag, char* time_info);
void print_red(char* str);

//input.c

int token_count(char* str);
char* remove_extra_spaces_and_tabs(char input[4096]);
char* remove_spaces_and_tabs_before_str(char* str);
char** input_to_tokens(char* input, int num_tokens);
char** parse_input(char* input, int num_tokens);
int find_num_non_empty_strings(char** commands, int num_tokens);
char** remove_empty_strings(char** commands, int num_tokens, int num_non_empty_strings);

//hop.c

char* hop(char* next_dir);
void hop_controller(char* command, char* prev_dir);

//reveal.c
extern int total_blocks;
int find_num_files(char* dir, int flag);
char** get_filenames(char* dir, int flag);
int is_dir(char* obj);
int is_lexicographically_smaller_than(char* str1, char* str2);
void merge_sort_filenames(char** filenames, int num_files);
char* get_actual_path(char* relative_path, char* path);
void print_color_coded_name(char* filepath, char* filename, int flag);
void print_file_info(char* filepath, char* filename);
void reveal(char *input, char* prev_dir);

//log.c

Node* PushFront(char* c,Node*L);
Node* PopBack(Node*L);
void print(Node* L);
void free_linked_list(Node* L);

Log* init_log();
void load_log_from_file(Log* log);
void add_to_log(Log *log, char *command);
int log_controller(char* command, Log* log, char* prev_dir);
void save_log_to_file(Log* log); 
int log_execute(Log* log, int index, char* prev_dir);

//sys_commands.c

int execute_process(char* command, int background);
void handle_sigchild(int sig);

//proclore.c

void get_process_info(pid_t pid);
void proclore_controller(char* command);

//seek.c

extern char **file_paths; 
extern int file_count;
int get_file_paths(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);
int is_prefix(char* str1, char* str2);
int find_num_fwd_slashes(char* path);
char* get_name_from_path(char* path);
void print_file_contents(char *filename);
int has_perms(char* path, int isdir);
void seek_controller(char* command, char* prev_dir);

//io_redirection.c

char* find_substring_before_char(char* str, char c);
void process_io_command(char* command, char* prev_dir, Log* log, int p);

//pipes.c

int find_num_pipes(char* command);
void pipe_handler(char* command, char* prev_dir, Log* log);

//activities.c
extern int fg_proc_count;
Process proc_list[100];
Process fg_proc_list[100];
extern int proc_count;
void add_to_proc_list(char* cname, pid_t f);
void add_to_fg_proc_list(char* cname, pid_t f);
int compare_by_pid(const void *a, const void *b);
void get_proc_info(pid_t pid);
void activities_handler();

//signals.c

void send_signal(pid_t pid, int signal);
void interrupt(int sig);
void stop(int sig);
void kill_all();
void ping(char* command);

//fg_bg.c

void bring_to_fg(pid_t pid, int pgid);
void bring_to_bg(pid_t pid);
void fg_bg_handler(char* command);

//iMan.c

void iMan(char *command_name);
void iMan_handler(char* command);



