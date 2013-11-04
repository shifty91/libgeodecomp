#ifndef LIBGEODECOMP_STORAGE_PATCHPROVIDER_H
#define LIBGEODECOMP_STORAGE_PATCHPROVIDER_H

#ifdef LIBGEODECOMP_FEATURE_HPX
#include <hpx/lcos/local/spinlock.hpp>
#endif

#include <libgeodecomp/geometry/region.h>
#include <libgeodecomp/misc/stdcontaineroverloads.h>
#include <libgeodecomp/misc/stringops.h>
#include <libgeodecomp/storage/gridvecconv.h>

namespace LibGeoDecomp {

/**
 * PatchProvider fills in grid patches into a Stepper, which is
 * important for ghostzone synchronization, but could be used for
 * steering, too.
 */
template<class GRID_TYPE>
class PatchProvider
{
public:
    typedef typename GRID_TYPE::CellType CellType;
    const static int DIM = GRID_TYPE::DIM;

    virtual ~PatchProvider() {};

    virtual void setRegion(const Region<DIM>& region)
    {
        // empty as most implementations won't need it anyway.
    }

    // fixme: add nextRequiredNanoStep() to let steppers check if PatchProviders need to modify the grid

    virtual void get(
        GRID_TYPE *destinationGrid,
        const Region<DIM>& patchableRegion,
        const std::size_t nanoStep,
        const bool remove=true) = 0;

#ifdef LIBGEODECOMP_FEATURE_HPX
    virtual void get(
        GRID_TYPE *destinationGrid,
        const Region<DIM>& patchableRegion,
        const std::size_t nanoStep,
        hpx::lcos::local::spinlock& mutex,
        const bool remove=true)
    {
        hpx::lcos::local::spinlock::scoped_lock lock(mutex);
        this->get(destinationGrid, patchableRegion, nanoStep, remove);
    }
#endif

protected:
    std::set<std::size_t> storedNanoSteps;

    void checkNanoStepGet(const std::size_t nanoStep) const
    {
        if (storedNanoSteps.empty()) {
            throw std::logic_error("no nano step available");
        }

        if ((min)(storedNanoSteps) != nanoStep) {
            throw std::logic_error(
                std::string(
                    "requested time step doesn't match expected nano step.")
                + " expected: " + StringOps::itoa((min)(storedNanoSteps))
                + " is: " + StringOps::itoa(nanoStep));
        }
    }
};

}

#endif