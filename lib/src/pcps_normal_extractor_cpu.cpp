/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_normal_extractor.h"

#include <cmath>
#include <array>
#include "pcps_cloud.h"
#include "pcps_logger.h"
#include "pcps_epsilon.h"
#include "pcps_device_cloud.h"

namespace pcps
{

namespace
{
    void computeCircleIndices(int x0, int y0, int radius, int cols, int rows, std::vector<int>& indices)
    {
        int xMin = std::max(-radius, -x0);
        int xMax = std::min(radius, cols - x0 - 1);
        int yMin = std::max(-radius, -y0);
        int yMax = std::min(radius, rows - y0 - 1);
        int r2 = radius * radius;
        indices.clear();

        for(int y = yMin; y <= yMax; ++y)
        {
            int index = ((y0 + y) * cols) + (x0 + xMin);
            int y2 = y * y;

            for(int x = xMin; x <= xMax; x++)
            {
                if((x * x) + y2 <= r2)
                {
                    indices.push_back(index);
                }

                ++index;
            }
        }
    }

    /**
     * @brief Compute the 3x3 covariance matrix of a given set of points using their indices.
     *
     * http://docs.pointclouds.org/trunk/group__common.html#ga72dfb6e965df9752c88790e026a8ab5f
     *
     * @param points The input cloud points.
     * @param indices The point cloud indices that need to be used.
     * @param covarianceMatrix The resultant 3x3 covariance matrix.
     * @return The finite points count.
     */
    std::size_t computeCovarianceMatrix(const std::vector<Point>& points, const std::vector<int>& indices,
            std::array<float, 9>& covarianceMatrix) noexcept
    {
        std::size_t pointCount = 0;
        std::array<float, 9> accum = { 0 };

        for(int index : indices)
        {
            const Point& point = points[std::size_t(index)];

            if(point.isFinite())
            {
                ++pointCount;

                accum[0] += point.x * point.x;
                accum[1] += point.x * point.y;
                accum[2] += point.x * point.z;
                accum[3] += point.y * point.y;
                accum[4] += point.y * point.z;
                accum[5] += point.z * point.z;
                accum[6] += point.x;
                accum[7] += point.y;
                accum[8] += point.z;
            }
        }

        if(pointCount > 0)
        {
            for(float& accumValue : accum)
            {
                accumValue /= pointCount;
            }

            covarianceMatrix[0] = accum[0] - accum[6] * accum[6];
            covarianceMatrix[1] = accum[1] - accum[6] * accum[7];
            covarianceMatrix[2] = accum[2] - accum[6] * accum[8];
            covarianceMatrix[4] = accum[3] - accum[7] * accum[7];
            covarianceMatrix[5] = accum[4] - accum[7] * accum[8];
            covarianceMatrix[8] = accum[5] - accum[8] * accum[8];
            covarianceMatrix[3] = covarianceMatrix[1];
            covarianceMatrix[6] = covarianceMatrix[2];
            covarianceMatrix[7] = covarianceMatrix[5];
        }

        return pointCount;
    }

    float computeRootsFirstValue(const std::array<float, 9>& covarianceMatrix) noexcept
    {
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

        float c1 = m00 * m11 -
                m01 * m01 +
                m00 * m22 -
                m02 * m02 +
                m11 * m22 -
                m12 * m12;

        float c2 = m00 + m11 + m22;
        float result = 0;

        if(std::abs(c0) > epsilon)
        {
            float s_inv3 = 1.0f / 3.0f;
            float s_sqrt3 = std::sqrt(3.0f);

            // Construct the parameters used in classifying the roots of the equation
            // and in solving the equation for the roots in closed form:

            float c2_over_3 = c2 * s_inv3;
            float half_b = 0.5f * (c0 + c2_over_3 * (2.0f * c2_over_3 * c2_over_3 - c1));
            float a_over_3 = (c1 - c2 * c2_over_3) * s_inv3;
            float q = half_b * half_b;
            float rho;
            float q_sqrt;

            if(a_over_3 > 0)
            {
                a_over_3 = 0;
                rho = 0;
            }
            else
            {
                rho = std::sqrt(-a_over_3);
                q += a_over_3 * a_over_3 * a_over_3;
            }

            if(q > 0)
            {
                q_sqrt = 0;
            }
            else
            {
                q_sqrt = std::sqrt(-q);
            }

            // Compute the eigenvalues by solving for the roots of the polynomial:

            float theta = std::atan2(q_sqrt, half_b) * s_inv3;
            float cos_theta = std::cos(theta);
            float sin_theta = std::sin(theta);
            float roots0 = c2_over_3 + 2.0f * rho * cos_theta;
            float roots1 = c2_over_3 - rho * (cos_theta + s_sqrt3 * sin_theta);
            float roots2 = c2_over_3 - rho * (cos_theta - s_sqrt3 * sin_theta);

            // Sort in increasing order:

            if(roots0 >= roots1)
            {
                std::swap(roots0, roots1);
            }

            if(roots1 >= roots2)
            {
                std::swap(roots1, roots2);

                if(roots0 >= roots1)
                {
                    std::swap(roots0, roots1);
                }
            }

            if(roots0 > 0)
            {
                result = roots0;
            }
        }

        return result;
    }

