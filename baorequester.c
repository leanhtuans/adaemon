#include "baodaemon.h"

int main(int argc, char *argv[])
{
	mqd_t mq_baodaemon_request;
	baodaemon_message msg;
	int mq_ret = -1, select_ret = 0;
	char temp[MAX_SIZE] = {'\0'};
	
	msg.requester_pid = getpid();

	mq_baodaemon_request = mq_open(BAODAEMON_IN, O_WRONLY);
	if (mq_baodaemon_request == INVALID_RET)
	{
		printf("[PP][%s] BaoRequester open failed. With errno = %d\n", __FUNCTION__, errno);
		return EXIT_FAILURE;
	}

		printf("Creating new message:\n5\tCLOSE_BAO_REQ\t\t\t1\tOPEN_BAO_REQ\n2\tPLAY_DEFAULT_FILE\t\t3\tPLAY_FILE_WITH_NAME\n4\tPLAY_FILE_WITH_PATH\t\tE\tExit requester\n");
		
	while(1)
	{
		printf("Enter Message ID = ");
		if (fgets(temp, MAX_SIZE, stdin) != NULL)
		{
			if (!strcmp("E\n", temp))
			{
				printf("\nGood bye!!!\n");
				break;
			}
			
			msg.message = atoi(temp);
			
			switch (msg.message)
			{
				case CLOSE_BAO_REQ:
				case OPEN_BAO_REQ:
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
					continue;
			}			
			
			mq_ret = mq_send(mq_baodaemon_request, (char*)&msg, sizeof(baodaemon_message), 1);
			if (mq_ret != INVALID_RET)
			{
				printf("[PP][%s] BaoRequester send message ID %d, content: %s\n", __FUNCTION__, msg.message, msg.content);
			}
			else
			{
				printf("[PP][%s] BaoRequester send failed. With errno = %d\n", __FUNCTION__, errno);
				return EXIT_FAILURE;
			}
			if (msg.message == CLOSE_BAO_REQ)
			{
				break;
			}
		}
		
	}

	printf("[PP][%s] BaoRequester is terminated~~\n", __FUNCTION__);

	return EXIT_SUCCESS;
}