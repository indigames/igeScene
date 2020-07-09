#pragma once

#include "utils/PyxieHeaders.h"

#include <json/json.hpp>
using json = nlohmann::json;

namespace nlohmann {

    //! Serialize Vec<N>
    template <int N>
    struct adl_serializer<Vec<N>>
    {
        static void to_json(json& j, const Vec<N>& opt)
        {
            if(N > 3) j = { opt[0], opt[1], opt[2], opt[3] };
            else if (N > 2) j = { opt[0], opt[1], opt[2] };
            else j = { opt[0], opt[1] };
        }

        static void from_json(const json& j, Vec<N>& opt)
        {
            opt[0] = j.at(0);
            opt[1] = j.at(1);
            if (N > 2) opt[2] = j.at(2);
            if (N > 3) opt[3] = j.at(3);
        }
    };

    //! Serialize Quat
    template <>
    struct adl_serializer<Quat>
    {
        static void to_json(json& j, const Quat& opt)
        {
            j = { opt[0], opt[1], opt[2], opt[3] };
        }

        static void from_json(const json& j, Quat& opt)
        {
            for(int i = 0; i < 4; i++)
            {
                j.at(i).get_to(opt[i]);
            }
        }
    };

    //! Serialize Mat<M,N>
    template <int M, int N>
    struct adl_serializer<Mat<M, N>>
    {
        static void to_json(json& j, const Mat<M, N>& opt)
        {
            if(N > 3) j = { opt[0], opt[1], opt[2], opt[3] };
            else if (N > 2) j = { opt[0], opt[1], opt[2] };
            else j = { opt[0], opt[1] };
        }

        static void from_json(const json& j, Mat<M, N>& opt)
        {
            opt[0] = j.at(0);
            opt[1] = j.at(1);
            if (N > 2) opt[2] = j.at(2);
            if (N > 3) opt[3] = j.at(3);
        }
    };
}
