#ifndef BAODAEMON_H_
#define BAODAEMON_H_

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <syslog.h>
#include <systemd/sd-daemon.h>
#include <signal.h>

#define BAODAEMON_IN			"/baodaemon_request"
#define D_DEFAULT_FILE			D_DEFAULT_FOLDER "/audio/default.wav"
#define D_DEFAULT_AUDIO_FOLDER	D_DEFAULT_FOLDER "/audio"
#define D_DEFAULT_FOLDER		"/home/tuanbao/baodaemon"
#define MAX_SIZE				1024
#define INVALID_RET				-1
#define	D_LOG_FILE				D_DEFAULT_FOLDER "/baodaemon_logs.txt"

typedef struct {
	int message;
	pid_t requester_pid;
	char content[MAX_SIZE];
}baodaemon_message;

typedef enum {
	OPEN_BAO_REQ 		= 1,
	PLAY_DEFAULT_FILE	= 2,
	PLAY_FILE_WITH_NAME	= 3,
	PLAY_FILE_WITH_PATH	= 4,
	CLOSE_BAO_REQ		= 5,
}BAODAEMON_MESSAGE_ID;

static void bao_sig_handler(int signal);

#endif //!BAODAEMON_H_