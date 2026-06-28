#include "../headers/pairs.h"


Pair *mPairCreate(u64 pair_id, str word, str translation, f32 learning_curve, bool saved, bool new_pair) {
  Pair *p = make(Pair);
  *p = (Pair){.pair_id = pair_id,
	      .word = strCopy(word),
	      .translation = strCopy(translation),
	      .learning_curve = learning_curve,
	      .saved = saved,
	      .new_pair = new_pair};
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
