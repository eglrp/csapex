/// HEADER
#include "pointcloud_to_pointmatrix.h"

/// PROJECT
#include <csapex/model/connector_in.h>
#include <csapex/model/connector_out.h>
#include <csapex_transform/time_stamp_message.h>
#include <csapex_core_plugins/string_message.h>
#include <utils_param/parameter_factory.h>
#include <csapex_vision/cv_mat_message.h>

/// SYSTEM
#include <csapex/utility/register_apex_plugin.h>
#include <pcl/point_types.h>
#include <pcl/conversions.h>

CSAPEX_REGISTER_CLASS(csapex::PointCloutToPointMatrix, csapex::Node)

using namespace csapex;
using namespace csapex::connection_types;

PointCloutToPointMatrix::PointCloutToPointMatrix()
{
}

void PointCloutToPointMatrix::allConnectorsArrived()
{
    PointCloudMessage::Ptr msg(input_->getMessage<PointCloudMessage>());

    boost::apply_visitor (PointCloudMessage::Dispatch<PointCloutToPointMatrix>(this), msg->value);
}

void PointCloutToPointMatrix::setup()
{
    setSynchronizedInputs(true);
    input_  = addInput<PointCloudMessage>("PointCloud");
    output_ = addOutput<CvMatMessage>("Point Matrix");
}

namespace implementation {
template<class PointT>
struct Impl {
    static void convert(cv::Mat &matrix, typename pcl::PointCloud<PointT>::Ptr cloud)
    {
        int height = cloud->height;
        int width  = cloud->width;
        matrix = cv::Mat(height, width, CV_32FC3);

        for(int i = 0 ; i < height ; ++i) {
            for(int j = 0 ; j < width ; ++j) {
                PointT pos = cloud->at(i * width + j);
                matrix.at<float>(i, (j * 3 + 0)) = pos.data[0];
                matrix.at<float>(i, (j * 3 + 1)) = pos.data[1];
                matrix.at<float>(i, (j * 3 + 2)) = pos.data[2];
            }
        }
    }
};
template <>
struct Impl<pcl::PointXY> {
    static void convert(cv::Mat &matrix, typename pcl::PointCloud<pcl::PointXY>::Ptr cloud)
    {
        std::runtime_error("Conversion is not supported for pcl::PointXY!");
    }
};
}


template <class PointT>
void PointCloutToPointMatrix::inputCloud(typename pcl::PointCloud<PointT>::Ptr cloud)
{
    CvMatMessage::Ptr out(new CvMatMessage(enc::unknown));
    implementation::Impl<PointT>::convert(out->value, cloud);
    output_->publish(out);
}
