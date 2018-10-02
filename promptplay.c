#include "promptplay.h"

int main(int argc, char *argv[])
{
	fd_set readfds;
	struct timeval timeout = {5, 0}; // timer set as 5 secs
	mqd_t mq_promptplay_request;
	promptplay_message msg;
	char commandPlayToneFile[MAX_SIZE];
	char PlayToneFilePath[MAX_SIZE];
	int mq_ret = -1, select_ret = 0;
	
	//pthread_t tid = pthread_self();
	//pthread_setname_np(tid, "promptplay");
	
	struct mq_attr pp_mq_attr;
	pp_mq_attr.mq_maxmsg = 10;//MAX_SIZE;
	pp_mq_attr.mq_msgsize = (int)sizeof(promptplay_message);

	mq_promptplay_request = mq_open(PROMPTPLAY_IN, O_RDWR|O_CREAT|O_EXCL, 0666, &pp_mq_attr);
	if (mq_promptplay_request == INVALID_RET)
	{
		if (EEXIST == errno)
		{
			if (mq_promptplay_request = mq_open(PROMPTPLAY_IN, O_RDWR) == INVALID_RET)
			{					
				printf("[PP][%s] promptPlay open failed. With errno = %d\n", __FUNCTION__, errno);
				return EXIT_FAILURE;
			}
		}
	}
	
	printf("Message ID:\n5\tCLOSE_PP_REQ\t\t\t1\tOPEN_PP_REQ\n2\tPLAY_DEFAULT_FILE\t\t3\tPLAY_FILE_WITH_NAME\n4\tPLAY_FILE_WITH_PATH\n\nWaiting for message...\n");

	while(1)
	{
		// wait for request
		FD_ZERO(&readfds);
		FD_SET((int)mq_promptplay_request, &readfds);

		timeout = (struct timeval) {5, 0};
		select_ret = select((int)mq_promptplay_request+1, &readfds, NULL, NULL, &timeout);

		if ( select_ret <= 0 ) {
			continue;
		}

		if ( FD_ISSET(mq_promptplay_request, &readfds) ) {
			mq_ret = mq_receive(mq_promptplay_request, (char*)&msg, sizeof(promptplay_message), NULL);
			if ( mq_ret > 0 )
			{				
				printf("[PP][%s] Received message ID = %d\n", __FUNCTION__, msg.message);
				if (CLOSE_PP_REQ == msg.message)
				{
					break;
				}
				switch (msg.message)
				{
					case PLAY_DEFAULT_FILE:
						sprintf(commandPlayToneFile, "aplay %s", D_DEFAULT_FILE);
						system(commandPlayToneFile);
					break;
					case PLAY_FILE_WITH_NAME:
						sprintf(commandPlayToneFile, "aplay %s%s%s", D_DEFAULT_FOLDER, "/", msg.content);
						system(commandPlayToneFile);
					break;
					case PLAY_FILE_WITH_PATH:
						sprintf(commandPlayToneFile, "aplay %s", msg.content);
						system(commandPlayToneFile);
					break;
					default:
						//nothing
					break;
				}
			}
		}
		
	}

	printf("[PP][%s] promptPlay is terminated~~\n", __FUNCTION__);
	mq_unlink(PROMPTPLAY_IN);

	return EXIT_SUCCESS;
}