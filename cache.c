/*
 * cache.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "cache.h"
#include "main.h"

#define MASK 0xFFFFFFFF

/* cache configuration parameters */
static int cache_split = 0;
static int cache_usize = DEFAULT_CACHE_SIZE;
static int cache_isize = DEFAULT_CACHE_SIZE;
static int cache_dsize = DEFAULT_CACHE_SIZE;
static int cache_block_size = DEFAULT_CACHE_BLOCK_SIZE;
static int words_per_block = DEFAULT_CACHE_BLOCK_SIZE / WORD_SIZE;
static int cache_assoc = DEFAULT_CACHE_ASSOC;
static int cache_writeback = DEFAULT_CACHE_WRITEBACK;
static int cache_writealloc = DEFAULT_CACHE_WRITEALLOC;

/* cache model data structures */
static Pcache icache;
static Pcache dcache;
static cache c1;
static cache c2;
static cache_stat cache_stat_inst;
static cache_stat cache_stat_data;

/************************************************************/
void set_cache_param(param, value)
  int param;
  int value;
{

  switch (param) {
  case CACHE_PARAM_BLOCK_SIZE:
    cache_block_size = value;
    words_per_block = value / WORD_SIZE; /* En bytes */
    break;
  case CACHE_PARAM_USIZE:
    cache_split = FALSE;
    cache_usize = value;
    break;
  case CACHE_PARAM_ISIZE:
    cache_split = TRUE;
    cache_isize = value;
    break;
  case CACHE_PARAM_DSIZE:
    cache_split = TRUE;
    cache_dsize = value;
    break;
  case CACHE_PARAM_ASSOC:
    cache_assoc = value;
    break;
  case CACHE_PARAM_WRITEBACK:
    cache_writeback = TRUE;
    break;
  case CACHE_PARAM_WRITETHROUGH:
    cache_writeback = FALSE;
    break;
  case CACHE_PARAM_WRITEALLOC:
    cache_writealloc = TRUE;
    break;
  case CACHE_PARAM_NOWRITEALLOC:
    cache_writealloc = FALSE;
    break;
  default:
    printf("error set_cache_param: bad parameter value\n");
    exit(-1);
  }

}
/************************************************************/

// type: -1 unified, 0 i cache, 1 d cache
void init_pcache(pcache, type)
  Pcache pcache;
  int type;
{
    int pcachesize = (type==-1) ? cache_usize : ( (type==0) ? cache_isize : cache_dsize );

    pcache->size = pcachesize;
    pcache->associativity = cache_assoc;
    pcache->n_sets = pcache->size / cache_block_size / pcache->associativity;
    pcache->LRU_head = (Pcache_line*)malloc(sizeof(Pcache_line)*pcache->n_sets);
    pcache->LRU_tail=NULL;
    //pcache->contents=NULL;
    pcache->tag_mask_offset = LOG2(pcache->size);
    pcache->tag_mask = MASK << pcache->tag_mask_offset;
    pcache->index_mask_offset = pcache->tag_mask_offset - LOG2(pcache->n_sets);
    pcache->index_mask = (MASK >> ( (DIR_SIZE - pcache->tag_mask_offset) + pcache->index_mask_offset ) ) << pcache->index_mask_offset;
    for(int i=0; i < pcache->n_sets; i++)
      pcache->LRU_head[i]=NULL;
}

/************************************************************/
void init_cache()
{

  /* initialize the cache, and cache statistics data structures */

  // data structures
  cache_stat_inst.accesses = 0;
  cache_stat_inst.misses = 0;
  cache_stat_inst.replacements = 0;
  cache_stat_inst.demand_fetches = 0;
  cache_stat_inst.copies_back = 0;

  cache_stat_data.accesses = 0;
  cache_stat_data.misses = 0;
  cache_stat_data.replacements = 0;
  cache_stat_data.demand_fetches = 0;
  cache_stat_data.copies_back = 0;

  // cache
  icache = &c1;
  dcache = &c2;

  if(cache_split){
      init_pcache(icache, 0);
      init_pcache(dcache, 1);
  }
  else{
      init_pcache(dcache, -1);
  }
}
/************************************************************/

