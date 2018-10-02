#include "promptplay.h"

int main(int argc, char *argv[])
{
	mqd_t mq_promptplay_request;
	promptplay_message msg;
	int mq_ret = -1, select_ret = 0;
	char temp[MAX_SIZE] = {'\0'};
	
	// pthread_t tid = pthread_self();
	// pthread_setname_np(tid, "promptplay_requester");

	mq_promptplay_request = mq_open(PROMPTPLAY_IN, O_WRONLY);
	if (mq_promptplay_request == INVALID_RET)
	{
		printf("[PP][%s] promptPlay_requester open failed. With errno = %d\n", __FUNCTION__, errno);
		return EXIT_FAILURE;
	}

		printf("Creating new message:\n5\tCLOSE_PP_REQ\t\t\t1\tOPEN_PP_REQ\n2\tPLAY_DEFAULT_FILE\t\t3\tPLAY_FILE_WITH_NAME\n4\tPLAY_FILE_WITH_PATH\n");
		
	while(1)
	{
		printf("Enter Message ID = ");
		if (fgets(temp, MAX_SIZE, stdin) != NULL)
		{			
			msg.message = atoi(temp);
			
			switch (msg.message)
			{
				case CLOSE_PP_REQ:
				case OPEN_PP_REQ:
				case PLAY_DEFAULT_FILE:
					memset(msg.content, '\0', MAX_SIZE);
					break;
				case PLAY_FILE_WITH_NAME:
					printf("Enter file name: ");
					if (fgets(temp, MAX_SIZE, stdin) != NULL)
					{
						temp[strlen(temp)-1] = '\0';
						stpcpy(msg.content, temp);
					}
					break;
				case PLAY_FILE_WITH_PATH:					
					printf("Enter file path: ");
					if (fgets(temp, MAX_SIZE, stdin) != NULL)
					{
						temp[strlen(temp)-1] = '\0';
						stpcpy(msg.content, temp);
					}
					break;
				default:
					break;
			}
			
			if (msg.message > CLOSE_PP_REQ)
			{
				continue;
			}
			
			mq_ret = mq_send(mq_promptplay_request, (char*)&msg, sizeof(promptplay_message), 1);
			if (mq_ret != INVALID_RET)
			{
				printf("[PP][%s] promptPlay_requester send message ID %d, content: %s\n", __FUNCTION__, msg.message, msg.content);
			}
			else
			{
				printf("[PP][%s] promptPlay_requester send failed. With errno = %d\n", __FUNCTION__, errno);
				return EXIT_FAILURE;
			}
			if (msg.message == CLOSE_PP_REQ)
			{
				break;
			}
		}
		
	}

	printf("[PP][%s] promptPlay_requester is terminated~~\n", __FUNCTION__);

	return EXIT_SUCCESS;
}