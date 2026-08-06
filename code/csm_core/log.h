#ifndef log_h_INCLUDED
#define log_h_INCLUDED

void log_msg(char* str);
void log_err(char* str);
void log_fail(char* str);

#ifdef CSM_IMPLEMENTATION

void log_msg(char* str) {
    printf("%s\n", str);
}

void log_err(char* str) {
    printf("%s\n", str);
}

void log_fail(char* str) {
    log_err(str);
    panic();
}

#endif // CSM_IMPLEMENTATION
#endif // log_h_INCLUDED
