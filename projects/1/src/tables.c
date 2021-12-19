
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "tables.h"

const int SYMTBL_NON_UNIQUE = 0;
const int SYMTBL_UNIQUE_NAME = 1;

const int SYMTBL_CAP_EXTND_VALUE = 50;

/*******************************
 * Helper Functions
 *******************************/

void allocation_failed()
{
    write_to_log("Error: allocation failed\n");
    exit(1);
}

void addr_alignment_incorrect()
{
    write_to_log("Error: address is not a multiple of 4.\n");
}

void name_already_exists(const char *name)
{
    write_to_log("Error: name '%s' already exists in table.\n", name);
}

void write_symbol(FILE *output, uint32_t addr, const char *name)
{
    fprintf(output, "%u\t%s\n", addr, name);
}

/*******************************
 * Symbol Table Functions
 *******************************/

/* Creates a new SymbolTable containg 0 elements and returns a pointer to that
   table. Multiple SymbolTables may exist at the same time. 
   If memory allocation fails, you should call allocation_failed(). 
   Mode will be either SYMTBL_NON_UNIQUE or SYMTBL_UNIQUE_NAME. You will need
   to store this value for use during add_to_table().
 */
SymbolTable *create_table(int mode)
{
    SymbolTable *table;

    table = malloc(sizeof(SymbolTable));
    if (table == NULL)
    {
        allocation_failed();
    }

    table->tbl = malloc(sizeof(Symbol) * SYMTBL_CAP_EXTND_VALUE);

    if (table->tbl == NULL)
    {
        free(table);
        allocation_failed();
    }

    table->cap = SYMTBL_CAP_EXTND_VALUE;
    table->mode = mode;
    table->len = 0;

    return table;
}

/* Frees the given SymbolTable and all associated memory. */
void free_table(SymbolTable *table)
{
    Symbol *symbol;

    for (size_t i = 0; i < table->len; i++)
    {
        symbol = table->tbl + i;
        free(symbol->name);
    }

    free(table->tbl);
    free(table);
}

Symbol *find_symbol(SymbolTable *table, char *name)
{
    Symbol *symbol;

    for (size_t i = 0; i < table->len; i++)
    {
        symbol = table->tbl + i;
        if (strcmp(symbol->name, name) == 0)
        {
            return symbol;
        }
    }

    return NULL;
}

/* Adds a new symbol and its address to the SymbolTable pointed to by TABLE. 
   ADDR is given as the byte offset from the first instruction. The SymbolTable
   must be able to resize itself as more elements are added. 

   Note that NAME may point to a temporary array, so it is not safe to simply
   store the NAME pointer. You must store a copy of the given string.

   If ADDR is not word-aligned, you should call addr_alignment_incorrect() and
   return -1. If the table's mode is SYMTBL_UNIQUE_NAME and NAME already exists 
   in the table, you should call name_already_exists() and return -1. If memory
   allocation fails, you should call allocation_failed(). 

   Otherwise, you should store the symbol name and address and return 0.
 */
int add_to_table(SymbolTable *table, const char *name, uint32_t addr)
{
    if (addr % 4 != 0)
    {
        addr_alignment_incorrect();
        return -1;
    }

    Symbol *found_symbol = find_symbol(table, name);
    if (found_symbol != NULL)
    {
        if (table->mode == SYMTBL_UNIQUE_NAME)
        {
            name_already_exists(name);
            return -1;
        }
        else
        {
            found_symbol->addr = addr;
        }
    }

    if (table->len == table->cap)
    {
        table->cap += SYMTBL_CAP_EXTND_VALUE;
        table->tbl = realloc(table->tbl, table->cap * sizeof(Symbol));
        if (table->tbl == NULL)
        {
            allocation_failed();
        }
    }

    Symbol *symbol = table->tbl + table->len;

    char *new_name = malloc(sizeof(char) * strlen(name));
    if (new_name == NULL)
    {
        allocation_failed();
    }

    symbol->name = strcpy(new_name, name);
    symbol->addr = addr;
    table->len += 1;

    return 0;
}

/* Returns the address (byte offset) of the given symbol. If a symbol with name
   NAME is not present in TABLE, return -1.
 */
int64_t get_addr_for_symbol(SymbolTable *table, const char *name)
{
    Symbol *symbol = find_symbol(table, name);

    if (symbol == NULL)
    {
        return -1;
    }

    return symbol->addr;
}

/* Writes the SymbolTable TABLE to OUTPUT. You should use write_symbol() to
   perform the write. Do not print any additional whitespace or characters.
 */
void write_table(SymbolTable *table, FILE *output)
{
    Symbol *symbol;

    for (size_t i = 0; i < table->len; i++)
    {
        symbol = table->tbl + i;
        write_symbol(output, symbol->addr, symbol->name);
    }
}
