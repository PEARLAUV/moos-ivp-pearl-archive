#ifndef CLEARPATH
#define CLEARPATH

// 0 on success, 1 on failure
int clearpath_init(const char* address, int port);

// 0 on success, 1 if socket has closed.
int clearpath_spin();

int clearpath_set_output(const double left, const double right);
double clearpath_get_voltage();
double clearpath_get_compass();

// Call this on program exit.
void clearpath_destroy();

#endif

//
