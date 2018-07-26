/** Shape descriptors.
 *
 *  This package contain functions to compute shape descriptors.
 *  @defgroup PkgDescriptor Descriptor
 *  @ingroup PkgAnalysis
 */
#pragma once

#include <vector>
#include <CGAL/boost/graph/properties.h>
#include <Eigen/Core>

namespace Euclid
{
/** @{*/

/** The spin image descriptor.
 *
 *  Spin image is an image-based local shape descriptor. For every point
 *  on a mesh, an image is generated by projecting points onto the image
 *  plane within a local support.
 *
 *  **Reference**
 *
 *  Johnson A E, Hebert M.
 *  Using spin images for efficient object recognition in cluttered 3D
 * 	scenes[J].
 *  IEEE Transactions on pattern analysis and machine intelligence,
 *  1999, 21(5): 433-449.
 */
template<typename Mesh>
class SpinImage
{
public:
    using VPMap =
        typename boost::property_map<Mesh, boost::vertex_point_t>::type;
    using Point_3 = typename boost::property_traits<VPMap>::value_type;
    using Kernel = typename CGAL::Kernel_traits<Point_3>::Kernel;
    using Vector_3 = typename Kernel::Vector_3;
    using FT = typename Kernel::FT;
    using Vertex = typename boost::graph_traits<Mesh>::vertex_descriptor;

public:
    /** Constructor.
     *
     *  Initialize some structures needed to compute a spin image.
     *
     *  @param mesh The input mesh.
     *  @param resolution The resolution of the mesh, default to zero and let
     *  the construtor estimate this value based on average edge length.
     *  @param vnormals Vertex normals of the mesh, default to nullptr and let
     *  the constructor compute the values.
     */
    explicit SpinImage(const Mesh& mesh,
                       FT resolution = 0.0,
                       const std::vector<Vector_3>* vnormals = nullptr);

    /** Destructor. */
    ~SpinImage();

    /** Compute the spin image descriptor for a vertex.
     *
     *  @param v The vertex descriptor.
     *  @param spin_img The output spin image for v.
     *  @param bin_size Multiples of the mesh resolution.
     *  @param image_width Number of rows and colums for the image.
     *  @param support_angle Maximum support angle in degrees.
     */
    template<typename T>
    void compute(const Vertex& v,
                 std::vector<T>& spin_img,
                 float bin_size = 1.0f,
                 int image_width = 15,
                 float support_angle = 60.0f);

private:
    const Mesh* _mesh;
    const std::vector<Vector_3>* _vnormals;
    FT _resolution;
    bool _is_shared;
};

/** Heat kernel signature.
 *
 *  HKS is a intrinsic, multiscale, local shape descriptor.
 *
 *  **Reference**
 *
 *  Sun J., Ovsjanikov M., Guibas L..
 *  A concise and provably informative multi-scale signature based on heat
 *  diffusion.
 *  Proceedings of the Symposium on Geometry Processing, 2009.
 */
template<typename Mesh>
class HKS
{
public:
    using VPMap =
        typename boost::property_map<Mesh, boost::vertex_point_t>::type;
    using Point_3 = typename boost::property_traits<VPMap>::value_type;
    using Kernel = typename CGAL::Kernel_traits<Point_3>::Kernel;
    using Vector_3 = typename Kernel::Vector_3;
    using FT = typename Kernel::FT;
    using Vertex = typename boost::graph_traits<Mesh>::vertex_descriptor;
    using Vec = Eigen::Matrix<FT, Eigen::Dynamic, 1>;
    using Mat = Eigen::Matrix<FT, Eigen::Dynamic, Eigen::Dynamic>;

public:
    /** Constructor.
     *
     *  Compute eigen decomposition of the mesh Laplacian.
     *
     *  @param mesh Input mesh.
     *  @param k Number of eigenvalues/eigenvectors to use.
     */
    HKS(const Mesh& mesh, unsigned k = 300);

    /** Compute hks for a single vertex.
     *
     *  @param v Vertex descriptor.
     *  @param hks Output heat kernel signature.
     *  @param tscales Number of time scales to use.
     *  @param tmin The minimum time value, default to -1 which will use the
     *  parameter setting described in the paper.
     *  @param tmax The maximum time value, default to -1 which will use the
     *  parameter setting described in the paper.
     */
    template<typename T>
    void compute(const Vertex& v,
                 std::vector<T>& hks,
                 unsigned tscales = 100,
                 float tmin = -1.0f,
                 float tmax = -1.0f);

public:
    /** The mesh being processed. */
    const Mesh* mesh;

    /** The number of eigenvalues/eigenvectors.
     *
     *  Note that this value might be smaller than what is requested in the
     *  constructor because of numerical issues and some eigenvalues may not
     *  converge.
     */
    unsigned k;

    /** The eigenvalues. */
    Vec eigenvalues;

    /** The eigenfunctions. */
    Mat eigenfunctions;
};

/** @}*/
} // namespace Euclid

#include "src/SpinImage.cpp"
#include "src/HKS.cpp"
