#include <stdio.h>
#include <signal.h>

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

int main(void)
{
	sigset_t newmask, oldmask, pendmask, zeromask;
	struct sigaction usr1_act, usr2_act, usr1_oact, usr2_oact;

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
	printf("blocked\n");
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
	exit(0);
}
