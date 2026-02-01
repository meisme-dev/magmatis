#include "program_info.h"
#include <core/magmatis.h>
#include <core/window.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

static Magmatis *program = NULL;
static int enable_validation = 0;

static void sigint_handler(int sig) {
  signal(sig, SIG_IGN);
  exit(program_cleanup(program));
}

static void command_line_process(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--enable-validation") == 0) {
      enable_validation = 1;
      break;
    }
  }
}

int main(int argc, char *argv[]) {
  signal(SIGINT, sigint_handler);
  command_line_process(argc, argv);

  program = program_new(DEFAULT_W, DEFAULT_H, PROGRAM_NAME, enable_validation);
  window_glfw_event_loop_run(program->window);
  return program_cleanup(program);
}