#include "baodaemon.h"

pid_t bao_pid = 0;
FILE * fd_logs = NULL;

int main(int argc, char *argv[])
{	
	sd_notify(0, "READY=1");
	
	fd_set readfds;
	struct timeval timeout = {5, 0}; // timer set as 5 secs
	mqd_t mq_baodaemon_request;
	baodaemon_message msg;
	char filepath[MAX_SIZE];
	char log_line[MAX_SIZE];
	int mq_ret = -1, select_ret = 0;
	struct sigaction sa;
	
	bao_pid = getpid();
		
	sa.sa_handler = bao_sig_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;	
	if (sigaction(SIGTERM, &sa, NULL))
	{
		printf("[BAO.%d][%s] Failed to setup SIGTERM handler\n", bao_pid, __FUNCTION__);
		exit(-1);
	}
	
/*	
	struct stat st = {0};

	if (stat(D_DEFAULT_FOLDER, &st) == INVALID_RET) {
		if (INVALID_RET == mkdir(D_DEFAULT_FOLDER, 0700))
		{
			printf("[BAO.%d][%s] folder create failed. With errno = %d\n", bao_pid, __FUNCTION__, errno);
			return EXIT_FAILURE;
		}
	}
	
	if (INVALID_RET == access(D_LOG_FILE, F_OK))
	{
		if (INVALID_RET == open(D_LOG_FILE, O_CREAT|O_RDWR))
		{
			printf("[BAO.%d][%s] log file create failed. With errno = %d\n", bao_pid, __FUNCTION__, errno);
			return EXIT_FAILURE;
		}		
	}
*/	

	fd_logs = freopen(D_LOG_FILE, "a+", stdout);
//	fd_logs = fopen(D_LOG_FILE, "a+");
	if (NULL == fd_logs)
	{
		freopen("/dev/tty", "w", stdout);
		printf("[BAO.%d][%s] log file %s open failed. With errno = %d\n", bao_pid, __FUNCTION__, D_LOG_FILE, errno);
		return EXIT_FAILURE;
	}
	
	//pthread_t tid = pthread_self();
	//pthread_setname_np(tid, "baodaemon");
	
	struct mq_attr pp_mq_attr;
	pp_mq_attr.mq_maxmsg = 10;//MAX_SIZE;
	pp_mq_attr.mq_msgsize = (int)sizeof(baodaemon_message);

	mq_baodaemon_request = mq_open(BAODAEMON_IN, O_RDWR|O_CREAT|O_EXCL, 0666, &pp_mq_attr);
	if (mq_baodaemon_request == INVALID_RET)
	{
		printf("[BAO.%d][%s] mqueue %s already exist. Retry to open it...\n", bao_pid, __FUNCTION__, BAODAEMON_IN);
		if (EEXIST == errno)
		{
			if (mq_baodaemon_request = mq_open(BAODAEMON_IN, O_RDWR) == INVALID_RET)
			{
				printf("[BAO.%d][%s] mq open failed. %s\n", bao_pid, __FUNCTION__, strerror(errno));
				//sprintf(log_line, "[BAO.%d][%s] BaoDaemon open failed. With errno = %d\n", bao_pid, __FUNCTION__, errno);
				//fputs(log_line, fd_logs);
				return EXIT_FAILURE;
			}
		}
	}
	
	printf("Message ID:\n5\tCLOSE_BAO_REQ\t\t\t1\tOPEN_BAO_REQ\n2\tPLAY_DEFAULT_FILE\t\t3\tPLAY_FILE_WITH_NAME\n4\tPLAY_FILE_WITH_PATH\n\nWaiting for message...\n");
	fflush(fd_logs);

	while(1)
	{
		sd_notify(0, "WATCHDOG=1");
		
		// wait for request
		FD_ZERO(&readfds);
		FD_SET((int)mq_baodaemon_request, &readfds);

		timeout = (struct timeval) {5, 0};
		select_ret = select((int)mq_baodaemon_request+1, &readfds, NULL, NULL, &timeout);

		if ( select_ret <= 0 ) {
			continue;
		}

		if ( FD_ISSET(mq_baodaemon_request, &readfds) ) {
			mq_ret = mq_receive(mq_baodaemon_request, (char*)&msg, sizeof(baodaemon_message), NULL);
			if ( INVALID_RET == mq_ret)
			{
				printf("[BAO.%d][%s] mq receive failed. %s\n", bao_pid, __FUNCTION__, strerror(errno));
			}
			else if ( mq_ret > 0 )
			{				
				printf("[BAO.%d][%s] Received message ID = %d from PID %d\n", bao_pid, __FUNCTION__, msg.message, (int) msg.requester_pid);
				fflush(fd_logs);
				// sprintf(log_line, "[BAO.%d][%s] Received message ID = %d\n", bao_pid, __FUNCTION__, msg.message);
				// fputs(log_line, fd_logs);
				if (CLOSE_BAO_REQ == msg.message)
				{
					break;
				}
				switch (msg.message)
				{
					case PLAY_DEFAULT_FILE:
						sprintf(filepath, "%s", D_DEFAULT_FILE);
						//system(filepath);
						printf("[BAO.%d][%s] BaoDaemon is playing %s.\n", bao_pid, __FUNCTION__, filepath);
						fflush(fd_logs);
						sleep(1);
					break;
					case PLAY_FILE_WITH_NAME:
						sprintf(filepath, "%s%s%s", D_DEFAULT_AUDIO_FOLDER, "/", msg.content);
						//system(filepath);
						if (INVALID_RET == access(filepath, F_OK))
						{
							printf("[BAO.%d][%s] BaoDaemon couldn't play %s. %s\n", bao_pid, __FUNCTION__, filepath, strerror(ENOENT));
						}
						else
						{
							printf("[BAO.%d][%s] BaoDaemon is playing %s.\n", bao_pid, __FUNCTION__, filepath);
						}
						fflush(fd_logs);
						sleep(5);
					break;
					case PLAY_FILE_WITH_PATH:
						sprintf(filepath, "%s", msg.content);
						//system(filepath);
						if (INVALID_RET == access(filepath, F_OK))
						{
							printf("[BAO.%d][%s] BaoDaemon couldn't play %s. %s\n", bao_pid, __FUNCTION__, filepath, strerror(ENOENT));
						}
						else
						{
							printf("[BAO.%d][%s] BaoDaemon is playing %s.\n", bao_pid, __FUNCTION__, filepath);
						}
						fflush(fd_logs);
						sleep(10);
					break;
					default:
						//nothing
					break;
				}
			}
		}
		
	}

	printf("[BAO.%d][%s] BaoDaemon is terminated~~\n", bao_pid, __FUNCTION__);
	fflush(fd_logs);
	// sprintf(log_line, "[BAO.%d][%s] BaoDaemon is terminated~~\n", bao_pid, __FUNCTION__);
	// fputs(log_line, fd_logs);
	mq_unlink(BAODAEMON_IN);
	
	freopen("/dev/tty", "w", stdout);
	fclose(fd_logs);

	return EXIT_SUCCESS;
}

static void bao_sig_handler(int signal)
{
	if ( (signal == SIGINT) || (signal == SIGTERM) || (signal == SIGTSTP) )
	{
		printf("[BAO.%d][%s] BaoDaemon is terminated~~\n", bao_pid, __FUNCTION__);
		fflush(fd_logs);
		
		mq_unlink(BAODAEMON_IN);
	}
}