/************************************************************/
void perform_access(addr, access_type)
  unsigned addr, access_type;
{

  /* handle an access to the cache */

  unsigned tag, ind;
  tag = dcache->tag_mask & addr;
  ind = (addr & dcache->index_mask) >> dcache->index_mask_offset;

  switch (access_type) {
      case TRACE_DATA_LOAD:
          if (cache_writealloc && cache_writeback)
              ;
          else if (cache_writealloc && !cache_writeback)
              ;
          else if (!cache_writealloc && cache_writeback)
              ;
          else if (!cache_writealloc && !cache_writeback)
              ;
          else
              printf("TRACE_DATA_LOAD - ups\n");
      break;
      case TRACE_DATA_STORE:
          if (cache_writealloc && cache_writeback)
              ;
          else if (cache_writealloc && !cache_writeback)
              ;
          else if (!cache_writealloc && cache_writeback)
              ;
          else if (!cache_writealloc && !cache_writeback)
              ;
          else
              printf("TRACE_DATA_STORE - ups\n");
      break;
      case TRACE_INST_LOAD:
          if (cache_writealloc && cache_writeback)
              ;
          else if (cache_writealloc && !cache_writeback)
              ;
          else if (!cache_writealloc && cache_writeback)
              ;
          else if (!cache_writealloc && !cache_writeback)
              ;
          else
              printf("TRACE_INST_LOAD - ups\n");
      break;
      default:
        printf("skipping access, unknown type(%d)\n", access_type);

/*
    case TRACE_DATA_LOAD:
        cache_stat_data.accesses++;
        if(dcache->LRU_head[ind] == NULL) // miss
        {
            cache_stat_data.misses++;
            dcache->LRU_head[ind]=malloc(sizeof(cache_line));
            dcache->LRU_head[ind]->tag = tag;
            dcache->LRU_head[ind]->dirty = 0;
            cache_stat_data.demand_fetches += words_per_block;
        }
        else if(dcache->LRU_head[ind]->tag != tag) //miss
        {
            if (dcache->LRU_head[ind]->dirty) {
                cache_stat_data.copies_back += words_per_block;
            }
            cache_stat_data.misses++;
            cache_stat_data.replacements++;
            cache_stat_data.demand_fetches += words_per_block;
            dcache->LRU_head[ind]->tag = tag;
            dcache->LRU_head[ind]->dirty = 0;
        }
        break;
    case TRACE_DATA_STORE:
        cache_stat_data.accesses++;
        if(dcache->LRU_head[ind] == NULL) // miss
        {
            cache_stat_data.misses++;
            dcache->LRU_head[ind] = malloc(sizeof(cache_line));
            dcache->LRU_head[ind]->tag = tag;
            dcache->LRU_head[ind]->dirty = 1;
            cache_stat_data.demand_fetches += words_per_block;
        }
        else if(dcache->LRU_head[ind]->tag != tag) //miss
        {
            if (dcache->LRU_head[ind]->dirty) {
                cache_stat_data.copies_back += words_per_block;
            }
            cache_stat_data.misses++;
            cache_stat_data.replacements++;
            cache_stat_data.demand_fetches += words_per_block;
            dcache->LRU_head[ind]->tag = tag;
            dcache->LRU_head[ind]->dirty = 1;
        }
        else
        {
            dcache->LRU_head[ind]->dirty = 1;
        }
        break;
    case TRACE_INST_LOAD:
        cache_stat_inst.accesses++;
        if(!cache_split){
            if(dcache->LRU_head[ind] == NULL) // miss
            {
                cache_stat_inst.misses++;
                dcache->LRU_head[ind]=malloc(sizeof(cache_line));
                dcache->LRU_head[ind]->tag = tag;
                dcache->LRU_head[ind]->dirty = 0;
                cache_stat_inst.demand_fetches+=words_per_block;
            }
            else if(dcache->LRU_head[ind]->tag != tag) //miss
            {
                if (dcache->LRU_head[ind]->dirty) {
                    cache_stat_data.copies_back+=words_per_block;
                }
                cache_stat_inst.misses++;
                cache_stat_inst.replacements++;
                cache_stat_inst.demand_fetches+=words_per_block;
                dcache->LRU_head[ind]->tag = tag;
                dcache->LRU_head[ind]->dirty = 0;
            }
        }
        else{
            if(icache->LRU_head[ind] == NULL) // miss
            {
                cache_stat_inst.misses++;
                icache->LRU_head[ind]=malloc(sizeof(cache_line));
                icache->LRU_head[ind]->tag = tag;
                icache->LRU_head[ind]->dirty = 0;
                cache_stat_inst.demand_fetches+=words_per_block;
            }
            else if(icache->LRU_head[ind]->tag != tag) //miss
            {
                if (icache->LRU_head[ind]->dirty) {
                    cache_stat_data.copies_back+=words_per_block;
                }
                cache_stat_inst.misses++;
                cache_stat_inst.replacements++;
                cache_stat_inst.demand_fetches+=words_per_block;
                icache->LRU_head[ind]->tag = tag;
                icache->LRU_head[ind]->dirty = 0;
            }
        }
            break;
        default:
            printf("skipping access, unknown type(%d)\n", access_type);
*/
  }

}
/************************************************************/

