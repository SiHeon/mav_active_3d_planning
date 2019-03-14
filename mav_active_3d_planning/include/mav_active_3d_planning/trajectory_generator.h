#ifndef MAV_ACTIVE_3D_PLANNING_TRAJECTORY_GENERATOR_H_
#define MAV_ACTIVE_3D_PLANNING_TRAJECTORY_GENERATOR_H_

#include "mav_active_3d_planning/trajectory_segment.h"
#include "mav_active_3d_planning/module.h"
#include "mav_active_3d_planning/defaults.h"

#include <voxblox_ros/esdf_server.h>
#include <Eigen/Core>

#include <string>

namespace mav_active_3d_planning {

    // Forward declaration
    class SegmentSelector;
    class GeneratorUpdater;

    // Base class for trajectory generation to provide uniform interface with other classes
    class TrajectoryGenerator : public Module {
        friend class ModuleFactory;

    public:
        TrajectoryGenerator(voxblox::EsdfServer *voxblox_ptr, std::string param_ns);

        virtual ~TrajectoryGenerator() {}

        // Expansion policy where to expand (from full tree)
        virtual bool selectSegment(TrajectorySegment *result, TrajectorySegment *root);

        // Expand a selected trajectory segment
        virtual bool expandSegment(TrajectorySegment *target) = 0;

        // Whether and how to update existing segments when a new trajectory is executed
        virtual bool updateSegments(TrajectorySegment *root);

        // Utility function for collision checking. Returns true if the position is reachable.
        bool checkTraversable(const Eigen::Vector3d &position);

    protected:
        TrajectoryGenerator() {}

        // Pointer to the esdf server for collision checking (and others)
        std::shared_ptr<voxblox::EsdfServer> voxblox_ptr_;

        // bounding box
        defaults::BoundingVolume bounding_volume_;

        // default modules
        std::unique_ptr<SegmentSelector> segment_selector_;
        std::unique_ptr<GeneratorUpdater> generator_updater_;

        // Parameters
        bool p_collision_optimistic_;
        double p_collision_radius_;
        std::string p_namespace_;

        // factory setup accessor
        void setVoxbloxPtr(const std::shared_ptr<voxblox::EsdfServer> &voxblox_ptr);

        virtual void setupFromParamMap(Module::ParamMap *param_map);
    };

    // Abstract encapsulation for default/modular implementations of the selectSegment method
    class SegmentSelector : public Module {
    public:
        virtual bool selectSegment(TrajectorySegment *result, TrajectorySegment *root) = 0;
    };

    // Abstract encapsulation for default/modular implementations of the updateSegments method
    class GeneratorUpdater : public Module {
        friend class ModuleFactory;

    public:
        GeneratorUpdater(TrajectoryGenerator* parent = nullptr) : parent_(parent) {};

        virtual bool updateSegments(TrajectorySegment *root) = 0;

    protected:
        TrajectoryGenerator* parent_;

        void setParent(TrajectoryGenerator* parent);
    };

}  // namespace mav_active_3d_planning
#endif  // MAV_ACTIVE_3D_PLANNING_TRAJECTORY_GENERATOR_H_