#include <libgeodecomp/config.h>
#ifdef LIBGEODECOMP_FEATURE_MPI
#ifndef _libgeodecomp_parallelization_hiparsimulator_patchlink_h_
#define _libgeodecomp_parallelization_hiparsimulator_patchlink_h_

#include <deque>
#include <libgeodecomp/mpilayer/mpilayer.h>
#include <libgeodecomp/parallelization/hiparsimulator/patchaccepter.h>
#include <libgeodecomp/parallelization/hiparsimulator/patchprovider.h>

namespace LibGeoDecomp {
namespace HiParSimulator {

/**
 * PatchLink encapsulates the transmission of patches to and from
 * remote processes. PatchLink::Accepter takes the patches from a
 * Stepper hands them on to MPI, while PatchLink::Provider will receive
 * the patches from the net and provide then to a Stepper.
 */
template<class GRID_TYPE>
class PatchLink
{
public:
    const static int DIM = GRID_TYPE::DIM;
    const static int ENDLESS = -1;

    class Link 
    {
    public:
        typedef typename GRID_TYPE::CellType CellType;

        // fixme: there may be multiple PatchLinks connecting any two
        // nodes. Since MPI matches messages by node, datatype and tag
        // and the first two of these three will be identical, we need
        // to make sure that the tag differs. We could use the "level"
        // of the UpdateGroup in the hierarchy for this or some kind
        // of registry.
        inline Link(
            const Region<DIM>& _region,
            const int& _tag,
            MPI::Comm *communicator = &MPI::COMM_WORLD) :
            lastNanoStep(0),
            stride(1),
            mpiLayer(communicator),
            region(_region),
            buffer(_region.size()),
            tag(_tag)
        {}

        virtual ~Link()
        {
            wait();
        }

        virtual void charge(const long& next, const long& last, const long& newStride) 
        {          
            lastNanoStep = last;
            stride = newStride;
        }

        inline void wait()
        {
            mpiLayer.wait(tag);
        }

        inline void cancel()
        {
            mpiLayer.cancelAll();
        }

    protected:
        long lastNanoStep;
        long stride;
        MPILayer mpiLayer;
        Region<DIM> region;
        SuperVector<CellType> buffer;
        int tag;
    };

    class Accepter : 
        public Link,
        public PatchAccepter<GRID_TYPE>
    {
    public:
        using Link::buffer;
        using Link::lastNanoStep;
        using Link::mpiLayer;
        using Link::region;
        using Link::stride;
        using Link::tag;
        using Link::wait;
        using PatchAccepter<GRID_TYPE>::checkNanoStepPut;
        using PatchAccepter<GRID_TYPE>::pushRequest;
        using PatchAccepter<GRID_TYPE>::requestedNanoSteps;

        inline Accepter(
            const Region<DIM>& _region,
            const int& _dest,
            const int& _tag,
            const MPI::Datatype& _cellMPIDatatype,
            MPI::Comm *communicator = &MPI::COMM_WORLD) : 
            Link(_region, _tag, communicator),
            dest(_dest),
            cellMPIDatatype(_cellMPIDatatype)

        {}

        virtual void charge(const long& next, const long& last, const long& newStride) 
        {
            Link::charge(next, last, newStride);
            pushRequest(next);
        }

        virtual void put(
            const GRID_TYPE& grid, 
            const Region<DIM>& /*validRegion*/, 
            const long& nanoStep) 
        {
            if (!checkNanoStepPut(nanoStep))
                return;

            wait();
            GridVecConv::gridToVector(grid, &buffer, region);
            mpiLayer.send(
                &buffer[0], dest, buffer.size(), tag, cellMPIDatatype);

            long nextNanoStep = requestedNanoSteps.min() + stride;
            if ((lastNanoStep == ENDLESS) || 
                (nextNanoStep < lastNanoStep)) {
                requestedNanoSteps << nextNanoStep;
            }
            requestedNanoSteps.erase_min();
        }

    private:
        int dest;
        MPI::Datatype cellMPIDatatype;
    };

    class Provider : 
        public Link,
        public PatchProvider<GRID_TYPE>
    {
    public:
        using Link::buffer;
        using Link::lastNanoStep;
        using Link::mpiLayer;
        using Link::region;
        using Link::stride;
        using Link::tag;
        using Link::wait;
        using PatchProvider<GRID_TYPE>::checkNanoStepGet;
        using PatchProvider<GRID_TYPE>::storedNanoSteps;

        inline Provider(
            const Region<DIM>& _region,
            const int& _source,
            const int& _tag,
            const MPI::Datatype& _cellMPIDatatype,
            MPI::Comm *communicator = &MPI::COMM_WORLD) : 
            Link(_region, _tag, communicator),
            source(_source),
            cellMPIDatatype(_cellMPIDatatype)
        {}

        virtual void charge(const long& next, const long& last, const long& newStride) 
        {
            Link::charge(next, last, newStride);
            recv(next);
        }

        virtual void get(
            GRID_TYPE *grid, 
            const Region<DIM>& patchableRegion, 
            const long& nanoStep,
            const bool& remove=true) 
        {
            checkNanoStepGet(nanoStep);
            wait();
            GridVecConv::vectorToGrid(buffer, grid, region);

            long nextNanoStep = storedNanoSteps.min() + stride;
            if ((lastNanoStep == ENDLESS) || 
                (nextNanoStep < lastNanoStep)) {
                recv(nextNanoStep);
            }
            storedNanoSteps.erase_min();
        }

        void recv(const long& nanoStep)
        {
            storedNanoSteps << nanoStep;
            mpiLayer.recv(&buffer[0], source, buffer.size(), tag, cellMPIDatatype);
        }

    private:
        int source;
        MPI::Datatype cellMPIDatatype;
    };

};

}
}

#endif
#endif
