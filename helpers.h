void print_usage();
void rle_exit();
void get_current_time(); 
void log_debug(char *message);
void log_error(char *error_message);
void rle_free(void *ptr, char *pointer_name);
void * rle_malloc(size_t size, char *module);
int file_exists(char *filename);

FILE *log_file;