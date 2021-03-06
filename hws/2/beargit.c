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

  FILE *findex = fopen(INDEX_PATH, "w");
  fclose(findex);

  FILE *fbranches = fopen(BRANCHES_PATH, "w");
  fprintf(fbranches, "%s\n", "master");
  fclose(fbranches);

  write_string_to_file(PREV_PATH, "0000000000000000000000000000000000000000");
  write_string_to_file(CURRENT_BRANCH_PATH, "master");

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
  FILE *findex = fopen(INDEX_PATH, "r");
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

  fs_mv(".beargit/.newindex", INDEX_PATH);

  return 0;
}

/* beargit rm <filename>
 * 
 * See "Step 2" in the homework 1 spec.
 *
 */
int beargit_rm(const char *filename)
{
  FILE *findex = fopen(INDEX_PATH, "r");
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

  fs_mv(".beargit/.newindex", INDEX_PATH);

  return 0;
}

const char *go_bears = "GO BEARS!";
/* beargit commit -m <msg>
 *
 * See "Step 3" in the homework 1 spec.
 *
 */
int is_commit_msg_ok(const char *msg)
{
  int i, j;

  for (i = 0; msg[i] != '\0'; i++)
  {
    for (j = 0; go_bears[j] && go_bears[j] == msg[i + j]; j++)
    {
    }

    if (go_bears[j] == '\0' || j == strlen(go_bears) - 1)
    {
      return 1;
    }
  }

  return 0;
}

int random_int(int min, int max)
{
  return min + rand() % (max + 1 - min);
}
char id_symbols[] = "61c";
void next_commit_id_hw1(char *commit_id)
{
  int i, acc = 0;
  for (i = 0; commit_id[i] != '\0'; i++)
  {
    acc += commit_id[i];
  }

  srand(time(NULL) + acc);

  while (*commit_id)
  {
    *commit_id = id_symbols[random_int(0, 2)];
    commit_id++;
  }
}

int beargit_commit_hw1(const char *msg)
{
  if (!is_commit_msg_ok(msg))
  {
    fprintf(stderr, "ERROR: Message must contain \"%s\"\n", go_bears);
    return 1;
  }

  char commit_id[COMMIT_ID_SIZE];
  read_string_from_file(PREV_PATH, commit_id, COMMIT_ID_SIZE);
  next_commit_id(commit_id);

  char path[FILENAME_SIZE + FILENAME_SIZE];
  sprintf(path, ".beargit/%s", commit_id);
  fs_mkdir(path); // create .beargit/<commit_id> dir

  sprintf(path, ".beargit/%s/.index", commit_id);
  fs_cp(INDEX_PATH, path); // copy .beargit/.index into .beargit/<commit_id>

  sprintf(path, ".beargit/%s/.prev", commit_id);
  fs_cp(PREV_PATH, path); // copy .beargit/.prev into .beargit/<commit_id>

  FILE *fp = fopen(INDEX_PATH, "r");
  char line[FILENAME_SIZE];
  while (fgets(line, FILENAME_SIZE, fp) != NULL)
  {
    strtok(line, "\n");

    sprintf(path, ".beargit/%s/%s", commit_id, line);
    fs_cp(line, path); // copying tracked files into .beargit/<commit_id>
  }

  fclose(fp);

  sprintf(path, ".beargit/%s/.msg", commit_id);
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
    sprintf(path, ".beargit/%s/.msg", commit_id);
    read_string_from_file(path, commit_msg, 100);

    printf("\ncommit %s\n", commit_id);
    printf("    %s\n", commit_msg);

    sprintf(path, ".beargit/%s/.prev", commit_id);
    read_string_from_file(path, commit_id, COMMIT_ID_SIZE);
  }

  printf("\n");

  return 0;
}

// ---------------------------------------
// HOMEWORK 2
// ---------------------------------------

// This adds a check to beargit_commit that ensures we are on the HEAD of a branch.
int beargit_commit(const char *msg)
{
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(CURRENT_BRANCH_PATH, current_branch, BRANCHNAME_SIZE);

  if (strlen(current_branch) == 0)
  {
    fprintf(stderr, "ERROR: Need to be on HEAD of a branch to commit\n");
    return 1;
  }

  return beargit_commit_hw1(msg);
}

const char *digits = "61c";

void next_commit_id(char *commit_id)
{
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(CURRENT_BRANCH_PATH, current_branch, BRANCHNAME_SIZE);

  // The first COMMIT_ID_BRANCH_BYTES=10 characters of the commit ID will
  // be used to encode the current branch number. This is necessary to avoid
  // duplicate IDs in different branches, as they can have the same pre-
  // decessor (so next_commit_id has to depend on something else).
  int n = get_branch_number(current_branch);
  for (int i = 0; i < COMMIT_ID_BRANCH_BYTES; i++)
  {
    // This translates the branch number into base 3 and substitutes 0 for
    // 6, 1 for 1 and c for 2.
    commit_id[i] = digits[n % 3];
    n /= 3;
  }

  // Use next_commit_id to fill in the rest of the commit ID.
  next_commit_id_hw1(commit_id + COMMIT_ID_BRANCH_BYTES);
}

