#include <Eigen/Dense>

#include "svd.h"

std::vector<Point> svd::compute(std::vector<Point>& points)
{
    /** compute centroid */
    Point centroid = Point::centroid(points);

    /** represent points as matrix */
    Eigen::MatrixXd pointsMat(points.size(), R);
    int row = 0;
    for (auto point : points) {
        pointsMat(row, xCol) = point.m_x;
        pointsMat(row, yCol) = point.m_y;
        pointsMat(row, zCol) = point.m_z;
        row++;
    }
    /**   translate points to direction vectors */
    pointsMat.col(xCol).array() -= centroid.m_x;
    pointsMat.col(yCol).array() -= centroid.m_y;
    pointsMat.col(zCol).array() -= centroid.m_z;

    /** compute svd */
    auto svd = pointsMat.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV);

    /** get normal */
    Eigen::MatrixXd normalMat = svd.matrixV().col(2);
    Eigen::Vector3d n(normalMat(0, 0), normalMat(1, 0), normalMat(2, 0));

    /** container for growing region of interest */
    std::vector<Point> proposal;

    /** if the point-norm form is satisfied, corresponding point sits on plane
     */
    const double E_MAX = 350.0;
    const double E_MIN = -350.0;

    int index = 0;
    for (auto point : points) {
        Eigen::Vector3d v(pointsMat(index, xCol), pointsMat(index, yCol),
            pointsMat(index, zCol));

        /** grow region */
        if (n.dot(v) < E_MAX && n.dot(v) > E_MIN) {
            proposal.push_back(point);
        }
        index++;
    }
    return proposal;
}
