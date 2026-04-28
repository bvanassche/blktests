#define _GNU_SOURCE

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

/*
 * Usage: ./copy_file_range <src> <dest> <src_offset> <dest_offset> <len>
 */

int main(int argc, char *argv[])
{
	if (argc != 6) {
		fprintf(stderr,
			"Usage: %s <src> <dest> <src_off> <dest_off> <len>\n",
			argv[0]);
		return 1;
	}

	const char *src_path = argv[1];
	const char *dest_path = argv[2];
	loff_t src_off = atoll(argv[3]);
	loff_t dest_off = atoll(argv[4]);
	size_t len = (size_t)atoll(argv[5]);

	int fd_in = open(src_path, O_RDONLY | O_DIRECT);
	if (fd_in == -1) {
		perror("Error opening source");
		return 1;
	}

	int fd_out = open(dest_path, O_WRONLY | O_DIRECT);
	if (fd_out == -1) {
		perror("Error opening destination");
		goto close_in;
	}

	size_t total_copied = 0;
	while (len > 0) {
		ssize_t ret = copy_file_range(fd_in, &src_off, fd_out, &dest_off,
					      len, 0);
		if (ret == -1) {
			perror("copy_file_range() failed");
			goto close_out;
		}

		if (ret == 0) {
			printf("EOF reached unexpectedly.\n");
			goto close_out;
		}

		len -= ret;
		total_copied += ret;
	}

	printf("Done. Successfully copied %zu bytes.\n", total_copied);

	close(fd_out);
	close(fd_in);
	return 0;

close_out:
	close(fd_out);
close_in:
	close(fd_in);
	return 1;
}
