cmake_minimum_required(VERSION 3.14)
project(vis-brain)

set(CMAKE_CXX_STANDARD 17)

add_executable(vis-brain main.cpp mpi/App.h mpi/exceptions.h mpi/Communicator.h mpi/Group.h mpi/Group.cpp
        mpi/Communicator.cpp mpi/CartesianCommunicator.cpp mpi/CartesianCommunicator.h mpi/Datatype.cpp
        mpi/Datatype.h mpi/Request.h mpi/Operation.h mpi/Intercommunicator.cpp mpi/Intercommunicator.h
        mpi/SuperIntercommunicator.h mpi/GraphCommunicator.cpp mpi/GraphCommunicator.h mpi/AbstractGraphItem.cpp
        mpi/AbstractGraphItem.h mpi/Graph.cpp mpi/Graph.h mpi/GraphItem.cpp mpi/GraphItem.h mpi/App.cpp sys/Semaphore.h
        sys/exceptions.h sys/system.cpp mpi/exceptions.cpp mpi/File.cpp mpi/File.h mpi/Status.h mpi/Info.cpp mpi/Info.h
        param/Engine.cpp param/Engine.h param/exceptions.h compile_options.h sys/FileInfo.h sys/auxiliary.cpp
        sys/auxiliary.h param/Object.cpp param/Object.h param/Loadable.h Application.cpp Application.h exceptions.h
        sys/Folder.cpp sys/Folder.h log/Logger.h log/UserLogger.cpp log/UserLogger.h log/Logger.cpp log/SystemLogger.cpp
        log/SystemLogger.h log/exceptions.h log/Engine.cpp log/Engine.h log/output.h data/Matrix.cpp data/Matrix.h
        data/LocalMatrix.cpp data/LocalMatrix.h data/exceptions.h data/ContiguousMatrix.cpp data/ContiguousMatrix.h
        data/LuDecomposer.cpp data/LuDecomposer.h data/reader/Reader.cpp data/reader/Reader.h data/reader/Saver.cpp
        data/reader/Saver.h data/reader/BinReader.cpp data/reader/BinReader.h data/reader/Loader.cpp
        data/reader/Loader.h data/reader/PngReader.cpp data/reader/PngReader.h data/reader/ExternalSaver.cpp
        data/reader/ExternalSaver.h data/Interpolator.cpp data/Interpolator.h data/stream/Stream.cpp
        data/stream/Stream.h data/stream/BinStream.cpp data/stream/BinStream.h data/graph/ColorAxis.cpp
        data/graph/ColorAxis.h data/stream/ExternalStream.cpp data/stream/ExternalStream.h data/noise/NoiseEngine.cpp
        data/noise/NoiseEngine.h data/noise/distributions.h data/noise/noise.cpp data/noise/noise.h
        processors/Processor.cpp processors/Processor.h processors/Equation.cpp processors/Equation.h
        stimuli/Stimulus.cpp stimuli/Stimulus.h stimuli/StationaryStimulus.cpp stimuli/StationaryStimulus.h
        stimuli/StationaryGrating.cpp stimuli/StationaryGrating.h param/Loadable.cpp processors/exceptions.h
        stimuli/StationaryGaborGrating.cpp stimuli/StationaryGaborGrating.h stimuli/StationaryRectangularGrating.cpp
        stimuli/StationaryRectangularGrating.h stimuli/StationaryDotStimulus.cpp stimuli/StationaryDotStimulus.h
        stimuli/StationaryBarStimulus.cpp stimuli/StationaryBarStimulus.h stimuli/StationaryReaderStimulus.cpp
        stimuli/StationaryReaderStimulus.h stimuli/StepStimulusParameters.cpp stimuli/StepStimulusParameters.h
        stimuli/GratingStimulusParameters.cpp stimuli/GratingStimulusParameters.h stimuli/MovingStimulus.cpp
        stimuli/MovingStimulus.h stimuli/MovingGrating.cpp stimuli/MovingGrating.h stimuli/MovingGaborGrating.cpp
        stimuli/MovingGaborGrating.h stimuli/MovingRectangularGrating.cpp stimuli/MovingRectangularGrating.h
        stimuli/BarStimulusParameters.cpp stimuli/BarStimulusParameters.h stimuli/MovingBarStimulus.cpp
        stimuli/MovingBarStimulus.h stimuli/DotStimulusParameters.cpp stimuli/DotStimulusParameters.h
        stimuli/MovingDotStimulus.cpp stimuli/MovingDotStimulus.h stimuli/StreamStimulus.cpp stimuli/StreamStimulus.h
        sys/ExternalMechanism.cpp sys/ExternalMechanism.h stimuli/ExternalStimulus.cpp stimuli/ExternalStimulus.h
        stimuli/ComplexStimulus.cpp stimuli/ComplexStimulus.h stimuli/WeightedStimulus.cpp stimuli/WeightedStimulus.h
        stimuli/BoundedStimulus.cpp stimuli/BoundedStimulus.h stimuli/SequenceStimulus.cpp stimuli/SequenceStimulus.h
        models/abstract/glm/StimulusSaturation.cpp models/abstract/glm/NoStimulusSaturation.cpp
        models/abstract/glm/GlmLayer.cpp models/abstract/glm/BrokenLineStimulusSaturation.cpp
        models/abstract/glm/BrokenLineStimulusSaturation.h models/abstract/glm/HalfSigmoidStimulusSaturation.cpp
        models/abstract/glm/HalfSigmoidStimulusSaturation.h processors/Ode.h processors/SingleOde.cpp
        processors/SingleOde.h models/abstract/glm/TemporalKernel.cpp models/abstract/glm/TemporalKernel.h
        models/abstract/glm/OdeTemporalKernel.cpp models/abstract/glm/OdeTemporalKernel.h methods/ExplicitEuler.cpp
        methods/ExplicitEuler.h methods/ExplicitRecountEuler.cpp methods/ExplicitRecountEuler.h methods/KhoinMethod.cpp methods/KhoinMethod.h methods/ExplicitRungeKutta.cpp methods/ExplicitRungeKutta.h models/abstract/glm/SpatialKernel.cpp models/abstract/glm/SpatialKernel.h models/abstract/glm/GaussianSpatialKernel.cpp models/abstract/glm/GaussianSpatialKernel.h models/abstract/glm/DogFilter.cpp models/abstract/glm/DogFilter.h processors/State.cpp processors/State.h)
add_library(stimulus-test-library SHARED stimuli/test_library.cpp methods/Method.h)

target_link_libraries(vis-brain pthread png /usr/local/lib/libv8_monolith.a dl)

# include_directories(/home/serik1987/v8/v8/include)
include_directories(/usr/local/include/v8)

# These lines were added for compatibility with MS MPI
# include_directories("C:\\Program Files (x86)\\Microsoft SDKs\\MPI\\Include")
# link_directories("C:\\Program Files (x86)\\Microsoft SDKs\\MPI\\Lib\\x64")
# target_link_libraries(mpi2 "C:\\Program Files (x86)\\Microsoft SDKs\\MPI\\Lib\\x64\\msmpi.lib")