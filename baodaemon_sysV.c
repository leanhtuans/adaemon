#include "baodaemon.h"

int main(void)
{        
	/* Our process ID and Session ID */
	pid_t pid, sid;
	
	/* Fork off the parent process */
	pid = fork();
	if (pid < 0) {
			exit(EXIT_FAILURE);
	}
	/* If we got a good PID, then
	   we can exit the parent process. */
	if (pid > 0) {
			exit(EXIT_SUCCESS);
	}

	/* Change the file mode mask */
	umask(0);
			
	/* Open any logs here */        
			
	/* Create a new SID for the child process */
	sid = setsid();
	if (sid < 0) {
			/* Log the failure */
			exit(EXIT_FAILURE);
	}
	

	
	/* Change the current working directory */
	if ((chdir("/")) < 0) {
			/* Log the failure */
			exit(EXIT_FAILURE);
	}
	
	/* Close out the standard file descriptors */
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	
	/* Daemon-specific initialization goes here */
	
	fd_set readfds;
	struct timeval timeout = {5, 0}; // timer set as 5 secs
	mqd_t mq_baodaemon_request;
	baodaemon_message msg;
	char syscommand[MAX_SIZE];
	char log_line[MAX_SIZE];
	int mq_ret = -1, select_ret = 0;
	
	FILE * fd_logs;
	
	fd_logs = freopen(D_LOG_FILE, "a+", stdout);
	if (NULL == fd_logs)
	{
		freopen("/dev/tty", "w", stdout);
		printf("[PP][%s] log file %s open failed. With errno = %d\n", __FUNCTION__, D_LOG_FILE, errno);
		return EXIT_FAILURE;
	}
			
	struct mq_attr pp_mq_attr;
	pp_mq_attr.mq_maxmsg = 10;//MAX_SIZE;
	pp_mq_attr.mq_msgsize = (int)sizeof(baodaemon_message);

	mq_baodaemon_request = mq_open(BAODAEMON_IN, O_RDWR|O_CREAT|O_EXCL, 0666, &pp_mq_attr);
	if (mq_baodaemon_request == INVALID_RET)
	{
		printf("[PP][%s] mq open failed. With errno = %d\n", __FUNCTION__, errno);
		if (EEXIST == errno)
		{
			if (mq_baodaemon_request = mq_open(BAODAEMON_IN, O_RDWR) == INVALID_RET)
			{					
				printf("[PP][%s] mq open failed. With errno = %d\n", __FUNCTION__, errno);
				sprintf(log_line, "[PP][%s] promptPlay open failed. With errno = %d\n", __FUNCTION__, errno);
				fputs(log_line, fd_logs);
				return EXIT_FAILURE;
			}
		}
	}
	
	printf("Message ID:\n5\tCLOSE_BAO_REQ\t\t\t1\tOPEN_BAO_REQ\n2\tPLAY_DEFAULT_FILE\t\t3\tPLAY_FILE_WITH_NAME\n4\tPLAY_FILE_WITH_PATH\n\nWaiting for message...\n");
	fflush(fd_logs);
	
	/* The Big Loop */
	while (1) {
	   /* Do some task here ... */
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
			if ( mq_ret > 0 )
			{				
				printf("[PP][%s] Received message ID = %d from PID %d\n", __FUNCTION__, msg.message, (int) msg.requester_pid);
				fflush(fd_logs);
				// sprintf(log_line, "[PP][%s] Received message ID = %d\n", __FUNCTION__, msg.message);
				// fputs(log_line, fd_logs);
				if (CLOSE_BAO_REQ == msg.message)
				{
					break;
				}
				switch (msg.message)
				{
					case PLAY_DEFAULT_FILE:
						sprintf(syscommand, "aplay %s", D_DEFAULT_FILE);
						system(syscommand);
					break;
					case PLAY_FILE_WITH_NAME:
						sprintf(syscommand, "aplay %s%s%s", D_DEFAULT_AUDIO_FOLDER, "/", msg.content);
						system(syscommand);
					break;
					case PLAY_FILE_WITH_PATH:
						sprintf(syscommand, "aplay %s", msg.content);
						system(syscommand);
					break;
					default:
						//nothing
					break;
				}
			}
		}			
	}
	
	printf("[PP][%s] promptPlay is terminated~~\n", __FUNCTION__);
	fflush(fd_logs);
	// sprintf(log_line, "[PP][%s] promptPlay is terminated~~\n", __FUNCTION__);
	// fputs(log_line, fd_logs);
	mq_unlink(BAODAEMON_IN);
	
	freopen("/dev/tty", "w", stdout);
	fclose(fd_logs);
	
	exit(EXIT_SUCCESS);
}