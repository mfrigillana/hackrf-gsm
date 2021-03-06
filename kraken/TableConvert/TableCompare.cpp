#include "Factory.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

int main(int argc, const char* argv[])
{
    const char* usage = "usage: %s xy file1... file2...\n"               \
        "  where xy are format codes for the input.\n"                  \
        "  m - Multifile\n"                                             \
        "  s - SSD (blockdevice + index)\n"                             \
        "  d - Delta (transitional compressed format without index.)\n";


    if (argc<4) {
        printf(usage,argv[0]);
        return -1;
    }
    if (strlen(argv[1])<2) {
        printf(usage,argv[0]);
        return -1;
    }

    int consume = argc-2;

    BaseReader* dr = CreateReader(argv[1][0],&argv[2],consume);
    if (dr==NULL) {
        printf("No reader for type %c found.\n",argv[1][0]);
        return -1;
    }
    BaseReader* dtr = CreateReader(argv[1][1],&argv[argc-consume],consume);
    if (dtr==NULL) {
        printf("No reader for type %c found.\n",argv[1][1]);
        return -1;
    }

    if ((!dr->isOK())||(!dtr->isOK())) {
        printf("Error opening files.\n");
        return -1;
    }

    uint64_t e,i;
    uint64_t e2,i2;
    /* Read the first chain */
    bool more = dr->Read(e,i);
    bool more2 = dtr->Read(e2,i2);
    printf("Comparing files. Please wait");
    // printf("%llx %llx\n",e,i);
    // printf("%llx %llx\n",e2,i2);
    assert(e==e2);
    assert(i==i2);
    uint64_t cnt = 0;
    while(more) {
        // printf("%llx %llx\n",e,i);
        cnt++;
        more = dr->Read(e,i);
        more2 = dtr->Read(e2,i2);
        assert(more2==more);
        assert(e==e2);
        assert(i==i2);
    }
    cnt++;

    printf("%lli chains read.\n",cnt);

    delete dr;
    delete dtr;
    return 0;
}
