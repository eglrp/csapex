/// HEADER
#include "pointcloud_to_pointmatrix.h"

/// PROJECT
#include <csapex/model/connector_in.h>
#include <csapex/model/connector_out.h>
#include <utils_param/parameter_factory.h>
#include <csapex_vision/cv_mat_message.h>

/// SYSTEM
#include <csapex/utility/register_apex_plugin.h>
#include <pcl/point_types.h>
#include <pcl/conversions.h>

CSAPEX_REGISTER_CLASS(csapex::PointCloudToPointMatrix, csapex::Node)

using namespace csapex;
using namespace csapex::connection_types;

PointCloudToPointMatrix::PointCloudToPointMatrix()
{
}

void PointCloudToPointMatrix::process()
{
    PointCloudMessage::Ptr msg(input_->getMessage<PointCloudMessage>());

    boost::apply_visitor (PointCloudMessage::Dispatch<PointCloudToPointMatrix>(this), msg->value);
}

void PointCloudToPointMatrix::setup()
{
    setSynchronizedInputs(true);
    input_  = addInput<PointCloudMessage>("PointCloud");
    output_ = addOutput<CvMatMessage>("Point Matrix");
}

namespace implementation {
template<class PointT>
struct Impl {
    static void convert(const typename pcl::PointCloud<PointT>::Ptr cloud, cv::Mat &matrix)
    {
        int height = cloud->height;
        int width  = cloud->width;
        matrix = cv::Mat(height, width, CV_32FC3);

        for(int i = 0 ; i < height ; ++i) {
            for(int j = 0 ; j < width ; ++j) {
                PointT pos = cloud->at(i * width + j);
                matrix.at<float>(i, (j * 3 + 0)) = pos.x;
                matrix.at<float>(i, (j * 3 + 1)) = pos.y;
                matrix.at<float>(i, (j * 3 + 2)) = pos.z;
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
void PointCloudToPointMatrix::inputCloud(typename pcl::PointCloud<PointT>::Ptr cloud)
{
    #warning "Fix unsupported type encoding!"
    CvMatMessage::Ptr out(new CvMatMessage(enc::unknown));
    implementation::Impl<PointT>::convert(cloud, out->value);
    output_->publish(out);
}
