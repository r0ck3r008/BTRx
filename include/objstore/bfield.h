#ifndef BFIELD
#define BFIELD

#include<vector>
#include<pthread.h>

using std::vector;

namespace objstore {
        class Bfield {
                vector<uint8_t> bfield;
                pthread_rwlock_t rwlock;
                bool needlock;
        private:
                void RdLock();
                void WrLock();
                void UnLock();
        public:
                Bfield(bool, bool, int);
                Bfield();
                Bfield(vector<uint8_t> &);
                ~Bfield();
                void diff(vector<uint8_t>&,
                                vector<uint8_t>&);
                bool exists(int);
                void flip(int);
        };
}

#endif