/************************************************************/
void flush()
{

  /* flush the cache */
  for(int i=0; i < dcache->n_sets; i++)
      if(dcache->LRU_head[i]!=NULL)
        if(dcache->LRU_head[i]->dirty)
            cache_stat_data.copies_back+=words_per_block;
}
/************************************************************/

/************************************************************/
void delete(head, tail, item)
  Pcache_line *head, *tail;
  Pcache_line item;
{
  if (item->LRU_prev) {
    item->LRU_prev->LRU_next = item->LRU_next;
  } else {
    /* item at head */
    *head = item->LRU_next;
  }

  if (item->LRU_next) {
    item->LRU_next->LRU_prev = item->LRU_prev;
  } else {
    /* item at tail */
    *tail = item->LRU_prev;
  }
}
/************************************************************/

/************************************************************/
/* inserts at the head of the list */
void insert(head, tail, item)
  Pcache_line *head, *tail;
  Pcache_line item;
{
  item->LRU_next = *head;
  item->LRU_prev = (Pcache_line)NULL;

  if (item->LRU_next)
    item->LRU_next->LRU_prev = item;
  else
    *tail = item;

  *head = item;
}
/************************************************************/

void custom_print()
{
  printf("%d B, %s, %d, %d, %s, %s, %d, %d, %d, %d, %d, %d\n",
    cache_split ? cache_isize : cache_usize,
    cache_split ? "Split" : "Unified",
    cache_block_size, cache_assoc,
    cache_writeback ? "WB" : "WT",
    cache_writealloc ? "WA" : "WNA",
    cache_stat_inst.misses, cache_stat_inst.replacements,
    cache_stat_data.misses, cache_stat_data.replacements,
    cache_stat_inst.demand_fetches + cache_stat_data.demand_fetches,
    cache_stat_inst.copies_back + cache_stat_data.copies_back);
}

/************************************************************/
void dump_settings()
{
  printf("*** CACHE SETTINGS ***\n");
  if (cache_split) {
    printf("  Split I- D-cache\n");
    printf("  I-cache size: \t%d\n", cache_isize);
    printf("  D-cache size: \t%d\n", cache_dsize);
  } else {
    printf("  Unified I- D-cache\n");
    printf("  Size: \t%d\n", cache_usize);
  }
  printf("  Associativity: \t%d\n", cache_assoc);
  printf("  Block size: \t%d\n", cache_block_size);
  printf("  Write policy: \t%s\n",
	 cache_writeback ? "WRITE BACK" : "WRITE THROUGH");
  printf("  Allocation policy: \t%s\n",
	 cache_writealloc ? "WRITE ALLOCATE" : "WRITE NO ALLOCATE");
}

/************************************************************/

/************************************************************/
void print_stats()
{
  printf("\n*** CACHE STATISTICS ***\n");

  printf(" INSTRUCTIONS\n");
  printf("  accesses:  %d\n", cache_stat_inst.accesses);
  printf("  misses:    %d\n", cache_stat_inst.misses);
  if (!cache_stat_inst.accesses)
    printf("  miss rate: 0 (0)\n");
  else
    printf("  miss rate: %2.4f (hit rate %2.4f)\n",
	 (float)cache_stat_inst.misses / (float)cache_stat_inst.accesses,
	 1.0 - (float)cache_stat_inst.misses / (float)cache_stat_inst.accesses);
  printf("  replace:   %d\n", cache_stat_inst.replacements);

  printf(" DATA\n");
  printf("  accesses:  %d\n", cache_stat_data.accesses);
  printf("  misses:    %d\n", cache_stat_data.misses);
  if (!cache_stat_data.accesses)
    printf("  miss rate: 0 (0)\n");
  else
    printf("  miss rate: %2.4f (hit rate %2.4f)\n",
	 (float)cache_stat_data.misses / (float)cache_stat_data.accesses,
	 1.0 - (float)cache_stat_data.misses / (float)cache_stat_data.accesses);
  printf("  replace:   %d\n", cache_stat_data.replacements);

  printf(" TRAFFIC (in words)\n");
  printf("  demand fetch:  %d\n", cache_stat_inst.demand_fetches +
	 cache_stat_data.demand_fetches);
  printf("  copies back:   %d\n", cache_stat_inst.copies_back +
	 cache_stat_data.copies_back);
}
/************************************************************/
