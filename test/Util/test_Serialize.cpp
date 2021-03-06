#include <catch2/catch.hpp>
#include <Euclid/Util/Serialize.h>

#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>
#include <igl/isdiag.h>
#include <igl/speye.h>

#include <config.h>

TEST_CASE("Util, Serialize", "[util][serialize]")
{
    SECTION("Eigen::Matrix")
    {
        Eigen::MatrixXf from = Eigen::MatrixXf::Random(1000, 1000);
        Eigen::MatrixXf to;
        std::string file(TMP_DIR);
        file.append("matrix.cereal");

        Euclid::serialize(file, from);
        Euclid::deserialize(file, to);

        REQUIRE(from.size() == to.size());
        REQUIRE(from(0, 0) == to(0, 0));
    }

    SECTION("Eigen::Array")
    {
        Eigen::ArrayXXd from = Eigen::ArrayXXd::Random(1000, 1000);
        Eigen::ArrayXXd to;
        std::string file(TMP_DIR);
        file.append("array.cereal");

        Euclid::serialize(file, from);
        Euclid::deserialize(file, to);

        REQUIRE(from.size() == to.size());
        REQUIRE(from(0, 0) == to(0, 0));
    }

    SECTION("Eigen::SparseMatrix")
    {
        Eigen::SparseMatrix<float> from, to;
        igl::speye(1000, 1000, from);
        std::string file(TMP_DIR);
        file.append("spmat.cereal");

        Euclid::serialize(file, from);
        Euclid::deserialize(file, to);

        REQUIRE(from.nonZeros() == to.nonZeros());
        REQUIRE(igl::isdiag(to));
    }

    SECTION("mix with stl")
    {
        std::vector<Eigen::Vector3f> from(1000), to;
        for (auto& v : from) {
            v = Eigen::Vector3f::Random();
        }
        std::string file(TMP_DIR);
        file.append("stl.cereal");

        {
            auto mode = std::ios_base::out | std::ios_base::binary |
                        std::ios_base::trunc;
            std::ofstream ofs(file, mode);
            cereal::BinaryOutputArchive archive(ofs);
            archive(from);
        }

        {
            auto mode = std::ios_base::in | std::ios_base::binary;
            std::ifstream ifs(file, mode);
            cereal::BinaryInputArchive archive(ifs);
            archive(to);
        }

        REQUIRE(from.size() == to.size());
        REQUIRE(from[0] == to[0]);
    }

    SECTION("serialize to json")
    {
        Eigen::Vector3f from, to;
        from.setRandom();
        std::string file(TMP_DIR);
        file.append("vec.cereal.json");

        {
            auto mode = std::ios_base::out | std::ios_base::trunc;
            std::ofstream ofs(file, mode);
            cereal::JSONOutputArchive archive(ofs);
            archive(cereal::make_nvp("Vec3", from));
        }

        {
            auto mode = std::ios_base::in;
            std::ifstream ifs(file, mode);
            cereal::JSONInputArchive archive(ifs);
            archive(cereal::make_nvp("Vec3", to));
        }

        REQUIRE(from.size() == to.size());
        REQUIRE(from == to);
    }
}
