#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PROCFS_THRESHOLD "/proc/battery_threshold"
#define PROCFS_NOTIFY_PID "/proc/battery_notify"

static void sig_usr(int signo)
{
	if(signo == SIGUSR1)
	{
		printf("절전모드\n");
	}
	else if(signo == SIGUSR2)
	{
		printf("일반모드\n");
	}
	return;
}

int main(int argc, char *argv[])
{
	int proc_fd;
	int thresh, pid;
	char buffer[1024];
	sigset_t newmask, oldmask, pendmask, zeromask;
	struct sigaction usr1_act, usr2_act, usr1_oact, usr2_oact;

	pid = getpid();
	if(argc == 1)
		thresh = 30;
	else if(argc > 1)
		thresh = atoi(argv[1]);
	printf("process pid, threshold : %d,  %d\n", pid, thresh);

	// Notyfy process pid and threshold of battery level
	proc_fd = open(PROCFS_THRESHOLD, O_WRONLY | O_NDELAY);
	sprintf(buffer, "%d\n", thresh);
	write(proc_fd, buffer, sizeof(buffer));
	close(proc_fd);
	
	proc_fd = open(PROCFS_NOTIFY_PID, O_WRONLY | O_NDELAY);
	sprintf(buffer, "%d\n", pid);
	write(proc_fd, buffer, sizeof(buffer));
	close(proc_fd);

	// Set up for sigaction
	usr1_act.sa_handler = sig_usr;
	sigemptyset(&usr1_act.sa_mask);
	usr1_act.sa_flags = 0;
	usr1_act.sa_flags |= SA_RESTART; 		/*SVR4, 4.3+BSD*/
	sigaction(SIGUSR1, &usr1_act, &usr1_oact);

	usr2_act.sa_handler = sig_usr;
	sigemptyset(&usr2_act.sa_mask);
	usr2_act.sa_flags = 0;
	usr2_act.sa_flags |= SA_RESTART; 		/*SVR4, 4.3+BSD*/
	sigaction(SIGUSR2, &usr2_act, &usr2_oact);

	// Set up for sigpending
	sigemptyset(&newmask);
	sigemptyset(&zeromask);
	sigaddset(&newmask, SIGINT);
	sigaddset(&newmask, SIGSTOP);
	sigaddset(&newmask, SIGQUIT);

	/* block SIGINT, SIGSTOP, SIGQUIT and save current signal mask */
	if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
	{
		printf("SIG_BLOCK error\n");
	}

	printf("blocked SIGINT, SIGSTOP, SIGQUIT success\n\n");
	while(1)
	{
		/*
		if (sigpending(&pendmask) < 0)
		{
			printf("sigpending call error\n");
		}
		if (sigismember(&pendmask, SIGINT))
		{
			printf("Received : SIGINT\n");
			//sigdelset(&pendmask, SIGINT);
			//sigsuspend(&zeromask);
		}
		if (sigismember(&pendmask, SIGSTOP))
		{
			printf("Received : SIGSTOP\n");
			//sigdelset(&pendmask, SIGSTOP);
		}
		if (sigismember(&pendmask, SIGQUIT))
		{
			printf("Received : SIGQUIT\n");
			//sigdelset(&pendmask, SIGQUIT);
		}
		*/
	}
	/* reset signal mask which unblocks SIGQUIT */
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
	{
		printf("SIG_SETMASK error");
	}
	printf("unblocked\n");
	return 0;
}
