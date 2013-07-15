/**
 *******************************************************************************
 * @file mzip.c
 * @author Keidan
 * @date 03/02/2013
 * @par Project
 * unzip
 *
 * @par Copyright
 * Copyright 2011-2013 Keidan, all right reserved
 *
 * This software is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY.
 *
 * Licence summary : 
 *    You can modify and redistribute the sources code and binaries.
 *    You can send me the bug-fix
 *
 * Term of the licence in in the file licence.txt.
 *
 *******************************************************************************
 */
#include <getopt.h>
#include <signal.h>
#include <tk/text/string.h>
#include <tk/sys/z.h>
#include <sys/stat.h>
#include <sys/types.h>


static z_t z = NULL;

static const struct option long_options[] = { 
    { "help"   , 0, NULL, 'h' },
    { "input"  , 1, NULL, 'o' },
    { NULL     , 0, NULL,  0  } 
};

static void mzip_sig_int(sig_t s);
static void mzip_cleanup(void);

/**
 * Affichage du 'usage'.
 * @param err Code passe a exit.
 */
void usage(int err) {
  fprintf(stdout, "usage: mzip --input [zip] options\n");
  fprintf(stdout, "\t--help, -h: Print this help.\n");
  fprintf(stdout, "\t--input, -i: ZIP file.\n");
  exit(err);
}

void mzip_file_content(z_t z, struct zentry_s entry) {
  if(entry.isdir) {
    printf("Create directory: %s\n", entry.name);
    mkdir(entry.name, 0777);
  } else {
    printf("Create file: %s\n", entry.name);
    FILE* f = fopen(entry.name, "w+");
    fwrite(entry.content, 1, entry.info.uncompressed_size, f);
    fclose(f);
  }
}

int main(int argc, char** argv) {
  char filename[FILENAME_MAX];
  fprintf(stdout, "Mzip is a FREE software.\nCopyright 2013-2013 By kei\nLicense GPL.\n\n");
  /* pour fermer proprement sur le kill */
  atexit(mzip_cleanup);
  signal(SIGINT, (__sighandler_t)mzip_sig_int);

  bzero(filename, FILENAME_MAX);

  int opt;
  while ((opt = getopt_long(argc, argv, "hi:", long_options, NULL)) != -1) {
    switch (opt) {
      case 'h': usage(0); break;
      case 'i': /* input */
	strncpy(filename, optarg, FILENAME_MAX);
	break;
      default: /* '?' */
	fprintf(stderr, "Unknown option '%c'\n", opt);
	usage(EXIT_FAILURE);
	break;
    }
  }

  z = z_open(filename);
  if(z_get_contents(z, mzip_file_content))
    fprintf(stderr, "Unable to get the zip fie content\n");

  return EXIT_SUCCESS;
}


static void mzip_sig_int(sig_t s) {
  printf("\n"); /* saute le ^C qui s'affiche dans la console... */
  exit(0); /* call atexit */
}

static void mzip_cleanup(void) {
  if(z) z_close(z), z = NULL;
}