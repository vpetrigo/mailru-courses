#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <inttypes.h>
#include <sys/types.h>
#include <fcntl.h>

#define LINE_SIZE 1024

struct command {
  char **argv;
};

char *check_pipe_sym(const char *line) {
  static const char *pipe_sym = "|";
  // do we have a pipe symbol in our input line
  return strpbrk(line, pipe_sym);
}

size_t skip_char(const char *str) {
  size_t count = 0;

  while (!isspace(*str++)) {
    ++count;
  }

  return count;
}

size_t skip_whitespace(const char *str) {
  size_t count = 0;

  while (isspace(*str++)) {
    ++count;
  }

  return count;
}

void free_commands(struct command *cmds, const size_t num_of_commands) {
  for (size_t i = 0; i < num_of_commands; ++i) {
    free(cmds[i].argv[0]);
    free(cmds[i].argv);
  }

  free(cmds);
}

void set_null(char **arg_arr, const size_t start_idx, const size_t end_idx) {
  if (end_idx <= start_idx) {
    return;
  }

  for (size_t i = start_idx; i < end_idx; ++i) {
    arg_arr[i] = NULL;
  }
}

size_t count_commands(const char *input_line, size_t input_len, size_t *delim_positions) {
  size_t commands_cnt = 0;
  size_t dp_idx = 1;
  char *delim_pos = NULL;
  ptrdiff_t cmd_size = 0;

  while ((delim_pos = check_pipe_sym(input_line + cmd_size + 1)) != NULL) {
    cmd_size = delim_pos - input_line;
    delim_positions[dp_idx++] = cmd_size;
    ++commands_cnt;
  }

  delim_positions[dp_idx++] = input_len;
  ++commands_cnt;

  return commands_cnt;
}

struct command get_argv(const char *line, size_t len) {
  struct command cmd;
  cmd.argv = calloc(len, sizeof(char *));
  cmd.argv[0] = calloc(len * len, sizeof(char));

  for (size_t i = 1; i < len; ++i) {
    cmd.argv[i] = cmd.argv[i - 1] + len;
  }

  size_t prev_pos = 0;
//  size_t cur_pos = 0;
  size_t arr_idx = 0;
  size_t cnt = 0;
  const char *begin = line;
  const char *ptr = begin;

  while (isspace(*begin)) {
    ++begin;
  }

  ptr = begin;

  while (*ptr && *ptr != '|') {
    cnt = skip_char(ptr);
    strncpy(cmd.argv[arr_idx], begin + prev_pos, cnt);
    cnt += skip_whitespace(ptr + cnt);
    // start from the next character after a space
    prev_pos = cnt;
    ptr += cnt;
    ++arr_idx;
  }

  set_null(cmd.argv, arr_idx, len);

  return cmd;
}

struct command *parse_commands(const char *input_line, size_t q_cmd, size_t *char_pos) {
  struct command *cmds = calloc(q_cmd, sizeof(struct command));

  for (size_t i = 0; i < q_cmd; ++i) {
    size_t offset = (i != 0) ? 1 : 0;
    cmds[i] = get_argv(input_line + char_pos[i] + offset, char_pos[i + 1] - char_pos[i]);
  }

  return cmds;
}

int spawn_proc(int in, int out, struct command *cmd) {
  pid_t pid;

  if ((pid = fork()) == 0) {
    if (in != STDIN_FILENO) {
      dup2(in, STDIN_FILENO);
      close(in);
    }

    if (out != STDOUT_FILENO) {
      dup2(out, STDOUT_FILENO);
      close(out);
    }

    return execvp (cmd->argv[0], (char * const *)cmd->argv);
  }

  return pid;
}

int fork_pipes(struct command *cmds, size_t num_of_cmds) {
  int i;
  int fd[2];

  /* The first process should get its input from the original file descriptor 0.  */
  int in = 0;
  /* Note the loop bound, we spawn here all, but the last stage of the pipeline.  */
  for (i = 0; i < num_of_cmds - 1; ++i) {
    pipe(fd);
    /* f[1] is the write end of the pipe, we carry `in` from the prev iteration.  */
    spawn_proc(in, fd[1], cmds + i);
    /* No need for the write end of the pipe, the child will write here.  */
    close(fd[1]);
    /* Keep the read end of the pipe, the next child will read from there.  */
    in = fd[0];
  }

  /* Last stage of the pipeline - set stdin be the read end of the previous pipe
     and output to the original file descriptor 1. */
  if (in != 0) {
    dup2(in, 0);
  }

  int file_fd = open("/home/vpetrigo/result.out", O_WRONLY | O_CREAT, 0666);
  dup2(file_fd, 1);
  /* Execute the last stage with the current process. */
  return execvp(cmds[i].argv[0], cmds[i].argv);
}

int main() {
  char line[LINE_SIZE];

  fgets(line, LINE_SIZE, stdin);
  size_t input_len = strlen(line);

  // printf("Read %s (size: %zu)\n", line, input_len);

  if (input_len != 0) {
    // Handle several programs
    size_t d_pos[input_len];
    memset(d_pos, 0, input_len * sizeof(size_t));

    size_t q_cmd = count_commands(line, input_len, d_pos);
    struct command *cmds = parse_commands(line, q_cmd, d_pos);

    fork_pipes(cmds, q_cmd);
    free_commands(cmds, q_cmd);
  }

  return 0;
}
