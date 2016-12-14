#include "fsm.h"
#include "unity.h"

typedef enum {
  TEST_FSM_EVENT_A = 0,
  TEST_FSM_EVENT_B,
  TEST_FSM_EVENT_C,
} TEST_FSM_EVENT;

static FSM s_fsm;
static uint16_t s_num_output;

FSM_DECLARE_STATE(test_a);
FSM_DECLARE_STATE(test_b);
FSM_DECLARE_STATE(test_c);

FSM_STATE_TRANSITION(test_a) {
  FSM_ADD_TRANSITION(TEST_FSM_EVENT_A, test_a);
  FSM_ADD_TRANSITION(TEST_FSM_EVENT_B, test_b);
  FSM_ADD_TRANSITION(TEST_FSM_EVENT_C, test_c);
}

FSM_STATE_TRANSITION(test_b) {
  FSM_ADD_TRANSITION(TEST_FSM_EVENT_C, test_c);
  FSM_ADD_TRANSITION(TEST_FSM_EVENT_A, test_a);
}

FSM_STATE_TRANSITION(test_c) {
  FSM_ADD_TRANSITION(TEST_FSM_EVENT_C, test_c);
}

static void prv_output(struct FSM *fsm, const Event *e) {
  printf("[%s:%s] State reached from %s (Event %d, data %d)\n",
         fsm->name, fsm->current_state->name, fsm->last_state->name,
         e->id, e->data);
  s_num_output++;
}

void setup_test(void) {
  fsm_state_init(test_c, prv_output);
  fsm_init(&s_fsm, "test_fsm", &test_a);
  s_num_output = 0;
}

void teardown_test(void) { }

void test_fsm_transition(void) {
  Event e = {
    .id = TEST_FSM_EVENT_A,
    .data = 10
  };

  // Expect A -> A -> B -> fail (B) -> C (output) -> C (output) -> fail (C)
  bool transitioned = fsm_process_event(&s_fsm, &e);
  TEST_ASSERT_TRUE(transitioned);

  e.id = TEST_FSM_EVENT_B;
  transitioned = fsm_process_event(&s_fsm, &e);
  TEST_ASSERT_TRUE(transitioned);

  transitioned = fsm_process_event(&s_fsm, &e);
  TEST_ASSERT_FALSE(transitioned);

  e.id = TEST_FSM_EVENT_C;
  transitioned = fsm_process_event(&s_fsm, &e);
  TEST_ASSERT_TRUE(transitioned);
  TEST_ASSERT_EQUAL(1, s_num_output);

  transitioned = fsm_process_event(&s_fsm, &e);
  TEST_ASSERT_TRUE(transitioned);
  TEST_ASSERT_EQUAL(2, s_num_output);

  e.id = TEST_FSM_EVENT_A;
  transitioned = fsm_process_event(&s_fsm, &e);
  TEST_ASSERT_FALSE(transitioned);
}
