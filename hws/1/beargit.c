#include <stdio.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <sys/stat.h>

#include "beargit.h"
#include "util.h"

/* Implementation Notes:
 *
 * - Functions return 0 if successful, 1 if there is an error.
 * - All error conditions in the function description need to be implemented
 *   and written to stderr. We catch some additional errors for you in main.c.
 * - Output to stdout needs to be exactly as specified in the function description.
 * - Only edit this file (beargit.c)
 * - You are given the following helper functions:
 *   * fs_mkdir(dirname): create directory <dirname>
 *   * fs_rm(filename): delete file <filename>
 *   * fs_mv(src,dst): move file <src> to <dst>, overwriting <dst> if it exists
 *   * fs_cp(src,dst): copy file <src> to <dst>, overwriting <dst> if it exists
 *   * write_string_to_file(filename,str): write <str> to filename (overwriting contents)
 *   * read_string_from_file(filename,str,size): read a string of at most <size> (incl.
 *     NULL character) from file <filename> and store it into <str>. Note that <str>
 *     needs to be large enough to hold that string.
 *  - You NEED to test your code. The autograder we provide does not contain the
 *    full set of tests that we will run on your code. See "Step 5" in the homework spec.
 */

/* beargit init
 *
 * - Create .beargit directory
 * - Create empty .beargit/.index file
 * - Create .beargit/.prev file containing 0..0 commit id
 *
 * Output (to stdout):
 * - None if successful
 */
int beargit_init(void)
{
  fs_mkdir(".beargit");

  FILE *findex = fopen(".beargit/.index", "w");
  fclose(findex);

  write_string_to_file(".beargit/.prev", "0000000000000000000000000000000000000000");

  return 0;
}

/* beargit add <filename>
 * 
 * - Append filename to list in .beargit/.index if it isn't in there yet
 *
 * Possible errors (to stderr):
 * >> ERROR: File <filename> already added
 *
 * Output (to stdout):
 * - None if successful
 */
int beargit_add(const char *filename)
{
  FILE *findex = fopen(".beargit/.index", "r");
  FILE *fnewindex = fopen(".beargit/.newindex", "w");

  char line[FILENAME_SIZE];
  while (fgets(line, sizeof(line), findex))
  {
    strtok(line, "\n");
    if (strcmp(line, filename) == 0)
    {
      fprintf(stderr, "ERROR: File %s already added\n", filename);
      fclose(findex);
      fclose(fnewindex);
      fs_rm(".beargit/.newindex");
      return 3;
    }

    fprintf(fnewindex, "%s\n", line);
  }

  fprintf(fnewindex, "%s\n", filename);
  fclose(findex);
  fclose(fnewindex);

  fs_mv(".beargit/.newindex", ".beargit/.index");

  return 0;
}

/* beargit rm <filename>
 * 
 * See "Step 2" in the homework 1 spec.
 *
 */
int beargit_rm(const char *filename)
{
  FILE *findex = fopen(".beargit/.index", "r");
  FILE *fnewindex = fopen(".beargit/.newindex", "w");
  int found = 0;

  char line[FILENAME_SIZE];
  while (fgets(line, sizeof(line), findex))
  {
    strtok(line, "\n");
    if (strcmp(line, filename) == 0)
    {
      found = 1;
    }
    else
    {
      fprintf(fnewindex, "%s\n", line);
    }
  }

  if (!found)
  {
    fprintf(stderr, "ERROR: File %s not tracked\n", filename);
    fclose(findex);
    fclose(fnewindex);
    fs_rm(".beargit/.newindex");
    return 1;
  }

  fclose(findex);
  fclose(fnewindex);

  fs_mv(".beargit/.newindex", ".beargit/.index");

  return 0;
}

/* beargit commit -m <msg>
 *
 * See "Step 3" in the homework 1 spec.
 *
 */
const char *go_bears = "GO BEARS!";

int is_commit_msg_ok(const char *msg)
{
  int i = 0;

  while (go_bears[i] == msg[i])
  {
    if (go_bears[i] == '\0' && msg[i] == '\0')
    {
      return 1;
    }

    i++;
  }

  return 0;
}

int random_int(int min, int max)
{
  return min + rand() % (max + 1 - min);
}
char id_symbols[] = "61c";
void next_commit_id(char *commit_id)
{
  int i = 0;

  srand(time(NULL));

  while (i < COMMIT_ID_BYTES)
  {
    commit_id[i] = id_symbols[random_int(0, 2)];
    i++;
  }

  commit_id[COMMIT_ID_BYTES] = '\0';
}

int beargit_commit(const char *msg)
{
  if (!is_commit_msg_ok(msg))
  {
    fprintf(stderr, "ERROR: Message must contain \"%s\"\n", go_bears);
    return 1;
  }

  char commit_id[COMMIT_ID_SIZE];
  read_string_from_file(PREV_PATH, commit_id, COMMIT_ID_SIZE);
  next_commit_id(commit_id);

  char path[250];
  strcpy(path, ".beargit/");
  strcat(path, commit_id);
  fs_mkdir(path); // create .beargit/<commit_id> dir

  strcpy(path, ".beargit/");
  strcat(path, commit_id);
  strcat(path, "/.index");
  fs_cp(INDEX_PATH, path); // copy .beargit/.index into .beargit/<commit_id>

  strcpy(path, ".beargit/");
  strcat(path, commit_id);
  strcat(path, "/.prev");
  fs_cp(PREV_PATH, path); // copy .beargit/.prev into .beargit/<commit_id>

  FILE *fp = fopen(INDEX_PATH, "r");
  char line[FILENAME_SIZE];
  while (fgets(line, FILENAME_SIZE, fp) != NULL)
  {
    strtok(line, "\n");

    strcpy(path, ".beargit/");
    strcat(path, commit_id);
    strcat(path, "/");
    strcat(path, line);
    fs_cp(line, path); // copying tracked files into .beargit/<commit_id>
  }

  fclose(fp);

  strcpy(path, ".beargit/");
  strcat(path, commit_id);
  strcat(path, "/.msg");
  write_string_to_file(path, msg);

  write_string_to_file(PREV_PATH, commit_id);

  return 0;
}

/* beargit status
 *
 * See "Step 1" in the homework 1 spec.
 *
 */
int beargit_status()
{
  FILE *fp;
  char line[FILENAME_SIZE];
  int line_count = 0;

  fp = fopen(INDEX_PATH, "r");

  printf("Tracked files:\n\n");

  while (fgets(line, FILENAME_SIZE, fp) != NULL)
  {
    printf("  %s", line);
    line_count++;
  }

  printf("\n%d files total\n", line_count);

  fclose(fp);

  return 0;
}

/* beargit log
 *
 * See "Step 4" in the homework 1 spec.
 *
 */
int beargit_log()
{
  char commit_msg[100];
  char commit_id[COMMIT_ID_SIZE];
  read_string_from_file(PREV_PATH, commit_id, COMMIT_ID_SIZE);

  if (commit_id[0] == '0')
  {
    fprintf(stderr, "ERROR: There are no commits!\n");
    return 1;
  }

  char path[250];
  while (commit_id[0] != '0')
  {
    strcpy(path, ".beargit/");
    strcat(path, commit_id);
    strcat(path, "/.msg");
    read_string_from_file(path, commit_msg, 100);

    printf("\ncommit %s\n", commit_id);
    printf("    %s\n", commit_msg);

    strcpy(path, ".beargit/");
    strcat(path, commit_id);
    strcat(path, "/.prev");
    read_string_from_file(path, commit_id, COMMIT_ID_SIZE);
  }

  return 0;
}
