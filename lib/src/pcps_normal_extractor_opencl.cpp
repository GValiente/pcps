/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_normal_extractor.h"

#include "pcps_cloud.h"
#include "pcps_logger.h"
#include "pcps_context.h"
#include "pcps_epsilon.h"
#include "pcps_device_cloud.h"
#include "pcps_boost_compute.h"

namespace bpc = boost::compute;
using DeviceView = bpc::mapped_view<bpc::float4_>;

namespace pcps
{

namespace
{
    static_assert(sizeof(Point) == sizeof(bpc::float4_), "");

    bool computeNormals(const DeviceView& devicePoints, const Point& flipViewPoint, int cols, int rows,
                        int neighborLevels, DeviceView& deviceNormals, Context& context)
    {
        std::array<int, 3> intArgs = { neighborLevels, cols, rows };
        bpc::mapped_view<int> deviceIntArgs(intArgs.data(), intArgs.size(), *context.context);
        std::array<float, 4> floatArgs = { flipViewPoint.x, flipViewPoint.y, flipViewPoint.z, epsilon };
        bpc::mapped_view<float> deviceFloatArgs(floatArgs.data(), floatArgs.size(), *context.context);

        BOOST_COMPUTE_CLOSURE(bpc::float4_, normalTransform, (int index), (devicePoints, deviceIntArgs, deviceFloatArgs),
        {
            int neighborLevels = deviceIntArgs[0];
            int cols = deviceIntArgs[1];
            int rows = deviceIntArgs[2];

            float4 flipViewPoint = (float4)(deviceFloatArgs[0], deviceFloatArgs[1], deviceFloatArgs[2], 0);
            float epsilon = deviceFloatArgs[3];

            // computeCovarianceMatrix:

            int pointCount = 0;
            float accum[9] = { 0 };
            int x0 = index % cols;
            int y0 = index / cols;
            int r2 = neighborLevels * neighborLevels;
            int xMin = max(-neighborLevels, -x0);
            int xMax = min(neighborLevels, cols - x0 - 1);
            int yMin = max(-neighborLevels, -y0);
            int yMax = min(neighborLevels, rows - y0 - 1);

            for(int y = yMin; y <= yMax; ++y)
            {
                int pointIndex = ((y0 + y) * cols) + (x0 + xMin);
                int y2 = y * y;

                for(int x = xMin; x <= xMax; ++x)
                {
                    if((x * x) + y2 <= r2)
                    {
                        float4 point = devicePoints[pointIndex];
                        float pX = point.x;
                        float pY = point.y;
                        float pZ = point.z;

                        if(! isnan(pX) && ! isnan(pY) && ! isnan(pZ))
                        {
                            accum[0] += pX * pX;
                            accum[1] += pX * pY;
                            accum[2] += pX * pZ;
                            accum[3] += pY * pY;
                            accum[4] += pY * pZ;
                            accum[5] += pZ * pZ;
                            accum[6] += pX;
                            accum[7] += pY;
                            accum[8] += pZ;
                            ++pointCount;
                        }
                    }

                    ++pointIndex;
                }
            }

            if(! pointCount)
            {
                return (float4)(0, 0, 0, 0);
            }

            for(int index = 0; index < 9; ++index)
            {
                accum[index] /= pointCount;
            }

            float covarianceMatrix[9];
            covarianceMatrix[0] = accum[0] - accum[6] * accum[6];
            covarianceMatrix[1] = accum[1] - accum[6] * accum[7];
            covarianceMatrix[2] = accum[2] - accum[6] * accum[8];
            covarianceMatrix[4] = accum[3] - accum[7] * accum[7];
            covarianceMatrix[5] = accum[4] - accum[7] * accum[8];
            covarianceMatrix[8] = accum[5] - accum[8] * accum[8];
            covarianceMatrix[3] = covarianceMatrix[1];
            covarianceMatrix[6] = covarianceMatrix[2];
            covarianceMatrix[7] = covarianceMatrix[5];

            // Scale the matrix so its entries are in [-1,1].
            // The scaling is applied only when at least one matrix entry has magnitude larger than 1:

            float scale = fabs(covarianceMatrix[0]);

            for(int index = 1; index < 9; ++index)
            {
                scale = max(fabs(covarianceMatrix[index]), scale);
            }

            if(scale > epsilon)
            {
                float scaleInv = 1 / scale;

                for(int index = 0; index < 9; ++index)
                {
                    covarianceMatrix[index] *= scaleInv;
                }
            }

            // computeFirstRoot:

            float m00 = covarianceMatrix[0];
            float m01 = covarianceMatrix[1];
            float m02 = covarianceMatrix[2];
            float m11 = covarianceMatrix[4];
            float m12 = covarianceMatrix[5];
            float m22 = covarianceMatrix[8];

            // The characteristic equation is x^3 - c2*x^2 + c1*x - c0 = 0.
            // The eigenvalues are the roots to this equation, all guaranteed to be real-valued,
            // because the matrix is symmetric:

            float c0 = m00 * m11 * m22 +
                    2.0f * m01 * m02 * m12 -
                    m00 * m12 * m12 -
                    m11 * m02 * m02 -
                    m22 * m01 * m01;

            float eigenFirstValue = 0;

            if(fabs(c0) > epsilon)
            {
                float c1 = m00 * m11 -
                        m01 * m01 +
                        m00 * m22 -
                        m02 * m02 +
                        m11 * m22 -
                        m12 * m12;

                float c2 = m00 + m11 + m22;

                const float s_inv3 = 1.0f / 3.0f;
                const float s_sqrt3 = 1.732050808f;

                // Construct the parameters used in classifying the roots of the equation
                // and in solving the equation for the roots in closed form:

                float c2_over_3 = c2 * s_inv3;
                float half_b = 0.5f * (c0 + c2_over_3 * (2.0f * c2_over_3 * c2_over_3 - c1));
                float q = half_b * half_b;
                float a_over_3 = (c1 - c2 * c2_over_3) * s_inv3;
                float rho;
                float q_sqrt;

                if(a_over_3 > 0)
                {
                    a_over_3 = 0;
                    rho = 0;
                }
                else
                {
                    rho = sqrt(-a_over_3);
                    q += a_over_3 * a_over_3 * a_over_3;
                }

                if(q > 0)
                {
                    q_sqrt = 0.0f;
                }
                else
                {
                    q_sqrt = sqrt(-q);
                }

                // Compute the eigenvalues by solving for the roots of the polynomial:

                float theta = atan2(q_sqrt, half_b) * s_inv3;
                float cos_theta = cos(theta);
                float sin_theta = sin(theta);
                float roots0 = c2_over_3 + 2.0f * rho * cos_theta;
                float roots1 = c2_over_3 - rho * (cos_theta + s_sqrt3 * sin_theta);
                float roots2 = c2_over_3 - rho * (cos_theta - s_sqrt3 * sin_theta);

                // Sort in increasing order:

                float temp;

                if(roots0 >= roots1)
                {
                    temp = roots0;
                    roots0 = roots1;
                    roots1 = temp;
                }

                if(roots1 >= roots2)
                {
                    temp = roots1;
                    roots1 = roots2;
                    roots2 = temp;

                    if(roots0 >= roots1)
                    {
                        temp = roots0;
                        roots0 = roots1;
                        roots1 = temp;
                    }
                }

                // eigenval for symetric positive semi-definite matrix can not be negative! Set it to 0:

                if(roots0 > 0)
                {
                    eigenFirstValue = roots0;
                }
            }

            covarianceMatrix[0] -= eigenFirstValue;
            covarianceMatrix[4] -= eigenFirstValue;
            covarianceMatrix[8] -= eigenFirstValue;

            // eigen33:

            float4 row0 = (float4)(covarianceMatrix[0], covarianceMatrix[1], covarianceMatrix[2], 0);
            float4 row1 = (float4)(covarianceMatrix[3], covarianceMatrix[4], covarianceMatrix[5], 0);
            float4 row2 = (float4)(covarianceMatrix[6], covarianceMatrix[7], covarianceMatrix[8], 0);

            float4 vec1 = cross(row0, row1);
            float4 vec2 = cross(row0, row2);
            float4 vec3 = cross(row1, row2);

            float len1 = (vec1.x * vec1.x) + (vec1.y * vec1.y) + (vec1.z * vec1.z);
            float len2 = (vec2.x * vec2.x) + (vec2.y * vec2.y) + (vec2.z * vec2.z);
            float len3 = (vec3.x * vec3.x) + (vec3.y * vec3.y) + (vec3.z * vec3.z);

            float4 normal;

            if(len1 >= len2 && len1 >= len3)
            {
                normal = vec1;
                normal /= sqrt(len1);
            }
            else if(len2 >= len1 && len2 >= len3)
            {
                normal = vec2;
                normal /= sqrt(len2);
            }
            else
            {
                normal = vec3;
                normal /= sqrt(len3);
            }

            if(fabs(normal.x) > epsilon || fabs(normal.y) > epsilon || fabs(normal.z) > epsilon)
            {
                normal.w = 1;

                // Flip the estimated normal of a point towards the given viewpoint:

                float4 point = devicePoints[index];
                float4 viewPoint = flipViewPoint;
                viewPoint.x -= point.x;
                viewPoint.y -= point.y;
                viewPoint.z -= point.z;

                float cosTheta = (viewPoint.x * normal.x + viewPoint.y * normal.y + viewPoint.z * normal.z);

                if(cosTheta < 0)
                {
                    normal.x *= -1;
                    normal.y *= -1;
                    normal.z *= -1;
                }
            }
            else
            {
                normal = (float4)(0, 0, 0, 0);
            }

            return normal;
        });

        bpc::counting_iterator<int> first(0);
        bpc::counting_iterator<int> last = first + (rows * cols);

        try
        {
            bpc::transform(first, last, deviceNormals.begin(), normalTransform, *context.queue);
        }
        catch(const bpc::program_build_failure& programBuildFailure)
        {
            PCPS_LOG_ERROR << "Program build failure: " << std::endl;
            PCPS_LOG_ERROR << programBuildFailure.build_log() << std::endl;
            return false;
        }

        return true;
    }
}

bool NormalExtractor::_extract(const DeviceCloud& inputPointDeviceCloud, int neighborLevels,
                               void* outputNormalDeviceData, Context& context) const
{
    const Cloud& inputPointCloud = inputPointDeviceCloud.getHostCloud();
    auto inputDevicePoints = static_cast<const DeviceView*>(inputPointDeviceCloud.getDeviceData());
    auto outputDeviceNormals = static_cast<DeviceView*>(outputNormalDeviceData);
    const Point& sensorOrigin = inputPointCloud.sensorOrigin;
    Point flipViewPoint = _flipNormals ? _flipViewPoint : sensorOrigin;
    int cols = inputPointCloud.width;
    int rows = inputPointCloud.height;

    if(! computeNormals(*inputDevicePoints, flipViewPoint, cols, rows, neighborLevels, *outputDeviceNormals, context))
    {
        PCPS_LOG_ERROR << "Normals computation failed" << std::endl;
        return false;
    }

    return true;
}

}