    Point eigen33(std::array<float, 9>& covarianceMatrix) noexcept
    {
        // Scale the matrix so its entries are in [-1,1].
        // The scaling is applied only when at least one matrix entry has magnitude larger than 1:

        float scale = std::abs(covarianceMatrix[0]);

        for(std::size_t index = 1; index < 9; ++index)
        {
            scale = std::max(std::abs(covarianceMatrix[index]), scale);
        }

        if(scale > epsilon)
        {
            float scaleInv = 1 / scale;

            for(float& covarianceMatrixValue : covarianceMatrix)
            {
                covarianceMatrixValue *= scaleInv;
            }
        }

        float eigenFirstValue = computeRootsFirstValue(covarianceMatrix);
        covarianceMatrix[0] -= eigenFirstValue;
        covarianceMatrix[4] -= eigenFirstValue;
        covarianceMatrix[8] -= eigenFirstValue;

        Point row0{ covarianceMatrix[0], covarianceMatrix[1], covarianceMatrix[2], 0 };
        Point row1{ covarianceMatrix[3], covarianceMatrix[4], covarianceMatrix[5], 0 };
        Point row2{ covarianceMatrix[6], covarianceMatrix[7], covarianceMatrix[8], 0 };

        Point vec1 = row0.cross(row1);
        Point vec2 = row0.cross(row2);
        Point vec3 = row1.cross(row2);

        float len1 = (vec1.x * vec1.x) + (vec1.y * vec1.y) + (vec1.z * vec1.z);
        float len2 = (vec2.x * vec2.x) + (vec2.y * vec2.y) + (vec2.z * vec2.z);
        float len3 = (vec3.x * vec3.x) + (vec3.y * vec3.y) + (vec3.z * vec3.z);

        Point normal;

        if(len1 >= len2 && len1 >= len3)
        {
            normal = vec1;
            normal /= std::sqrt(len1);
        }
        else if(len2 >= len1 && len2 >= len3)
        {
            normal = vec2;
            normal /= std::sqrt(len2);
        }
        else
        {
            normal = vec3;
            normal /= std::sqrt(len3);
        }

        return normal;
    }

    /**
     * @brief Estimate the least-squares plane normal of a given 3x3 covariance matrix.
     *
     * http://docs.pointclouds.org/trunk/group__features.html#gae5798e2ee9ef558efd8a9f1da82c5eb1
     *
     * @param covarianceMatrix The 3x3 covariance matrix.
     * @return The resultant plane parameters as: a, b, c, d (ax + by + cz + d = 0).
     */
    Point solvePlaneParameters(std::array<float, 9>& covarianceMatrix) noexcept
    {
        // Extract the eigenvector of the smallest eigenvalue
        return eigen33(covarianceMatrix);
    }

    /**
     * @brief Compute the Least-Squares plane fit for a given set of points, using their indices,
     * and return the estimated plane parameters.
     *
     * http://docs.pointclouds.org/trunk/group__features.html#gacd392447cd77d22a66f1f7b885f923e1
     *
     * @param cloud The input cloud points.
     * @param indices The point cloud indices that need to be used.
     * @return The resultant plane parameters as: a, b, c, d (ax + by + cz + d = 0).
     */
    Point computePointNormal(const std::vector<Point>& points, const std::vector<int>& indices) noexcept
    {
        std::array<float, 9> covarianceMatrix;
        Point result;

        if(computeCovarianceMatrix(points, indices, covarianceMatrix))
        {
            result = solvePlaneParameters(covarianceMatrix);

            if(std::abs(result.x) > epsilon || std::abs(result.y) > epsilon || std::abs(result.z) > epsilon)
            {
                result.aux = 1;
            }
            else
            {
                result = Point{ 0, 0, 0, 0 };
            }
        }
        else
        {
            result = Point{ 0, 0, 0, 0 };
        }

        return result;
    }

    /**
     * @brief Flip the estimated normal of a point towards a given viewpoint.
     *
     * @param point A given point.
     * @param viewPoint The given viewpoint.
     * @param normal The plane normal to be flipped.
     */
    void flipNormalTowardsViewpoint(const Point& point, Point viewPoint, Point& normal) noexcept
    {
        viewPoint.x -= point.x;
        viewPoint.y -= point.y;
        viewPoint.z -= point.z;

        // Dot product between the (viewpoint - point) and the plane normal
        float cosTheta = (viewPoint.x * normal.x + viewPoint.y * normal.y + viewPoint.z * normal.z);

        // Flip the plane normal
        if(cosTheta < 0)
        {
            normal.x *= -1;
            normal.y *= -1;
            normal.z *= -1;
        }
    }

    void computeNormals(const std::vector<Point>& points, const Point& flipViewPoint, int cols, int rows,
                        int neighborLevels, Point* normals)
    {
        std::vector<int> indices;
        std::size_t index = 0;

        for(int row = 0; row < rows; ++row)
        {
            for(int col = 0; col < cols; ++col)
            {
                computeCircleIndices(col, row, neighborLevels, cols, rows, indices);

                const Point& point = points[index];
                Point& normal = normals[index];
                normal = computePointNormal(points, indices);
                flipNormalTowardsViewpoint(point, flipViewPoint, normal);
                ++index;
            }
        }
    }
}

bool NormalExtractor::_extract(const DeviceCloud& inputPointDeviceCloud, int neighborLevels,
                               void* outputNormalDeviceData, Context&) const
{
    const Cloud& inputPointCloud = inputPointDeviceCloud.getHostCloud();
    const Point& sensorOrigin = inputPointCloud.sensorOrigin;
    Point flipViewPoint = _flipNormals ? _flipViewPoint : sensorOrigin;
    int cols = inputPointCloud.width;
    int rows = inputPointCloud.height;
    auto outputNormals = static_cast<Point*>(outputNormalDeviceData);
    computeNormals(inputPointCloud.points, flipViewPoint, cols, rows, neighborLevels, outputNormals);
    return true;
}

}
