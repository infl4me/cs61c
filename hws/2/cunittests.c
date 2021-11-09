#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <CUnit/Basic.h>
#include "beargit.h"
#include "util.h"

/* printf/fprintf calls in this tester will NOT go to file. */

#undef printf
#undef fprintf

/* The suite initialization function.
 * You'll probably want to delete any leftover files in .beargit from previous
 * tests, along with the .beargit directory itself.
 *
 * You'll most likely be able to share this across suites.
 */
int init_suite(void)
{
   // preps to run tests by deleting the .beargit directory if it exists
   fs_force_rm_beargit_dir();
   unlink("TEST_STDOUT");
   unlink("TEST_STDERR");
   return 0;
}

/* You can also delete leftover files after a test suite runs, but there's
 * no need to duplicate code between this and init_suite 
 */
int clean_suite(void)
{
   return 0;
}

int are_files_equal(char *filename1, char *filename2)
{
   FILE *fp1, *fp2;
   int line_size = 512;
   char line1[line_size], line2[line_size], *lp1, *lp2;

   fp1 = fopen(filename1, "r");
   fp2 = fopen(filename2, "r");

   while (line1 != NULL && line2 != NULL)
   {
      lp1 = fgets(line1, line_size, fp1);
      lp2 = fgets(line2, line_size, fp2);

      if (lp1 == NULL && lp2 == NULL)
      {
         return 1;
      }
      else if (strcmp(line1, line2))
      {
         return 0;
      }
   }

   fclose(fp1);
   fclose(fp2);

   return 0;
}

/* Simple test of fread().
 * Reads the data previously written by testFPRINTF()
 * and checks whether the expected characters are present.
 * Must be run after testFPRINTF().
 */
void simple_sample_test(void)
{
   // This is a very basic test. Your tests should likely do more than this.
   // We suggest checking the outputs of printfs/fprintfs to both stdout
   // and stderr. To make this convenient for you, the tester replaces
   // printf and fprintf with copies that write data to a file for you
   // to access. To access all output written to stdout, you can read
   // from the "TEST_STDOUT" file. To access all output written to stderr,
   // you can read from the "TEST_STDERR" file.
   int retval;
   retval = beargit_init();
   CU_ASSERT(0 == retval);
   retval = beargit_add("asdf.txt");
   CU_ASSERT(0 == retval);
}

struct commit
{
   char msg[MSG_SIZE];
   struct commit *next;
};

void free_commit_list(struct commit **commit_list)
{
   if (*commit_list)
   {
      free_commit_list(&((*commit_list)->next));
      free(*commit_list);
   }

   *commit_list = NULL;
}

void run_commit(struct commit **commit_list, const char *msg)
{
   int retval = beargit_commit(msg);
   CU_ASSERT(0 == retval);

   struct commit *new_commit = (struct commit *)malloc(sizeof(struct commit));
   new_commit->next = *commit_list;
   strcpy(new_commit->msg, msg);
   *commit_list = new_commit;
}

void status_test(void)
{
   int retval;
   beargit_init();

   FILE *fp1 = fopen("test1", "w");
   FILE *fp2 = fopen("test2", "w");

   fclose(fp1);
   fclose(fp2);

   beargit_add("test1");
   beargit_add("test2");

   CU_ASSERT(beargit_status() == 0);
   CU_ASSERT(are_files_equal("../__fixtures__/status", "TEST_STDOUT") == 1);
}

void simple_log_test(void)
{
   struct commit *commit_list = NULL;
   int retval;
   retval = beargit_init();
   CU_ASSERT(0 == retval);
   FILE *asdf = fopen("asdf.txt", "w");
   fclose(asdf);
   retval = beargit_add("asdf.txt");
   CU_ASSERT(0 == retval);
   run_commit(&commit_list, "GO BEARS!1");
   run_commit(&commit_list, "GO BEARS!2");
   run_commit(&commit_list, "GO BEARS!3");

   retval = beargit_log();
   CU_ASSERT(0 == retval);

   struct commit *cur_commit = commit_list;

   const int LINE_SIZE = 512;
   char line[LINE_SIZE];

   FILE *fstdout = fopen("TEST_STDOUT", "r");
   CU_ASSERT_PTR_NOT_NULL(fstdout);

   while (cur_commit != NULL)
   {
      char refline[LINE_SIZE];

      // First line is empty
      CU_ASSERT_PTR_NOT_NULL(fgets(line, LINE_SIZE, fstdout));
      CU_ASSERT(!strcmp(line, "\n"));

      // Second line is commit -- don't check the ID.
      CU_ASSERT_PTR_NOT_NULL(fgets(line, LINE_SIZE, fstdout));
      CU_ASSERT(!strncmp(line, "commit", strlen("commit")));

      // Third line is msg
      sprintf(refline, "    %s\n", cur_commit->msg);
      CU_ASSERT_PTR_NOT_NULL(fgets(line, LINE_SIZE, fstdout));
      CU_ASSERT_STRING_EQUAL(line, refline);

      cur_commit = cur_commit->next;
   }

   // Last line is empty
   CU_ASSERT_PTR_NOT_NULL(fgets(line, LINE_SIZE, fstdout));
   CU_ASSERT(!strcmp(line, "\n"));

   CU_ASSERT_PTR_NULL(fgets(line, LINE_SIZE, fstdout));

   // It's the end of output
   CU_ASSERT(feof(fstdout));
   fclose(fstdout);

   free_commit_list(&commit_list);
}

/* The main() function for setting up and running the tests.
 * Returns a CUE_SUCCESS on successful running, another
 * CUnit error code on failure.
 */
int cunittester()
{
   CU_pSuite pSuite = NULL;
   CU_pSuite pSuite2 = NULL;
   CU_pSuite pSuite3 = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite("Suite_1", init_suite, clean_suite);
   if (NULL == pSuite)
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Add tests to the Suite #1 */
   if (NULL == CU_add_test(pSuite, "Simple Test #1", simple_sample_test))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   pSuite2 = CU_add_suite("Suite_2", init_suite, clean_suite);
   if (NULL == pSuite2)
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Add tests to the Suite #2 */
   if (NULL == CU_add_test(pSuite2, "Log output test", simple_log_test))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   pSuite3 = CU_add_suite("Suite_3", init_suite, clean_suite);
   if (NULL == pSuite3)
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Add tests to the Suite #3 */
   if (NULL == CU_add_test(pSuite3, "Status output test", status_test))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Run all tests using the CUnit Basic interface */
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   CU_cleanup_registry();
   return CU_get_error();
}
