#ifndef PROMPTPLAY_H_
#define PROMPTPLAY_H_

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

#define PROMPTPLAY_IN		"/promptplay_request"
#define D_DEFAULT_FILE		"/usr/share/promptplay/default.wav"
#define D_DEFAULT_FOLDER	"/usr/share/promptplay"
#define MAX_SIZE			1024
#define INVALID_RET			-1

typedef struct {
	int message;
	char content[MAX_SIZE];
}promptplay_message;

typedef enum {
	OPEN_PP_REQ 		= 1,
	PLAY_DEFAULT_FILE	= 2,
	PLAY_FILE_WITH_NAME	= 3,
	PLAY_FILE_WITH_PATH	= 4,
	CLOSE_PP_REQ		= 5,
}PROMPTPLAY_MESSAGE_ID;

#endif //!PROMPTPLAY_H_