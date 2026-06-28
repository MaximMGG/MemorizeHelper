#ifndef MEMORIZE_PAIR_H
#define MEMORIZE_PAIR_H
#include <cstdext/core.h>

typedef struct {
  u64 pair_id;
  str word;
  str translation;
  bool saved;
  bool new_pair;
  f32 learning_curve;
} Pair;

Pair *mPairCreate(u64 pair_id, str word, str translation, f32 learning_curve, bool saved, bool new_pair);
void mPairDestroy(Pair *p);

#endif //MEMORIZE_PAIR_H
