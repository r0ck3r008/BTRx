#ifndef BFIELD
#define BFIELD

#include<vector>
#include<pthread.h>

using std::vector;

namespace objstore {
        class Bfield {
                pthread_rwlock_t rwlock;
                bool needlock;

                void RdLock();
                void WrLock();
                void UnLock();
        public:
                vector<uint8_t> bfield;

                Bfield(bool, bool, int);
                Bfield(bool, int);
                Bfield();
                Bfield(vector<uint8_t> &);
                ~Bfield();
                void diff(vector<uint8_t>&,
                                vector<uint8_t>&);
                bool exists(int);
                void flip(int);
                void clone(vector<uint8_t> &);
        };
}

#endif
