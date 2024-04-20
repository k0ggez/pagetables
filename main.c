#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "mlpt.h"

size_t ptbr;

size_t translate(size_t va) {
    if ((size_t *) ptbr == NULL) { // if ptbr is unset, return all ones
        return 0xFFFFFFFFFFFFFFFF;
    }

    size_t offset = (va << (64 - POBITS)) >> (64 - POBITS);
    size_t ppn = ptbr;

    for (int i = 0; i + 1 < LEVELS; i += 1) {
        size_t vpn = va >> POBITS; // chop off offset
        vpn = vpn >> ((POBITS - 3) * (LEVELS - (i + 1))); // shift until we get to current vpn, when it does nothing
        vpn &= (1 << (POBITS - 3)) - 1; // mask to chop off everything above vpn

        size_t pte = (vpn * sizeof(size_t)) + ppn; // dereference the current (ppn + 8*vpn) to get the pte

        if ( !(*(size_t *)pte & 1) ) { // check if the valid bit isn't there
            return 0xFFFFFFFFFFFFFFFF;
        }
        ppn = (*(size_t *)pte >> POBITS) << POBITS; // zero offset from pte to get address
    }shut
    size_t pa = ppn + offset; // append the va offset to final ppn

    return pa;
}

void page_allocate(size_t va) {
    size_t sizeOfTable = (size_t) pow(2, (double)POBITS);
    if ((size_t *)ptbr == NULL) { // check if ptbr isn't allocated
        posix_memalign((void **)&ptbr, sizeOfTable, sizeOfTable );
        memset((size_t *)ptbr, 0, sizeOfTable);
    }

    size_t ppn = ptbr;

    for (int i = 0; i < LEVELS; i += 1) {
        size_t vpn = va >> POBITS; // chop off offset
        vpn = vpn >> ((POBITS - 3) * (LEVELS - (i + 1))); // shift by size of vpn until we get to current vpn
        vpn &= (1 << (POBITS - 3)) - 1; // mask to chop off everything above vpn

        size_t pte = ((vpn * sizeof(size_t)) + ppn); // dereference the current (ppn + 8*vpn) to get the pte
        if (!((*(size_t *)pte) & 1)) { // check if pte is invalid ergo unallocated
            posix_memalign((void **)&pte, sizeOfTable, sizeOfTable); // allocate a page at the value of pte
            memset((size_t *)pte, 0, sizeOfTable); // zero out page table at pte
            pte |= 1; // set valid bit to true
        }

        *(size_t *)(ppn + vpn * sizeof(size_t)) = pte;
        ppn = ((pte) >> POBITS) << POBITS; // zero offset from pte to get address
    }
}
