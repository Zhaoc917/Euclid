#include <catch2/catch.hpp>
#include <Euclid/Analysis/Descriptor.h>

#include <vector>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <Euclid/Geometry/MeshHelpers.h>
#include <Euclid/IO/PlyIO.h>
#include <Euclid/Math/Distance.h>
#include <stb_image_write.h>

#include <config.h>

using Kernel = CGAL::Simple_cartesian<double>;
using Vector_3 = Kernel::Vector_3;
using Mesh = CGAL::Surface_mesh<Kernel::Point_3>;
using Vertex = Mesh::Vertex_index;

TEST_CASE("Analysis, Descriptor", "[analysis][descriptor]")
{
    std::vector<double> positions;
    std::vector<double> normals;
    std::vector<unsigned> indices;
    std::string filename(DATA_DIR);
    filename.append("bunny_vn.ply");
    Euclid::read_ply<3>(
        filename, positions, &normals, nullptr, &indices, nullptr);
    Mesh mesh;
    Euclid::make_mesh<3>(mesh, positions, indices);

    auto idx1 = 7505;
    auto v1 = Vertex(idx1); // vertex on ear
    auto idx2 = 695;
    auto v2 = Vertex(idx2); // vertex next by
    auto idx3 = 3915;
    auto v3 = Vertex(idx3); // vertex on the other ear
    auto idx4 = 0;
    auto v4 = Vertex(idx4); // vertex far away

    SECTION("spin images")
    {
        const unsigned width = 256;

        Euclid::SpinImage<Mesh> si;
        si.build(mesh);

        Eigen::ArrayXd si_v1;
        si.compute(v1, si_v1, 1.0f, width, 60.0f);
        Eigen::ArrayXd si_v2;
        si.compute(v2, si_v2, 1.0f, width, 60.0f);
        Eigen::ArrayXd si_v3;
        si.compute(v3, si_v3, 1.0f, width, 60.0f);
        Eigen::ArrayXd si_v4;
        si.compute(v4, si_v4, 1.0f, width, 60.0f);

        auto d12 = Euclid::l2(si_v1, si_v2);
        auto d13 = Euclid::l2(si_v1, si_v3);
        auto d14 = Euclid::l2(si_v1, si_v4);
        REQUIRE(d12 < d13);
        REQUIRE(d13 < d14);

        Eigen::ArrayXXd si_all;
        si.compute(si_all, 1.0f, width, 60.0f);

        REQUIRE(si_v1.isApprox(si_all.col(idx1)));
        REQUIRE(si_v2.isApprox(si_all.col(idx2)));
        REQUIRE(si_v3.isApprox(si_all.col(idx3)));
        REQUIRE(si_v4.isApprox(si_all.col(idx4)));

        Euclid::SpinImage<Mesh> si1;
        si1.build(mesh, si.vnormals.get(), si.resolution);

        Eigen::ArrayXd si1_v1;
        si1.compute(v1, si1_v1, 1.0f, width, 60.0f);
        Eigen::ArrayXd si1_v2;
        si1.compute(v2, si1_v2, 1.0f, width, 60.0f);
        Eigen::ArrayXd si1_v3;
        si1.compute(v3, si1_v3, 1.0f, width, 60.0f);
        Eigen::ArrayXd si1_v4;
        si1.compute(v4, si1_v4, 1.0f, width, 60.0f);

        REQUIRE(si_v1.isApprox(si1_v1));
        REQUIRE(si_v2.isApprox(si1_v2));
        REQUIRE(si_v3.isApprox(si1_v3));
        REQUIRE(si_v4.isApprox(si1_v4));

        SECTION("output to image")
        {
            {
                auto vmax = si_v1.maxCoeff();
                si_v1 *= 255.0 / vmax;
                std::string fout(TMP_DIR);
                fout.append("bunny_spin_image1.png");
                stbi_write_png(fout.c_str(),
                               width,
                               width,
                               1,
                               &si_v1(0),
                               width * sizeof(double));
            }
            {
                auto vmax = si_v2.maxCoeff();
                si_v2 *= 255.0 / vmax;
                std::string fout(TMP_DIR);
                fout.append("bunny_spin_image2.png");
                stbi_write_png(fout.c_str(),
                               width,
                               width,
                               1,
                               &si_v2(0),
                               width * sizeof(double));
            }
            {
                auto vmax = si_v3.maxCoeff();
                si_v3 *= 255.0 / vmax;
                std::string fout(TMP_DIR);
                fout.append("bunny_spin_image3.png");
                stbi_write_png(fout.c_str(),
                               width,
                               width,
                               1,
                               &si_v3(0),
                               width * sizeof(double));
            }
            {
                auto vmax = si_v4.maxCoeff();
                si_v4 *= 255.0 / vmax;
                std::string fout(TMP_DIR);
                fout.append("bunny_spin_image4.png");
                stbi_write_png(fout.c_str(),
                               width,
                               width,
                               1,
                               &si_v4(0),
                               width * sizeof(double));
            }
        }
    }

    SECTION("heat kernel signature")
    {
        Euclid::HKS<Mesh> hks;
        hks.build(mesh, 100);

        Eigen::ArrayXd hks_v1;
        hks.compute(v1, hks_v1);
        Eigen::ArrayXd hks_v2;
        hks.compute(v2, hks_v2);
        Eigen::ArrayXd hks_v3;
        hks.compute(v3, hks_v3);
        Eigen::ArrayXd hks_v4;
        hks.compute(v4, hks_v4);

        auto d12 = Euclid::l2(hks_v1, hks_v2);
        auto d13 = Euclid::l2(hks_v1, hks_v3);
        auto d14 = Euclid::l2(hks_v1, hks_v4);
        REQUIRE(d12 < d13);
        REQUIRE(d13 < d14);

        Eigen::ArrayXXd hks_all;
        hks.compute(hks_all);
        REQUIRE(hks_v1.isApprox(hks_all.col(idx1)));
        REQUIRE(hks_v2.isApprox(hks_all.col(idx2)));
        REQUIRE(hks_v3.isApprox(hks_all.col(idx3)));
        REQUIRE(hks_v4.isApprox(hks_all.col(idx4)));

        Euclid::HKS<Mesh> hks1;
        hks1.build(mesh, hks.eigenvalues.get(), hks.eigenfunctions.get());

        Eigen::ArrayXd hks1_v1;
        hks1.compute(v1, hks1_v1);
        Eigen::ArrayXd hks1_v2;
        hks1.compute(v2, hks1_v2);
        Eigen::ArrayXd hks1_v3;
        hks1.compute(v3, hks1_v3);
        Eigen::ArrayXd hks1_v4;
        hks1.compute(v4, hks1_v4);

        REQUIRE(hks_v1.isApprox(hks1_v1));
        REQUIRE(hks_v2.isApprox(hks1_v2));
        REQUIRE(hks_v3.isApprox(hks1_v3));
        REQUIRE(hks_v4.isApprox(hks1_v4));
    }
}
