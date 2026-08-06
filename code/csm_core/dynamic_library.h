#ifndef dynamic_library_h_INCLUDED
#define dynamic_library_h_INCLUDED

/* Usage

void* lib_handle;
char* lib_name = "filename_not_including_.so";
f64 lib_last_modified = 0;
bool lib_modified = dynamic_lib_update_library(lib_handle, lib_name, &lib_last_modified);

while(game_running) {
	if(dynamic_lib_update_library(lib_handle, lib_name, &lib_last_modified) {
		dynamic_lib_load_function(_function_pointer_1, "function_name_1");
		dynamic_lib_load_function(_function_pointer_2, "function_name_2");
		// ...
	}
}

 */

bool dynamic_lib_update_library(void** lib_handle, char* lib_name, f64* last_modified);
void* dynamic_lib_load_function(void* lib_handle, char* function_name);

#ifdef CSM_IMPLEMENTATION

bool dynamic_lib_update_library(void** lib_handle, char* lib_name, f64* last_modified) {
	char lib_filename[256];
	sprintf(lib_filename, "%s.so", lib_name);
	struct stat lib_stat;
	stat(lib_filename, &lib_stat);
	u64 actual_last_modified = lib_stat.st_mtim.tv_sec;
	if(actual_last_modified > *last_modified) {
		if(*last_modified != 0.0f) {
			dlclose(*lib_handle);
		}
		*last_modified = actual_last_modified;

		// Copy filename to earlier version
		char copy_fname[256];
		time_t t;
		time(&t);
		sprintf(copy_fname, "./%s_%li.so", lib_name, t);
		char cmd[2048];
		sprintf(cmd, "cp %s.so %s", lib_name, copy_fname);
		system(cmd);

		*lib_handle = dlopen(copy_fname, RTLD_NOW);
		char* err;
		if((err = dlerror()) != NULL) {
			fprintf(stderr, "dlerror: %s\n", err);
		}
		assert(*lib_handle != NULL);
		return true;
	}
	return false;
}

void* dynamic_lib_load_function(void* lib_handle, char* function_name) {
	void* ptr = dlsym(lib_handle, function_name);
	char* err;
	if((err = dlerror()) != NULL) {
		printf("dlerror: %s\n", err);
		panic();
	}
	return ptr;
}

#endif // CSM_IMPLEMENTATION
#endif // dynamic_library_h_INCLUDED
