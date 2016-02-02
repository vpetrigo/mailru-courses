#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <inttypes.h>

// http://stackoverflow.com/questions/8082932/connecting-n-commands-with-pipes-in-a-shell

struct command {
  char **argv;
};

char *check_pipe_sym(const char *line) {
  static const char *pipe_sym = "|";
  // do we have a pipe symbol in our input line
  return strpbrk(line, pipe_sym);
}

void set_null(char **arg_arr, const size_t start_idx, const size_t end_idx) {
  if (end_idx <= start_idx) {
    return;
  }

  for (size_t i = start_idx; i < end_idx; ++i) {
    arg_arr[i] = NULL;
  }
}

char **get_argv(const char *line, size_t len) {
  char **arg_arr = calloc(len, sizeof(char *));
  arg_arr[0] = calloc(len * len, sizeof(char));

  for (size_t i = 1; i < len; ++i) {
    arg_arr[i] = arg_arr[i - 1] + len;
  }

  size_t prev_pos = 0;
  size_t cur_pos = 0;
  size_t arr_idx = 0;
  size_t cnt = 0;
  const char *ptr = line;

  while (*ptr) {
    while (!isspace(*ptr)) {
      ++ptr;
      ++cur_pos;
      ++cnt;
    }

    // increment because we are on the space character
    ++ptr;
    strncpy(arg_arr[arr_idx], line + prev_pos, cnt);
    // start from the next character after a space
    prev_pos = cur_pos + 1;
    ++arr_idx;
  }

  set_null(arg_arr, arr_idx, len);

  return arg_arr;
}

int main() {
  char line[256];

  fgets(line, 256, stdin);
  size_t input_len = strlen(line);

  printf("Read %s (size: %Iu)\n", line, input_len);

  if (input_len != 0) {
    char *ptr = NULL;
    if ((ptr = check_pipe_sym(line)) != NULL) {
      // Handle several programs
      printf("HERE\n");
    }
    else {
      char **arguments = get_argv(line, input_len);
      execvp(arguments[0], arguments);

      free(arguments[0]);
      free(arguments);
    }
  }

  return 0;
}
