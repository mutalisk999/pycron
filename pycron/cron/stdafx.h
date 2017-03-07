#if defined(_WIN32)
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <process.h>
#include <io.h>
#include <sys/utime.h>

#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

#else
#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>

#include <pthread.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <utime.h>
#endif

