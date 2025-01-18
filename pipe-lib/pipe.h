//
// Created by Denis on 18. 1. 2025.
//

#ifndef PIPE_H
#define PIPE_H

void pipe_init(const char *path);
void pipe_destroy(const char *path);
int pipe_open_write(const char *path);
int pipe_open_read(const char *path);
void pipe_close(int fd);

#endif //PIPE_H
