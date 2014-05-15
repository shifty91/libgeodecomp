#ifndef LIBGEODECOMP_MISC_SIMULATIONFACTORY_H
#define LIBGEODECOMP_MISC_SIMULATIONFACTORY_H

#include <libgeodecomp/io/parallelwriter.h>
#include <libgeodecomp/misc/simulationparameters.h>
#include <libgeodecomp/parallelization/cacheblockingsimulator.h>
#include <libgeodecomp/parallelization/serialsimulator.h>

namespace LibGeoDecomp {

/**
 * A convenience class for setting up all objects (e.g. writers and
 * steerers) necessary for conduction a simulation.
 */
template<typename CELL>
class SimulationFactory
{
public:
    SimulationFactory()
    {
        std::vector<std::string> simulatorTypes;
        simulatorTypes << "SerialSimulator"
                       << "CacheBlockingSimulator";
        params.addParameter("Simulator", simulatorTypes);
        params.addParameter("WavefrontWidth",  10, 1000);
        params.addParameter("WavefrontHeight", 10, 1000);
        params.addParameter("PipelineLength",  1,  30);
    }

    void addWriter(const ParallelWriter<CELL>& writer)
    {
        parallelWriters.push_back(boost::shared_ptr<ParallelWriter<CELL> >(writer.clone));
    }

    void addWriter(const Writer<CELL>& writer)
    {
        writers.push_back(boost::shared_ptr<Writer<CELL> >(writer.clone));
    }

    /**
     * Returns a new simulator according to the previously specified
     * parameters. The user is expected to delete the simulator.
     */
    Simulator<CELL> *operator()(Initializer<CELL> *initializer)
    {
        Simulator<CELL> *sim = buildSimulator(initializer);
        // FIXME: add writers here?
        return sim;
    }

    SimulationParameters& parameters()
    {
        return params;
    }

private:
    SimulationParameters params;
    std::vector<boost::shared_ptr<ParallelWriter<CELL> > > parallelWriters;
    std::vector<boost::shared_ptr<Writer<CELL> > > writers;

    Simulator<CELL> *buildSimulator(Initializer<CELL> *initializer)
    {
        if (params["Simulator"] == "SerialSimulator") {
            return new SerialSimulator<CELL>(initializer);
        }

        if (params["Simulator"] == "CacheBlockingSimulator") {
            int pipelineLength  = params["PipelineLength"];
            int wavefrontWidth  = params["WavefrontWidth"];
            int wavefrontHeight = params["WavefrontHeight"];
            Coord<2> wavefrontDim(wavefrontWidth, wavefrontHeight);
            return new CacheBlockingSimulator<CELL>(initializer, pipelineLength, wavefrontDim);
        }

        throw std::invalid_argument("unknown Simulator type");

    }
};

}

#endif