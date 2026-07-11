#ifndef MEMORIZE_PAIR_H
#define MEMORIZE_PAIR_H
#include <cstdext/core.h>

#define PAIR_STATE_NEW                 (0x01)
#define PAIR_STATE_SAVED               (0x01 << 1)
#define PAIR_STATE_REMOVED             (0x01 << 2)
#define PAIR_STATE_LEARNING_CURVE      (0x01 << 3)

#define PAIR_STATE_CHECK(ps, flag) ((ps & flag) > 0)
#define PAIR_STATE_SET(ps, flag) (ps |= flag)
#define PAIR_STATE_UNSET(ps, flag) (ps ^= flag);

typedef struct {
  u64 pair_id;
  str word;
  str translation;
  u32 pair_state;
  f32 learning_curve;
} Pair;

Pair *mPairCreate(str word, str translation);
Pair *mPairLoad(u64 pair_id, str word, str translation, f32 learning_curve);
void  mPairDestroy(Pair *p);
bool  mPairIsNew(Pair *p);
bool  mPairIsDeleted(Pair *p);
bool  mPairIsSave(Pair *p);
bool  mPairIsLearningChange(Pair *p);

#endif //MEMORIZE_PAIR_H
