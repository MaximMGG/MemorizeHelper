#include "../headers/pairs.h"


Pair *mPairCreate(str word, str translation) {
  Pair *p = make(Pair);
  *p = (Pair){.pair_id = 0,
	      .word = strCopy(word),
	      .translation = strCopy(translation),
	      .learning_curve = 0,
	      .pair_state = 0};
  PAIR_STATE_SET(p->pair_state, PAIR_STATE_NEW);

  return p;
}

Pair *mPairLoad(u64 pair_id, str word, str translation, f32 learning_curve) {
  Pair *p = make(Pair);
  *p = (Pair){
    .pair_id = pair_id,
    .word = strCopy(word),
    .translation = strCopy(translation),
    .learning_curve = learning_curve,
    .pair_state = 0
  };
  PAIR_STATE_SET(p->pair_state, PAIR_STATE_SAVED);
  return p;
}

void mPairDestroy(Pair *p) {
  if (p != null) {
    if (p->word != null) {
      DEALLOC(p->word);
    }
    if (p->translation != null) {
      DEALLOC(p->translation);
    }
    DEALLOC(p);
  }
}

bool  mPairIsNew(Pair *p) {
  return PAIR_STATE_CHECK(p->pair_state, PAIR_STATE_NEW);
}

bool  mPairIsDeleted(Pair *p) {
  return PAIR_STATE_CHECK(p->pair_state, PAIR_STATE_REMOVED);
}

bool  mPairIsSave(Pair *p) {
  return PAIR_STATE_CHECK(p->pair_state, PAIR_STATE_SAVED);
}
