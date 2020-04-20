#include <generic_control_toolbox/joint_position_integrator.hpp>

namespace generic_control_toolbox
{
JointPositionIntegrator::JointPositionIntegrator(ros::NodeHandle& nh) : nh_(nh)
{
  if (!init())
  {
    throw std::logic_error(
        "Missing parameters for the JointPositionIntegrator");
  }

  reset();
}

void JointPositionIntegrator::reset() { got_state_ = false; }

KDL::JntArray JointPositionIntegrator::update(const KDL::JntArray& q_dot,
                                              const KDL::JntArray& q, float dt)
{
  if (!got_state_)
  {
    got_state_ = true;
    virt_q_ = q;
  }

  KDL::JntArray out = virt_q_;

  out.data += q_dot.data * dt;

  for (int i = 0; i < out.rows(); i++)
  {
    if (std::abs(out(i) - q(i)) > max_joint_error_)
    {
      return virt_q_;
    }
  }

  virt_q_ = out;
  return virt_q_;
}

bool JointPositionIntegrator::init()
{
  if (!nh_.getParam("max_joint_error", max_joint_error_))
  {
    ROS_ERROR("Missing parameter max_joint_error");
    return false;
  }

  return true;
}
}  // namespace generic_control_toolbox