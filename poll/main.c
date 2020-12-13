#include <stdio.h>
#include <sys/wait.h>
#include <poll.h>
#include <fcntl.h>

#define FREQUENT_FEATURES_SINGLE_FILE
#include "../frequent_features.h"

/**************************************************************
*                       Scheme of work                        *
***************************************************************
*                                                             *
*                 parent              parent                  *
*            [ ]--------->[ ]    [ ]--------->[ ]             *
*             ^            |      ^            |              *
*             |            |      |            |              *
*             |pipe        |pipe  |pipe        |              *
*             |            |      |            |              *
*    stdin    |            v      |            |    stdout    *
*   ------>[child0]        [child1]         [child3]------>   *
*                                                             *
**************************************************************/

int main(int argc, const char *argv[])
{
	int nchildren;
	
	if (argc != 2)
		error("incorrect usage: expected 1 parameter - number of child processes");
	if (sscanf(argv[1], "%d", &nchildren) != 1 || nchildren <= 0)
		error("incorrect usage: expected positive number - number of child processes");

	/*  fds to be polled.
	 *  from_child_fds[nchildren-1] is not a valid pipe (last child send output to stdout) */
	struct pollfd from_child_fds[nchildren];
	
	/*  Inputs of children pipes. Input is transmitted to the next child,
	 * i.e. if data is received from from_child_fds[i], it will be
	 * sent to to_child_fds[i + 1].
	 *  to_child_fds[0] is not a valid pipe (child #0 takes input from stdin)*/
	int to_child_fds[nchildren];

	/* creating children */
	for (int i = 0; i < nchildren; ++i) {
		int pipe_to_child[2];
		int pipe_from_child[2];

		if (i == 0) {
			pipe_to_child[0] = STDIN_FILENO;
			pipe_to_child[1] = -1;
		} else if (pipe(pipe_to_child) == -1)
			error("cannot create pipe: %s", strerror(errno));
	
		if (i == nchildren - 1) {
			pipe_from_child[0] = -1;
			pipe_from_child[1] = STDOUT_FILENO;
		} else if (pipe(pipe_from_child) == -1)
			error("cannot create pipe: %s", strerror(errno));

		from_child_fds[i].fd = pipe_from_child[0];
		to_child_fds[i] = pipe_to_child[1];

		PARALLEL({
			/* closing pipe dublicates */
			for (int j = 0; j <= i && j < nchildren - 1; ++j) // not closing last child - writes to stdout
				close(from_child_fds[j].fd);
			for (int j = 1; j <= i; ++j) // not closing first child - reads from stdin
				close(to_child_fds[j]);
			copyfile(pipe_to_child[0], pipe_from_child[1]); // copyfile must be after closing pipes, or it will block forever
			close(pipe_from_child[1]);
		});
		if (pipe_to_child[0] != STDIN_FILENO)
			close(pipe_to_child[0]);
		if (pipe_from_child[1] != STDOUT_FILENO)
			close(pipe_from_child[1]);
	}

	for (int i = 0; i < nchildren - 1; ++i) {
		from_child_fds[i].events = POLLIN;
		if (fcntl(from_child_fds[i].fd, F_SETFL, O_NONBLOCK) == -1)
			error("fcntl() error: %s", strerror(errno));
	}

	/* starting polling */
	int npolled = nchildren - 1; // number of not closed pipes
	while (npolled > 0) {
		if (poll(from_child_fds, nchildren, -1) == -1)
			error("poll() error: %s", strerror(errno));
		for (int i = 0; i < nchildren - 1; ++i) {
			if ((from_child_fds[i].revents & POLLIN)
				&& copyfile(from_child_fds[i].fd, STDOUT_FILENO) == -1
				&& errno != EAGAIN)
				error("read or write failed: %s", strerror(errno));
			if (from_child_fds[i].revents & POLLHUP) { // child closed his output pipe
				close(to_child_fds[i + 1]); //transmitting EOF to the next child
				--npolled;
				/* fd=0 cannot be ignored by negating fd value in pollfd.
				 * But fd!=0, because such fd is already used for stdin */
				assert(from_child_fds[i].fd != 0);
				from_child_fds[i].fd = -1; // ignore this fd next time. STDIN is n
			}
			if (from_child_fds[i].revents & POLLERR)
				error("poll() set POLLERR flag to fd=%d (child %d)", from_child_fds[i].fd, i);
			if (from_child_fds[i].revents & POLLNVAL)
				error("poll() set POLLNVAL flag to fd=%d (child %d)", from_child_fds[i].fd, i);
		}
	}

	/* every polled descriptor is closed, waiting for children */
	for (int i = 0; i < nchildren; ++i)
		wait(NULL);
	return 0;
}