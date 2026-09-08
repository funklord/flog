//! Pool and lifetime tests for flog

//! @file test_pool.c
//!
//! test.c is a demonstration program: it exercises the API the way a consumer
//! does and its value is that it has never had to change. This asserts things
//! instead, and it exists because the fixed pool that backs create_flog_t()
//! without an allocator is hand-written index bookkeeping with failure modes
//! nothing else would notice -- a slot handed out twice, a slot never
//! reclaimed, or one past the end of the array.
//!
//! It runs in both configurations. Where they genuinely differ, it asserts
//! the difference rather than skipping: a pool has a bound and a heap does
//! not, and that IS the contract.

#include "flog.h"
#include <stdio.h>
#include <string.h>

static int failures;

#define CHECK(cond)                                                    \
  do {                                                           \
    if (!(cond)) {                                         \
      fprintf(stderr, "FAIL %s:%d: %s\n",             \
             __FILE__, __LINE__, #cond);       \
      failures++;                                    \
    }                                                      \
  } while (0)


//! Every slot is handed out, and one more is refused.
//!
//! Refusal is the contract create_flog_t() already had: it returned NULL when
//! malloc failed, so a caller that handles exhaustion needs no new code path.
//! With an allocator there is no bound to reach, and asserting that is the
//! point rather than a gap -- it is what makes the two configurations
//! interchangeable to everything except a target with no heap.
static void test_the_pool_hands_out_its_slots_and_then_refuses(void)
{
	flog_t *logs[FLOG_CONFIG_MAX_LOGS];
	int i;
	for(i=0;i<FLOG_CONFIG_MAX_LOGS;i++) {
		logs[i]=create_flog_t("slot",FLOG_ACCEPT_ALL);
		CHECK(logs[i]!=NULL);
	}
	flog_t *over=create_flog_t("overflow",FLOG_ACCEPT_ALL);
#ifdef FLOG_CONFIG_ALLOCATION
	//! No bound to exhaust, so the extra one succeeds.
	CHECK(over!=NULL);
	destroy_flog_t(over);
#else
	CHECK(over==NULL);
#endif
	for(i=0;i<FLOG_CONFIG_MAX_LOGS;i++)
		destroy_flog_t(logs[i]);
}


//! A destroyed log's storage becomes available again, and the whole pool
//! does. Reclaiming one slot and reclaiming all of them are different bugs:
//! an index compared with the wrong bound loses the last slot only, which a
//! single create-destroy-create would not notice.
static void test_destroying_a_log_returns_its_slot(void)
{
	flog_t *logs[FLOG_CONFIG_MAX_LOGS];
	int i;
	for(i=0;i<FLOG_CONFIG_MAX_LOGS;i++)
		logs[i]=create_flog_t("slot",FLOG_ACCEPT_ALL);

	destroy_flog_t(logs[3]);
	logs[3]=create_flog_t("reused",FLOG_ACCEPT_ALL);
	CHECK(logs[3]!=NULL);

	for(i=0;i<FLOG_CONFIG_MAX_LOGS;i++)
		destroy_flog_t(logs[i]);

	//! All of them, not just the one: a bound that is off by one leaves the
	//! last slot permanently used and only shows up on a full second round.
	for(i=0;i<FLOG_CONFIG_MAX_LOGS;i++) {
		logs[i]=create_flog_t("again",FLOG_ACCEPT_ALL);
		CHECK(logs[i]!=NULL);
	}
	for(i=0;i<FLOG_CONFIG_MAX_LOGS;i++)
		destroy_flog_t(logs[i]);
}


//! Two live logs are distinct objects with distinct names.
//!
//! The failure this guards is a pool that hands the same slot out twice, or
//! one whose name storage is shared between slots -- both of which would let
//! every test above pass while every log in a tree reported the same name.
static void test_slots_do_not_alias(void)
{
	flog_t *a=create_flog_t("alpha",FLOG_ACCEPT_ALL);
	flog_t *b=create_flog_t("bravo",FLOG_ACCEPT_ALL);
	CHECK(a!=NULL);
	CHECK(b!=NULL);
	if(a&&b) {
		CHECK(a!=b);
		CHECK(a->name!=NULL);
		CHECK(b->name!=NULL);
		if(a->name&&b->name) {
			CHECK(strcmp(a->name,"alpha")==0);
			CHECK(strcmp(b->name,"bravo")==0);
		}
	}
	destroy_flog_t(a);
	destroy_flog_t(b);
}


//! A sublog tree is built and walked without an allocator.
//!
//! The intrusive list replaced a realloc'd array, so appending can no longer
//! fail -- and a log may sit in only one parent, which the array silently
//! allowed and this refuses.
static void test_sublogs_link_without_allocating(void)
{
	flog_t *parent=create_flog_t("parent",FLOG_ACCEPT_ALL);
	flog_t *one=create_flog_t("one",FLOG_ACCEPT_ALL);
	flog_t *two=create_flog_t("two",FLOG_ACCEPT_ALL);
	CHECK(parent&&one&&two);
	if(parent&&one&&two) {
		CHECK(flog_append_sublog(parent,one)==0);
		CHECK(flog_append_sublog(parent,two)==0);
		//! Order is preserved, as the array did.
		CHECK(parent->first_sublog==one);
		CHECK(one->next_sibling==two);
		CHECK(two->next_sibling==NULL);
		//! A second parent is refused rather than truncating one of the two
		//! lists that would then share next_sibling.
		flog_t *other=create_flog_t("other",FLOG_ACCEPT_ALL);
		if(other) {
			CHECK(flog_append_sublog(other,one)!=0);
			destroy_flog_t(other);
		}
	}
	destroy_flog_t(two);
	destroy_flog_t(one);
	destroy_flog_t(parent);
}


int main(void)
{
	test_the_pool_hands_out_its_slots_and_then_refuses();
	test_destroying_a_log_returns_its_slot();
	test_slots_do_not_alias();
	test_sublogs_link_without_allocating();
	if(failures) {
		fprintf(stderr,"test_pool: %d failure(s)\n",failures);
		return(1);
	}
	printf("test_pool: all passed\n");
	return(0);
}