// This helper function returns the branch number for a specific branch, or
// returns -1 if the branch does not exist.
int get_branch_number(const char *branch_name)
{
  FILE *fbranches = fopen(BRANCHES_PATH, "r");

  int branch_index = -1;
  int counter = 0;
  char line[FILENAME_SIZE];
  while (fgets(line, sizeof(line), fbranches))
  {
    strtok(line, "\n");
    if (strcmp(line, branch_name) == 0)
    {
      branch_index = counter;
    }
    counter++;
  }

  fclose(fbranches);

  return branch_index;
}

/* beargit branch
 *
 * See "Step 6" in the homework 1 spec.
 *
 */
int beargit_branch()
{
  FILE *fp;
  char line[BRANCHNAME_SIZE];
  char current_branch[BRANCHNAME_SIZE];

  read_string_from_file(CURRENT_BRANCH_PATH, current_branch, BRANCHNAME_SIZE);

  fp = fopen(BRANCHES_PATH, "r");

  while (fgets(line, FILENAME_SIZE, fp) != NULL)
  {
    strtok(line, "\n");

    if (strcmp(current_branch, line) == 0)
    {
      printf("* %s\n", line);
    }
    else
    {

      printf("  %s\n", line);
    }
  }

  fclose(fp);

  return 0;
}

/* beargit checkout
 *
 * See "Step 7" in the homework 1 spec.
 *
 */
int checkout_commit(const char *commit_id)
{
  char line[FILENAME_SIZE];
  FILE *index_fp = fopen(INDEX_PATH, "r");

  while (fgets(line, FILENAME_SIZE, index_fp) != NULL)
  {
    strtok(line, "\n");

    fs_rm(line); // Going through the index of the current index file, delete all those files (in the current directory; i.e., the directory where we ran beargit).
  }

  if (commit_id[0] == '0')
  {
    write_string_to_file(INDEX_PATH, "");
    write_string_to_file(PREV_PATH, commit_id);
    return 0;
  }

  FILE *commit_index_fp;
  char commit_index_path[FILENAME_SIZE];
  sprintf(commit_index_path, ".beargit/%s/.index", commit_id);
  char commit_file_path[FILENAME_SIZE + FILENAME_SIZE];

  fs_cp(commit_index_path, INDEX_PATH);       //Copy the index from the commit that is being checked out to the .beargit directory
  write_string_to_file(PREV_PATH, commit_id); //Write the ID of the commit that is being checked out into .prev.

  commit_index_fp = fopen(commit_index_path, "r");
  while (fgets(line, FILENAME_SIZE, commit_index_fp) != NULL)
  {
    strtok(line, "\n");
    sprintf(commit_file_path, ".beargit/%s/%s", commit_id, line);

    fs_cp(commit_file_path, line); // copying tracked files from .beargit/<commit_id> into current dir
  }

  fclose(index_fp);
  fclose(commit_index_fp);

  return 0;
}

int is_it_a_commit_id(const char *commit_id)
{
  while (*commit_id)
  {
    if (strchr(digits, *commit_id) == NULL)
    {
      return 0;
    }

    commit_id++;
  }

  return 1;
}

int beargit_checkout(const char *arg, int new_branch)
{
  // Get the current branch
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(CURRENT_BRANCH_PATH, current_branch, BRANCHNAME_SIZE);

  // If not detached, update the current branch by storing the current HEAD into that branch's file...
  // Even if we cancel later, this is still ok.
  if (strlen(current_branch))
  {
    char current_branch_file[BRANCHNAME_SIZE + 50];
    sprintf(current_branch_file, ".beargit/.branch_%s", current_branch);
    fs_cp(PREV_PATH, current_branch_file);
  }

  // Check whether the argument is a commit ID. If yes, we just stay in detached mode
  // without actually having to change into any other branch.
  if (is_it_a_commit_id(arg))
  {
    char commit_dir[FILENAME_SIZE] = ".beargit/";
    strcat(commit_dir, arg);
    if (!fs_check_dir_exists(commit_dir))
    {
      fprintf(stderr, "ERROR: Commit %s does not exist\n", arg);
      return 1;
    }

    // Set the current branch to none (i.e., detached).
    write_string_to_file(CURRENT_BRANCH_PATH, "");

    return checkout_commit(arg);
  }

  // Just a better name, since we now know the argument is a branch name.
  const char *branch_name = arg;

  // Read branches file (giving us the HEAD commit id for that branch).
  int branch_exists = (get_branch_number(branch_name) >= 0);

  // Check for errors.
  if (branch_exists && new_branch)
  {
    fprintf(stderr, "ERROR: A branch named %s already exists\n", branch_name);
    return 1;
  }
  else if (!new_branch && !branch_exists)
  {
    fprintf(stderr, "ERROR: No branch %s exists\n", branch_name);
    return 1;
  }

  // File for the branch we are changing into.
  char branch_file[BRANCHNAME_SIZE + 50] = ".beargit/.branch_";
  strcat(branch_file, branch_name);

  // Update the branch file if new branch is created (now it can't go wrong anymore)
  if (new_branch)
  {
    FILE *fbranches = fopen(BRANCHES_PATH, "a");
    fprintf(fbranches, "%s\n", branch_name);
    fclose(fbranches);
    fs_cp(PREV_PATH, branch_file);
  }

  write_string_to_file(CURRENT_BRANCH_PATH, branch_name);

  // Read the head commit ID of this branch.
  char branch_head_commit_id[COMMIT_ID_SIZE];
  read_string_from_file(branch_file, branch_head_commit_id, COMMIT_ID_SIZE);

  // Check out the actual commit.
  return checkout_commit(branch_head_commit_id);
}
